#!/bin/sh

. ./init_env.sh

psql -d $DB_NAME -c "drop table customer;"
psql -d $DB_NAME -c "drop table district;"
psql -d $DB_NAME -c "drop table history;"
psql -d $DB_NAME -c "drop table item;"
psql -d $DB_NAME -c "drop table new_order;"
psql -d $DB_NAME -c "drop table order_line;"
psql -d $DB_NAME -c "drop table orders;"
psql -d $DB_NAME -c "drop table stock;"
psql -d $DB_NAME -c "drop table warehouse;"
