#!/bin/sh

WAREHOUSES=1
SID=DBT2

echo This is a sample script to create a database with $WAREHOUSES warehouses.
echo

echo Generating data...
cd ../../datagen
./datagen -w $WAREHOUSES
cd -
echo

echo Creating the database dev spaces...
./create_db_sample.sh
echo

echo Creating the tables...
/opt/sapdb/depend/bin/dbmcli -d $SID -u dbm,dbm -uSQL dbt,dbt -i create_tables.sql
echo

echo Loading the database...
/opt/sapdb/depend/bin/repmcli -u dbt,dbt -d $SID -b warehouse.sql
/opt/sapdb/depend/bin/repmcli -u dbt,dbt -d $SID -b district.sql
/opt/sapdb/depend/bin/repmcli -u dbt,dbt -d $SID -b customer.sql
/opt/sapdb/depend/bin/repmcli -u dbt,dbt -d $SID -b history.sql
/opt/sapdb/depend/bin/repmcli -u dbt,dbt -d $SID -b new_order.sql
/opt/sapdb/depend/bin/repmcli -u dbt,dbt -d $SID -b orders.sql
/opt/sapdb/depend/bin/repmcli -u dbt,dbt -d $SID -b order_line.sql
/opt/sapdb/depend/bin/repmcli -u dbt,dbt -d $SID -b item.sql
/opt/sapdb/depend/bin/repmcli -u dbt,dbt -d $SID -b stock.sql
echo

echo Creating indexes...
./create_indexes.sh
echo

echo Loading stored procedures...
./load_dbproc.sh
echo

echo Backing up database...
./backup_db.sh
