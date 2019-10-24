#!/bin/sh

SID=DBT2
SAPBINDIR=/opt/sapdb/depend/bin/
export PATH=$PATH:$SAPBINDIR

dbmcli -d $SID -u dbm,dbm -uSQL dbt,dbt sql_execute drop dbproc new_order
dbmcli -d $SID -u dbm,dbm -uSQL dbt,dbt sql_execute drop dbproc new_order_2

dbmcli -d $SID -u dbm,dbm -uSQL dbt,dbt sql_execute drop dbproc payment

dbmcli -d $SID -u dbm,dbm -uSQL dbt,dbt sql_execute drop dbproc order_status

dbmcli -d $SID -u dbm,dbm -uSQL dbt,dbt sql_execute drop dbproc delivery
dbmcli -d $SID -u dbm,dbm -uSQL dbt,dbt sql_execute drop dbproc delivery_2

dbmcli -d $SID -u dbm,dbm -uSQL dbt,dbt sql_execute drop dbproc stock_level
