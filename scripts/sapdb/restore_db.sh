#!/bin/sh

SID=DBT2

echo "changing data_cache to 25000"
_o=`cat <<EOF |  /opt/sapdb/depend/bin/dbmcli -d $SID -u dbm,dbm 2>&1
param_startsession
param_put DATA_CACHE 25000
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
_o=`cat <<EOF | /opt/sapdb/depend/bin/dbmcli -d $SID -u dbm,dbm 2>&1
db_cold
util_connect dbm,dbm
util_execute init config
recover_start data 
quit
EOF`
echo "$_o"
_test=`echo $_o | grep ERR`
if ! [ "$_test" = "" ]; then
	echo "restore failed:"
	exit 1
fi

echo "force checkpoint (by backing up again)"
_o=`cat <<EOF | /opt/sapdb/depend/bin/dbmcli -d $SID -u dbm,dbm 2>&1
util_connect dbm,dbm
backup_start trash migration
quit
EOF`
echo "$_o"
_test=`echo $_o | grep ERR`
if ! [ "$_test" = "" ]; then
        echo "backup failed:"
fi

echo "restore/backup complete"
