#!/bin/sh
DBNAME=DBT2
_o=`cat <<EOF | /opt/sapdb/depend/bin/dbmcli -d $DBNAME -u dba,dba -uSQL dbt,dbt 2>&1
sql_execute create unique index i_orders2 on orders (o_w_id, o_d_id, o_c_id)
EOF`
echo $_o
