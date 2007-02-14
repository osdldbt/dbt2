#!/bin/sh

SAPDBBINDIR=/opt/sapdb/depend/bin
export PATH=$PATH:$SAPDBBINDIR

set -x

_o=`cat <<EOF | dbmcli -d DBT2 -u dbm,dbm 2>&1
util_connect dbm,dbm
backup_start data migration
backup_start incr migration
quit
EOF`
_test=`echo $_o | grep ERR`
if ! [ "$_test" = "" ]; then
        echo "backup failed: $_o"
        exit 1
fi
