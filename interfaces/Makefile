# Makefile
# 12 june 2002

include ../common.mk

all: db.so

%.so:%.c
	$(CC) -c -shared -fpic $(CFLAGS) -I$(ODBCINCLUDEDIR) -o $@ $<

db.so: ../common.mk Makefile db.c ../include/db.h

clean: 
	rm -f *.so
