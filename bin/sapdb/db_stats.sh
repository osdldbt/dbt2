#!/bin/sh

if [ $# -ne 4 ]; then
	echo "usage: db_stats.sh <database_name> <output_dir> <iterations> <sleep>"
	exit
fi

SID=$1
OUTPUT_DIR=$2
ITERATIONS=$3
SAMPLE_LENGTH=$4

SAPDBBINDIR=/opt/sapdb/depend/bin
SAPDBBINDIR2=/opt/sapdb/indep_prog/bin
export PATH=$PATH:$SAPDBBINDIR:$SAPDBBINDIR2

COUNTER=0

# put db info into the readme.txt file
dbmcli dbm_version >> $OUTPUT_DIR/readme.txt
echo >> $OUTPUT_DIR/readme.txt

# save the database parameters
dbmcli -d $SID -u dbm,dbm -uSQL dbt,dbt -c param_extgetall | sort > $OUTPUT_DIR/param.out
read RN < .run_number
CURRENT_NUM=`expr $RN - 1`
PREV_NUM=`expr $RN - 2`

CURRENT_DIR=output/$CURRENT_NUM
PREV_DIR=output/$PREV_NUM

echo "Changed SAP DB parameters:" >> $OUTPUT_DIR/readme.txt
diff -U 0 $CURRENT_DIR/param.out $PREV_DIR/param.out >> $OUTPUT_DIR/readme.txt
echo >> $OUTPUT_DIR/readme.txt

# record data and log devspace space information before the test
dbmcli -d $SID -u dbm,dbm -uSQL dbt,dbt -c info data > $OUTPUT_DIR/datadev0.txt
dbmcli -d $SID -u dbm,dbm -uSQL dbt,dbt -c info log > $OUTPUT_DIR/logdev0.txt

# reset monitor tables
echo "resetting monitor tables"
dbmcli -s -d $1 -u dba,dba -uSQL dbt,dbt "sql_execute monitor init"

# Is the monitor init taking too much time?
date
echo "starting database statistics collection"
while [ $COUNTER -lt $ITERATIONS ]; do
	# collect x_cons output
	x_cons $1 show all >> $OUTPUT_DIR/x_cons.out
	
	# check lock statistics
	dbmcli -d $SID -u dba,dba -uSQL dbt,dbt sql_execute "SELECT * FROM LOCKSTATISTICS" >> $OUTPUT_DIR/lockstats.out

	# read the monitor tables
	dbmcli -s -d $1 -u dba,dba -uSQL dbt,dbt "sql_execute select * from monitor_caches" >> $OUTPUT_DIR/m_cache.out
	dbmcli -s -d $1 -u dba,dba -uSQL dbt,dbt "sql_execute select * from monitor_load" >> $OUTPUT_DIR/m_load.out
	dbmcli -s -d $1 -u dba,dba -uSQL dbt,dbt "sql_execute select * from monitor_lock" >> $OUTPUT_DIR/m_lock.out
	dbmcli -s -d $1 -u dba,dba -uSQL dbt,dbt "sql_execute select * from monitor_log" >> $OUTPUT_DIR/m_log.out
	dbmcli -s -d $1 -u dba,dba -uSQL dbt,dbt "sql_execute select * from monitor_pages" >> $OUTPUT_DIR/m_pages.out
	dbmcli -s -d $1 -u dba,dba -uSQL dbt,dbt "sql_execute select * from monitor_row" >> $OUTPUT_DIR/m_row.out
	dbmcli -s -d $1 -u dba,dba -uSQL dbt,dbt "sql_execute select * from monitor_trans" >> $OUTPUT_DIR/m_trans.out

	let COUNTER=COUNTER+1
	sleep $SAMPLE_LENGTH
done

# record devspace space information after the test
dbmcli -d $SID -u dbm,dbm -uSQL dbt,dbt -c info data > $OUTPUT_DIR/datadev1.txt
dbmcli -d $SID -u dbm,dbm -uSQL dbt,dbt -c info log > $OUTPUT_DIR/logdev1.txt
