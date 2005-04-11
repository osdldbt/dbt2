#!/bin/sh

#
# This file is released under the terms of the Artistic License.
# Please see the file LICENSE, included in this package, for details.
#
# Copyright (C) 2002 Mark Wong & Open Source Development Lab, Inc.
#

DIR=`dirname $0`
. ${DIR}/pgsql_profile || exit 1

${PSQL} -e -d ${DBNAME} -f $TOP_DIR/storedproc/pgsql/c/delivery.sql || exit 1
${PSQL} -e -d ${DBNAME} -f $TOP_DIR/storedproc/pgsql/c/new_order.sql || exit 1
${PSQL} -e -d ${DBNAME} -f $TOP_DIR/storedproc/pgsql/c/order_status.sql || exit 1
${PSQL} -e -d ${DBNAME} -f $TOP_DIR/storedproc/pgsql/c/payment.sql || exit 1
${PSQL} -e -d ${DBNAME} -f $TOP_DIR/storedproc/pgsql/c/stock_level.sql || exit 1

