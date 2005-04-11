#!/bin/sh

#
# This file is released under the terms of the Artistic License.
# Please see # the file LICENSE, included in this package, for details.
#
# Copyright (C) 2002 Mark Wong & Open Source Development Lab, Inc.
#

DIR=`dirname $0`
. ${DIR}/pgsql_profile || exit 1

# Create tables
${PSQL} -e -d ${DBNAME} -f create_tables.sql || exit 1

# Load tables
echo "Loading customer table..."
${PSQL} -e -d ${DBNAME} -c "COPY customer FROM '$DBDATA/customer.data' WITH NULL AS '';" || exit 1
echo "Loading district table..."
${PSQL} -e -d ${DBNAME} -c "COPY district FROM '$DBDATA/district.data' WITH NULL AS '';" || exit 1
echo "Loading history table..."
${PSQL} -e -d ${DBNAME} -c "COPY history FROM '$DBDATA/history.data' WITH NULL AS '';" || exit 1
echo "Loading item table..."
${PSQL} -e -d ${DBNAME} -c "COPY item FROM '$DBDATA/item.data' WITH NULL AS '';" || exit 1
echo "Loading new_order table..."
${PSQL} -e -d ${DBNAME} -c "COPY new_order FROM '$DBDATA/new_order.data' WITH NULL AS '';" || exit 1
echo "Loading order_line table..."
${PSQL} -e -d ${DBNAME} -c "COPY order_line FROM '$DBDATA/order_line.data' WITH NULL AS '';" || exit 1
echo "Loading orders table..."
${PSQL} -e -d ${DBNAME} -c "COPY orders FROM '$DBDATA/order.data' WITH NULL AS '';" || exit 1
echo "Loading stock table..."
${PSQL} -e -d ${DBNAME} -c "COPY stock FROM '$DBDATA/stock.data' WITH NULL AS '';" || exit 1
echo "Loading warehouse table..."
${PSQL} -e -d ${DBNAME} -c "COPY warehouse FROM '$DBDATA/warehouse.data' WITH NULL AS '';" || exit 1

${SHELL} create_indexes.sh || exit 1

# load C or SQL implementation of the stored procedures
if true; then
  ${SHELL} load_stored_funcs.sh || exit 1
else
  ${SHELL} load_stored_procs.sh || exit 1
fi

${PSQL} -e -d ${DBNAME} -c "SELECT setseed(0);" || exit 1

# VACUUM FULL ANALYZE: Build optimizer statistics for newly-created
# tables. The VACUUM FULL is probably unnecessary; we want to scan the
# heap and update the commit-hint bits on each new tuple, but a regular
# VACUUM ought to suffice for that.
$VACUUMDB -z -f -d ${DBNAME} || exit 1

exit 0
