#!/bin/bash

DIR=`dirname $0`
. ${DIR}/pgsql_profile || exit 1

$PSQL -d $DB_NAME -c "DROP TABLE customer;"
$PSQL -d $DB_NAME -c "DROP TABLE district;"
$PSQL -d $DB_NAME -c "DROP TABLE history;"
$PSQL -d $DB_NAME -c "DROP TABLE item;"
$PSQL -d $DB_NAME -c "DROP TABLE new_order;"
$PSQL -d $DB_NAME -c "DROP TABLE order_line;"
$PSQL -d $DB_NAME -c "DROP TABLE orders;"
$PSQL -d $DB_NAME -c "DROP TABLE stock;"
$PSQL -d $DB_NAME -c "DROP TABLE warehouse;"
