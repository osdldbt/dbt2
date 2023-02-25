#!/bin/sh
#
# This file is released under the terms of the Artistic License.
# Please see the file LICENSE, included in this package, for details.
#
# Copyright (C) 2006      Open Source Development Labs, Inc.
#               2014      2ndQuadrant, Ltd.
#               2006-2022 Mark Wong
#

MIXFILE=$1

if [ $# -lt 1 ]; then
	echo "$(basename $0) is the DBT-2 mix log analyzer"
	echo ""
	echo "Usage:"
	echo "  $(basename $0) mix.log [mix-1.log [...]]"
	echo
fi

if [ ! "x$VERBOSE" = "x1" ]; then
	VERBOSE=0
fi

# Covert the list of file names from the comand line into a quoted and comma
# separated list for R.
FILENAMES=""
for FILENAME in $@; do
	FILENAMES="$FILENAMES \"$FILENAME\""
done
FILENAMES=$(echo $FILENAMES | sed -e "s/ /,/g")

R --slave --no-save << __EOF__
filenames <- c($FILENAMES)
mix <- do.call(rbind, lapply(filenames, read.csv, header=FALSE))

colnames(mix)[1] <- 'ctime'
colnames(mix)[2] <- 'txn'
colnames(mix)[3] <- 'status'
colnames(mix)[4] <- 'response'
colnames(mix)[5] <- 'id'
colnames(mix)[6] <- 'wid'
colnames(mix)[7] <- 'did'

# Get the ctime of the last START marker and the ctime the first TERMINATED
# marker to calculate steady state.
start <- max(mix\$ctime[mix\$txn == "START"])
end <- min(mix\$ctime[mix\$txn == "TERMINATED"])

total_txn <- sum(mix\$ctime > start & mix\$ctime < end)

duration = end - start
errors <- sum(mix\$status == "E")

cat(paste("============  =====  =========  =========  ===========  ",
          "===========  =====\n", sep = ""))
cat(paste("          ..     ..    Response Time (s)            ..  ",
          "         ..     ..\n", sep = ""))
cat(paste("------------  -----  --------------------  -----------  ",
          "-----------  -----\n", sep = ""))
cat(paste(" Transaction      %   Average     90th %        Total    ",
          "Rollbacks      %\n", sep = ""))
cat(paste("============  =====  =========  =========  ===========  ",
          "===========  =====\n", sep = ""))

txn <- c("d", "n", "o", "p", "s")
txn_name <- c("Delivery", "New Order", "Order Status", "Payment",
              "Stock Level")

for (i in 1:5) {
  t_total <- sum(mix\$txn == txn[i] & mix\$ctime > start & mix\$ctime < end)
  if (i == 2) {
    total_n <- t_total
  }
  t_rollback <- sum(mix\$txn == txn[i] & mix\$status == "R" &
                    mix\$ctime > start & mix\$ctime < end, na.rm=TRUE)
  t_mean <- mean(mix\$response[mix\$txn == txn[i] & mix\$ctime > start &
                 mix\$ctime < end], na.rm=TRUE)
  t_q90 <- quantile(mix\$response[mix\$txn == txn[i] & mix\$ctime > start &
                    mix\$ctime < end], .9, na.rm=TRUE)
  cat(sprintf("%12s  %5.2f  %9.3f  %9.3f  %11d  %11d  %5.2f\n",
              txn_name[i], t_total / total_txn * 100, t_mean, t_q90,
              t_total, t_rollback, t_rollback / t_total * 100))
}

cat(paste("============  =====  =========  =========  ===========  ",
          "===========  =====\n\n", sep = ""))
cat(sprintf("* Throughput: %0.2f new-order transactions per minute (NOTPM)\n",
            total_n / (duration / 60)))
cat(sprintf("* Duration: %0.1f minute(s)\n", duration / 60))
cat(sprintf("* Unknown Errors: %d\n", errors))

# Calculate the ramp up time.

duration <- start - min(mix\$ctime)
cat(sprintf("* Ramp Up Time: %0.1f minute(s)\n", duration / 60))

if ($VERBOSE == 1) {
  wid_min <- min(mix\$wid, na.rm=TRUE)
  wid_max <- max(mix\$wid, na.rm=TRUE)

  E_w <- total_txn / (wid_max - wid_min + 1)
  chi_square_w <- 0
  for (i in wid_min:wid_max) {
    O_w <- sum(mix\$wid == i & mix\$ctime > start & mix\$ctime < end)
    chi_square_w <- chi_square_w + (O_w - E_w)^2
  }
  chi_square_w <- chi_square_w / E_w
  p_w <- 1 - pchisq(chi_square_w, df=(wid_max - wid_min))
  cat(sprintf("* Warehouse (%d-%d) chi-square p-value: %f\n",
              wid_min, wid_max, p_w))

  for (i in wid_min:wid_max) {
    # FIXME: Should know district range used for test as opposed to hard code
    #        to 10.
    E_d <- O_w / 10
    chi_square_d <- 0
    for (j in 1:10) {
      O_d <- sum(mix\$wid == i & mix\$did == j & mix\$ctime > start &
                 mix\$ctime < end)
      chi_square_d <- chi_square_d + (O_d - E_d)^2
    }
    chi_square_d <- chi_square_d / E_d
    p_d <- 1 - pchisq(chi_square_d, df=9)
    cat(sprintf("* Warehouse %d District chi-square p-value: %f\n", i, p_d))
  }
}
__EOF__
