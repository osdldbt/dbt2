# Makefile
# 16 may 2002

include ../common.mk

all: datagen

datagen: ../common.mk Makefile ../common/common.c ../include/common.h main.c
	$(CC) $(CFLAGS) main.c -o datagen ../common/common.so

clean: 
	rm -f *.data datagen
