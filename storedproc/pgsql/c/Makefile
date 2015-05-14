MODULE_big = dbt2
OBJS = delivery.o new_order.o order_status.o payment.o stock_level.o

EXTENSION = dbt2
DATA = dbt2--0.41.0.sql
PGFILEDESC = "dbt-2 c stored functions"

PG_CPPFLAGS=-g -fno-omit-frame-pointer
PGXS := $(shell pg_config --pgxs)
include $(PGXS)
