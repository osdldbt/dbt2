# Makefile
# 16 may 2002

include ../common.mk

all: common.so transaction_data.so _socket.so client_interface.so logging.so

_socket.so: ../common.mk Makefile _socket.c ../include/_socket.h

common.so: ../common.mk Makefile common.c ../include/common.h

logging.so: ../common.mk Makefile logging.c ../include/logging.h \
	../include/common.h

transaction_data.so: ../common.mk Makefile common.c ../include/common.h \
	transaction_data.c ../include/transaction_data.h

client_interface.so: ../common.mk Makefile \
	client_interface.c \
	../include/client_interface.h
	$(CC) -c -shared -fpic $(CFLAGS) -I$(ODBCINCLUDEDIR) -o $@ $<

clean: 
	rm -f *.so
