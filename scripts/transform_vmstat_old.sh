#!/bin/sh

VMSTAT_FILE=$1

cat $VMSTAT_FILE | grep -v "procs                      memory      swap          io     system      cpu" | grep -v " r  b   swpd   free   buff  cache   si   so    bi    bo   in    cs us sy id wa" | awk '{ print NR, $14, $15, $16 }' > $VMSTAT_FILE.data
