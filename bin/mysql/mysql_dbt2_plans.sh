#!/bin/sh

usage() {

  if [ "$1" != "" ]; then
    echo ''
    echo "error: $1"
  fi

  echo ''
  echo 'usage: mysql_dbt2_plans.sh [options]'
  echo 'options:'
  echo '       -d <database name>'
  echo '       -o <output file name>'
  echo '       -c <path to mysql client binary. (default: /usr/bin/mysql)>'
  echo '       -s <database socket>'
  echo '       -h <database host (default: localhost>'
  echo ''
  echo 'Example: sh mysql_dbt2_plans.sh -d dbt2 -o explain.out'
  echo ''
}


while getopts "o:d:c:s:h:" opt; do
	case $opt in
	o)
		OUTPUT_FILE=$OPTARG
		;;
	d)
		DB_NAME=$OPTARG
		;;
        c)
                MYSQL=$OPTARG
                ;;
        s)
                DB_SOCKET=$OPTARG
                ;;
        h)
                DB_HOST=$OPTARG
                ;;
        ?)
                usage
                exit 1
                ;;
	esac
done

if [ "$OUTPUT_FILE" == "" ]; then
   usage "specify ouput filename -o <outfile>"
   exit 1
fi

if [ "$MYSQL" == "" ]; then
  MYSQL="/usr/bin/mysql"
fi

if [ "$DB_SOCKET" == "" ]; then
  DB_SOCKET="/tmp/mysql.sock"
fi

if [ "$DB_HOST" == "" ]; then
  DB_HOST="localhost"
fi

if [ "$DB_NAME" == "" ]; then
  usage "specify database name using -d #"
  exit 1
fi


MYSQL="$MYSQL -vvv"

echo "--------------------" >> $OUTPUT_FILE
echo "Delivery Transaction" >> $OUTPUT_FILE
echo "--------------------" >> $OUTPUT_FILE

SQL="EXPLAIN SELECT no_o_id FROM new_order WHERE no_w_id = 1 AND no_d_id = 1;"
#echo "$SQL" >> $OUTPUT_FILE
$MYSQL --socket=$DB_SOCKET  $DB_NAME -e "$SQL" >> $OUTPUT_FILE

#SQL="EXPLAIN DELETE FROM new_order WHERE no_o_id = 1 AND no_w_id = 1 AND no_d_id = 1;"
##echo "$SQL" >> $OUTPUT_FILE
#$MYSQL --socket=$DB_SOCKET  $DB_NAME -e "$SQL" >> $OUTPUT_FILE

SQL="EXPLAIN SELECT o_c_id FROM orders WHERE o_id = 1 AND o_w_id = 1 AND o_d_id = 1;"
#echo "$SQL" >> $OUTPUT_FILE
$MYSQL --socket=$DB_SOCKET  $DB_NAME -e "$SQL" >> $OUTPUT_FILE

#SQL="EXPLAIN UPDATE orders SET o_carrier_id = 1 WHERE o_id = 1 AND o_w_id = 1 AND o_d_id = 1;"
##echo "$SQL" >> $OUTPUT_FILE
#$MYSQL --socket=$DB_SOCKET  $DB_NAME -e "$SQL" >> $OUTPUT_FILE

#SQL="EXPLAIN UPDATE order_line SET ol_delivery_d = current_timestamp WHERE ol_o_id = 1 AND ol_w_id = 1 AND ol_d_id = 1;"
##echo "$SQL" >> $OUTPUT_FILE
#$MYSQL --socket=$DB_SOCKET  $DB_NAME -e "$SQL" >> $OUTPUT_FILE

SQL="EXPLAIN SELECT SUM(ol_amount * ol_quantity) FROM order_line WHERE ol_o_id = 1 AND ol_w_id = 1 AND ol_d_id = 1;"
#echo "$SQL" >> $OUTPUT_FILE
$MYSQL --socket=$DB_SOCKET  $DB_NAME -e "$SQL" >> $OUTPUT_FILE

#SQL="EXPLAIN UPDATE customer SET c_delivery_cnt = c_delivery_cnt + 1, c_balance = c_balance + 1 WHERE c_id = 1 AND c_w_id = 1 AND c_d_id = 1;"
##echo "$SQL" >> $OUTPUT_FILE
#$MYSQL --socket=$DB_SOCKET  $DB_NAME -e "$SQL" >> $OUTPUT_FILE

echo "---------------------" >> $OUTPUT_FILE
echo "New-Order Transaction" >> $OUTPUT_FILE
echo "---------------------" >> $OUTPUT_FILE

SQL="EXPLAIN SELECT w_tax FROM warehouse WHERE w_id = 1;"
#echo "$SQL" >> $OUTPUT_FILE
$MYSQL --socket=$DB_SOCKET  $DB_NAME -e "$SQL" >> $OUTPUT_FILE

SQL="EXPLAIN SELECT d_tax, d_next_o_id FROM district WHERE d_w_id = 1 AND d_id = 1;"
#echo "$SQL" >> $OUTPUT_FILE
$MYSQL --socket=$DB_SOCKET  $DB_NAME -e "$SQL" >> $OUTPUT_FILE

#SQL="EXPLAIN UPDATE district SET d_next_o_id = d_next_o_id + 1 WHERE d_w_id = 1 AND d_id = 1;"
##echo "$SQL" >> $OUTPUT_FILE
#$MYSQL --socket=$DB_SOCKET  $DB_NAME -e "$SQL" >> $OUTPUT_FILE

SQL="EXPLAIN SELECT c_discount, c_last, c_credit FROM customer WHERE c_w_id = 1 AND c_d_id = 1 AND c_id = 1;"
#echo "$SQL" >> $OUTPUT_FILE
$MYSQL --socket=$DB_SOCKET  $DB_NAME -e "$SQL" >> $OUTPUT_FILE

#SQL="EXPLAIN INSERT INTO new_order (no_o_id, no_d_id, no_w_id) VALUES (-1, 1, 1);"
##echo "$SQL" >> $OUTPUT_FILE
#$MYSQL --socket=$DB_SOCKET  $DB_NAME -e "$SQL" >> $OUTPUT_FILE

#SQL="EXPLAIN INSERT INTO orders (o_id, o_d_id, o_w_id, o_c_id, o_entry_d, o_carrier_id, o_ol_cnt, o_all_local) VALUES (-1, 1, 1, 1, current_timestamp, NULL, 1, 1);"
##echo "$SQL" >> $OUTPUT_FILE
#$MYSQL --socket=$DB_SOCKET  $DB_NAME -e "$SQL" >> $OUTPUT_FILE

SQL="EXPLAIN SELECT i_price, i_name, i_data FROM item WHERE i_id = 1;"
#echo "$SQL" >> $OUTPUT_FILE
$MYSQL --socket=$DB_SOCKET  $DB_NAME -e "$SQL" >> $OUTPUT_FILE

SQL="EXPLAIN SELECT s_quantity, s_dist_01, s_data FROM stock WHERE s_i_id = 1 AND s_w_id = 1;"
#echo "$SQL" >> $OUTPUT_FILE
$MYSQL --socket=$DB_SOCKET  $DB_NAME -e "$SQL" >> $OUTPUT_FILE

#SQL="EXPLAIN UPDATE stock SET s_quantity = s_quantity - 10 WHERE s_i_id = 1 AND s_w_id = 1;"
##echo "$SQL" >> $OUTPUT_FILE
#$MYSQL --socket=$DB_SOCKET  $DB_NAME -e "$SQL" >> $OUTPUT_FILE

#SQL="EXPLAIN INSERT INTO order_line (ol_o_id, ol_d_id, ol_w_id, ol_number, ol_i_id, ol_supply_w_id, ol_delivery_d, ol_quantity, ol_amount, ol_dist_info) VALUES (-1, 1, 1, 1, 1, 1, NULL, 1, 1.0, 'hello kitty');" >> $OUTPUT_FILE
##echo "$SQL" >> $OUTPUT_FILE
#$MYSQL --socket=$DB_SOCKET  $DB_NAME -e "$SQL" >> $OUTPUT_FILE

echo "------------------------" >> $OUTPUT_FILE
echo "Order-Status Transaction" >> $OUTPUT_FILE
echo "------------------------" >> $OUTPUT_FILE

SQL="EXPLAIN SELECT c_id FROM customer WHERE c_w_id = 1 AND c_d_id = 1 AND c_last = 'BARBARBAR' ORDER BY c_first ASC;"
#echo "$SQL" >> $OUTPUT_FILE
$MYSQL --socket=$DB_SOCKET  $DB_NAME -e "$SQL" >> $OUTPUT_FILE

SQL="EXPLAIN SELECT c_first, c_middle, c_last, c_balance FROM customer WHERE c_w_id = 1 AND c_d_id = 1 AND c_id = 1;"
#echo "$SQL" >> $OUTPUT_FILE
$MYSQL --socket=$DB_SOCKET  $DB_NAME -e "$SQL" >> $OUTPUT_FILE

SQL="EXPLAIN SELECT o_id, o_carrier_id, o_entry_d, o_ol_cnt FROM orders WHERE o_w_id = 1 AND o_d_id = 1 AND o_c_id = 1 ORDER BY o_id DESC;"
#echo "$SQL" >> $OUTPUT_FILE
$MYSQL --socket=$DB_SOCKET  $DB_NAME -e "$SQL" >> $OUTPUT_FILE

SQL="EXPLAIN SELECT ol_i_id, ol_supply_w_id, ol_quantity, ol_amount, ol_delivery_d FROM order_line WHERE ol_w_id = 1 AND ol_d_id = 1 AND ol_o_id = 1;"
#echo "$SQL" >> $OUTPUT_FILE
$MYSQL --socket=$DB_SOCKET  $DB_NAME -e "$SQL" >> $OUTPUT_FILE

echo "-------------------" >> $OUTPUT_FILE
echo "Payment Transaction" >> $OUTPUT_FILE
echo "-------------------" >> $OUTPUT_FILE

SQL="EXPLAIN SELECT w_name, w_street_1, w_street_2, w_city, w_state, w_zip FROM warehouse WHERE w_id = 1;"
#echo "$SQL" >> $OUTPUT_FILE
$MYSQL --socket=$DB_SOCKET  $DB_NAME -e "$SQL" >> $OUTPUT_FILE

#SQL="EXPLAIN UPDATE warehouse SET w_ytd = w_ytd + 1.0 WHERE w_id = 1;"
##echo "$SQL" >> $OUTPUT_FILE
#$MYSQL --socket=$DB_SOCKET  $DB_NAME -e "$SQL" >> $OUTPUT_FILE

SQL="EXPLAIN SELECT d_name, d_street_1, d_street_2, d_city, d_state, d_zip FROM district WHERE d_id = 1 AND d_w_id = 1;"
#echo "$SQL" >> $OUTPUT_FILE
$MYSQL --socket=$DB_SOCKET  $DB_NAME -e "$SQL" >> $OUTPUT_FILE

#SQL="EXPLAIN UPDATE district SET d_ytd = d_ytd + 1.0 WHERE d_id = 1 AND d_w_id = 1;"
##echo "$SQL" >> $OUTPUT_FILE
#$MYSQL --socket=$DB_SOCKET  $DB_NAME -e "$SQL" >> $OUTPUT_FILE

SQL="EXPLAIN SELECT c_id FROM customer WHERE c_w_id = 1 AND c_d_id = 1 AND c_last = 'BARBARBAR' ORDER BY c_first ASC;"
#echo "$SQL" >> $OUTPUT_FILE
$MYSQL --socket=$DB_SOCKET  $DB_NAME -e "$SQL" >> $OUTPUT_FILE

SQL="EXPLAIN SELECT c_first, c_middle, c_last, c_street_1, c_street_2, c_city, c_state, c_zip, c_phone, c_since, c_credit, c_credit_lim, c_discount, c_balance, c_data, c_ytd_payment FROM customer WHERE c_w_id = 1 AND c_d_id = 1 AND c_id = 1;"
#echo "$SQL" >> $OUTPUT_FILE
$MYSQL --socket=$DB_SOCKET  $DB_NAME -e "$SQL" >> $OUTPUT_FILE

#SQL="EXPLAIN UPDATE customer SET c_balance = c_balance - 1.0, c_ytd_payment = c_ytd_payment + 1 WHERE c_id = 1 AND c_w_id = 1 AND c_d_id = 1;"
##echo "$SQL" >> $OUTPUT_FILE
#$MYSQL --socket=$DB_SOCKET  $DB_NAME -e "$SQL" >> $OUTPUT_FILE

#SQL="EXPLAIN UPDATE customer SET c_balance = c_balance - 1.0, c_ytd_payment = c_ytd_payment + 1, c_data = 'hello dogger' WHERE c_id = 1 AND c_w_id = 1 AND c_d_id = 1;"
##echo "$SQL" >> $OUTPUT_FILE
#$MYSQL --socket=$DB_SOCKET  $DB_NAME -e "$SQL" >> $OUTPUT_FILE

#SQL="EXPLAIN INSERT INTO history (h_c_id, h_c_d_id, h_c_w_id, h_d_id, h_w_id, h_date, h_amount, h_data) VALUES (1, 1, 1, 1, 1, current_timestamp, 1.0, 'ab    cd');"
##echo "$SQL" >> $OUTPUT_FILE
#$MYSQL --socket=$DB_SOCKET  $DB_NAME -e "$SQL" >> $OUTPUT_FILE

echo "-----------------------" >> $OUTPUT_FILE
echo "Stock-Level Transaction" >> $OUTPUT_FILE
echo "-----------------------" >> $OUTPUT_FILE

SQL="EXPLAIN SELECT d_next_o_id FROM district WHERE d_w_id = 1 AND d_id = 1;"
#echo "$SQL" >> $OUTPUT_FILE
$MYSQL --socket=$DB_SOCKET  $DB_NAME -e "$SQL" >> $OUTPUT_FILE

SQL="EXPLAIN SELECT count(*) FROM order_line, stock, district WHERE d_id = 1 AND d_w_id = 1 AND d_id = ol_d_id AND d_w_id = ol_w_id AND ol_i_id = s_i_id AND ol_w_id = s_w_id AND s_quantity < 15 AND ol_o_id BETWEEN (1) AND (20);"
#echo "$SQL" >> $OUTPUT_FILE
$MYSQL --socket=$DB_SOCKET  $DB_NAME -e "$SQL" >> $OUTPUT_FILE
