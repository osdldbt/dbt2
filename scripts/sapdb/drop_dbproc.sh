#!/bin/sh

SID=DBT2

/opt/sapdb/depend/bin/dbmcli -d $SID -u dbm,dbm -uSQL dbt,dbt sql_execute drop dbproc new_order
/opt/sapdb/depend/bin/dbmcli -d $SID -u dbm,dbm -uSQL dbt,dbt sql_execute drop dbproc new_order_2

/opt/sapdb/depend/bin/dbmcli -d $SID -u dbm,dbm -uSQL dbt,dbt sql_execute drop dbproc payment

/opt/sapdb/depend/bin/dbmcli -d $SID -u dbm,dbm -uSQL dbt,dbt sql_execute drop dbproc order_status

/opt/sapdb/depend/bin/dbmcli -d $SID -u dbm,dbm -uSQL dbt,dbt sql_execute drop dbproc delivery
/opt/sapdb/depend/bin/dbmcli -d $SID -u dbm,dbm -uSQL dbt,dbt sql_execute drop dbproc delivery_2

/opt/sapdb/depend/bin/dbmcli -d $SID -u dbm,dbm -uSQL dbt,dbt sql_execute drop dbproc stock_level
/opt/sapdb/depend/bin/dbmcli -d $SID -u dbm,dbm -uSQL dbt,dbt sql_execute drop dbproc stock_level_2
