#!/bin/sh
/opt/sapdb/depend/bin/dbmcli -d DBT2 -u dbm,dbm db_cold
/opt/sapdb/depend/bin/dbmcli -d DBT2 -u dbm,dbm db_offline
