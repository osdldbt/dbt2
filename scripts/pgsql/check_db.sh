#!/bin/sh

#
# This file is released under the terms of the Artistic License.
# Please see the file LICENSE, included in this package, for details.
#
# Copyright (C) 2002 Mark Wong & Open Source Development Lab, Inc.
#

DIR=`dirname $0`
. ${DIR}/pgsql_profile || exit 1

# Load tables
echo customer
${PSQL} -d ${DBNAME} -c "select count(*) from customer"
echo district
${PSQL} -d ${DBNAME} -c "select count(*) from district"
echo history 
${PSQL} -d ${DBNAME} -c "select count(*) from history"
echo item    
${PSQL} -d ${DBNAME} -c "select count(*) from item"
echo new_order
${PSQL} -d ${DBNAME} -c "select count(*) from new_order"
echo order_line
${PSQL} -d ${DBNAME} -c "select count(*) from order_line"
echo orders  
${PSQL} -d ${DBNAME} -c "select count(*) from orders"
echo stock   
${PSQL} -d ${DBNAME} -c "select count(*) from stock"
echo warehouse
${PSQL} -d ${DBNAME} -c "select count(*) from warehouse"
