# Makefile
# 25 july 2002

include ../common.mk

all: transaction_queue.so listener.so db_threadpool.so client

client: Makefile main.c \
	../Makefile ../common.mk \
	../include/common.h \
	../common/common.c \
	../common/common.so \
	../include/logging.h \
	../common/logging.c \
	../common/logging.so \
	../include/_socket.h ../common/_socket.c ../common/_socket.so \
	../include/transaction_data.h ../common/transaction_data.so \
	../include/client_interface.h ../common/client_interface.c \
	../common/client_interface.so \
	../include/transaction_queue.h transaction_queue.c transaction_queue.so \
	../include/db_threadpool.h db_threadpool.so db_threadpool.c \
	../include/db.h ../interfaces/db.c ../interfaces/db.so \
	../include/odbc_new_order.h \
	../include/odbc_payment.h \
	../include/odbc_order_status.h \
	../include/odbc_delivery.h \
	../include/odbc_stock_level.h \
	../interfaces/odbc/odbc_new_order.c \
	../interfaces/odbc/odbc_payment.c \
	../interfaces/odbc/odbc_order_status.c \
	../interfaces/odbc/odbc_delivery.c \
	../interfaces/odbc/odbc_stock_level.c \
	../interfaces/odbc/odbc_new_order.c \
	../interfaces/odbc/odbc_payment.c \
	../interfaces/odbc/odbc_order_status.so \
	../interfaces/odbc/odbc_delivery.so \
	../interfaces/odbc/odbc_stock_level.so \
	../include/listener.h listener.so
	$(CC) $(CFLAGS) -L/usr/local/lib main.c -o client \
		db_threadpool.so \
		listener.so \
		../common/common.so \
		../common/logging.so \
		../common/client_interface.so \
		transaction_queue.so \
		../common/_socket.so \
		../common/transaction_data.so \
		../interfaces/db.so \
		../interfaces/odbc/odbc_common.so \
		../interfaces/odbc/odbc_new_order.so \
		../interfaces/odbc/odbc_payment.so \
		../interfaces/odbc/odbc_order_status.so \
		../interfaces/odbc/odbc_delivery.so \
		../interfaces/odbc/odbc_stock_level.so \
		-lodbc \
		-lpthread -lm

db_threadpool.so: Makefile db_threadpool.c ../include/db_threadpool.h \
	../Makefile ../common.mk

listener.so: Makefile listener.c ../include/listener.h \
	../Makefile ../common.mk

transaction_queue.so: Makefile transaction_queue.c \
	../include/transaction_queue.h ../Makefile ../common.mk

clean: 
	rm -f client *.log *.so
