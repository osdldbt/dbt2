# This file is released under the terms of the Artistic License.  Please see
# the file LICENSE, included in this package, for details.
#
# Copyright (C) 2002 Mark Wong & Open Source Development Labs, Inc.
#
# 18 March 2004

AUTOMAKE_OPTIONS = no-dependencies

INCLUDES = -I../include

INTERFACE_SRCS=\
  @DBC_CORE@/dbc_common.c \
  @DBC_FUNCTIONS@/dbc_new_order.c \
  @DBC_FUNCTIONS@/dbc_payment.c \
  @DBC_FUNCTIONS@/dbc_order_status.c \
  @DBC_FUNCTIONS@/dbc_delivery.c \
  @DBC_FUNCTIONS@/dbc_stock_level.c

if NONSP 
NONSP_SRCS=@DBC_FUNCTIONS@/dbc_nonsp.c
endif

COMMON_SRCS=\
  common.c \
  logging.c \
  _socket.c \
  client_interface.c \
  transaction_data.c \
  _semaphore.c 

bin_PROGRAMS = client datagen driver results transaction_test

client_SOURCES = client.c transaction_queue.c listener.c db_threadpool.c db.c \
                 $(COMMON_SRCS) $(INTERFACE_SRCS) $(NONSP_SRCS)

datagen_SOURCES = datagen.c common.c

driver_SOURCES = driver_main.c driver.c input_data_generator.c \
                 $(COMMON_SRCS)

results_SOURCES = results.c common.c

transaction_test_SOURCES = transaction_test.c db.c input_data_generator.c \
                           $(COMMON_SRCS) $(INTERFACE_SRCS) $(NONSP_SRCS)

EXTRA_DIST = ../include/libpq_common.h \
             ../include/odbc_common.h \
             ../include/mysql_common.h \
             ../include/_socket.h \
             ../include/_semaphore.h \
             ../include/libpq_delivery.h \
             ../include/odbc_delivery.h \
             ../include/mysql_delivery.h \
             ../include/client_interface.h \
             ../include/libpq_new_order.h \
             ../include/odbc_new_order.h \
             ../include/mysql_new_order.h \
             ../include/common.h \
             ../include/libpq_order_status.h \
             ../include/odbc_order_status.h \
             ../include/mysql_order_status.h \
             ../include/db.h \
             ../include/libpq_payment.h \
             ../include/odbc_payment.h \
             ../include/mysql_payment.h \
             ../include/db_threadpool.h \
             ../include/libpq_stock_level.h \
             ../include/odbc_stock_level.h \
             ../include/mysql_stock_level.h \
             ../include/driver.h \
             ../include/listener.h \
             ../include/transaction_data.h \
             ../include/input_data_generator.h \
             ../include/logging.h \
             ../include/transaction_queue.h \
             odbc/dbc_new_order.c \
             odbc/dbc_stock_level.c \
             odbc/dbc_common.c \
             odbc/dbc_order_status.c \
             odbc/dbc_delivery.c \
             odbc/dbc_payment.c \
             libpq/dbc_new_order.c \
             libpq/dbc_stock_level.c \
             libpq/dbc_common.c \
             libpq/dbc_order_status.c \
             libpq/dbc_delivery.c \
             libpq/dbc_payment.c \
             mysql/dbc_new_order.c \
             mysql/dbc_stock_level.c \
             mysql/dbc_common.c \
             mysql/dbc_order_status.c \
             mysql/dbc_delivery.c \
             mysql/dbc_payment.c

