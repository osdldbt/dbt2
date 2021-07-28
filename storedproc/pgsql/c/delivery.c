/*
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright (C) 2003-2008 Mark Wong & Open Source Development Labs, Inc.
 *
 * Based on TPC-C Standard Specification Revision 5.0 Clause 2.8.2.
 */

#include <sys/types.h>
#include <unistd.h>
#include <postgres.h>
#include <fmgr.h>
#include <funcapi.h>
#include <catalog/pg_type.h>    /* for INT4OID */
#include <executor/spi.h>       /* this should include most necessary APIs */
#include <utils/builtins.h>     /* for numeric_in() */

#include "dbt2common.h"

#ifdef PG_MODULE_MAGIC
PG_MODULE_MAGIC;
#endif

/*
 * Delivery transaction SQL statements.
 */

#define DELIVERY_1 statements[0].plan
#define DELIVERY_2 statements[1].plan
#define DELIVERY_3 statements[2].plan
#define DELIVERY_5 statements[3].plan
#define DELIVERY_6 statements[4].plan
#define DELIVERY_7 statements[5].plan

static cached_statement statements[] =
{
	/* DELIVERY_1 */
	{
	"SELECT no_o_id\n" \
	"FROM new_order\n" \
	"WHERE no_w_id = $1\n" \
	"  AND no_d_id = $2\n" \
	"ORDER BY no_o_id ASC\n" \
	"LIMIT 1",
	2,
	{ INT4OID, INT4OID }
	},

	/* DELIVERY_2 */
	{
	"DELETE FROM new_order\n" \
	"WHERE no_o_id = $1\n" \
	"  AND no_w_id = $2\n" \
	"  AND no_d_id = $3",
	3,
	{ INT4OID, INT4OID, INT4OID }
	},

	/* DELIVERY_3 */
	{
	"UPDATE orders\n" \
	"SET o_carrier_id = $1\n" \
	"WHERE o_id = $2\n" \
	"  AND o_w_id = $3\n" \
	"  AND o_d_id = $4\n" \
	"RETURNING o_c_id",
	4,
	{ INT4OID, INT4OID, INT4OID, INT4OID }
	},

	/* DELIVERY_5 */
	{
	"UPDATE order_line\n" \
	"SET ol_delivery_d = current_timestamp\n" \
	"WHERE ol_o_id = $1\n" \
	"  AND ol_w_id = $2\n" \
	"  AND ol_d_id = $3",
	3,
	{ INT4OID, INT4OID, INT4OID }
	},

	/* DELIVERY_6 */
	{
	"SELECT SUM(ol_amount * ol_quantity)\n" \
	"FROM order_line\n" \
	"WHERE ol_o_id = $1\n" \
	"  AND ol_w_id = $2\n" \
	"  AND ol_d_id = $3",
	3,
	{ INT4OID, INT4OID, INT4OID }
	},

	/* DELIVERY_7 */
	{
	"UPDATE customer\n" \
	"SET c_delivery_cnt = c_delivery_cnt + 1,\n" \
	"    c_balance = c_balance + $1\n" \
	"WHERE c_id = $2\n" \
	"  AND c_w_id = $3\n" \
	"  AND c_d_id = $4",
	4,
	{ NUMERICOID, INT4OID, INT4OID, INT4OID }
	},

	{ NULL }
};

/* Prototypes to prevent potential gcc warnings. */

Datum delivery(PG_FUNCTION_ARGS);

PG_FUNCTION_INFO_V1(delivery);

Datum delivery(PG_FUNCTION_ARGS)
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
		int32 o_carrier_id = PG_GETARG_INT32(1);

		TupleDesc tupdesc;
		SPITupleTable *tuptable;
		HeapTuple tuple;

		int d_id;
		int ret;
		char *ol_amount;
		int no_o_id;
		int o_c_id;

		int32 **pp;
		int32 *offset;

		funcctx = SRF_FIRSTCALL_INIT();
		oldcontext = MemoryContextSwitchTo(funcctx->multi_call_memory_ctx);
		if (get_call_result_type(fcinfo, NULL, &tupdesc) != TYPEFUNC_COMPOSITE)
			ereport(ERROR, (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
					errmsg("delivery cannot accept type record")));
		attinmeta = TupleDescGetAttInMetadata(tupdesc);
		funcctx->attinmeta = attinmeta;

		SPI_connect();

		plan_queries(statements);

		/* Hard coded to 10 districts per warehouse. */
		funcctx->user_fctx = MemoryContextAlloc(funcctx->multi_call_memory_ctx,
				sizeof(int32 *) * 10 + sizeof(int32) * 2 * 10);

		pp = (int32 **) funcctx->user_fctx;
		offset = (int32 *) &pp[10];
		funcctx->max_calls = 0;
		for (d_id = 1; d_id <= 10; d_id++, offset += 2) {
			Datum	args[4];
			char	nulls[4] = { ' ', ' ', ' ', ' ' };

			pp[funcctx->max_calls] = offset;

			args[0] = Int32GetDatum(w_id);
			args[1] = Int32GetDatum(d_id);
			ret = SPI_execute_plan(DELIVERY_1, args, nulls, true, 0);
			if (ret == SPI_OK_SELECT && SPI_processed > 0) {
				tupdesc = SPI_tuptable->tupdesc;
				tuptable = SPI_tuptable;
				tuple = tuptable->vals[0];

				no_o_id = atoi(SPI_getvalue(tuple, tupdesc, 1));
				elog(DEBUG1, "no_o_id = %d", no_o_id);
			} else {
				/* Nothing to deliver for this district, try next district. */
				continue;
			}

			args[0] = Int32GetDatum(no_o_id);
			args[1] = Int32GetDatum(w_id);
			args[2] = Int32GetDatum(d_id);
			ret = SPI_execute_plan(DELIVERY_2, args, nulls, false, 0);
			if (ret != SPI_OK_DELETE) {
				ereport(ERROR, (errcode(ERRCODE_INTERNAL_ERROR),
					 errmsg("DELIVERY_2 failed")));
			}

			args[0] = Int32GetDatum(o_carrier_id);
			args[1] = Int32GetDatum(no_o_id);
			args[2] = Int32GetDatum(w_id);
			args[3] = Int32GetDatum(d_id);
			ret = SPI_execute_plan(DELIVERY_3, args, nulls, false, 0);
			if (ret == SPI_OK_UPDATE_RETURNING && SPI_processed > 0) {
				tupdesc = SPI_tuptable->tupdesc;
				tuptable = SPI_tuptable;
				tuple = tuptable->vals[0];

				o_c_id = atoi(SPI_getvalue(tuple, tupdesc, 1));
				elog(DEBUG1, "o_c_id = %d", o_c_id);
			} else {
				ereport(ERROR, (errcode(ERRCODE_INTERNAL_ERROR),
					 errmsg("DELIVERY_3 failed")));
			}

			args[0] = Int32GetDatum(no_o_id);
			args[1] = Int32GetDatum(w_id);
			args[2] = Int32GetDatum(d_id);
			ret = SPI_execute_plan(DELIVERY_5, args, nulls, false, 0);
			if (ret != SPI_OK_UPDATE) {
				ereport(ERROR, (errcode(ERRCODE_INTERNAL_ERROR),
					 errmsg("DELIVERY_5 failed")));
			}

			args[0] = Int32GetDatum(no_o_id);
			args[1] = Int32GetDatum(w_id);
			args[2] = Int32GetDatum(d_id);
			ret = SPI_execute_plan(DELIVERY_6, args, nulls, true, 0);
			if (ret == SPI_OK_SELECT && SPI_processed > 0) {
				tupdesc = SPI_tuptable->tupdesc;
				tuptable = SPI_tuptable;
				tuple = tuptable->vals[0];

				ol_amount = SPI_getvalue(tuple, tupdesc, 1);
				elog(DEBUG1, "ol_amount = %s", ol_amount);
			} else {
				ereport(ERROR, (errcode(ERRCODE_INTERNAL_ERROR),
					 errmsg("DELIVERY_6 failed")));
			}
			args[0] = DirectFunctionCall3(numeric_in,
					CStringGetDatum(ol_amount), ObjectIdGetDatum(InvalidOid),
					Int32GetDatum(((24 << 16) | 12) + VARHDRSZ));
			args[1] = Int32GetDatum(o_c_id);
			args[2] = Int32GetDatum(w_id);
			args[3] = Int32GetDatum(d_id);
			ret = SPI_execute_plan(DELIVERY_7, args, nulls, false, 0);
			if (ret != SPI_OK_UPDATE) {
				ereport(ERROR, (errcode(ERRCODE_INTERNAL_ERROR),
					 errmsg("DELIVERY_7 failed")));
			}

			pp[funcctx->max_calls][0] = d_id;
			pp[funcctx->max_calls][1] = no_o_id;
			++funcctx->max_calls;
		}

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
		char **values = (char **) palloc(2 * sizeof(char *));

		int32 **pp = (int32 **) funcctx->user_fctx;

		values[0] = (char *) palloc(11 * sizeof(char));
		values[1] = (char *) palloc(11 * sizeof(char));

		snprintf(values[0], 10, "%d", pp[funcctx->call_cntr][0]);
		snprintf(values[1], 10, "%d", pp[funcctx->call_cntr][1]);

		tuple = BuildTupleFromCStrings(attinmeta, values);
		result = HeapTupleGetDatum(tuple);
		SRF_RETURN_NEXT(funcctx, result);
	} else {
		SRF_RETURN_DONE(funcctx);
	}
}
