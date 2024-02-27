/*
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright The DBT-2 Authors
 *
 * Based on TPC-C Standard Specification Revision 5.11 Clause 2.4.2.
 */

#include <catalog/pg_type.h>   /* for OIDs */
#include <executor/executor.h> /* for GetAttributeByName() */
#include <executor/spi.h>	   /* this should include most necessary APIs */
#include <fmgr.h>
#include <funcapi.h> /* for returning set of rows in order_status */
#include <postgres.h>
#include <sys/types.h>
#include <unistd.h>
#include <utils/builtins.h>

#include "dbt2common.h"

/*
 * New Order transaction SQL statements.
 */

#define NEW_ORDER_1 statements[0].plan
#define NEW_ORDER_2 statements[1].plan
#define NEW_ORDER_4 statements[2].plan
#define NEW_ORDER_5 statements[3].plan
#define NEW_ORDER_6 statements[4].plan
#define NEW_ORDER_7 statements[5].plan
#define NEW_ORDER_8 statements[6].plan
#define NEW_ORDER_9 statements[7].plan
#define NEW_ORDER_10 statements[8].plan

const char s_dist[10][11] = {"s_dist_01", "s_dist_02", "s_dist_03", "s_dist_04",
							 "s_dist_05", "s_dist_06", "s_dist_07", "s_dist_08",
							 "s_dist_09", "s_dist_10"};

static cached_statement statements[] = {

		{/* NEW_ORDER_1 */
		 "SELECT w_tax\n"
		 "FROM warehouse\n"
		 "WHERE w_id = $1",
		 1,
		 {INT4OID}},

		{/* NEW_ORDER_2 */
		 "UPDATE district\n"
		 "SET d_next_o_id = d_next_o_id + 1\n"
		 "WHERE d_w_id = $1\n"
		 "  AND d_id = $2\n"
		 "RETURNING d_tax, d_next_o_id",
		 2,
		 {INT4OID, INT4OID}},

		{/* NEW_ORDER_4 */
		 "SELECT c_discount, c_last, c_credit\n"
		 "FROM customer\n"
		 "WHERE c_w_id = $1\n"
		 "  AND c_d_id = $2\n"
		 "  AND c_id = $3",
		 3,
		 {INT4OID, INT4OID, INT4OID}},

		{/* NEW_ORDER_5 */
		 "INSERT INTO new_order (no_o_id, no_w_id, no_d_id)\n"
		 "VALUES ($1, $2, $3)",
		 3,
		 {INT4OID, INT4OID, INT4OID}},

		{/* NEW_ORDER_6 */
		 "INSERT INTO orders (o_id, o_d_id, o_w_id, o_c_id, o_entry_d,\n"
		 "		    o_carrier_id, o_ol_cnt, o_all_local)\n"
		 "VALUES ($1, $2, $3, $4, current_timestamp, NULL, $5, $6)",
		 6,
		 {INT4OID, INT4OID, INT4OID, INT4OID, INT4OID, INT4OID}},

		{/* NEW_ORDER_7 */
		 "SELECT i_price, i_name, i_data\n"
		 "FROM item\n"
		 "WHERE i_id = $1",
		 1,
		 {INT4OID}},

		{/* NEW_ORDER_8 */
		 "SELECT s_quantity, $1, s_data\n"
		 "FROM stock\n"
		 "WHERE s_i_id = $2\n"
		 "  AND s_w_id = $3",
		 3,
		 {TEXTOID, INT4OID, INT4OID}},

		{/* NEW_ORDER_9 */
		 "UPDATE stock\n"
		 "SET s_quantity = s_quantity - $1\n"
		 "WHERE s_i_id = $2\n"
		 "  AND s_w_id = $3",
		 3,
		 {INT4OID, INT4OID, INT4OID}},

		{/* NEW_ORDER_10 */
		 "INSERT INTO order_line (ol_o_id, ol_d_id, ol_w_id, "
		 "ol_number,\n"
		 "			ol_i_id, ol_supply_w_id, ol_delivery_d,\n"
		 "			ol_quantity, ol_amount, ol_dist_info)\n"
		 "VALUES ($1, $2, $3, $4, $5, $6, NULL, $7, $8, $9)",
		 9,
		 {INT4OID, INT4OID, INT4OID, INT4OID, INT4OID, INT4OID, INT4OID,
		  FLOAT4OID, TEXTOID}},

		{NULL}};

/* Prototypes to prevent potential gcc warnings. */
Datum new_order(PG_FUNCTION_ARGS);

PG_FUNCTION_INFO_V1(new_order);

typedef struct {
	int ol_supply_w_id;
	int ol_i_id;
	char i_name[4 * (I_NAME_LEN + 1)];
	int ol_quantity;
	int s_quantity;
	float i_price;
	float ol_amount;
	char brand_generic;
} no_order_line;

Datum new_order(PG_FUNCTION_ARGS) {
	FuncCallContext *funcctx;
	int call_cntr;
	int max_calls;
	AttInMetadata *attinmeta;

	if (SRF_IS_FIRSTCALL()) {
		MemoryContext oldcontext;

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

		char *w_tax = NULL;

		char *d_tax = NULL;
		int32 d_next_o_id;

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

		Datum args[9];
		char nulls[9] = {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '};

		no_order_line *pp;

		/* Loop through the last set of parameters. */
		for (i = 0, j = 5; i < 15; i++) {
			bool isnull;
			HeapTupleHeader t = PG_GETARG_HEAPTUPLEHEADER(j++);
			ol_i_id[i] =
					DatumGetInt32(GetAttributeByName(t, "ol_i_id", &isnull));
			ol_supply_w_id[i] = DatumGetInt32(
					GetAttributeByName(t, "ol_supply_w_id", &isnull));
			ol_quantity[i] = DatumGetInt32(
					GetAttributeByName(t, "ol_quantity", &isnull));
		}

		elog(DEBUG1, "IN w_id = %d", w_id);
		elog(DEBUG1, "IN d_id = %d", d_id);
		elog(DEBUG1, "IN c_id = %d", c_id);
		elog(DEBUG1, "IN o_all_local = %d", o_all_local);
		elog(DEBUG1, "IN o_ol_cnt = %d", o_ol_cnt);

		elog(DEBUG1, "IN ##  ol_i_id  ol_supply_w_id  ol_quantity");
		elog(DEBUG1, "IN --  -------  --------------  -----------");
		for (i = 0; i < o_ol_cnt; i++) {
			elog(DEBUG1, "IN %2d  %7d  %14d  %11d", i + 1, ol_i_id[i],
				 ol_supply_w_id[i], ol_quantity[i]);
		}

		funcctx = SRF_FIRSTCALL_INIT();
		oldcontext = MemoryContextSwitchTo(funcctx->multi_call_memory_ctx);
		if (get_call_result_type(fcinfo, NULL, &tupdesc) !=
			TYPEFUNC_COMPOSITE) {
			ereport(ERROR, (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
							errmsg("delivery cannot accept type record")));
		}
		attinmeta = TupleDescGetAttInMetadata(tupdesc);
		funcctx->attinmeta = attinmeta;

		SPI_connect();

		plan_queries(statements);

		funcctx->user_fctx = MemoryContextAlloc(
				funcctx->multi_call_memory_ctx,
				sizeof(no_order_line) * o_ol_cnt);

		args[0] = Int32GetDatum(w_id);
		ret = SPI_execute_plan(NEW_ORDER_1, args, nulls, true, 0);
		if (ret == SPI_OK_SELECT && SPI_processed > 0) {
			tupdesc = SPI_tuptable->tupdesc;
			tuptable = SPI_tuptable;
			tuple = tuptable->vals[0];

			w_tax = SPI_getvalue(tuple, tupdesc, 1);
			elog(DEBUG1, "w_tax = %s", w_tax);
		} else {
			ereport(ERROR, (errcode(ERRCODE_INTERNAL_ERROR),
							errmsg("NEW_ORDER_1 failed")));
		}

		args[0] = Int32GetDatum(w_id);
		args[1] = Int32GetDatum(d_id);
		ret = SPI_execute_plan(NEW_ORDER_2, args, nulls, false, 0);
		if (ret == SPI_OK_UPDATE_RETURNING && SPI_processed > 0) {
			tupdesc = SPI_tuptable->tupdesc;
			tuptable = SPI_tuptable;
			tuple = tuptable->vals[0];

			d_tax = SPI_getvalue(tuple, tupdesc, 1);
			d_next_o_id = atoi(SPI_getvalue(tuple, tupdesc, 2));
			elog(DEBUG1, "d_tax = %s", d_tax);
			elog(DEBUG1, "d_next_o_id = %d", d_next_o_id);
		} else {
			ereport(ERROR, (errcode(ERRCODE_INTERNAL_ERROR),
							errmsg("NEW_ORDER_3 failed")));
		}

		args[0] = Int32GetDatum(w_id);
		args[1] = Int32GetDatum(d_id);
		args[2] = Int32GetDatum(c_id);
		ret = SPI_execute_plan(NEW_ORDER_4, args, nulls, false, 0);
		if (ret == SPI_OK_SELECT && SPI_processed > 0) {
			tupdesc = SPI_tuptable->tupdesc;
			tuptable = SPI_tuptable;
			tuple = tuptable->vals[0];

			c_discount = SPI_getvalue(tuple, tupdesc, 1);
			c_last = SPI_getvalue(tuple, tupdesc, 2);
			c_credit = SPI_getvalue(tuple, tupdesc, 3);
			elog(DEBUG1, "c_discount = %s", c_discount);
			elog(DEBUG1, "c_last = %s", c_last);
			elog(DEBUG1, "c_credit = %s", c_credit);
		} else {
			ereport(ERROR, (errcode(ERRCODE_INTERNAL_ERROR),
							errmsg("NEW_ORDER_4 failed")));
		}

		args[0] = Int32GetDatum(d_next_o_id);
		args[1] = Int32GetDatum(w_id);
		args[2] = Int32GetDatum(d_id);
		ret = SPI_execute_plan(NEW_ORDER_5, args, nulls, false, 0);
		if (ret != SPI_OK_INSERT) {
			ereport(ERROR, (errcode(ERRCODE_INTERNAL_ERROR),
							errmsg("NEW_ORDER_5 failed")));
		}

		args[0] = Int32GetDatum(d_next_o_id);
		args[1] = Int32GetDatum(d_id);
		args[2] = Int32GetDatum(w_id);
		args[3] = Int32GetDatum(c_id);
		args[4] = Int32GetDatum(o_ol_cnt);
		args[5] = Int32GetDatum(o_all_local);
		ret = SPI_execute_plan(NEW_ORDER_6, args, nulls, false, 0);
		if (ret != SPI_OK_INSERT) {
			ereport(ERROR, (errcode(ERRCODE_INTERNAL_ERROR),
							errmsg("NEW_ORDER_6 failed")));
		}

		pp = (no_order_line *) funcctx->user_fctx;
		for (i = 0; i < o_ol_cnt; i++) {
			int decr_quantity;

			args[0] = Int32GetDatum(ol_i_id[i]);
			ret = SPI_execute_plan(NEW_ORDER_7, args, nulls, true, 0);
			/*
			 * Shouldn't have to check if ol_i_id is 0, but if the row
			 * doesn't exist, the query still passes.
			 */
			if (ol_i_id[i] != 0 && ret == SPI_OK_SELECT && SPI_processed > 0) {
				tupdesc = SPI_tuptable->tupdesc;
				tuptable = SPI_tuptable;
				tuple = tuptable->vals[0];

				i_price[i] = SPI_getvalue(tuple, tupdesc, 1);
				pp[i].i_price = atof(i_price[i]);
				i_name[i] = SPI_getvalue(tuple, tupdesc, 2);
				strncpy(pp[i].i_name, i_name[i], 4 * I_NAME_LEN);
				i_data[i] = SPI_getvalue(tuple, tupdesc, 3);
				elog(DEBUG1, "i_price[%d] = %s", i, i_price[i]);
				elog(DEBUG1, "i_name[%d] = %s", i, i_name[i]);
				elog(DEBUG1, "i_data[%d] = %s", i, i_data[i]);
			} else {
				/* Item doesn't exist, rollback transaction. */
				ereport(ERROR, (errcode(ERRCODE_INTERNAL_ERROR),
								errmsg("NEW_ORDER_8 failed: item not found")));
				SPI_finish();
				PG_RETURN_NULL();
			}

			ol_amount[i] = atof(i_price[i]) * (float) ol_quantity[i];

			args[0] = CStringGetTextDatum(s_dist[d_id - 1]);
			args[1] = Int32GetDatum(ol_i_id[i]);
			args[2] = Int32GetDatum(w_id);
			ret = SPI_execute_plan(NEW_ORDER_8, args, nulls, true, 0);
			if (ret == SPI_OK_SELECT && SPI_processed > 0) {
				tupdesc = SPI_tuptable->tupdesc;
				tuptable = SPI_tuptable;
				tuple = tuptable->vals[0];

				s_quantity[i] = SPI_getvalue(tuple, tupdesc, 1);
				pp[i].s_quantity = atoi(s_quantity[i]);
				my_s_dist[i] = SPI_getvalue(tuple, tupdesc, 2);
				s_data[i] = SPI_getvalue(tuple, tupdesc, 3);
				elog(DEBUG1, "s_quantity[%d] = %s", i, s_quantity[i]);
				elog(DEBUG1, "my_s_dist[%d] = %s", i, my_s_dist[i]);
				elog(DEBUG1, "s_data[%d] = %s", i, s_data[i]);
			} else {
				ereport(ERROR, (errcode(ERRCODE_INTERNAL_ERROR),
								errmsg("NEW_ORDER_8 failed")));
			}
			order_amount += ol_amount[i];

			if (atoi(s_quantity[i]) > ol_quantity[i] + 10) {
				decr_quantity = ol_quantity[i];
			} else {
				decr_quantity = ol_quantity[i] - 91;
			}
			args[0] = Int32GetDatum(decr_quantity);
			args[1] = Int32GetDatum(ol_i_id[i]);
			args[2] = Int32GetDatum(w_id);
			ret = SPI_execute_plan(NEW_ORDER_9, args, nulls, false, 0);
			if (ret != SPI_OK_UPDATE) {
				ereport(ERROR, (errcode(ERRCODE_INTERNAL_ERROR),
								errmsg("NEW_ORDER_9 failed")));
			}

			args[0] = Int32GetDatum(d_next_o_id);
			args[1] = Int32GetDatum(d_id);
			args[2] = Int32GetDatum(w_id);
			args[3] = Int32GetDatum(i + 1);
			args[4] = pp[i].ol_i_id = Int32GetDatum(ol_i_id[i]);
			args[5] = pp[i].ol_supply_w_id = Int32GetDatum(ol_supply_w_id[i]);
			args[6] = pp[i].ol_quantity = Int32GetDatum(ol_quantity[i]);
			args[7] = pp[i].ol_amount = Float4GetDatum(ol_amount[i]);
			args[8] = CStringGetTextDatum(my_s_dist[i]);
			ret = SPI_execute_plan(NEW_ORDER_10, args, nulls, false, 0);
			if (ret != SPI_OK_INSERT) {
				ereport(ERROR, (errcode(ERRCODE_INTERNAL_ERROR),
								errmsg("NEW_ORDER_10 failed")));
			}

			if (strstr(i_data[i], "ORIGINAL") &&
				strstr(s_data[i], "ORIGINAL")) {
				pp[i].brand_generic = 'B';
			} else {
				pp[i].brand_generic = 'G';
			}
			elog(DEBUG1, "brand_generic[%d] = %c", i, pp[i].brand_generic);
		}
		funcctx->max_calls = o_ol_cnt;

		SPI_finish();
		MemoryContextSwitchTo(oldcontext);
	}

	funcctx = SRF_PERCALL_SETUP();

	call_cntr = funcctx->call_cntr;
	max_calls = funcctx->max_calls;
	attinmeta = funcctx->attinmeta;

	if (call_cntr < max_calls) {
		HeapTuple tuple;
		Datum result;
		char **values = (char **) palloc(8 * sizeof(char *));

		no_order_line *pp = (no_order_line *) funcctx->user_fctx;

		values[0] = (char *) palloc(11 * sizeof(char));
		values[1] = (char *) palloc(11 * sizeof(char));
		values[2] = (char *) palloc(4 * (I_NAME_LEN + 1) * sizeof(char));
		values[3] = (char *) palloc(11 * sizeof(char));
		values[4] = (char *) palloc(11 * sizeof(char));
		values[5] = (char *) palloc(11 * sizeof(char));
		values[6] = (char *) palloc(11 * sizeof(char));
		values[7] = (char *) palloc(2 * sizeof(char));

		snprintf(values[0], 10, "%d", pp[funcctx->call_cntr].ol_supply_w_id);
		snprintf(values[1], 10, "%d", pp[funcctx->call_cntr].ol_i_id);
		strncpy(values[2], pp[funcctx->call_cntr].i_name, 4 * I_NAME_LEN);
		snprintf(values[3], 10, "%d", pp[funcctx->call_cntr].ol_quantity);
		snprintf(values[4], 10, "%d", pp[funcctx->call_cntr].s_quantity);
		snprintf(values[5], 10, "%f", pp[funcctx->call_cntr].i_price);
		snprintf(values[6], 10, "%f", pp[funcctx->call_cntr].ol_amount);
		values[7][0] = pp[funcctx->call_cntr].brand_generic;
		values[7][1] = '\0';

		tuple = BuildTupleFromCStrings(attinmeta, values);
		result = HeapTupleGetDatum(tuple);
		SRF_RETURN_NEXT(funcctx, result);
	} else {
		SRF_RETURN_DONE(funcctx);
	}
}
