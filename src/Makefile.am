#
# This file is released under the terms of the Artistic License.
# Please see the file LICENSE, included in this package, for details.
#
# Copyright (C) 2002 Mark Wong & Open Source Development Labs, Inc.
#

AUTOMAKE_OPTIONS = no-dependencies

INCLUDES = -Iinclude

COMMON_SRCS=\
  common.c \
  logging.c \
  _socket.c \
  client_interface.c \
  transaction_data.c \
  _semaphore.c 

INTERFACE_SRCS=\
  @DBC_CORE@/dbc_common.c \
  @DBC_FUNCTIONS@/dbc_new_order.c \
  @DBC_FUNCTIONS@/dbc_payment.c \
  @DBC_FUNCTIONS@/dbc_order_status.c \
  @DBC_FUNCTIONS@/dbc_delivery.c \
  @DBC_FUNCTIONS@/dbc_stock_level.c \
  @DBC_FUNCTIONS@/dbc_integrity.c

if NONSP
INTERFACE_SRCS+=@DBC_FUNCTIONS@/dbc_nonsp.c
endif

bin_PROGRAMS = client datagen driver transaction_test

client_SOURCES = client.c transaction_queue.c listener.c db_threadpool.c db.c \
                 $(COMMON_SRCS)
nodist_client_SOURCES = $(INTERFACE_SRCS)

datagen_SOURCES = datagen.c common.c

driver_SOURCES = driver_main.c driver.c input_data_generator.c \
                 $(COMMON_SRCS)

transaction_test_SOURCES = transaction_test.c db.c input_data_generator.c \
                           $(COMMON_SRCS)
nodist_transaction_test_SOURCES = $(INTERFACE_SRCS)

EXTRA_DIST = include/_semaphore.h \
             include/_socket.h \
             include/client_interface.h \
             include/common.h \
             include/db.h \
             include/db_threadpool.h \
             include/driver.h \
             include/input_data_generator.h \
             include/libpq_integrity.h \
             include/libpq_common.h \
             include/libpq_delivery.h \
             include/libpq_new_order.h \
             include/libpq_order_status.h \
             include/libpq_payment.h \
             include/libpq_stock_level.h \
             include/listener.h \
             include/logging.h \
             include/mysql_common.h \
             include/mysql_delivery.h \
             include/mysql_integrity.h \
             include/mysql_new_order.h \
             include/mysql_order_status.h \
             include/mysql_payment.h \
             include/mysql_stock_level.h \
             include/nonsp_common.h \
             include/nonsp_delivery.h \
             include/nonsp_integrity.h \
             include/nonsp_new_order.h \
             include/nonsp_order_status.h \
             include/nonsp_payment.h \
             include/nonsp_stock_level.h \
             include/odbc_common.h \
             include/odbc_delivery.h \
             include/odbc_new_order.h \
             include/odbc_order_status.h \
             include/odbc_payment.h \
             include/odbc_stock_level.h \
             include/transaction_data.h \
             include/transaction_queue.h \
             libpq/dbc_new_order.c \
             libpq/dbc_stock_level.c \
             libpq/dbc_common.c \
             libpq/dbc_order_status.c \
             libpq/dbc_delivery.c \
             libpq/dbc_payment.c \
             libpq/dbc_integrity.c \
             mysql/dbc_new_order.c \
             mysql/dbc_stock_level.c \
             mysql/dbc_common.c \
             mysql/dbc_order_status.c \
             mysql/dbc_delivery.c \
             mysql/dbc_payment.c \
             mysql/dbc_integrity.c \
             nonsp/dbc_new_order.c \
             nonsp/dbc_stock_level.c \
             nonsp/dbc_nonsp.c \
             nonsp/dbc_order_status.c \
             nonsp/dbc_delivery.c \
             nonsp/dbc_payment.c \
             nonsp/dbc_integrity.c \
             odbc/dbc_new_order.c \
             odbc/dbc_stock_level.c \
             odbc/dbc_common.c \
             odbc/dbc_order_status.c \
             odbc/dbc_delivery.c \
             odbc/dbc_payment.c \
             odbc/dbc_integrity.c
