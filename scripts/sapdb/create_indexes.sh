#!/bin/sh
DBNAME=DBT2
_o=`cat <<EOF | /opt/sapdb/depend/bin/dbmcli -d $DBNAME -u dba,dba -uSQL dbt,dbt 2>&1
sql_execute create unique index i_w_id on warehouse (w_id asc)
sql_execute create unique index i_d_w_id_d_id on district (d_w_id, d_id)
sql_execute create unique index i_c_w_id_c_d_id_c_id on customer (c_w_id, c_d_id, c_id)
sql_execute create unique index i_nw_w_id_no_d_id_no_o_id on new_order (no_w_id, no_d_id, no_o_id)
sql_execute create unique index i_o_w_id_o_d_id_o_id on orders (o_w_id, o_d_id, o_id)
sql_execute create unique index i_ol_w_id_ol_d_id_ol_o_id_ol_number on order_line (ol_w_id, ol_d_id, ol_o_id, ol_number)
sql_execute create unique index i_i_id on item (i_id)
sql_execute create unique index i_s_w_id_s_i_id on stock(s_w_id, s_i_id)
EOF`
echo $_o
