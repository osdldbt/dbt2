#!/bin/sh

VMSTAT_FILE=$1

cat $VMSTAT_FILE | grep -v "^procs -----------memory---------- ---swap-- -----io---- --system-- ----cpu----" | grep -v " r  b   swpd   free   buff  cache   si   so    bi    bo   in    cs us sy id wa" | awk '{ print NR, $13 }' > $VMSTAT_FILE.us.data
cat $VMSTAT_FILE | grep -v "^procs -----------memory---------- ---swap-- -----io---- --system-- ----cpu----" | grep -v " r  b   swpd   free   buff  cache   si   so    bi    bo   in    cs us sy id wa" | awk '{ print NR, $14 }' > $VMSTAT_FILE.sy.data
cat $VMSTAT_FILE | grep -v "^procs -----------memory---------- ---swap-- -----io---- --system-- ----cpu----" | grep -v " r  b   swpd   free   buff  cache   si   so    bi    bo   in    cs us sy id wa" | awk '{ print NR, $15 }' > $VMSTAT_FILE.id.data
cat $VMSTAT_FILE | grep -v "^procs -----------memory---------- ---swap-- -----io---- --system-- ----cpu----" | grep -v " r  b   swpd   free   buff  cache   si   so    bi    bo   in    cs us sy id wa" | awk '{ print NR, $16 }' > $VMSTAT_FILE.wa.data
cat $VMSTAT_FILE | grep -v "^procs -----------memory---------- ---swap-- -----io---- --system-- ----cpu----" | grep -v " r  b   swpd   free   buff  cache   si   so    bi    bo   in    cs us sy id wa" | awk '{ print NR, $13+$14+$16 }' > $VMSTAT_FILE.all.data
