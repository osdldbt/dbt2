# Makefile
# 16 may 2002

include ../common.mk

all: transaction_test

transaction_test: ../common.mk Makefile ../common/common.c ../include/common.h \
	../terminal/input_data_generator.c ../terminal/input_data_generator.so \
	../include/input_data_generator.h ../common/transaction_data.c \
	../common/transaction_data.so ../include/transaction_data.h \
	../include/odbc_common.h ../interfaces/odbc/odbc_common.c \
	../interfaces/odbc/odbc_common.so ../include/odbc_new_order.h \
	../interfaces/odbc/odbc_new_order.c ../interfaces/odbc/odbc_new_order.so \
	../include/odbc_payment.h \
	../interfaces/odbc/odbc_payment.c ../interfaces/odbc/odbc_payment.so \
	../include/db.h ../interfaces/db.c ../interfaces/db.so \
	../include/odbc_order_status.h \
	../interfaces/odbc/odbc_order_status.c \
	../interfaces/odbc/odbc_order_status.so \
	../interfaces/odbc/odbc_delivery.c \
	../interfaces/odbc/odbc_delivery.so \
	../interfaces/odbc/odbc_stock_level.so \
	transaction_test.c
	$(CC) $(CFLAGS) -I$(ODBCINCLUDEDIR) transaction_test.c -o transaction_test \
		../common/common.so ../terminal/input_data_generator.so \
		../common/transaction_data.so ../interfaces/odbc/odbc_common.so \
		../interfaces/db.so \
		../interfaces/odbc/odbc_new_order.so \
		../interfaces/odbc/odbc_payment.so \
		../interfaces/odbc/odbc_order_status.so \
		../interfaces/odbc/odbc_delivery.so \
		../interfaces/odbc/odbc_stock_level.so \
		$(ODBCLIBDIR)/libsqlod.so -lpthread -lm

clean: 
	rm -f odbc_test
