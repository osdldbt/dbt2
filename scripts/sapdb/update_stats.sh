#!/bin/sh

SAPDBBINDIR=/opt/sapdb/depend/bin

$SAPDBBINDIR/dbmcli -d DBT2 -u dba,dba -uSQL dbt,dbt sql_execute update statistics warehouse
$SAPDBBINDIR/dbmcli -d DBT2 -u dba,dba -uSQL dbt,dbt sql_execute update statistics district
$SAPDBBINDIR/dbmcli -d DBT2 -u dba,dba -uSQL dbt,dbt sql_execute update statistics customer
$SAPDBBINDIR/dbmcli -d DBT2 -u dba,dba -uSQL dbt,dbt sql_execute update statistics history
$SAPDBBINDIR/dbmcli -d DBT2 -u dba,dba -uSQL dbt,dbt sql_execute update statistics new_order
$SAPDBBINDIR/dbmcli -d DBT2 -u dba,dba -uSQL dbt,dbt sql_execute update statistics orders
$SAPDBBINDIR/dbmcli -d DBT2 -u dba,dba -uSQL dbt,dbt sql_execute update statistics order_line
$SAPDBBINDIR/dbmcli -d DBT2 -u dba,dba -uSQL dbt,dbt sql_execute update statistics item
$SAPDBBINDIR/dbmcli -d DBT2 -u dba,dba -uSQL dbt,dbt sql_execute update statistics stock
