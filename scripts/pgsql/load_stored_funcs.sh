#!/bin/sh

DIR=`dirname $0`
. ${DIR}/pgsql_profile || exit 1

$PSQL -d dbt2 -f $TOP_DIR/storedproc/pgsql/c/delivery.sql || exit 1
$PSQL -d dbt2 -f $TOP_DIR/storedproc/pgsql/c/new_order.sql || exit 1
$PSQL -d dbt2 -f $TOP_DIR/storedproc/pgsql/c/order_status.sql || exit 1
$PSQL -d dbt2 -f $TOP_DIR/storedproc/pgsql/c/payment.sql || exit 1
$PSQL -d dbt2 -f $TOP_DIR/storedproc/pgsql/c/stock_level.sql || exit 1

