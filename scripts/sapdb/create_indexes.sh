#!/bin/sh
DBNAME=DBT2
_o=`cat <<EOF | /opt/sapdb/depend/bin/dbmcli -d $DBNAME -u dba,dba -uSQL dbt,dbt 2>&1
sql_execute create unique index i_warehouse on warehouse (w_id asc)
sql_execute create unique index i_district on district (d_w_id, d_id)
sql_execute create unique index i_customer on customer (c_w_id, c_d_id, c_id)
sql_execute create unique index i_new_order on new_order (no_w_id, no_d_id, no_o_id)
sql_execute create unique index i_orders on orders (o_w_id, o_d_id, o_id)
sql_execute create unique index i_order_line on order_line (ol_w_id, ol_d_id, ol_o_id, ol_number)
sql_execute create unique index i_item on item (i_id)
sql_execute create unique index i_stock on stock(s_w_id, s_i_id)
EOF`
echo $_o
