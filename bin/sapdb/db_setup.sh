#!/bin/sh

SAPDBBINDIR=/opt/sapdb/depend/bin
WAREHOUSES=$1
OUTDIR=$2
SID=DBT2
export PATH=$PATH:$SAPDBBINDIR

if [ $# -ne 2 ] && [ $# -ne 0 ]; then
	echo 'usage: db_setup_sample.sh [ <warehouses> <datadir> ]'
	echo "	warehouses - Number of warehouses to generate."
	echo "	datadir    - Directory to generate data files."
fi

if [ $# -gt 0 ]; then
	echo This is a sample script to create a database with $WAREHOUSES warehouses.
	echo
	echo Generating data...
	cd ../../datagen
	./datagen --sapdb -w $WAREHOUSES -d $OUTDIR
	cd -
	echo
fi

echo Creating the database dev spaces...
./create_db_sample.sh
echo

echo Creating the tables...
dbmcli -d $SID -u dbm,dbm -uSQL dbt,dbt -i create_tables.sql
echo

echo Loading the database...
repmcli -u dbt,dbt -d $SID -b warehouse.sql
repmcli -u dbt,dbt -d $SID -b district.sql
repmcli -u dbt,dbt -d $SID -b customer.sql
repmcli -u dbt,dbt -d $SID -b history.sql
repmcli -u dbt,dbt -d $SID -b new_order.sql
repmcli -u dbt,dbt -d $SID -b orders.sql
repmcli -u dbt,dbt -d $SID -b order_line.sql
repmcli -u dbt,dbt -d $SID -b item.sql
repmcli -u dbt,dbt -d $SID -b stock.sql
echo

echo Creating indexes...
./create_indexes.sh
echo

#echo Loading TABLESTATISTICS and extracting table and sizing information
#cd ./db_create_stats
#bash get_it_all.sh > TABLE_SIZING_INFO.txt 2>&1
#cd -

echo Update table statistics
./update_stats.sh
echo

echo Loading stored procedures...
./load_dbproc.sh
echo

echo Backing up database...
./backup_db.sh

echo Database setup is complete.
