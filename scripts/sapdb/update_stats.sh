#!/bin/sh
/opt/sapdb/depend74/bin/dbmcli -d DBT2 -u dba,dba -uSQL dbt,dbt sql_execute update statistics warehouse
/opt/sapdb/depend74/bin/dbmcli -d DBT2 -u dba,dba -uSQL dbt,dbt sql_execute update statistics district
/opt/sapdb/depend74/bin/dbmcli -d DBT2 -u dba,dba -uSQL dbt,dbt sql_execute update statistics customer
/opt/sapdb/depend74/bin/dbmcli -d DBT2 -u dba,dba -uSQL dbt,dbt sql_execute update statistics history
/opt/sapdb/depend74/bin/dbmcli -d DBT2 -u dba,dba -uSQL dbt,dbt sql_execute update statistics new_order
/opt/sapdb/depend74/bin/dbmcli -d DBT2 -u dba,dba -uSQL dbt,dbt sql_execute update statistics orders
/opt/sapdb/depend74/bin/dbmcli -d DBT2 -u dba,dba -uSQL dbt,dbt sql_execute update statistics order_line
/opt/sapdb/depend74/bin/dbmcli -d DBT2 -u dba,dba -uSQL dbt,dbt sql_execute update statistics item
/opt/sapdb/depend74/bin/dbmcli -d DBT2 -u dba,dba -uSQL dbt,dbt sql_execute update statistics stock
