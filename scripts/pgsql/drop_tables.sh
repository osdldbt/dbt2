#!/bin/sh

DIR=`dirname $0`
. ${DIR}/init_env.sh || exit

$PSQL -d $DB_NAME -c "drop table customer;"
$PSQL -d $DB_NAME -c "drop table district;"
$PSQL -d $DB_NAME -c "drop table history;"
$PSQL -d $DB_NAME -c "drop table item;"
$PSQL -d $DB_NAME -c "drop table new_order;"
$PSQL -d $DB_NAME -c "drop table order_line;"
$PSQL -d $DB_NAME -c "drop table orders;"
$PSQL -d $DB_NAME -c "drop table stock;"
$PSQL -d $DB_NAME -c "drop table warehouse;"
