#!/bin/sh

# init_env.sh
#
# This file is released under the terms of the Artistic License.  Please see
# the file LICENSE, included in this package, for details.
#
# Copyright (C) 2002 Mark Wong & Open Source Development Lab, Inc.
#
# 01 May 2003

. ./init_env.sh

# Load tables
echo customer
psql -d $DB_NAME -c "select count(*) from customer"
echo district
psql -d $DB_NAME -c "select count(*) from district"
echo history 
psql -d $DB_NAME -c "select count(*) from history"
echo item    
psql -d $DB_NAME -c "select count(*) from item"
echo new_order
psql -d $DB_NAME -c "select count(*) from new_order"
echo order_line
psql -d $DB_NAME -c "select count(*) from order_line"
echo orders  
psql -d $DB_NAME -c "select count(*) from orders"
echo stock   
psql -d $DB_NAME -c "select count(*) from stock"
echo warehouse
psql -d $DB_NAME -c "select count(*) from warehouse"
