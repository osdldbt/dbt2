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
