/*
 * common.c
 *
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright The DBT-2 Authors
 *
 * Based on TPC-C Standard Specification Revision 5.0.
 */

#include <stdarg.h>
#include <math.h>
#include <string.h>
#include <locale.h>
#include <wchar.h>
#include <unistd.h>

#include "common.h"
#include "transaction_data.h"

char *output_path = NULL;
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

int create_pid_file(char *name)
{
	FILE * fpid;
	char pid_filename[512];

	sprintf(pid_filename, "%s/%s", output_path, name);

	fpid = fopen(pid_filename,"w");
	if (!fpid) {
		printf("cann't create pid file: %s\n", pid_filename);
		return ERROR;
	}

	fprintf(fpid,"%d", getpid());
	fclose(fpid);

	return OK;
}

double difftimeval(struct timeval rt1, struct timeval rt0)
{
	return (rt1.tv_sec - rt0.tv_sec) +
		(double) (rt1.tv_usec - rt0.tv_usec) / 1000000.00;
}

/* generates a random number on [0,1)-real-interval */
double genrand64_real2(pcg64f_random_t *rng)
{
    return (pcg64f_random_r(rng) >> 11) * (1.0 / 9007199254740992.0);
}

/* Clause 4.3.2.2.  */
void get_a_string(pcg64f_random_t *rng, wchar_t *a_string, int x, int y)
{
	int length;
	int i;
	pcg64f_random_t local_rng;

	pcg64f_srandom_r(&local_rng, pcg64f_random_r(rng));

	length = x + (int) get_random(&local_rng, (int) (y - x + 1)) + 1;
	a_string[length - 1] = L'\0';

	for (i = 0; i < length - 1; i++)
	{
		a_string[i] = a_string_char[get_random(&local_rng, A_STRING_CHAR_LEN)];
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

void get_l_string(pcg64f_random_t *rng, wchar_t *a_string, int x, int y)
{
	int length;
	int i;
	pcg64f_random_t local_rng;

	pcg64f_srandom_r(&local_rng, pcg64f_random_r(rng));

	length = x + (int) get_random(&local_rng, (int) (y - x + 1)) + 1;
	a_string[length - 1] = L'\0';

	for (i = 0; i < length - 1; i++)
	{
		a_string[i] =
				l_string_char[(int) get_random(&local_rng, L_STRING_CHAR_LEN)];
	}

	return;
}

/* Clause 4.3.2.2.  */
void get_n_string(pcg64f_random_t *rng, wchar_t *n_string, int x, int y)
{
	int length;
	int i;
	pcg64f_random_t local_rng;

	pcg64f_srandom_r(&local_rng, pcg64f_random_r(rng));

	length = x + (int) get_random(&local_rng, (int) (y - x + 1)) + 1;
	n_string[length - 1] = L'\0';

	for (i = 0; i < length - 1; i++)
	{
		n_string[i] =
				n_string_char[(int) get_random(&local_rng, N_STRING_CHAR_LEN)];
	}

	return;
}

/* Clause 2.1.6 */
int get_nurand(pcg64f_random_t *rng, int a, int x, int y)
{
	return ((get_random(rng, a + 1) |
			(x + get_random(rng, y + 1))) % (y - x + 1)) + x;
}

double get_percentage(pcg64f_random_t *rng)
{
	return (pcg64f_random_r(rng) >> 11) * (1.0/9007199254740991.0);
}

/* Return a number from [0 to max). */
int64_t get_random(pcg64f_random_t *rng, int64_t max)
{
	return (int64_t) ((double) (max) * genrand64_real2(rng));
}

/*
 * Clause 5.2.5.4
 * Calculate and return a think time using a negative exponential function.
 * think_time = -ln(r) * m
 * return: think time, in milliseconds
 * r: random number, where 0 < r <= 1
 * mean_think_time = mean think time, in milliseconds
 */
int get_think_time(pcg64f_random_t *rng, int mean_think_time)
{
	return (-1.0 * log(get_percentage(rng) + 0.000001) * mean_think_time);
}

int init_common()
{
	int rc = OK;

	printf("setting locale: %s\n", setlocale(LC_ALL, "en_US.utf8"));

	/* Initialize struct to have default table cardinalities. */
	table_cardinality.warehouses = 1;
	table_cardinality.districts = DISTRICT_CARDINALITY;
	table_cardinality.customers = CUSTOMER_CARDINALITY;
	table_cardinality.items = ITEM_CARDINALITY;
	table_cardinality.orders = ORDER_CARDINALITY;
	table_cardinality.new_orders = NEW_ORDER_CARDINALITY;

	return rc;
}

unsigned long int ntohll(long int x)
{
	if (ntohl(1) == 1)
		return x;
	else
		return (long int) (ntohl((int) ((x << 32) >> 32))) << 32 |
				(long int) ntohl(((int) (x >> 32)));
}
