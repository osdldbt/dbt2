#!/bin/bash

DIR=`dirname $0`
. ${DIR}/pgsql_profile || exit 1

$PSQL -d dbt2 -f $TOP_DIR/storedproc/pgsql/delivery.sql || exit 1
$PSQL -d dbt2 -f $TOP_DIR/storedproc/pgsql/new_order_2.sql || exit 1
$PSQL -d dbt2 -f $TOP_DIR/storedproc/pgsql/new_order.sql || exit 1
$PSQL -d dbt2 -f $TOP_DIR/storedproc/pgsql/order_status.sql || exit 1
$PSQL -d dbt2 -f $TOP_DIR/storedproc/pgsql/payment.sql || exit 1
$PSQL -d dbt2 -f $TOP_DIR/storedproc/pgsql/stock_level.sql || exit 1

