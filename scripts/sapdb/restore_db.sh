#!/bin/sh

_o=`cat <<EOF | /opt/sapdb/depend/bin/dbmcli -d DBT2 -u dbm,dbm 2>&1
db_cold
util_connect
recover_start data
quit
EOF`
_test=`echo $_o | grep OK`
if [ "$_test" = "" ]; then
	echo "restore failed: $_o"
	exit 1
fi

# Set DATA_CACHE to 25000 so we know the backup won't fail because 
# DATA_CACHE is set too high.

_o=`cat <<EOF |  /opt/sapdb/depend/bin/dbmcli -d $SID -u dbm,dbm 2>&1
param_startsession
param_put DATA_CACHE 25000
param_checkall
param_commitsession
quit
EOF`
_test=`echo $_o | grep OK`
if [ "$_test" = "" ]; then
	echo "set parameters failed: $_o"
	exit 1
fi

# Backup to /dev/null (trash) to force a checkpoint.
_o=`cat <<EOF | /opt/sapdb/depend/bin/dbmcli -d $SID -u dbm,dbm 2>&1
util_connect dbm,dbm
autolog_off
backup_start trash migration
autolog_on
quit
EOF`
_test=`echo $_o | grep OK`
if [ "$_test" = "" ]; then
	echo "backup failed: $_o"
	exit 1
fi
