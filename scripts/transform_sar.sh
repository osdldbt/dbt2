#!/bin/sh

SAROUTPUT=$1
SAMPLE_LENGTH=$2
OUTPUTDIR=$3

# Prepare processor utilization data.
sar -f $SAROUTPUT -u | grep all | grep -v Average | awk '{ print (NR - 1) * '$SAMPLE_LENGTH', $4 + $5 + $6 }' > $OUTPUTDIR/cpu_all.out
sar -f $SAROUTPUT -u | grep all | grep -v Average | awk '{ print (NR - 1) * '$SAMPLE_LENGTH', $4 }' > $OUTPUTDIR/cpu_user.out
sar -f $SAROUTPUT -u | grep all | grep -v Average | awk '{ print (NR - 1) * '$SAMPLE_LENGTH', $5 }' > $OUTPUTDIR/cpu_nice.out
sar -f $SAROUTPUT -u | grep all | grep -v Average | awk '{ print (NR - 1) * '$SAMPLE_LENGTH', $6 }' > $OUTPUTDIR/cpu_system.out
