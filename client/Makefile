# Makefile
# 25 july 2002

include ../common.mk

all: client

client: Makefile main.c \
	../include/common.h \
	../include/_socket.h ../common/_socket.so
	$(CC) $(CFLAGS) -I$(ODBCINCLUDEDIR) main.c -o client \
		../common/common.so \
		../common/_socket.so \
		../interfaces/odbc/odbc_common.so \
		$(ODBCLIBDIR)/libsqlod.so \
		-lpthread -lm

clean: 
	rm -f client *.log
