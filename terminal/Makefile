# Makefile
# 24 june 2002

include ../common.mk

all: driver.so input_data_generator.so driver

driver.so: ../Makefile \
	../common.mk \
	Makefile \
	../include/common.h \
	../include/driver.h \
	driver.c
	$(CC) -c $(CFLAGS) driver.c -o driver.so

input_data_generator.so: ../Makefile \
	../common.mk \
	Makefile \
	../include/common.h \
	../include/transaction_data.h \
	../include/input_data_generator.h \
	input_data_generator.c
	$(CC) -c $(CFLAGS) input_data_generator.c -o input_data_generator.so

driver: ../Makefile \
	../common.mk \
	Makefile \
	main.c \
	../include/common.h \
	../common/common.so \
	../include/logging.h \
	../common/logging.so \
	../include/_socket.h \
	../common/_socket.c \
	../common/_socket.so \
	../include/driver.h \
	../terminal/driver.c \
	../terminal/driver.so \
	../include/client_interface.h \
	../common/client_interface.so \
	../common/client_interface.c \
	../common/transaction_data.so \
	../common/transaction_data.c \
	../include/transaction_data.h \
	../include/input_data_generator.h \
	input_data_generator.so \
	input_data_generator.c
	$(CC) $(CFLAGS) -I$(ODBCINCLUDEDIR) main.c -o driver \
		../common/common.so \
		../common/logging.so \
		../common/_socket.so \
		../common/client_interface.so \
		../common/transaction_data.so \
		driver.so \
		input_data_generator.so \
		-lpthread -lm
	

clean: 
	rm -f *.so driver
