#!/bin/sh

DBNAME=DBT2
SAPDBBINDIR=/opt/sapdb/depend/bin
export PATH=$PATH:$SAPDBBINDIR

set -x

dbmcli -d $DBNAME -u dba,dba -uSQL dbt,dbt -c "sql_execute create unique index i_orders on orders (o_w_id, o_d_id, o_c_id, o_id)"

dbmcli -d $DBNAME -u dba,dba -uSQL dbt,dbt -c "sql_execute create index i_customer on customer (c_w_id, c_d_id, c_last, c_first, c_id)"
