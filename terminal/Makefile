# Makefile
# 24 june 2002

include ../common.mk

all: input_data_generator.so

input_data_generator.so: ../common.mk Makefile ../include/common.h \
	../include/transaction_data.h input_data_generator.c
	$(CC) -c $(CFLAGS) input_data_generator.c -o input_data_generator.so

clean: 
	rm -f *.so
