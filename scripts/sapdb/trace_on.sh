#!/bin/sh

SID=DBT2
SAPDBBINDIR=/opt/sapdb/depend/bin
export PATH=$PATH:$SAPDBBINDIR

dbmcli -d $SID -u dbm,dbm util_execute diagnose vtrace default on
