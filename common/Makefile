# Makefile
# 16 may 2002

include ../common.mk

all: common.so transaction_data.so _socket.so

%.so:%.c
	$(CC) -c -shared -fpic $(CFLAGS) -o $@ $<

_socket.so: ../common.mk Makefile _socket.c ../include/_socket.h

common.so: ../common.mk Makefile common.c ../include/common.h

transaction_data.so: ../common.mk Makefile common.c ../include/common.h \
	transaction_data.c ../include/transaction_data.h

clean: 
	rm -f *.so
