#!/bin/sh
/opt/sapdb/indep_prog/bin/dbmcli -d DBT2 -u dba,dba -uSQL dbt,dbt sql_execute update statistics warehouse
/opt/sapdb/indep_prog/bin/dbmcli -d DBT2 -u dba,dba -uSQL dbt,dbt sql_execute update statistics district
/opt/sapdb/indep_prog/bin/dbmcli -d DBT2 -u dba,dba -uSQL dbt,dbt sql_execute update statistics customer
/opt/sapdb/indep_prog/bin/dbmcli -d DBT2 -u dba,dba -uSQL dbt,dbt sql_execute update statistics history
/opt/sapdb/indep_prog/bin/dbmcli -d DBT2 -u dba,dba -uSQL dbt,dbt sql_execute update statistics new_order
/opt/sapdb/indep_prog/bin/dbmcli -d DBT2 -u dba,dba -uSQL dbt,dbt sql_execute update statistics orders
/opt/sapdb/indep_prog/bin/dbmcli -d DBT2 -u dba,dba -uSQL dbt,dbt sql_execute update statistics order_line
/opt/sapdb/indep_prog/bin/dbmcli -d DBT2 -u dba,dba -uSQL dbt,dbt sql_execute update statistics item
