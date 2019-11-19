/*
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright (C) 2003-2006 Mark Wong & Open Source Development Labs, Inc.
 *
 * Based on TPC-C Standard Specification Revision 5.0 Clause 2.8.2.
 */

#include <sys/types.h>
#include <unistd.h>
#include <postgres.h>
#include <fmgr.h>
#include <catalog/pg_type.h> /* for type OIDs */
#include <executor/spi.h> /* this should include most necessary APIs */
#include <funcapi.h> /* for returning set of rows in order_status */
#include <utils/builtins.h>

#include "dbt2common.h"

typedef struct
{
    char w_street_1[21];
    char w_street_2[21];
    char w_city[21];
    char w_state[3];
    char w_zip[10];
    char d_street_1[21];
    char d_street_2[21];
    char d_city[21];
    char d_state[3];
    char d_zip[10];
    char c_first[17];
    char c_middle[3];
    char c_last[17];
    char c_street_1[21];
    char c_street_2[21];
    char c_city[21];
    char c_state[3];
    char c_zip[10];
    char c_phone[17];
    char c_since[29];
    char c_credit[3];
    float c_credit_lim;
    float c_discount;
    float c_balance;
    char c_data[201];
    char h_date[29];
} payment_row;

/*
 * Payment transaction SQL statements.
 */

#define PAYMENT_1 statements[0].plan
#define PAYMENT_3 statements[1].plan
#define PAYMENT_5 statements[2].plan
#define PAYMENT_6 statements[3].plan
#define PAYMENT_7_GC statements[4].plan
#define PAYMENT_7_BC statements[5].plan
#define PAYMENT_8 statements[6].plan

static cached_statement statements[] =
{
	{ /* PAYMENT_1 */
	"UPDATE warehouse\n" \
	"SET w_ytd = w_ytd + $1\n" \
	"WHERE w_id = $2\n" \
	"RETURNING w_name, w_street_1, w_street_2, w_city, w_state, w_zip",
	2, { FLOAT4OID, INT4OID }
	},

	{ /* PAYMENT_3 */
	"UPDATE district\n" \
	"SET d_ytd = d_ytd + $1\n" \
	"WHERE d_id = $2\n" \
	"  AND d_w_id = $3\n" \
	"RETURNING d_name, d_street_1, d_street_2, d_city, d_state, d_zip",
	3, { FLOAT4OID, INT4OID, INT4OID }
	},

	{ /* PAYMENT_5 */
	"SELECT c_id\n" \
	"FROM customer\n" \
	"WHERE c_w_id = $1\n" \
	"  AND c_d_id = $2\n" \
	"  AND c_last = $3\n" \
	"ORDER BY c_first ASC",
	3, { INT4OID, INT4OID, TEXTOID }
	},

	{ /* PAYMENT_6 */
	"SELECT c_first, c_middle, c_last, c_street_1, c_street_2, c_city,\n" \
	"       c_state, c_zip, c_phone, c_since, c_credit,\n" \
	"       c_credit_lim, c_discount, c_balance, c_data, c_ytd_payment\n" \
	"FROM customer\n" \
	"WHERE c_w_id = $1\n" \
	"  AND c_d_id = $2\n" \
	"  AND c_id = $3",
	3, { INT4OID, INT4OID, INT4OID }
	},

	{ /* PAYMENT_7_GC */
	"UPDATE customer\n" \
	"SET c_balance = c_balance - $1,\n" \
	"    c_ytd_payment = c_ytd_payment + 1\n" \
	"WHERE c_id = $2\n" \
	"  AND c_w_id = $3\n" \
	"  AND c_d_id = $4",
	4, { FLOAT4OID, INT4OID, INT4OID, INT4OID }
	},

	{ /* PAYMENT_7_BC */
	"UPDATE customer\n" \
	"SET c_balance = c_balance - $1,\n" \
	"    c_ytd_payment = c_ytd_payment + 1,\n" \
	"    c_data = $2\n" \
	"WHERE c_id = $3\n" \
	"  AND c_w_id = $4\n" \
	"  AND c_d_id = $5",
	5, { FLOAT4OID, TEXTOID, INT4OID, INT4OID, INT4OID }
	},

	{ /* PAYMENT_8 */
	"INSERT INTO history (h_c_id, h_c_d_id, h_c_w_id, h_d_id, h_w_id,\n" \
	"		     h_date, h_amount, h_data)\n" \
	"VALUES ($1, $2, $3, $4, $5, current_timestamp, $6, $7 || '    ' || $8)",
	8, { INT4OID, INT4OID, INT4OID, INT4OID, INT4OID, FLOAT4OID, TEXTOID, TEXTOID }
	},

	{ NULL }
};

/* Prototypes to prevent potential gcc warnings. */
Datum payment(PG_FUNCTION_ARGS);

PG_FUNCTION_INFO_V1(payment);

Datum payment(PG_FUNCTION_ARGS)
{
	FuncCallContext *funcctx;
	int call_cntr;
	int max_calls;
	AttInMetadata *attinmeta;

	if (SRF_IS_FIRSTCALL())
	{
		MemoryContext oldcontext;

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

		Datum	args[8];
		char	nulls[8] = { ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ' };

		payment_row *pp;

		elog(DEBUG1, "w_id = %d", w_id);
		elog(DEBUG1, "d_id = %d", d_id);
		elog(DEBUG1, "c_id = %d", c_id);
		elog(DEBUG1, "c_w_id = %d", c_w_id);
		elog(DEBUG1, "c_d_id = %d", c_d_id);
		elog(DEBUG1, "c_last = %s",
				DatumGetCString(DirectFunctionCall1(textout,
				PointerGetDatum(c_last))));
		elog(DEBUG1, "h_amount = %f", h_amount);

		funcctx = SRF_FIRSTCALL_INIT();
		oldcontext = MemoryContextSwitchTo(funcctx->multi_call_memory_ctx);
		if (get_call_result_type(fcinfo, NULL, &tupdesc) != TYPEFUNC_COMPOSITE)
			ereport(ERROR, (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
					errmsg("delivery cannot accept type record")));
		attinmeta = TupleDescGetAttInMetadata(tupdesc);
		funcctx->attinmeta = attinmeta;

		SPI_connect();

		plan_queries(statements);

		funcctx->user_fctx = MemoryContextAlloc(funcctx->multi_call_memory_ctx,
					sizeof(payment_row));
		funcctx->max_calls = 0;
		pp = (payment_row *) funcctx->user_fctx;

		args[0] = Float4GetDatum(h_amount);
		args[1] = Int32GetDatum(w_id);
		ret = SPI_execute_plan(PAYMENT_1, args, nulls, false, 0);
		if (ret == SPI_OK_UPDATE_RETURNING && SPI_processed > 0) {
			tupdesc = SPI_tuptable->tupdesc;
			tuptable = SPI_tuptable;
			tuple = tuptable->vals[0];

			w_name = SPI_getvalue(tuple, tupdesc, 1);
			w_street_1 = SPI_getvalue(tuple, tupdesc, 2);
			strncpy(pp->w_street_1, w_street_1, 20);
			w_street_2 = SPI_getvalue(tuple, tupdesc, 3);
			strncpy(pp->w_street_2, w_street_2, 20);
			w_city = SPI_getvalue(tuple, tupdesc, 4);
			strncpy(pp->w_city, w_city, 20);
			w_state = SPI_getvalue(tuple, tupdesc, 5);
			strncpy(pp->w_state, w_state, 2);
			w_zip = SPI_getvalue(tuple, tupdesc, 6);
			strncpy(pp->w_zip, w_zip, 9);
			elog(DEBUG1, "w_name = %s", w_name);
			elog(DEBUG1, "w_street_1 = %s", w_street_1);
			elog(DEBUG1, "w_street_2 = %s", w_street_2);
			elog(DEBUG1, "w_city = %s", w_city);
			elog(DEBUG1, "w_state = %s", w_state);
			elog(DEBUG1, "w_zip = %s", w_zip);
		} else {
			ereport(ERROR, (errcode(ERRCODE_INTERNAL_ERROR),
				 errmsg("PAYMENT_1 failed")));
		}

		args[0] = Float4GetDatum(h_amount);
		args[1] = Int32GetDatum(d_id);
		args[2] = Int32GetDatum(w_id);
		ret = SPI_execute_plan(PAYMENT_3, args, nulls, false, 0);
		if (ret == SPI_OK_UPDATE_RETURNING && SPI_processed > 0) {
			tupdesc = SPI_tuptable->tupdesc;
			tuptable = SPI_tuptable;
			tuple = tuptable->vals[0];

			d_name = SPI_getvalue(tuple, tupdesc, 1);
			d_street_1 = SPI_getvalue(tuple, tupdesc, 2);
			strncpy(pp->d_street_1, d_street_1, 20);
			d_street_2 = SPI_getvalue(tuple, tupdesc, 3);
			strncpy(pp->d_street_2, d_street_2, 20);
			d_city = SPI_getvalue(tuple, tupdesc, 4);
			strncpy(pp->d_city, d_city, 20);
			d_state = SPI_getvalue(tuple, tupdesc, 5);
			strncpy(pp->d_state, d_state, 2);
			d_zip = SPI_getvalue(tuple, tupdesc, 6);
			strncpy(pp->d_zip, d_zip, 9);
			elog(DEBUG1, "d_name = %s", d_name);
			elog(DEBUG1, "d_street_1 = %s", d_street_1);
			elog(DEBUG1, "d_street_2 = %s", d_street_2);
			elog(DEBUG1, "d_city = %s", d_city);
			elog(DEBUG1, "d_state = %s", d_state);
			elog(DEBUG1, "d_zip = %s", d_zip);
		} else {
			ereport(ERROR, (errcode(ERRCODE_INTERNAL_ERROR),
				 errmsg("PAYMENT_3 failed")));
		}

		if (c_id == 0) {
			args[0] = Int32GetDatum(w_id);
			args[1] = Int32GetDatum(d_id);
			args[2] = PointerGetDatum(c_last);
			ret = SPI_execute_plan(PAYMENT_5, args, nulls, true, 0);
			count = SPI_processed;
			if (ret == SPI_OK_SELECT && SPI_processed > 0) {
				tupdesc = SPI_tuptable->tupdesc;
				tuptable = SPI_tuptable;
				tuple = tuptable->vals[count / 2];

				tmp_c_id = SPI_getvalue(tuple, tupdesc, 1);
				elog(DEBUG1, "c_id = %s, %d total, selected %d",
						tmp_c_id, count, count / 2);
				my_c_id = atoi(tmp_c_id);
			} else {
				ereport(ERROR, (errcode(ERRCODE_INTERNAL_ERROR),
					 errmsg("PAYMENT_5 failed")));
			}
		} else {
			my_c_id = c_id;
		}

		args[0] = Int32GetDatum(c_w_id);
		args[1] = Int32GetDatum(c_d_id);
		args[2] = Int32GetDatum(my_c_id);
		ret = SPI_execute_plan(PAYMENT_6, args, nulls, true, 0);
		if (ret == SPI_OK_SELECT && SPI_processed > 0) {
			tupdesc = SPI_tuptable->tupdesc;
			tuptable = SPI_tuptable;
			tuple = tuptable->vals[0];

			c_first = SPI_getvalue(tuple, tupdesc, 1);
			strncpy(pp->c_first, c_first, 16);
			c_middle = SPI_getvalue(tuple, tupdesc, 2);
			strncpy(pp->c_middle, c_middle, 2);
			my_c_last = SPI_getvalue(tuple, tupdesc, 3);
			strncpy(pp->c_last, my_c_last, 16);
			c_street_1 = SPI_getvalue(tuple, tupdesc, 4);
			strncpy(pp->c_street_1, c_street_1, 20);
			c_street_2 = SPI_getvalue(tuple, tupdesc, 5);
			strncpy(pp->c_street_2, c_street_2, 20);
			c_city = SPI_getvalue(tuple, tupdesc, 6);
			strncpy(pp->c_city, c_city, 20);
			c_state = SPI_getvalue(tuple, tupdesc, 7);
			strncpy(pp->c_state, c_state, 2);
			c_zip = SPI_getvalue(tuple, tupdesc, 8);
			strncpy(pp->c_zip, c_zip, 9);
			c_phone = SPI_getvalue(tuple, tupdesc, 9);
			strncpy(pp->c_phone, c_phone, 16);
			c_since = SPI_getvalue(tuple, tupdesc, 10);
			strncpy(pp->c_since, c_since, 28);
			c_credit = SPI_getvalue(tuple, tupdesc, 11);
			strncpy(pp->c_credit, c_credit, 2);
			c_credit_lim = SPI_getvalue(tuple, tupdesc, 12);
			pp->c_credit_lim = atof(c_credit_lim);
			c_discount = SPI_getvalue(tuple, tupdesc, 13);
			pp->c_discount = atof(c_discount);
			c_balance = SPI_getvalue(tuple, tupdesc, 14);
			pp->c_balance = atof(c_balance);
			c_data = SPI_getvalue(tuple, tupdesc, 15);
			strncpy(pp->c_data, c_data, 200);
			c_ytd_payment = SPI_getvalue(tuple, tupdesc, 16);
			elog(DEBUG1, "c_first = %s", c_first);
			elog(DEBUG1, "c_middle = %s", c_middle);
			elog(DEBUG1, "c_last = %s", my_c_last);
			elog(DEBUG1, "c_street_1 = %s", c_street_1);
			elog(DEBUG1, "c_street_2 = %s", c_street_2);
			elog(DEBUG1, "c_city = %s", c_city);
			elog(DEBUG1, "c_state = %s", c_state);
			elog(DEBUG1, "c_zip = %s", c_zip);
			elog(DEBUG1, "c_phone = %s", c_phone);
			elog(DEBUG1, "c_since = %s", c_since);
			elog(DEBUG1, "c_credit = %s", c_credit);
			elog(DEBUG1, "c_credit_lim = %s", c_credit_lim);
			elog(DEBUG1, "c_discount = %s", c_discount);
			elog(DEBUG1, "c_balance = %s", c_balance);
			elog(DEBUG1, "c_data = %s", c_data);
			elog(DEBUG1, "c_ytd_payment = %s", c_ytd_payment);
		} else {
			ereport(ERROR, (errcode(ERRCODE_INTERNAL_ERROR),
				 errmsg("PAYMENT_6 failed")));
		}

		/* It's either "BC" or "GC". */
		if (c_credit[0] == 'G') {
			args[0] = Float4GetDatum(h_amount);
			args[1] = Int32GetDatum(my_c_id);
			args[2] = Int32GetDatum(c_w_id);
			args[3] = Int32GetDatum(c_d_id);
			ret = SPI_execute_plan(PAYMENT_7_GC, args, nulls, false, 0);
		} else {
			char my_c_data[501];

			snprintf(my_c_data, 500, "%d %d %d %d %d %f %s", my_c_id, c_d_id,
					c_w_id, d_id, w_id, h_amount, c_data);

			args[0] = Float4GetDatum(h_amount);
			args[1] = CStringGetTextDatum(my_c_data);
			args[2] = Int32GetDatum(my_c_id);
			args[3] = Int32GetDatum(c_w_id);
			args[4] = Int32GetDatum(c_d_id);
			ret = SPI_execute_plan(PAYMENT_7_BC, args, nulls, false, 0);
		}
		if (ret != SPI_OK_UPDATE) {
			ereport(ERROR, (errcode(ERRCODE_INTERNAL_ERROR),
				 errmsg("PAYMENT_7_BC failed")));
		}

		args[0] = Int32GetDatum(my_c_id);
		args[1] = Int32GetDatum(c_d_id);
		args[2] = Int32GetDatum(c_w_id);
		args[3] = Int32GetDatum(d_id);
		args[4] = Int32GetDatum(w_id);
		args[5] = Float4GetDatum(h_amount);
		args[6] = CStringGetTextDatum(w_name);
		args[7] = CStringGetTextDatum(d_name);
		ret = SPI_execute_plan(PAYMENT_8, args, nulls, false, 0);
		if (ret != SPI_OK_INSERT) {
			ereport(ERROR, (errcode(ERRCODE_INTERNAL_ERROR),
				 errmsg("PAYMENT_8 failed")));
		}

		funcctx->max_calls = 1;
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
		char **values = (char **) palloc(26 * sizeof(char *));

		payment_row *pp = (payment_row *) funcctx->user_fctx;

		values[0] = (char *) palloc(21 * sizeof(char));
		values[1] = (char *) palloc(21 * sizeof(char));
		values[2] = (char *) palloc(21 * sizeof(char));
		values[3] = (char *) palloc(3 * sizeof(char));
		values[4] = (char *) palloc(10 * sizeof(char));
		values[5] = (char *) palloc(21 * sizeof(char));
		values[6] = (char *) palloc(21 * sizeof(char));
		values[7] = (char *) palloc(21 * sizeof(char));
		values[8] = (char *) palloc(3 * sizeof(char));
		values[9] = (char *) palloc(10 * sizeof(char));
		values[10] = (char *) palloc(17 * sizeof(char));
		values[11] = (char *) palloc(3 * sizeof(char));
		values[12] = (char *) palloc(17 * sizeof(char));
		values[13] = (char *) palloc(21 * sizeof(char));
		values[14] = (char *) palloc(21 * sizeof(char));
		values[15] = (char *) palloc(21 * sizeof(char));
		values[16] = (char *) palloc(3 * sizeof(char));
		values[17] = (char *) palloc(10 * sizeof(char));
		values[18] = (char *) palloc(17 * sizeof(char));
		values[19] = (char *) palloc(29 * sizeof(char));
		values[20] = (char *) palloc(3 * sizeof(char));
		values[21] = (char *) palloc(11 * sizeof(char));
		values[22] = (char *) palloc(11 * sizeof(char));
		values[23] = (char *) palloc(11 * sizeof(char));
		values[24] = (char *) palloc(201 * sizeof(char));
		values[25] = (char *) palloc(29 * sizeof(char));

		strncpy(values[0], pp->w_street_1, 20);
		strncpy(values[1], pp->w_street_2, 20);
		strncpy(values[2], pp->w_city, 20);
		strncpy(values[3], pp->w_state, 2);
		strncpy(values[4], pp->w_zip, 9);

		strncpy(values[5], pp->d_street_1, 20);
		strncpy(values[6], pp->d_street_2, 20);
		strncpy(values[7], pp->d_city, 20);
		strncpy(values[8], pp->d_state, 2);
		strncpy(values[9], pp->d_zip, 9);

		strncpy(values[10], pp->c_first, 16);
		strncpy(values[11], pp->c_middle, 2);
		strncpy(values[12], pp->c_last, 16);

		strncpy(values[13], pp->c_street_1, 20);
		strncpy(values[14], pp->c_street_2, 20);
		strncpy(values[15], pp->c_city, 20);
		strncpy(values[16], pp->c_state, 2);
		strncpy(values[17], pp->c_zip, 9);

		strncpy(values[18], pp->c_phone, 16);
		strncpy(values[19], pp->c_since, 28);
		strncpy(values[20], pp->c_credit, 2);

		snprintf(values[21], 10, "%f", pp->c_credit_lim);
		snprintf(values[22], 10, "%f", pp->c_discount);
		snprintf(values[23], 10, "%f", pp->c_balance);
		strncpy(values[24], pp->c_data, 200);
		values[25] = NULL;

		tuple = BuildTupleFromCStrings(attinmeta, values);
		result = HeapTupleGetDatum(tuple);
		SRF_RETURN_NEXT(funcctx, result);
	} else {
		SRF_RETURN_DONE(funcctx);
	}
}
