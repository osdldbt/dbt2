#!/bin/sh

if [ $# -ne 4 ]; then
	echo "usage: db_stats.sh <database_name> <output_dir> <iterations> <sleep>"
	exit
fi

OUTPUT_DIR=$2
ITERATIONS=$3
SAMPLE_LENGTH=$4

COUNTER=0

# put db info into the readme.txt file
/opt/sapdb/depend/bin/dbmcli dbm_version >> $OUTPUT_DIR/readme.txt
echo >> $OUTPUT_DIR/readme.txt

# save the database parameters
/opt/sapdb/depend/bin/dbmcli -d DBT2 -u dbm,dbm -uSQL dbt,dbt -c param_extgetall | sort > $OUTPUT_DIR/param.out
read RN < .run_number
CURRENT_NUM=`expr $RN - 1`
PREV_NUM=`expr $RN - 2`

CURRENT_DIR=output/$CURRENT_NUM
PREV_DIR=output/$PREV_NUM

echo "Changed SAP DB parameters:" >> $OUTPUT_DIR/readme.txt
diff -U 0 $CURRENT_DIR/param.out $PREV_DIR/param.out >> $OUTPUT_DIR/readme.txt
echo >> $OUTPUT_DIR/readme.txt

# record data and log devspace space information before the test
/opt/sapdb/depend/bin/dbmcli -d DBT2 -u dbm,dbm -uSQL dbt,dbt -c info data > $OUTPUT_DIR/datadev0.txt
/opt/sapdb/depend/bin/dbmcli -d DBT2 -u dbm,dbm -uSQL dbt,dbt -c info log > $OUTPUT_DIR/logdev0.txt

# reset monitor tables
echo "resetting monitor tables"
/opt/sapdb/depend/bin/dbmcli -s -d $1 -u dba,dba -uSQL dbt,dbt "sql_execute monitor init"

# Is the monitor init taking too much time?
date
echo "starting database statistics collection"
while [ $COUNTER -lt $ITERATIONS ]; do
	# collect x_cons output
	/opt/sapdb/indep_prog/bin/x_cons $1 show all >> $OUTPUT_DIR/x_cons.out
	
	# check lock statistics
	/opt/sapdb/indep_prog/bin/dbmcli -d DBT2 -u dba,dba -uSQL dbt,dbt sql_execute "SELECT * FROM LOCKSTATISTICS" >> $OUTPUT_DIR/lockstats.out

	# read the monitor tables
	/opt/sapdb/depend/bin/dbmcli -s -d $1 -u dba,dba -uSQL dbt,dbt "sql_execute select * from monitor_caches" >> $OUTPUT_DIR/m_cache.out
	/opt/sapdb/depend/bin/dbmcli -s -d $1 -u dba,dba -uSQL dbt,dbt "sql_execute select * from monitor_load" >> $OUTPUT_DIR/m_load.out
	/opt/sapdb/depend/bin/dbmcli -s -d $1 -u dba,dba -uSQL dbt,dbt "sql_execute select * from monitor_lock" >> $OUTPUT_DIR/m_lock.out
	/opt/sapdb/depend/bin/dbmcli -s -d $1 -u dba,dba -uSQL dbt,dbt "sql_execute select * from monitor_log" >> $OUTPUT_DIR/m_log.out
	/opt/sapdb/depend/bin/dbmcli -s -d $1 -u dba,dba -uSQL dbt,dbt "sql_execute select * from monitor_pages" >> $OUTPUT_DIR/m_pages.out
	/opt/sapdb/depend/bin/dbmcli -s -d $1 -u dba,dba -uSQL dbt,dbt "sql_execute select * from monitor_row" >> $OUTPUT_DIR/m_row.out
	/opt/sapdb/depend/bin/dbmcli -s -d $1 -u dba,dba -uSQL dbt,dbt "sql_execute select * from monitor_trans" >> $OUTPUT_DIR/m_trans.out

	let COUNTER=COUNTER+1
	sleep $SAMPLE_LENGTH
done

# record devspace space information after the test
/opt/sapdb/depend/bin/dbmcli -d DBT2 -u dbm,dbm -uSQL dbt,dbt -c info data > $OUTPUT_DIR/datadev1.txt
/opt/sapdb/depend/bin/dbmcli -d DBT2 -u dbm,dbm -uSQL dbt,dbt -c info log > $OUTPUT_DIR/logdev1.txt
