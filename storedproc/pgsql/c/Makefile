MODULE_big = dbt2
OBJS = delivery.o new_order.o order_status.o payment.o stock_level.o

EXTENSION = dbt2
DATA = dbt2--0.45.0.sql
PGFILEDESC = "DBT-2 C stored functions"

PG_CPPFLAGS=-g -fno-omit-frame-pointer -Wall
PGXS := $(shell pg_config --pgxs)
include $(PGXS)
