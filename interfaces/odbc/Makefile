# Makefile
# 12 june 2002

include ../../common.mk

all: odbc_common.so odbc_new_order.so odbc_payment.so odbc_order_status.so \
	odbc_delivery.so odbc_stock_level.so

odbc_common.so: ../../common.mk Makefile odbc_common.c \
	../../include/odbc_common.h ../../include/common.h \
	../../include/logging.h ../../include/logging.h \
	../../common/common.c ../../common/common.so

odbc_delivery.so: ../../common.mk Makefile odbc_common.c \
	../../include/odbc_common.h ../../include/common.h \
	../../common/common.c ../../common/common.so \
	odbc_common.c ../../include/common.h \
	odbc_delivery.c ../../include/odbc_delivery.h

odbc_new_order.so: ../../common.mk Makefile odbc_common.c \
	../../include/odbc_common.h ../../include/common.h \
	../../common/common.c ../../common/common.so \
	odbc_common.c ../../include/common.h \
	odbc_new_order.c ../../include/odbc_new_order.h

odbc_order_status.so: ../../common.mk Makefile odbc_common.c \
	../../include/odbc_common.h ../../include/common.h \
	../../common/common.c ../../common/common.so \
	odbc_common.c ../../include/common.h \
	odbc_order_status.c ../../include/odbc_order_status.h

odbc_payment.so: ../../common.mk Makefile odbc_common.c \
	../../include/odbc_common.h ../../include/common.h \
	../../common/common.c ../../common/common.so \
	odbc_common.c ../../include/common.h \
	odbc_payment.c ../../include/odbc_payment.h

odbc_stock_level.so: ../../common.mk Makefile odbc_common.c \
	../../include/odbc_common.h ../../include/common.h \
	../../common/common.c ../../common/common.so \
	odbc_common.c ../../include/common.h \
	odbc_stock_level.c ../../include/odbc_stock_level.h

clean: 
	rm -f *.so
