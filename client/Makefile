# Makefile
# 25 july 2002

include ../common.mk

all: db_threadpool.so client

client: Makefile main.c \
	../Makefile ../common.mk \
	../include/common.h \
	../include/_socket.h ../common/_socket.so
	$(CC) $(CFLAGS) -I$(ODBCINCLUDEDIR) main.c -o client \
		db_threadpool.so \
		../common/common.so \
		../common/_socket.so \
		../interfaces/odbc/odbc_common.so \
		$(ODBCLIBDIR)/libsqlod.so \
		-lpthread -lm

db_threadpool.so: Makefile db_threadpool.c ../include/db_threadpool.h \
	../Makefile ../common.mk

clean: 
	rm -f client *.log *.so
