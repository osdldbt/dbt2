/*
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright (C) 2003 Mark Wong & Open Source Development Lab, Inc.
 *
 * Based on TPC-C Standard Specification Revision 5.0 Clause 2.8.2.
 */

#include <postgres.h>
#include <fmgr.h>
#include <executor/spi.h>

/*
#define DEBUG
*/

#define DELIVERY_1 \
	"SELECT no_o_id\n" \
	"FROM new_order\n" \
	"WHERE no_w_id = %d\n" \
	"  AND no_d_id = %d"

#define DELIVERY_2 \
	"DELETE FROM new_order\n" \
	"WHERE no_o_id = %s\n" \
	"  AND no_w_id = %d\n" \
	"  AND no_d_id = %d"

#define DELIVERY_3 \
	"SELECT o_c_id\n" \
	"FROM orders\n" \
	"WHERE o_id = %s\n" \
	"  AND o_w_id = %d\n" \
	"  AND o_d_id = %d"

#define DELIVERY_4 \
	"UPDATE orders\n" \
	"SET o_carrier_id = %d\n" \
	"WHERE o_id = %s\n" \
	"  AND o_w_id = %d\n" \
	"  AND o_d_id = %d"

#define DELIVERY_5 \
	"UPDATE order_line\n" \
	"SET ol_delivery_d = current_timestamp\n" \
	"WHERE ol_o_id = %s\n" \
	"  AND ol_w_id = %d\n" \
	"  AND ol_d_id = %d"

#define DELIVERY_6 \
	"SELECT SUM(ol_amount * ol_quantity)\n" \
	"FROM order_line\n" \
	"WHERE ol_o_id = %s\n" \
	"  AND ol_w_id = %d\n" \
	"  AND ol_d_id = %d"

#define DELIVERY_7 \
	"UPDATE customer\n" \
	"SET c_delivery_cnt = c_delivery_cnt + 1,\n" \
	"    c_balance = c_balance + %s\n" \
	"WHERE c_id = %s\n" \
	"  AND c_w_id = %d\n" \
	"  AND c_d_id = %d"

#define NEW_ORDER_1 \
	"SELECT w_tax\n" \
	"FROM warehouse\n" \
	"WHERE w_id = %d"

#define NEW_ORDER_2 \
	"SELECT d_tax, d_next_o_id\n" \
	"FROM district \n" \
	"WHERE d_w_id = %d\n" \
	"  AND d_id = %d"

#define NEW_ORDER_3 \
	"UPDATE district\n" \
	"SET d_next_o_id = d_next_o_id + 1\n" \
	"WHERE d_w_id = %d\n" \
	"  AND d_id = %d"

#define NEW_ORDER_4 \
	"SELECT c_discount, c_last, c_credit\n" \
	"FROM customer\n" \
	"WHERE c_w_id = %d\n" \
	"  AND c_d_id = %d\n" \
	"  AND c_id = %d"

#define NEW_ORDER_5 \
	"INSERT INTO new_order (no_o_id, no_d_id, no_w_id)\n" \
        "VALUES (%s, %d, %d)"

#define NEW_ORDER_6 \
	"INSERT INTO orders (o_id, o_d_id, o_w_id, o_c_id, o_entry_d,\n" \
	"                    o_carrier_id, o_ol_cnt, o_all_local)\n" \
	"VALUES (%s, %d, %d, %d, current_timestamp, NULL, %d, %d)"

#define NEW_ORDER_7 \
	"SELECT i_price, i_name, i_data\n" \
	"FROM item\n" \
	"WHERE i_id = %d"

#define NEW_ORDER_8 \
	"SELECT s_quantity, %s, s_data\n" \
	"FROM stock\n" \
	"WHERE s_i_id = %d\n" \
	"  AND s_w_id = %d"

#define NEW_ORDER_9 \
	"UPDATE stock   "\
	"SET s_quantity = s_quantity - %d\n" \
	"WHERE s_i_id = %d\n" \
	"  AND s_w_id = %d"

#define NEW_ORDER_10 \
	"INSERT INTO order_line (ol_o_id, ol_d_id, ol_w_id, ol_number,\n" \
	"                        ol_i_id, ol_supply_w_id, ol_delivery_d,\n" \
	"                        ol_quantity, ol_amount, ol_dist_info)\n" \
	"VALUES (%s, %d, %d, %d, %d, %d, NULL, %d, %f, '%s')"

#define ORDER_STATUS_1 \
	"SELECT c_id\n" \
	"FROM customer\n" \
	"WHERE c_w_id = %d\n" \
	"  AND c_d_id = %d\n" \
	"  AND c_last = '%s'\n" \
	"ORDER BY c_first ASC"

#define ORDER_STATUS_2 \
	"SELECT c_first, c_middle, c_last, c_balance\n" \
	"FROM customer\n" \
	"WHERE c_w_id = %d\n" \
	"  AND c_d_id = %d\n" \
	"  AND c_id = %d"

#define ORDER_STATUS_3 \
	"SELECT o_id, o_carrier_id, o_entry_d, o_ol_cnt\n" \
	"FROM orders\n" \
	"WHERE o_w_id = %d\n" \
	"  AND o_d_id = %d \n" \
	"  AND o_c_id = %d\n" \
	"ORDER BY o_id DESC"

#define ORDER_STATUS_4 \
	"SELECT ol_i_id, ol_supply_w_id, ol_quantity, ol_amount,\n" \
	"       ol_delivery_d\n" \
	"FROM order_line\n" \
	"WHERE ol_w_id = %d\n" \
	"  AND ol_d_id = %d\n" \
	"  AND ol_o_id = %s"

#define PAYMENT_1 \
	"SELECT w_name, w_street_1, w_street_2, w_city, w_state, w_zip\n" \
	"FROM warehouse\n" \
	"WHERE w_id = %d"

#define PAYMENT_2 \
	"UPDATE warehouse\n" \
	"SET w_ytd = w_ytd + %f\n" \
	"WHERE w_id = %d"

#define PAYMENT_3 \
	"SELECT d_name, d_street_1, d_street_2, d_city, d_state, d_zip\n" \
	"FROM district\n" \
	"WHERE d_id = %d\n" \
	"  AND d_w_id = %d"

#define PAYMENT_4 \
	"UPDATE district\n" \
	"SET d_ytd = d_ytd + %f\n" \
	"WHERE d_id = %d\n" \
	"  AND d_w_id = %d"

#define PAYMENT_5 \
	"SELECT c_id\n" \
	"FROM customer\n" \
	"WHERE c_w_id = %d\n" \
	"  AND c_d_id = %d\n" \
	"  AND c_last = '%s'\n" \
	"ORDER BY c_first ASC"

#define PAYMENT_6 \
	"SELECT c_first, c_middle, c_last, c_street_1, c_street_2, c_city,\n" \
	"       c_state, c_zip, c_phone, c_since, c_credit,\n" \
	"       c_credit_lim, c_discount, c_balance, c_data, c_ytd_payment\n" \
	"FROM customer\n" \
	"WHERE c_w_id = %d\n" \
	"  AND c_d_id = %d\n" \
	"  AND c_id = %d"

#define PAYMENT_7_GC \
	"UPDATE customer\n" \
	"SET c_balance = c_balance - %f,\n" \
	"    c_ytd_payment = c_ytd_payment + 1\n" \
	"WHERE c_id = %d\n" \
	"  AND c_w_id = %d\n" \
	"  AND c_d_id = %d"

#define PAYMENT_7_BC \
	"UPDATE customer\n" \
	"SET c_balance = c_balance - %f,\n" \
	"    c_ytd_payment = c_ytd_payment + 1,\n" \
	"    c_data = '%s'\n" \
	"WHERE c_id = %d\n" \
	"  AND c_w_id = %d\n" \
	"  AND c_d_id = %d"

#define PAYMENT_8 \
	"INSERT INTO history (h_c_id, h_c_d_id, h_c_w_id, h_d_id, h_w_id,\n" \
	"                     h_date, h_amount, h_data)\n" \
	"VALUES (%d, %d, %d, %d, %d, current_timestamp, %f, '%s    %s')"

#define STOCK_LEVEL_1 \
	"SELECT d_next_o_id\n" \
	"FROM district\n" \
	"WHERE d_w_id = %d\n" \
	"AND d_id = %d"

#define STOCK_LEVEL_2 \
	"SELECT count(*)\n" \
	"FROM order_line, stock, district\n" \
	"WHERE d_id = %d\n" \
	"  AND d_w_id = %d\n" \
	"  AND d_id = ol_d_id\n" \
	"  AND d_w_id = ol_w_id\n" \
	"  AND ol_i_id = s_i_id\n" \
	"  AND ol_w_id = s_w_id\n" \
	"  AND s_quantity < %d\n" \
	"  AND ol_o_id BETWEEN (%d)\n" \
	"		  AND (%d)"

/* Prototypes to prevent potential gcc warnings. */

Datum delivery(PG_FUNCTION_ARGS);
Datum new_order(PG_FUNCTION_ARGS);
Datum order_status(PG_FUNCTION_ARGS);
Datum payment(PG_FUNCTION_ARGS);
Datum stock_level(PG_FUNCTION_ARGS);

PG_FUNCTION_INFO_V1(delivery);
PG_FUNCTION_INFO_V1(new_order);
PG_FUNCTION_INFO_V1(order_status);
PG_FUNCTION_INFO_V1(payment);
PG_FUNCTION_INFO_V1(stock_level);

const char s_dist[10][11] = {
	"s_dist_01", "s_dist_02", "s_dist_03", "s_dist_04", "s_dist_05",
	"s_dist_06", "s_dist_07", "s_dist_08", "s_dist_09", "s_dist_10"
};

void escape_str(char *orig_str, char *esc_str)
{
	int i, j;
	for (i = 0, j = 0; i < strlen(orig_str); i++) {
		if (orig_str[i] == '\'') {
			esc_str[j++] = '\'';
		} else if (orig_str[i] == '\\') {
			esc_str[j++] = '\\';
		} else if (orig_str[i] == ')') {
			esc_str[j++] = '\\';
		}
		esc_str[j++] = orig_str[i];
	}
	esc_str[j] = '\0';
}

Datum delivery(PG_FUNCTION_ARGS)
{
	/* Input variables. */
	int32 w_id = PG_GETARG_INT32(0);
	int32 o_carrier_id = PG_GETARG_INT32(1);

	TupleDesc tupdesc;
	SPITupleTable *tuptable;
	HeapTuple tuple;

	char query[256];
	int d_id;
	int ret;
	char *no_o_id = NULL;
	char *o_c_id = NULL;
	char *ol_amount = NULL;

	SPI_connect();

	for (d_id = 1; d_id <= 10; d_id++) {
		sprintf(query, DELIVERY_1, w_id, d_id);
#ifdef DEBUG
		elog(NOTICE, "%s", query);
#endif /* DEBUG */
		ret = SPI_exec(query, 0);
		if (ret == SPI_OK_SELECT && SPI_processed > 0) {
			tupdesc = SPI_tuptable->tupdesc;
			tuptable = SPI_tuptable;
	     		tuple = tuptable->vals[0];

			no_o_id = SPI_getvalue(tuple, tupdesc, 1);
#ifdef DEBUG
			elog(NOTICE, "no_o_id = %s", no_o_id);
#endif /* DEBUG */
		} else {
			/* Nothing to delivery for this district, try next. */
			continue;
		}

		sprintf(query, DELIVERY_2, no_o_id, w_id, d_id);
#ifdef DEBUG
		elog(NOTICE, "%s", query);
#endif /* DEBUG */
		ret = SPI_exec(query, 0);
		if (ret != SPI_OK_DELETE) {
			PG_RETURN_INT32(-1);
		}

		sprintf(query, DELIVERY_3, no_o_id, w_id, d_id);
#ifdef DEBUG
		elog(NOTICE, "%s", query);
#endif /* DEBUG */
		ret = SPI_exec(query, 0);
		if (ret == SPI_OK_SELECT && SPI_processed > 0) {
			tupdesc = SPI_tuptable->tupdesc;
			tuptable = SPI_tuptable;
	     		tuple = tuptable->vals[0];

			o_c_id = SPI_getvalue(tuple, tupdesc, 1);
#ifdef DEBUG
			elog(NOTICE, "o_c_id = %s", no_o_id);
#endif /* DEBUG */
		} else {
			PG_RETURN_INT32(-1);
		}

		sprintf(query, DELIVERY_4, o_carrier_id, no_o_id, w_id, d_id);
#ifdef DEBUG
		elog(NOTICE, "%s", query);
#endif /* DEBUG */
		ret = SPI_exec(query, 0);
		if (ret != SPI_OK_UPDATE) {
			PG_RETURN_INT32(-1);
		}

		sprintf(query, DELIVERY_5, no_o_id, w_id, d_id);
#ifdef DEBUG
		elog(NOTICE, "%s", query);
#endif /* DEBUG */
		ret = SPI_exec(query, 0);
		if (ret != SPI_OK_UPDATE) {
			PG_RETURN_INT32(-1);
		}

		sprintf(query, DELIVERY_6, no_o_id, w_id, d_id);
#ifdef DEBUG
		elog(NOTICE, "%s", query);
#endif /* DEBUG */
		ret = SPI_exec(query, 0);
		if (ret == SPI_OK_SELECT && SPI_processed > 0) {
			tupdesc = SPI_tuptable->tupdesc;
			tuptable = SPI_tuptable;
	     		tuple = tuptable->vals[0];

			ol_amount = SPI_getvalue(tuple, tupdesc, 1);
#ifdef DEBUG
			elog(NOTICE, "ol_amount = %s", no_o_id);
#endif /* DEBUG */
		} else {
			PG_RETURN_INT32(-1);
		}

		sprintf(query, DELIVERY_7, ol_amount, o_c_id, w_id, d_id);
#ifdef DEBUG
		elog(NOTICE, "%s", query);
#endif /* DEBUG */
		ret = SPI_exec(query, 0);
		if (ret != SPI_OK_UPDATE) {
			PG_RETURN_INT32(-1);
		}
	}

	SPI_finish();
	PG_RETURN_INT32(1);
}

Datum new_order(PG_FUNCTION_ARGS)
{
	/* Input variables. */
	int32 w_id = PG_GETARG_INT32(0);
	int32 d_id = PG_GETARG_INT32(1);
	int32 c_id = PG_GETARG_INT32(2);
	int32 o_all_local = PG_GETARG_INT32(3);
	int32 o_ol_cnt = PG_GETARG_INT32(4);

	TupleDesc tupdesc;
	SPITupleTable *tuptable;
	HeapTuple tuple;

	int32 ol_i_id[15];
	int32 ol_supply_w_id[15];
	int32 ol_quantity[15];

	int i, j;

	int ret;
	char query[1024];

	char *w_tax = NULL;

	char *d_tax = NULL;
	char *d_next_o_id = NULL;

	char *c_discount = NULL;
	char *c_last = NULL;
	char *c_credit = NULL;

	float order_amount = 0.0;

	char *i_price[15];
	char *i_name[15];
	char *i_data[15];

	float ol_amount[15];
	char *s_quantity[15];
	char *my_s_dist[15];
	char *s_data[15];

	for (i = 0, j = 5; i < 15; i++) {
		ol_i_id[i] = PG_GETARG_INT32(j++);
		ol_supply_w_id[i] = PG_GETARG_INT32(j++);
		ol_quantity[i] = PG_GETARG_INT32(j++);
	}

#ifdef DEBUG
	elog(NOTICE, "w_id = %d", w_id);
	elog(NOTICE, "d_id = %d", d_id);
	elog(NOTICE, "c_id = %d", c_id);
	elog(NOTICE, "o_all_local = %d", o_all_local);
	elog(NOTICE, "o_ol_cnt = %d", o_ol_cnt);

	elog(NOTICE, "##  ol_i_id  ol_supply_w_id  ol_quantity");
	elog(NOTICE, "--  -------  --------------  -----------");
	for (i = 0; i < o_ol_cnt; i++) {
		elog(NOTICE, "%2d  %7d  %14d  %11d",
			i + 1, ol_i_id[i], ol_supply_w_id[i], ol_quantity[i]);
	}
#endif /* DEBUG */

	SPI_connect();

	sprintf(query, NEW_ORDER_1, w_id);
#ifdef DEBUG
	elog(NOTICE, "%s", query);
#endif /* DEBUG */
	ret = SPI_exec(query, 0);
	if (ret == SPI_OK_SELECT && SPI_processed > 0) {
		tupdesc = SPI_tuptable->tupdesc;
		tuptable = SPI_tuptable;
	     	tuple = tuptable->vals[0];

		w_tax = SPI_getvalue(tuple, tupdesc, 1);
#ifdef DEBUG
		elog(NOTICE, "w_tax = %s", w_tax);
#endif /* DEBUG */
	} else {
		PG_RETURN_INT32(-1);
	}

	sprintf(query, NEW_ORDER_2, w_id, d_id);
#ifdef DEBUG
	elog(NOTICE, "%s", query);
#endif /* DEBUG */
	ret = SPI_exec(query, 0);
	if (ret == SPI_OK_SELECT && SPI_processed > 0) {
		tupdesc = SPI_tuptable->tupdesc;
		tuptable = SPI_tuptable;
	     	tuple = tuptable->vals[0];

		d_tax = SPI_getvalue(tuple, tupdesc, 1);
		d_next_o_id = SPI_getvalue(tuple, tupdesc, 2);
#ifdef DEBUG
		elog(NOTICE, "d_tax = %s", d_tax);
		elog(NOTICE, "d_next_o_id = %s", d_next_o_id);
#endif /* DEBUG */
	} else {
		PG_RETURN_INT32(-1);
	}

	sprintf(query, NEW_ORDER_3, w_id, d_id);
#ifdef DEBUG
	elog(NOTICE, "%s", query);
#endif /* DEBUG */
	ret = SPI_exec(query, 0);
	if (ret != SPI_OK_UPDATE) {
		PG_RETURN_INT32(-1);
	}

	sprintf(query, NEW_ORDER_4, w_id, d_id, c_id);
#ifdef DEBUG
	elog(NOTICE, "%s", query);
#endif /* DEBUG */
	ret = SPI_exec(query, 0);
	if (ret == SPI_OK_SELECT && SPI_processed > 0) {
		tupdesc = SPI_tuptable->tupdesc;
		tuptable = SPI_tuptable;
	     	tuple = tuptable->vals[0];

		c_discount = SPI_getvalue(tuple, tupdesc, 1);
		c_last = SPI_getvalue(tuple, tupdesc, 2);
		c_credit = SPI_getvalue(tuple, tupdesc, 3);
#ifdef DEBUG
		elog(NOTICE, "c_discount = %s", c_discount);
		elog(NOTICE, "c_last = %s", c_last);
		elog(NOTICE, "c_credit = %s", c_credit);
#endif /* DEBUG */
	} else {
		PG_RETURN_INT32(-1);
	}

	sprintf(query, NEW_ORDER_5, d_next_o_id, w_id, d_id);
#ifdef DEBUG
	elog(NOTICE, "%s", query);
#endif /* DEBUG */
	ret = SPI_exec(query, 0);
	if (ret != SPI_OK_INSERT) {
		PG_RETURN_INT32(-1);
	}

	sprintf(query, NEW_ORDER_6, d_next_o_id, d_id, w_id, c_id, o_ol_cnt,
		o_all_local);
#ifdef DEBUG
	elog(NOTICE, "%s", query);
#endif /* DEBUG */
	ret = SPI_exec(query, 0);
	if (ret != SPI_OK_INSERT) {
		PG_RETURN_INT32(-1);
	}

	for (i = 0; i < o_ol_cnt; i++) {
		sprintf(query, NEW_ORDER_7, ol_i_id[i]);
#ifdef DEBUG
		elog(NOTICE, "%s", query);
#endif /* DEBUG */
		ret = SPI_exec(query, 0);
		if (ret == SPI_OK_SELECT && SPI_processed > 0) {
			tupdesc = SPI_tuptable->tupdesc;
			tuptable = SPI_tuptable;
	     		tuple = tuptable->vals[0];

			i_price[i] = SPI_getvalue(tuple, tupdesc, 1);
			i_name[i] = SPI_getvalue(tuple, tupdesc, 2);
			i_data[i] = SPI_getvalue(tuple, tupdesc, 3);
#ifdef DEBUG
			elog(NOTICE, "i_price[%d] = %s", i, i_price[i]);
			elog(NOTICE, "i_name[%d] = %s", i, i_name[i]);
			elog(NOTICE, "i_data[%d] = %s", i, i_data[i]);
#endif /* DEBUG */
		} else {
			/* Item doesn't exist, rollback transaction. */
			PG_RETURN_INT32(-1);
		}

		ol_amount[i] = atof(i_price[i]) * (float) ol_quantity[i];
		sprintf(query, NEW_ORDER_8, s_dist[d_id - 1], ol_i_id[i], w_id);
#ifdef DEBUG
		elog(NOTICE, "%s", query);
#endif /* DEBUG */
		ret = SPI_exec(query, 0);
		if (ret == SPI_OK_SELECT && SPI_processed > 0) {
			tupdesc = SPI_tuptable->tupdesc;
			tuptable = SPI_tuptable;
	     		tuple = tuptable->vals[0];

			s_quantity[i] = SPI_getvalue(tuple, tupdesc, 1);
			my_s_dist[i] = SPI_getvalue(tuple, tupdesc, 2);
			s_data[i] = SPI_getvalue(tuple, tupdesc, 3);
#ifdef DEBUG
			elog(NOTICE, "s_quantity[%d] = %s", i, s_quantity[i]);
			elog(NOTICE, "my_s_dist[%d] = %s", i, my_s_dist[i]);
			elog(NOTICE, "s_data[%d] = %s", i, s_data[i]);
#endif /* DEBUG */
		} else {
			PG_RETURN_INT32(-1);
		}
		order_amount += ol_amount[i];

		if (atoi(s_quantity[i]) > ol_quantity[i] + 10) {
			sprintf(query, NEW_ORDER_9, ol_quantity[i],
				ol_i_id[i], w_id);
		} else {
			sprintf(query, NEW_ORDER_9, ol_quantity[i] + 91,
				ol_i_id[i], w_id);
		}
#ifdef DEBUG
		elog(NOTICE, "%s", query);
#endif /* DEBUG */
		ret = SPI_exec(query, 0);
		if (ret != SPI_OK_UPDATE) {
			PG_RETURN_INT32(-1);
		}

		sprintf(query, NEW_ORDER_10, d_next_o_id, d_id, w_id, i + 1,
			ol_i_id[i], ol_supply_w_id[i], ol_quantity[i],
			ol_amount[i], my_s_dist[i]);
#ifdef DEBUG
		elog(NOTICE, "%s", query);
#endif /* DEBUG */
		ret = SPI_exec(query, 0);
		if (ret != SPI_OK_INSERT) {
			PG_RETURN_INT32(-1);
		}
	}

	SPI_finish();

	PG_RETURN_INT32(1);
}

Datum order_status(PG_FUNCTION_ARGS)
{
	/* Input variables. */
	int32 c_id = PG_GETARG_INT32(0);
	int32 c_w_id = PG_GETARG_INT32(1);
	int32 c_d_id = PG_GETARG_INT32(2);
	text *c_last = PG_GETARG_TEXT_P(3);

	TupleDesc tupdesc;
	SPITupleTable *tuptable;
	HeapTuple tuple;

	int ret;
	int count;
	int j;
	char query[512];

	char *tmp_c_id;
	int my_c_id = 0;

	char *c_first = NULL;
	char *c_middle = NULL;
	char *my_c_last = NULL;
	char *c_balance = NULL;

	char *o_id = NULL;
	char *o_carrier_id = NULL;
	char *o_entry_d = NULL;
	char *o_ol_cnt = NULL;

	char *ol_i_id[15];
	char *ol_supply_w_id[15];
	char *ol_quantity[15];
	char *ol_amount[15];
	char *ol_delivery_d[15];

	SPI_connect();

	if (c_id == 0) {
		sprintf(query, ORDER_STATUS_1, c_w_id, c_d_id,
			DatumGetCString(DirectFunctionCall1(textout,
			PointerGetDatum(c_last))));
#ifdef DEBUG
		elog(NOTICE, "%s", query);
#endif /* DEBUG */
		ret = SPI_exec(query, 0);
		count = SPI_processed;
		if (ret == SPI_OK_SELECT && SPI_processed > 0) {
			tupdesc = SPI_tuptable->tupdesc;
			tuptable = SPI_tuptable;
	     		tuple = tuptable->vals[count / 2];

			tmp_c_id = SPI_getvalue(tuple, tupdesc, 1);
#ifdef DEBUG
			elog(NOTICE, "c_id = %s, %d total, selected %d",
				tmp_c_id, count, count / 2);
#endif /* DEBUG */
			my_c_id = atoi(tmp_c_id);
		} else {
			PG_RETURN_INT32(-1);
		}
	} else {
		my_c_id = c_id;
	}

	sprintf(query, ORDER_STATUS_2, c_w_id, c_d_id, my_c_id);
#ifdef DEBUG
	elog(NOTICE, "%s", query);
#endif /* DEBUG */
	ret = SPI_exec(query, 0);
	if (ret == SPI_OK_SELECT && SPI_processed > 0) {
		tupdesc = SPI_tuptable->tupdesc;
		tuptable = SPI_tuptable;
	     	tuple = tuptable->vals[0];

		c_first = SPI_getvalue(tuple, tupdesc, 1);
		c_middle = SPI_getvalue(tuple, tupdesc, 2);
		my_c_last = SPI_getvalue(tuple, tupdesc, 3);
		c_balance = SPI_getvalue(tuple, tupdesc, 4);
#ifdef DEBUG
		elog(NOTICE, "c_first = %s", c_first);
		elog(NOTICE, "c_middle = %s", c_middle);
		elog(NOTICE, "c_last = %s", my_c_last);
		elog(NOTICE, "c_balance = %s", c_balance);
#endif /* DEBUG */
	} else {
		PG_RETURN_INT32(-1);
	}

	/* Maybe this should be a join with the previous query. */
	sprintf(query, ORDER_STATUS_3, c_w_id, c_d_id, my_c_id);
#ifdef DEBUG
	elog(NOTICE, "%s", query);
#endif /* DEBUG */
	ret = SPI_exec(query, 0);
	if (ret == SPI_OK_SELECT && SPI_processed > 0) {
		tupdesc = SPI_tuptable->tupdesc;
		tuptable = SPI_tuptable;
	     	tuple = tuptable->vals[0];

		o_id = SPI_getvalue(tuple, tupdesc, 1);
		o_carrier_id = SPI_getvalue(tuple, tupdesc, 2);
		o_entry_d = SPI_getvalue(tuple, tupdesc, 3);
		o_ol_cnt = SPI_getvalue(tuple, tupdesc, 4);
#ifdef DEBUG
		elog(NOTICE, "o_id = %s", o_id);
		elog(NOTICE, "o_carrier_id = %s", o_carrier_id);
		elog(NOTICE, "o_entry_d = %s", o_entry_d);
		elog(NOTICE, "o_ol_cnt = %s", o_ol_cnt);
#endif /* DEBUG */
	} else {
		PG_RETURN_INT32(-1);
	}

	sprintf(query, ORDER_STATUS_4, c_w_id, c_d_id, o_id);
#ifdef DEBUG
	elog(NOTICE, "%s", query);
#endif /* DEBUG */
	ret = SPI_exec(query, 0);
	count = SPI_processed;
#ifdef DEBUG
	elog(NOTICE, "##  ol_i_id  ol_supply_w_id  ol_quantity  ol_amount  ol_delivery_d");
	elog(NOTICE, "--  -------  --------------  -----------  ---------  -------------");
#endif /* DEBUG */
	if (ret == SPI_OK_SELECT && SPI_processed > 0) {
		tupdesc = SPI_tuptable->tupdesc;
		tuptable = SPI_tuptable;

		for (j = 0; j < count; j++) {
	     		tuple = tuptable->vals[j];

			ol_i_id[j] = SPI_getvalue(tuple, tupdesc, 1);
			ol_supply_w_id[j] = SPI_getvalue(tuple, tupdesc, 2);
			ol_quantity[j] = SPI_getvalue(tuple, tupdesc, 3);
			ol_amount[j] = SPI_getvalue(tuple, tupdesc, 4);
			ol_delivery_d[j] = SPI_getvalue(tuple, tupdesc, 5);
#ifdef DEBUG
			elog(NOTICE, "%2d  %7s  %14s  %11s  %9.2f  %13s",
				j + 1, ol_i_id[j], ol_supply_w_id[j],
				ol_quantity[j], atof(ol_amount[j]),
				ol_delivery_d[j]);
#endif /* DEBUG */
		}
	} else {
		PG_RETURN_INT32(-1);
	}

	SPI_finish();
	PG_RETURN_INT32(1);
}

Datum payment(PG_FUNCTION_ARGS)
{
	/* Input variables. */
	int32 w_id = PG_GETARG_INT32(0);
	int32 d_id = PG_GETARG_INT32(1);
	int32 c_id = PG_GETARG_INT32(2);
	int32 c_w_id = PG_GETARG_INT32(3);
	int32 c_d_id = PG_GETARG_INT32(4);
	text *c_last = PG_GETARG_TEXT_P(5);
	float4 h_amount = PG_GETARG_FLOAT4(6);

	TupleDesc tupdesc;
	SPITupleTable *tuptable;
	HeapTuple tuple;

	int ret;
	char query[4096];
	char *w_name = NULL;
	char *w_street_1 = NULL;
	char *w_street_2 = NULL;
	char *w_city = NULL;
	char *w_state = NULL;
	char *w_zip = NULL;

	char *d_name = NULL;
	char *d_street_1 = NULL;
	char *d_street_2 = NULL;
	char *d_city = NULL;
	char *d_state = NULL;
	char *d_zip = NULL;

	char *tmp_c_id = NULL;
	int my_c_id = 0;
	int count;

	char *c_first = NULL;
	char *c_middle = NULL;
	char *my_c_last = NULL;
	char *c_street_1 = NULL;
	char *c_street_2 = NULL;
	char *c_city = NULL;
	char *c_state = NULL;
	char *c_zip = NULL;
	char *c_phone = NULL;
	char *c_since = NULL;
	char *c_credit = NULL;
	char *c_credit_lim = NULL;
	char *c_discount = NULL;
	char *c_balance = NULL;
	char *c_data = NULL;
	char *c_ytd_payment = NULL;

	char my_w_name[20];
	char my_d_name[20];

#ifdef DEBUG
	elog(NOTICE, "w_id = %d", w_id);
	elog(NOTICE, "d_id = %d", d_id);
	elog(NOTICE, "c_id = %d", c_id);
	elog(NOTICE, "c_w_id = %d", c_w_id);
	elog(NOTICE, "c_d_id = %d", c_d_id);
	elog(NOTICE, "c_last = %s",
		DatumGetCString(DirectFunctionCall1(textout,
		PointerGetDatum(c_last))));
	elog(NOTICE, "h_amount = %f", h_amount);
#endif /* DEBUG */

	SPI_connect();

	sprintf(query, PAYMENT_1, w_id);
#ifdef DEBUG
	elog(NOTICE, "%s", query);
#endif /* DEBUG */
	ret = SPI_exec(query, 0);
	if (ret == SPI_OK_SELECT && SPI_processed > 0) {
		tupdesc = SPI_tuptable->tupdesc;
		tuptable = SPI_tuptable;
	     	tuple = tuptable->vals[0];

		w_name = SPI_getvalue(tuple, tupdesc, 1);
		w_street_1 = SPI_getvalue(tuple, tupdesc, 2);
		w_street_2 = SPI_getvalue(tuple, tupdesc, 3);
		w_city = SPI_getvalue(tuple, tupdesc, 4);
		w_state = SPI_getvalue(tuple, tupdesc, 5);
		w_zip = SPI_getvalue(tuple, tupdesc, 6);
#ifdef DEBUG
		elog(NOTICE, "w_name = %s", w_name);
		elog(NOTICE, "w_street_1 = %s", w_street_1);
		elog(NOTICE, "w_street_2 = %s", w_street_2);
		elog(NOTICE, "w_city = %s", w_city);
		elog(NOTICE, "w_state = %s", w_state);
		elog(NOTICE, "w_zip = %s", w_zip);
#endif /* DEBUG */
	} else {
		PG_RETURN_INT32(-1);
	}

	sprintf(query, PAYMENT_2, h_amount, w_id);
#ifdef DEBUG
	elog(NOTICE, "%s", query);
#endif /* DEBUG */
	ret = SPI_exec(query, 0);
	if (ret != SPI_OK_UPDATE) {
		PG_RETURN_INT32(-1);
	}

	sprintf(query, PAYMENT_3, d_id, w_id);
#ifdef DEBUG
	elog(NOTICE, "%s", query);
#endif /* DEBUG */
	ret = SPI_exec(query, 0);
	if (ret == SPI_OK_SELECT && SPI_processed > 0) {
		tupdesc = SPI_tuptable->tupdesc;
		tuptable = SPI_tuptable;
	     	tuple = tuptable->vals[0];

		d_name = SPI_getvalue(tuple, tupdesc, 1);
		d_street_1 = SPI_getvalue(tuple, tupdesc, 2);
		d_street_2 = SPI_getvalue(tuple, tupdesc, 3);
		d_city = SPI_getvalue(tuple, tupdesc, 4);
		d_state = SPI_getvalue(tuple, tupdesc, 5);
		d_zip = SPI_getvalue(tuple, tupdesc, 6);
#ifdef DEBUG
		elog(NOTICE, "d_name = %s", d_name);
		elog(NOTICE, "d_street_1 = %s", d_street_1);
		elog(NOTICE, "d_street_2 = %s", d_street_2);
		elog(NOTICE, "d_city = %s", d_city);
		elog(NOTICE, "d_state = %s", d_state);
		elog(NOTICE, "d_zip = %s", d_zip);
#endif /* DEBUG */
	} else {
		PG_RETURN_INT32(-1);
	}

	sprintf(query, PAYMENT_4, h_amount, d_id, w_id);
#ifdef DEBUG
	elog(NOTICE, "%s", query);
#endif /* DEBUG */
	ret = SPI_exec(query, 0);
	if (ret != SPI_OK_UPDATE) {
		PG_RETURN_INT32(-1);
	}

	if (c_id == 0) {
		sprintf(query, PAYMENT_5, w_id, d_id,
			DatumGetCString(DirectFunctionCall1(textout,
			PointerGetDatum(c_last))));
#ifdef DEBUG
		elog(NOTICE, "%s", query);
#endif /* DEBUG */
		ret = SPI_exec(query, 0);
		count = SPI_processed;
		if (ret == SPI_OK_SELECT && SPI_processed > 0) {
			tupdesc = SPI_tuptable->tupdesc;
			tuptable = SPI_tuptable;
	     		tuple = tuptable->vals[count / 2];

			tmp_c_id = SPI_getvalue(tuple, tupdesc, 1);
#ifdef DEBUG
			elog(NOTICE, "c_id = %s, %d total, selected %d",
				tmp_c_id, count, count / 2);
#endif /* DEBUG */
			my_c_id = atoi(tmp_c_id);
		} else {
			PG_RETURN_INT32(-1);
		}
	} else {
		my_c_id = c_id;
	}

	sprintf(query, PAYMENT_6, c_w_id, c_d_id, my_c_id);
#ifdef DEBUG
	elog(NOTICE, "%s", query);
#endif /* DEBUG */
	ret = SPI_exec(query, 0);
	if (ret == SPI_OK_SELECT && SPI_processed > 0) {
		tupdesc = SPI_tuptable->tupdesc;
		tuptable = SPI_tuptable;
	     	tuple = tuptable->vals[0];

		c_first = SPI_getvalue(tuple, tupdesc, 1);
		c_middle = SPI_getvalue(tuple, tupdesc, 2);
		my_c_last = SPI_getvalue(tuple, tupdesc, 3);
		c_street_1 = SPI_getvalue(tuple, tupdesc, 4);
		c_street_2 = SPI_getvalue(tuple, tupdesc, 5);
		c_city = SPI_getvalue(tuple, tupdesc, 6);
		c_state = SPI_getvalue(tuple, tupdesc, 7);
		c_zip = SPI_getvalue(tuple, tupdesc, 8);
		c_phone = SPI_getvalue(tuple, tupdesc, 9);
		c_since = SPI_getvalue(tuple, tupdesc, 10);
		c_credit = SPI_getvalue(tuple, tupdesc, 11);
		c_credit_lim = SPI_getvalue(tuple, tupdesc, 12);
		c_discount = SPI_getvalue(tuple, tupdesc, 13);
		c_balance = SPI_getvalue(tuple, tupdesc, 14);
		c_data = SPI_getvalue(tuple, tupdesc, 15);
		c_ytd_payment = SPI_getvalue(tuple, tupdesc, 16);
#ifdef DEBUG
		elog(NOTICE, "c_first = %s", c_first);
		elog(NOTICE, "c_middle = %s", c_middle);
		elog(NOTICE, "c_last = %s", my_c_last);
		elog(NOTICE, "c_street_1 = %s", c_street_1);
		elog(NOTICE, "c_street_2 = %s", c_street_2);
		elog(NOTICE, "c_city = %s", c_city);
		elog(NOTICE, "c_state = %s", c_state);
		elog(NOTICE, "c_zip = %s", c_zip);
		elog(NOTICE, "c_phone = %s", c_phone);
		elog(NOTICE, "c_since = %s", c_since);
		elog(NOTICE, "c_credit = %s", c_credit);
		elog(NOTICE, "c_credit_lim = %s", c_credit_lim);
		elog(NOTICE, "c_discount = %s", c_discount);
		elog(NOTICE, "c_balance = %s", c_balance);
		elog(NOTICE, "c_data = %s", c_data);
		elog(NOTICE, "c_ytd_payment = %s", c_ytd_payment);
#endif /* DEBUG */
	} else {
		PG_RETURN_INT32(-1);
	}

	/* It's either "BC" or "GC". */
	if (c_credit[0] == 'G') {
		sprintf(query, PAYMENT_7_GC, h_amount, my_c_id, c_w_id, c_d_id);
#ifdef DEBUG
		elog(NOTICE, "%s", query);
#endif /* DEBUG */
		ret = SPI_exec(query, 0);
		if (ret != SPI_OK_UPDATE) {
			PG_RETURN_INT32(-1);
		}
	} else {
		char my_c_data[1000];

		sprintf(my_c_data, "%d %d %d %d %d %f ", my_c_id, c_d_id,
			c_w_id, d_id, w_id, h_amount);
		/* Copy and escape all at once! */
		escape_str(c_data, my_c_data);

		sprintf(query, PAYMENT_7_BC, h_amount, my_c_data, my_c_id,
			c_w_id, c_d_id);
#ifdef DEBUG
		elog(NOTICE, "%s", query);
#endif /* DEBUG */
		ret = SPI_exec(query, 0);
		if (ret != SPI_OK_UPDATE) {
			PG_RETURN_INT32(-1);
		}
	}

	/* Escape special characters. */
	escape_str(w_name, my_w_name);
	escape_str(d_name, my_d_name);

	sprintf(query, PAYMENT_8, my_c_id, c_d_id, c_w_id, d_id, w_id,
		h_amount, my_w_name, my_d_name);
#ifdef DEBUG
	elog(NOTICE, "%s", query);
#endif /* DEBUG */
	ret = SPI_exec(query, 0);
	if (ret != SPI_OK_INSERT) {
		PG_RETURN_INT32(-1);
	}

	SPI_finish();
	PG_RETURN_INT32(1);
}

Datum stock_level(PG_FUNCTION_ARGS)
{
	/* Input variables. */
	int32 w_id = PG_GETARG_INT32(0);
	int32 d_id = PG_GETARG_INT32(1);
	int32 threshold = PG_GETARG_INT32(2);

	TupleDesc tupdesc;
	SPITupleTable *tuptable;
	HeapTuple tuple;

	int d_next_o_id = 0;
	int low_stock = 0;
	int ret;
	char query[256];
	char *buf;

	SPI_connect();

	sprintf(query, STOCK_LEVEL_1, w_id, d_id);
#ifdef DEBUG
	elog(NOTICE, "%s", query);
#endif /* DEBUG */
	ret = SPI_exec(query, 0);
	if (ret == SPI_OK_SELECT && SPI_processed > 0) {
		tupdesc = SPI_tuptable->tupdesc;
		tuptable = SPI_tuptable;
	     	tuple = tuptable->vals[0];

		buf = SPI_getvalue(tuple, tupdesc, 1);
#ifdef DEBUG
		elog(NOTICE, "d_next_o_id = %s", buf);
#endif /* DEBUG */
		d_next_o_id = atoi(buf);
	} else {
		PG_RETURN_INT32(-1);
	}

	sprintf(query, STOCK_LEVEL_2, w_id, d_id, threshold, d_next_o_id - 20,
		d_next_o_id - 1);
#ifdef DEBUG
	elog(NOTICE, "%s", query);
#endif /* DEBUG */
	ret = SPI_exec(query, 0);
	if (ret == SPI_OK_SELECT && SPI_processed > 0) {
		tupdesc = SPI_tuptable->tupdesc;
		tuptable = SPI_tuptable;
	     	tuple = tuptable->vals[0];

		buf = SPI_getvalue(tuple, tupdesc, 1);
#ifdef DEBUG
		elog(NOTICE, "low_stock = %s", buf);
#endif /* DEBUG */
		low_stock = atoi(buf);
	} else {
		PG_RETURN_INT32(-1);
	}

	SPI_finish();
	PG_RETURN_INT32(low_stock);
}
