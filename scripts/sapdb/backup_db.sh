#!/bin/sh

_o=`cat <<EOF | /opt/sapdb/depend/bin/dbmcli -d DBT2 -u dbm,dbm 2>&1
util_connect dbm,dbm
backup_save data
quit
EOF`
_test=`echo $_o | grep OK`
if [ "$_test" = "" ]; then
        echo "backup failed: $_o"
        exit 1
fi
