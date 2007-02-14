#!/bin/sh

if ! [ $# -eq 1 ]; then
	echo "usage: trace_off.sh <outdir>"
	exit 1
fi

OUTDIR=$1
SID=DBT2
SAPDBBINDIR1=/opt/sapdb/depend/bin
SAPDBBINDIR2=/opt/sapdb/indep_prog/bin
export PATH=$PATH:$SAPDBBINDIR1:$SAPDBBINDIR2

dbmcli -d $SID -u dbm,dbm sql_execute vtrace
dbmcli -d $SID -u dbm,dbm trace_prot akb
dbmgetf -d $SID -u dbm,dbm -k KNLTRCPRT -f $OUTDIR
dbmcli -d $SID -u dbm,dbm util_execute diagnose vtrace default off
