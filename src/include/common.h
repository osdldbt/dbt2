/*
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright (C) 2002      Open Source Development Labs, Inc.
 *               2002-2022 Mark Wong
 *
 * 16 may 2002
 */

#ifndef _COMMON_H_
#define _COMMON_H_

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <time.h>
#include <sys/time.h>

#include "pcg_variants.h"

#if defined(ODBC) || defined(LIBMYSQL) || defined(LIBDRIZZLE)
#define DB_USER "dbt"
#define DB_PASS ""
#endif /* ODBC || LIBMYSQL || LIBDRIZZLE */

#if defined(COCKROACH) || defined(LIBPQ) || defined(LIBMYSQL) || \
		defined(LIBDRIZZLE)
#define DB_NAME "dbt2"
#endif /* COCKROACH || LIBPQ || LIBMYSQL || LIBDRIZZLE */

#define DELIVERY 0
#define NEW_ORDER 1
#define ORDER_STATUS 2
#define PAYMENT 3
#define STOCK_LEVEL 4
#define TRANSACTION_MAX 5
#define INTEGRITY 10

enum table {
	TABLE_WAREHOUSE,
	TABLE_DISTRICT,
	TABLE_CUSTOMER,
	TABLE_ITEM,
	TABLE_ORDER,
	TABLE_STOCK,
	TABLE_NEW_ORDER,
	TABLE_HISTORY,
	TABLE_ORDER_LINE,
	TABLE_ALL
};

#define ERROR 0
#define OK 1
#define EXIT_CODE 2
#define ERROR_SOCKET_CLOSED 3
#define STATUS_ROLLBACK 4
#define ERROR_RECEIVE_TIMEOUT 5

#define A_STRING_CHAR_LEN 128
#define L_STRING_CHAR_LEN 52
#define N_STRING_CHAR_LEN 10
#define TIMESTAMP_LEN 28

#define CUSTOMER_CARDINALITY 3000
#define DISTRICT_CARDINALITY 10
#define ITEM_CARDINALITY 100000
#define ORDER_CARDINALITY 3000
#define STOCK_CARDINALITY 100000
#define NEW_ORDER_CARDINALITY 900

#define D_ID_LEN 10
#define D_CITY_LEN 80
#define D_NAME_LEN 40
#define D_STATE_LEN 8
#define D_STREET_1_LEN 80
#define D_STREET_2_LEN 80
#define D_ZIP_LEN 9

#define C_ID_LEN 10
#define C_CREDIT_LEN 2
#define C_DATA_LEN 2000
#define C_FIRST_LEN 64
#define C_LAST_LEN 16
#define C_MIDDLE_LEN 2
#define C_STREET_1_LEN 80
#define C_STREET_2_LEN 80
#define C_CITY_LEN 80
#define C_PHONE_LEN 16
#define C_SINCE_LEN TIMESTAMP_LEN
#define C_STATE_LEN 8
#define C_ZIP_LEN 9

#define I_ID_LEN 10
#define I_DATA_LEN 50
#define I_NAME_LEN 96
#define I_PRICE_LEN 63

#define H_AMOUNT_LEN 63

#define O_ID_LEN 10
#define O_ENTRY_D_LEN TIMESTAMP_LEN
#define O_CARRIER_ID_LEN 10
#define O_ALL_LOCAL_LEN 2
#define O_OL_CNT_LEN 3

#define OL_O_ID_LEN 10
#define OL_DELIVERY_D_LEN TIMESTAMP_LEN
#define OL_QUANTITY_LEN 10
#define OL_AMOUNT_LEN 63

#define S_DIST_LEN 15

#define W_ID_LEN 10
#define W_CITY_LEN 80
#define W_NAME_LEN 40
#define W_STATE_LEN 8
#define W_STREET_1_LEN 80
#define W_STREET_2_LEN 80
#define W_ZIP_LEN 9

#define C_ID_UNKNOWN 0
#define C_LAST_SYL_MAX 10
#define D_ID_MAX 10
#define O_OL_CNT_MAX 15
#define O_CARRIER_ID_MAX 10

#define THRESHOLD_LEN 10

#define CLIENT_PORT 30000

#define CLIENT_PID_FILENAME "dbt2_client.pid"
#define DRIVER_PID_FILENAME "dbt2_driver.pid"


struct table_cardinality_t {
	int warehouses;
	int districts;
	int customers;
	int items;
	int orders;
	int new_orders;
};

/* Prototypes */
double difftimeval(struct timeval rt1, struct timeval rt0);
int edump(int type, void *data);
void get_a_string(pcg64f_random_t *, wchar_t *, int, int);
int get_c_last(wchar_t *, int);
void get_l_string(pcg64f_random_t *, wchar_t *, int, int);
void get_n_string(pcg64f_random_t *, wchar_t *, int, int);
int get_nurand(pcg64f_random_t *, int a, int x, int y);
double get_percentage(pcg64f_random_t *);
int64_t get_random(pcg64f_random_t *, int64_t);
int get_think_time(pcg64f_random_t *, int);
int init_common();
int create_pid_file();

extern char output_path[256];
extern const wchar_t *c_last_syl[C_LAST_SYL_MAX];
extern struct table_cardinality_t table_cardinality;
extern const char transaction_short_name[TRANSACTION_MAX];
extern char *transaction_name[TRANSACTION_MAX];

#endif /* _COMMON_H_ */
