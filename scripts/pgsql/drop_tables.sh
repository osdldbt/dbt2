#!/bin/sh

#
# This file is released under the terms of the Artistic License.
# Please see the file LICENSE, included in this package, for details.
#
# Copyright (C) 2002 Mark Wong & Open Source Development Lab, Inc.
#

DIR=`dirname $0`
. ${DIR}/pgsql_profile || exit 1

${PSQL} -d ${DBNAME} -c "DROP TABLE customer;"
${PSQL} -d ${DBNAME} -c "DROP TABLE district;"
${PSQL} -d ${DBNAME} -c "DROP TABLE history;"
${PSQL} -d ${DBNAME} -c "DROP TABLE item;"
${PSQL} -d ${DBNAME} -c "DROP TABLE new_order;"
${PSQL} -d ${DBNAME} -c "DROP TABLE order_line;"
${PSQL} -d ${DBNAME} -c "DROP TABLE orders;"
${PSQL} -d ${DBNAME} -c "DROP TABLE stock;"
${PSQL} -d ${DBNAME} -c "DROP TABLE warehouse;"
