#!/bin/sh

SID=DBT2

/opt/sapdb/depend/bin/repmcli -d $SID -u dbt,dbt -b ../../storedproc/sapdb/new_order_2.sql
/opt/sapdb/depend/bin/repmcli -d $SID -u dbt,dbt -b ../../storedproc/sapdb/new_order.sql

/opt/sapdb/depend/bin/repmcli -d $SID -u dbt,dbt -b ../../storedproc/sapdb/payment.sql

/opt/sapdb/depend/bin/repmcli -d $SID -u dbt,dbt -b ../../storedproc/sapdb/order_status.sql

/opt/sapdb/depend/bin/repmcli -d $SID -u dbt,dbt -b ../../storedproc/sapdb/delivery_2.sql
/opt/sapdb/depend/bin/repmcli -d $SID -u dbt,dbt -b ../../storedproc/sapdb/delivery.sql

/opt/sapdb/depend/bin/repmcli -d $SID -u dbt,dbt -b ../../storedproc/sapdb/stock_level.sql
