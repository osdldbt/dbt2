#!/bin/sh

DBNAME=DBT2
SAPDBBINDIR=/opt/sapdb/depend/bin
export PATH=$PATH:$SAPDBBINDIR

echo -------------
echo delivery_2.sql
echo -------------

SQL="SELECT no_o_id FROM dbt.new_order WHERE no_w_id = 1 AND no_d_id = 1 ORDER BY no_o_id ASC"
echo $SQL
_o=`cat <<EOF | dbmcli -d $DBNAME -u dba,dba -uSQL dbt,dbt 2>&1
sql_execute explain $SQL
sql_execute select * from show
quit
EOF`
echo $_o
echo 

SQL="SELECT o_c_id FROM dbt.orders WHERE o_id = 1 AND o_w_id = 1 AND o_d_id = 1"
echo $SQL
_o=`cat <<EOF | dbmcli -d $DBNAME -u dba,dba -uSQL dbt,dbt 2>&1
sql_execute explain $SQL
sql_execute select * from show
quit
EOF`
echo $_o
echo

SQL="SELECT SUM(ol_amount * ol_quantity) FROM dbt.order_line WHERE ol_w_id = 1 AND ol_d_id = 1 AND ol_o_id = 1"
echo $SQL
_o=`cat <<EOF | dbmcli -d $DBNAME -u dba,dba -uSQL dbt,dbt 2>&1
sql_execute explain $SQL
sql_execute select * from show
quit
EOF`
echo $_o
echo 

echo ---------------
echo new_order_2.sql
echo ---------------

SQL="SELECT s_quantity, s_dist_01, s_data FROM dbt.stock WHERE s_i_id = 1 AND s_w_id = 1"
echo $SQL
_o=`cat <<EOF | dbmcli -d $DBNAME -u dba,dba -uSQL dbt,dbt 2>&1
sql_execute explain $SQL
sql_execute select * from show
quit
EOF`
echo $_o
echo 

SQL="SELECT s_quantity, s_dist_02, s_data FROM dbt.stock WHERE s_i_id = 1 AND s_w_id = 1"
echo $SQL
_o=`cat <<EOF | dbmcli -d $DBNAME -u dba,dba -uSQL dbt,dbt 2>&1
sql_execute explain $SQL
sql_execute select * from show
quit
EOF`
echo $_o
echo 

SQL="SELECT s_quantity, s_dist_03, s_data FROM dbt.stock WHERE s_i_id = 1 AND s_w_id = 1"
echo $SQL
_o=`cat <<EOF | dbmcli -d $DBNAME -u dba,dba -uSQL dbt,dbt 2>&1
sql_execute explain $SQL
sql_execute select * from show
quit
EOF`
echo $_o
echo 

SQL="SELECT s_quantity, s_dist_04, s_data FROM dbt.stock WHERE s_i_id = 1 AND s_w_id = 1"
echo $SQL
_o=`cat <<EOF | dbmcli -d $DBNAME -u dba,dba -uSQL dbt,dbt 2>&1
sql_execute explain $SQL
sql_execute select * from show
quit
EOF`
echo $_o
echo 

SQL="SELECT s_quantity, s_dist_05, s_data FROM dbt.stock WHERE s_i_id = 1 AND s_w_id = 1"
echo $SQL
_o=`cat <<EOF | dbmcli -d $DBNAME -u dba,dba -uSQL dbt,dbt 2>&1
sql_execute explain $SQL
sql_execute select * from show
quit
EOF`
echo $_o
echo 

SQL="SELECT s_quantity, s_dist_06, s_data FROM dbt.stock WHERE s_i_id = 1 AND s_w_id = 1"
echo $SQL
_o=`cat <<EOF | dbmcli -d $DBNAME -u dba,dba -uSQL dbt,dbt 2>&1
sql_execute explain $SQL
sql_execute select * from show
quit
EOF`
echo $_o
echo 

SQL="SELECT s_quantity, s_dist_07, s_data FROM dbt.stock WHERE s_i_id = 1 AND s_w_id = 1"
echo $SQL
_o=`cat <<EOF | dbmcli -d $DBNAME -u dba,dba -uSQL dbt,dbt 2>&1
sql_execute explain $SQL
sql_execute select * from show
quit
EOF`
echo $_o
echo 

SQL="SELECT s_quantity, s_dist_08, s_data FROM dbt.stock WHERE s_i_id = 1 AND s_w_id = 1"
echo $SQL
_o=`cat <<EOF | dbmcli -d $DBNAME -u dba,dba -uSQL dbt,dbt 2>&1
sql_execute explain $SQL
sql_execute select * from show
quit
EOF`
echo $_o
echo 

SQL="SELECT s_quantity, s_dist_09, s_data FROM dbt.stock WHERE s_i_id = 1 AND s_w_id = 1"
echo $SQL
_o=`cat <<EOF | dbmcli -d $DBNAME -u dba,dba -uSQL dbt,dbt 2>&1
sql_execute explain $SQL
sql_execute select * from show
quit
EOF`
echo $_o
echo 

SQL="SELECT s_quantity, s_dist_10, s_data FROM dbt.stock WHERE s_i_id = 1 AND s_w_id = 1"
echo $SQL
_o=`cat <<EOF | dbmcli -d $DBNAME -u dba,dba -uSQL dbt,dbt 2>&1
sql_execute explain $SQL
sql_execute select * from show
quit
EOF`
echo $_o
echo 

echo -------------
echo new_order.sql
echo -------------

SQL="SELECT w_tax FROM dbt.warehouse WHERE w_id = 1"
echo $SQL
_o=`cat <<EOF | dbmcli -d $DBNAME -u dba,dba -uSQL dbt,dbt 2>&1
sql_execute explain $SQL
sql_execute select * from show
quit
EOF`
echo $_o
echo 

SQL="SELECT d_tax, d_next_o_id FROM dbt.district WHERE d_w_id = 1 AND d_id = 1"
echo $SQL
_o=`cat <<EOF | dbmcli -d $DBNAME -u dba,dba -uSQL dbt,dbt 2>&1
sql_execute explain $SQL
sql_execute select * from show
quit
EOF`
echo $_o
echo 

SQL="SELECT c_discount, c_last, c_credit FROM dbt.customer WHERE c_w_id = 1 AND c_d_id = 1 AND c_id = 1"
echo $SQL
_o=`cat <<EOF | dbmcli -d $DBNAME -u dba,dba -uSQL dbt,dbt 2>&1
sql_execute explain $SQL
sql_execute select * from show
quit
EOF`
echo $_o
echo 

SQL="SELECT i_price, i_name, i_data FROM dbt.item WHERE i_id = 1"
echo $SQL
_o=`cat <<EOF | dbmcli -d $DBNAME -u dba,dba -uSQL dbt,dbt 2>&1
sql_execute explain $SQL
sql_execute select * from show
quit
EOF`
echo $_o
echo 

SQL="SELECT i_price, i_name, i_data FROM dbt.item WHERE i_id = 1"
echo $SQL
_o=`cat <<EOF | dbmcli -d $DBNAME -u dba,dba -uSQL dbt,dbt 2>&1
sql_execute explain $SQL
sql_execute select * from show
quit
EOF`
echo $_o
echo 

echo ----------------
echo order_status.sql
echo ----------------

SQL="SELECT c_first, c_middle, c_last, c_balance, c_id FROM dbt.customer WHERE c_last = 'a' AND c_w_id = 1 AND c_d_id = 1"
echo $SQL
_o=`cat <<EOF | dbmcli -d $DBNAME -u dba,dba -uSQL dbt,dbt 2>&1
sql_execute explain $SQL
sql_execute select * from show
quit
EOF`
echo $_o
echo 

SQL="SELECT c_first, c_middle, c_last, c_balance FROM dbt.customer WHERE c_id = 1 AND c_w_id = 1 AND c_d_id = 1"
echo $SQL
_o=`cat <<EOF | dbmcli -d $DBNAME -u dba,dba -uSQL dbt,dbt 2>&1
sql_execute explain $SQL
sql_execute select * from show
quit
EOF`
echo $_o
echo 

SQL="SELECT o_id, o_carrier_id, CHAR(o_entry_d, ISO) FROM dbt.orders WHERE o_w_id = 1 AND o_d_id = 1 AND o_c_id = 1 ORDER BY o_id DESC"
echo $SQL
_o=`cat <<EOF | dbmcli -d $DBNAME -u dba,dba -uSQL dbt,dbt 2>&1
sql_execute explain $SQL
sql_execute select * from show
quit
EOF`
echo $_o
echo 

SQL="SELECT ol_i_id, ol_supply_w_id, ol_quantity, ol_amount, CHAR(ol_delivery_d, ISO) FROM dbt.order_line WHERE ol_w_id = 1 AND ol_d_id = 1 AND ol_o_id = 1"
echo $SQL
_o=`cat <<EOF | dbmcli -d $DBNAME -u dba,dba -uSQL dbt,dbt 2>&1
sql_execute explain $SQL
sql_execute select * from show
quit
EOF`
echo $_o
echo 

echo -------------
echo payment.sql
echo -------------

SQL="SELECT w_name, w_street_1, w_street_2, w_city, w_state, w_zip FROM dbt.warehouse WHERE w_id = 1"
echo $SQL
_o=`cat <<EOF | dbmcli -d $DBNAME -u dba,dba -uSQL dbt,dbt 2>&1
sql_execute explain $SQL
sql_execute select * from show
quit
EOF`
echo $_o
echo 

SQL="SELECT d_name, d_street_1, d_street_2, d_city, d_state, d_zip FROM dbt.district WHERE d_id = 1 AND d_w_id = 1"
echo $SQL
_o=`cat <<EOF | dbmcli -d $DBNAME -u dba,dba -uSQL dbt,dbt 2>&1
sql_execute explain $SQL
sql_execute select * from show
quit
EOF`
echo $_o
echo 

SQL="SELECT c_id FROM dbt.customer WHERE c_w_id = 1 AND c_d_id = 1 AND c_last = 'a' ORDER BY c_first ASC"
echo $SQL
_o=`cat <<EOF | dbmcli -d $DBNAME -u dba,dba -uSQL dbt,dbt 2>&1
sql_execute explain $SQL
sql_execute select * from show
quit
EOF`
echo $_o
echo 

SQL="SELECT c_first, c_middle, c_last, c_street_1, c_street_2, c_city, c_state, c_zip, c_phone, CHAR(c_since, ISO), c_credit, c_credit_lim, c_discount, c_balance, c_data, c_ytd_payment FROM dbt.customer WHERE c_id = 1 AND c_w_id = 1 AND c_d_id = 1"
echo $SQL
_o=`cat <<EOF | dbmcli -d $DBNAME -u dba,dba -uSQL dbt,dbt 2>&1
sql_execute explain $SQL
sql_execute select * from show
quit
EOF`
echo $_o
echo 

echo ---------------
echo stock_level.sql
echo ---------------

SQL="SELECT d_next_o_id FROM dbt.district WHERE d_w_id = 1 AND d_id = 1 WITH LOCK ISOLATION LEVEL 0"
echo $SQL
_o=`cat <<EOF | dbmcli -d $DBNAME -u dba,dba -uSQL dbt,dbt 2>&1
sql_execute explain $SQL
sql_execute select * from show
quit
EOF`
echo $_o
echo 

SQL="SELECT count(DISTINCT s_i_id) FROM dbt.order_line, dbt.stock, dbt.district WHERE d_id = 1 AND d_w_id = 1 AND d_id = ol_d_id AND d_w_id = ol_w_id AND ol_i_id = s_i_id AND ol_w_id = s_w_id AND s_quantity < 20 AND ol_o_id BETWEEN (1) AND (20) WITH LOCK ISOLATION LEVEL 0"
echo $SQL
_o=`cat <<EOF | dbmcli -d $DBNAME -u dba,dba -uSQL dbt,dbt 2>&1
sql_execute explain $SQL
sql_execute select * from show
quit
EOF`
echo $_o
echo 
