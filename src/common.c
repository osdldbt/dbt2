/*
 * common.c
 *
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright (C) 2002      Open Source Development Lab, Inc.
 *               2002-2021 Mark Wong
 *
 * 16 may 2002
 * Based on TPC-C Standard Specification Revision 5.0.
 */

#include <pthread.h>
#include <stdarg.h>
#include <math.h>
#include <string.h>
#include <locale.h>
#include <wchar.h>

#include "common.h"
#include "transaction_data.h"

char output_path[256] = "";
/*
 * Initialize a-string character set to 128 ascii characters.  Clause 4.3.2.2.
 *
 * Pick the 128 ASCII characters that don't need to be escaped and are UTF-8
 * friendly.
 */
const wchar_t a_string_char[A_STRING_CHAR_LEN] =
        L"A!#$%&()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[]^_`abcdef"
		"ghijklmnopqrstuvwxyz{|}~€‚ƒ„…†‡ˆ‰Š‹ŒŽ‘’“”•–—˜™š›œžŸ¡¢£¤¥¦§¨©";
const wchar_t n_string_char[N_STRING_CHAR_LEN] = L"0123456789";
const wchar_t l_string_char[L_STRING_CHAR_LEN] =
		L"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";


const wchar_t *c_last_syl[C_LAST_SYL_MAX] =
{
	L"BAR", L"OUGHT", L"ABLE", L"PRI", L"PRES", L"ESE", L"ANTI", L"CALLY",
	L"ATION", L"EING"
};

const char transaction_short_name[TRANSACTION_MAX] =
	{ 'd', 'n', 'o', 'p', 's' };

char *transaction_name[TRANSACTION_MAX] =
	{ "delivery    ",
	  "new-order   ",
	  "order-status",
	  "payment     ",
	  "stock-level "
	};

struct table_cardinality_t table_cardinality;

double difftimeval(struct timeval rt1, struct timeval rt0)
{
	return (rt1.tv_sec - rt0.tv_sec) +
		(double) (rt1.tv_usec - rt0.tv_usec) / 1000000.00;
}

/* Clause 4.3.2.2.  */
void get_a_string(wchar_t *a_string, int x, int y)
{
	int length;
	int i;

	length = x + get_random(y - x + 1) + 1;
	a_string[length - 1] = L'\0';

	for (i = 0; i < length - 1; i++)
	{
		a_string[i] = a_string_char[get_random(A_STRING_CHAR_LEN - 1)];
	}

	return;
}

/* Clause 4.3.2.3 */
int get_c_last(wchar_t *c_last, int i)
{
	wchar_t tmp[4];

	c_last[0] = L'\0';

	if (i < 0 || i > 999)
	{
		return ERROR;
	}

	/* Ensure the number is padded with leading 0's if it's less than 100. */
	swprintf(tmp, 4, L"%03d", i);

	wcscat(c_last, c_last_syl[tmp[0] - L'0']);
	wcscat(c_last, c_last_syl[tmp[1] - L'0']);
	wcscat(c_last, c_last_syl[tmp[2] - L'0']);
	return OK;
}

void get_l_string(wchar_t *a_string, int x, int y)
{
	int length;
	int i;

	length = x + get_random(y - x + 1) + 1;
	a_string[length - 1] = L'\0';

	for (i = 0; i < length - 1; i++)
	{
		a_string[i] = l_string_char[get_random(L_STRING_CHAR_LEN - 1)];
	}

	return;
}

/* Clause 4.3.2.2.  */
void get_n_string(wchar_t *n_string, int x, int y)
{
	int length;
	int i;

	length = x + get_random(y - x + 1) + 1;
	n_string[length - 1] = L'\0';

	for (i = 0; i < length - 1; i++)
	{
		n_string[i] = n_string_char[get_random(N_STRING_CHAR_LEN)];
	}

	return;
}

/* Clause 2.1.6 */
int get_nurand(int a, int x, int y)
{
	return ((get_random(a + 1) | (x + get_random(y + 1))) % (y - x + 1)) + x;
}

/* Return a number from 0 to max. */
double get_percentage()
{
	return (double) rand() / (double) RAND_MAX;
}

int get_random(int max)
{
	return rand() % max;
}

/*
 * Clause 5.2.5.4
 * Calculate and return a think time using a negative exponential function.
 * think_time = -ln(r) * m
 * return: think time, in milliseconds
 * r: random number, where 0 < r <= 1
 * mean_think_time = mean think time, in milliseconds
 */
int get_think_time(int mean_think_time)
{
	return (-1.0 * log(get_percentage() + 0.000001) * mean_think_time);
}

int init_common()
{
	int rc = OK;

	printf("setting locale: %s\n", setlocale(LC_ALL, "en_US.utf8"));
	srand(1);

	/* Initialize struct to have default table cardinalities. */
	table_cardinality.warehouses = 1;
	table_cardinality.districts = DISTRICT_CARDINALITY;
	table_cardinality.customers = CUSTOMER_CARDINALITY;
	table_cardinality.items = ITEM_CARDINALITY;
	table_cardinality.orders = ORDER_CARDINALITY;
	table_cardinality.new_orders = NEW_ORDER_CARDINALITY;

	return rc;
}
