# Makefile
# 25 july 2002

include ../common.mk

all: transaction_queue.so listener.so db_threadpool.so client

client: Makefile main.c \
	../Makefile ../common.mk \
	../include/common.h \
	../include/_socket.h ../common/_socket.so \
	../include/transaction_data.h ../common/transaction_data.so \
	../include/client_interface.h ../common/client_interface.so \
	../include/transaction_queue.h transaction_queue.so \
	../include/db.h ../interfaces/db.so \
	../include/listener.h listener.so
	$(CC) $(CFLAGS) -I$(ODBCINCLUDEDIR) main.c -o client \
		db_threadpool.so \
		listener.so \
		../common/common.so \
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
		$(ODBCLIBDIR)/libsqlod.so \
		-lpthread -lm

db_threadpool.so: Makefile db_threadpool.c ../include/db_threadpool.h \
	../Makefile ../common.mk

listener.so: Makefile listener.c ../include/listener.h \
	../Makefile ../common.mk

transaction_queue.so: Makefile transaction_queue.c \
	../include/transaction_queue.h ../Makefile ../common.mk

clean: 
	rm -f client *.log *.so
