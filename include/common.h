/*
 * common.h
 *
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright (C) 2002 Mark Wong & Open Source Development Lab, Inc.
 *
 * 16 may 2002
 */

#ifndef _COMMON_H_
#define _COMMON_H_

#define DB_USER "dbt"
#define DB_PASS "dbt"

#define DELIVERY 1
#define NEW_ORDER 2
#define ORDER_STATUS 3
#define PAYMENT 4
#define STOCK_LEVEL 5

#define ERROR 0
#define OK 1

#define A_STRING_CHAR_LEN 128
#define L_STRING_CHAR_LEN 52
#define N_STRING_CHAR_LEN 10
#define TIMESTAMP_LEN 28

#define D_CITY_LEN 20
#define D_NAME_LEN 10
#define D_STATE_LEN 2
#define D_STREET_1_LEN 20
#define D_STREET_2_LEN 20
#define D_ZIP_LEN 9

#define C_CREDIT_LEN 2
#define C_DATA_LEN 500
#define C_FIRST_LEN 16
#define C_LAST_LEN 16
#define C_MIDDLE_LEN 2
#define C_STREET_1_LEN 20
#define C_STREET_2_LEN 20
#define C_CITY_LEN 20
#define C_PHONE_LEN 16
#define C_SINCE_LEN TIMESTAMP_LEN
#define C_STATE_LEN 2
#define C_ZIP_LEN 9

#define I_NAME_LEN 24

#define O_ENTRY_D_LEN TIMESTAMP_LEN

#define OL_DELIVERY_D_LEN TIMESTAMP_LEN

#define W_CITY_LEN 20
#define W_NAME_LEN 10
#define W_STATE_LEN 2
#define W_STREET_1_LEN 20
#define W_STREET_2_LEN 20
#define W_ZIP_LEN 9

#define C_ID_UNKNOWN 0
#define C_LAST_SYL_MAX 10
#define D_ID_MAX 10
#define O_OL_CNT_MAX 15
#define O_CARRIER_ID_MAX 10

#define LOG_ERROR_MESSAGE(arg...) log_error_message(__FILE__, __LINE__, ## arg)

/* Prototypes */

void get_a_string(char *a_string, int x, int y);
int get_c_last(char *c_last, int i);
void get_l_string(char *l_string, int x, int y);
void get_n_string(char *n_string, int x, int y);
int get_nurand(int a, int x, int y);
double get_percentage();
int get_random(int max);
int init_common();

extern const char *c_last_syl[C_LAST_SYL_MAX];
extern int w_id_max;

#endif /* _COMMON_H_ */
