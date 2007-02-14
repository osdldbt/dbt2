#!/bin/sh

SID=DBT2
SAPDBBINDIR=/opt/sapdb/depend/bin
export PATH=$PATH:$SAPDBBINDIR

set -x

dbmcli -d $SID -u dba,dba -uSQL dbt,dbt sql_execute update statistics warehouse
dbmcli -d $SID -u dba,dba -uSQL dbt,dbt sql_execute update statistics district
dbmcli -d $SID -u dba,dba -uSQL dbt,dbt sql_execute update statistics customer
dbmcli -d $SID -u dba,dba -uSQL dbt,dbt sql_execute update statistics history
dbmcli -d $SID -u dba,dba -uSQL dbt,dbt sql_execute update statistics new_order
dbmcli -d $SID -u dba,dba -uSQL dbt,dbt sql_execute update statistics orders
dbmcli -d $SID -u dba,dba -uSQL dbt,dbt sql_execute update statistics order_line
dbmcli -d $SID -u dba,dba -uSQL dbt,dbt sql_execute update statistics item
dbmcli -d $SID -u dba,dba -uSQL dbt,dbt sql_execute update statistics stock
