# Makefile
# 16 may 2002

include ../common.mk

all: transaction_test results

results: ../common.mk ../Makefile Makefile results.c ../include/common.h
	$(CC) $(CFLAGS) results.c -o results ../common/common.so -lm

transaction_test: ../Makefile ../common.mk Makefile \
	../common/common.c ../include/common.h ../common/common.so \
	../common/logging.c ../include/logging.h ../common/logging.so \
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
	../include/_socket.h \
	../common/_socket.c \
	../common/_socket.so \
	../include/client_interface.h \
	../common/client_interface.c \
	../common/client_interface.so \
	transaction_test.c
	$(CC) $(CFLAGS) -L /usr/local/lib transaction_test.c -o transaction_test \
		../common/common.so ../terminal/input_data_generator.so \
		../common/logging.so \
		../common/transaction_data.so ../interfaces/odbc/odbc_common.so \
		../interfaces/db.so \
		../interfaces/odbc/odbc_new_order.so \
		../interfaces/odbc/odbc_payment.so \
		../interfaces/odbc/odbc_order_status.so \
		../interfaces/odbc/odbc_delivery.so \
		../interfaces/odbc/odbc_stock_level.so \
		../common/client_interface.so \
		../common/_socket.so \
		-lodbc -lpthread -lm

clean: 
	rm -f transaction_test results
