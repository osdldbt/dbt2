#!/bin/sh

SID=DBT2
SAPDBBINDIR=/opt/sapdb/depend/bin
export PATH=$PATH:$SAPDBBINDIR
DATA_CACHE=10000

set -x

echo "changing data_cache to $DATA_CACHE"
_o=`cat <<EOF |  dbmcli -d $SID -u dbm,dbm 2>&1
param_startsession
param_put DATA_CACHE $DATA_CACHE
param_checkall
param_commitsession
quit
EOF`
echo "$_o"
_test=`echo $_o | grep ERR`
if ! [ "$_test" = "" ]; then
        echo "set parameters failed"
        exit 1
fi

echo "restoring database"
_o=`cat <<EOF | dbmcli -d $SID -u dbm,dbm 2>&1
db_cold
util_connect dbm,dbm
util_execute init config
recover_start data 
recover_start incr 
quit
EOF`
echo "$_o"
_test=`echo $_o | grep ERR`
if ! [ "$_test" = "" ]; then
	echo "restore failed:"
	exit 1
fi

echo "restore complete"
