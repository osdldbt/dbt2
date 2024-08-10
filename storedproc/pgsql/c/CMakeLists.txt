install (FILES dbt2--0.45.0.sql
               dbt2common.h
               dbt2.control
               delivery.c
               Makefile
               new_order.c
               order_status.c
               payment.c
               stock_level.c
         DESTINATION "src/dbt2/storedproc/pgsql/c")
