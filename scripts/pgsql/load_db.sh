#!/bin/sh

# load_db.sh
#
# This file is released under the terms of the Artistic License.  Please see
# the file LICENSE, included in this package, for details.
#
# Copyright (C) 2002 Mark Wong & Open Source Development Lab, Inc.
#
# 01 May 2003

DIR=`dirname $0`
. ${DIR}/init_env.sh || exit 1

# Create tables
$PSQL -d $DB_NAME -f create_tables.sql || exit 1

# Load tables
echo "Loading customer table..."
$PSQL -d $DB_NAME -c "COPY customer FROM '$DBDATA/customer.data' WITH NULL AS '';" || exit 1
echo "Loading district table..."
$PSQL -d $DB_NAME -c "COPY district FROM '$DBDATA/district.data' WITH NULL AS '';" || exit 1
echo "Loading history table..."
$PSQL -d $DB_NAME -c "COPY history FROM '$DBDATA/history.data' WITH NULL AS '';" || exit 1
echo "Loading item table..."
$PSQL -d $DB_NAME -c "COPY item FROM '$DBDATA/item.data' WITH NULL AS '';" || exit 1
echo "Loading new_order table..."
$PSQL -d $DB_NAME -c "COPY new_order FROM '$DBDATA/new_order.data' WITH NULL AS '';" || exit 1
echo "Loading order_line table..."
$PSQL -d $DB_NAME -c "COPY order_line FROM '$DBDATA/order_line.data' WITH NULL AS '';" || exit 1
echo "Loading orders table..."
$PSQL -d $DB_NAME -c "COPY orders FROM '$DBDATA/order.data' WITH NULL AS '';" || exit 1
echo "Loading stock table..."
$PSQL -d $DB_NAME -c "COPY stock FROM '$DBDATA/stock.data' WITH NULL AS '';" || exit 1
echo "Loading warehouse table..."
$PSQL -d $DB_NAME -c "COPY warehouse FROM '$DBDATA/warehouse.data' WITH NULL AS '';" || exit 1

bash create_indexes.sh || exit 1

# load C or SQL implementation of the stored procedures
if true; then
  bash load_stored_funcs.sh || exit 1
else
  bash load_stored_procs.sh || exit 1
fi

$PSQL -d $DB_NAME -c "SELECT setseed(0);" || exit 1

# VACUUM FULL ANALYZE: Build optimizer statistics for newly-created
# tables. The VACUUM FULL is probably unnecessary; we want to scan the
# heap and update the commit-hint bits on each new tuple, but a regular
# VACUUM ought to suffice for that.
$VACUUMDB -z -f -d $DB_NAME || exit 1

