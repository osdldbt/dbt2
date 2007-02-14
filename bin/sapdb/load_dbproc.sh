#!/bin/sh

SID=DBT2
SAPDBBINDIR=/opt/sapdb/depend/bin
export PATH=$PATH:$SAPDBBINDIR

repmcli -d $SID -u dbt,dbt -b ../../storedproc/sapdb/new_order_2.sql
repmcli -d $SID -u dbt,dbt -b ../../storedproc/sapdb/new_order.sql

repmcli -d $SID -u dbt,dbt -b ../../storedproc/sapdb/payment.sql

repmcli -d $SID -u dbt,dbt -b ../../storedproc/sapdb/order_status.sql

repmcli -d $SID -u dbt,dbt -b ../../storedproc/sapdb/delivery_2.sql
repmcli -d $SID -u dbt,dbt -b ../../storedproc/sapdb/delivery.sql

repmcli -d $SID -u dbt,dbt -b ../../storedproc/sapdb/stock_level.sql
