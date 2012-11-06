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
#include <executor/spi.h> /* this should include most necessary APIs */
#include <executor/executor.h>  /* for GetAttributeByName() */
#include <funcapi.h> /* for returning set of rows in order_status */

#include "dbt2common.h"

#ifdef PG_MODULE_MAGIC
PG_MODULE_MAGIC;
#endif

/*
 * Delivery transaction SQL statements.
 */

#define DELIVERY_1 \
	"SELECT no_o_id\n" \
	"FROM new_order\n" \
	"WHERE no_w_id = %d\n" \
	"  AND no_d_id = %d" \
	"ORDER BY no_o_id " \
	"LIMIT 1"

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

/* Prototypes to prevent potential gcc warnings. */

Datum delivery(PG_FUNCTION_ARGS);

PG_FUNCTION_INFO_V1(delivery);

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
		elog(DEBUG1, "%s", query);
		ret = SPI_exec(query, 0);
		if (ret == SPI_OK_SELECT && SPI_processed > 0) {
			tupdesc = SPI_tuptable->tupdesc;
			tuptable = SPI_tuptable;
			tuple = tuptable->vals[0];

			no_o_id = SPI_getvalue(tuple, tupdesc, 1);
			elog(DEBUG1, "no_o_id = %s", no_o_id);
		} else {
			/* Nothing to deliver for this district, try next district. */
			continue;
		}

		sprintf(query, DELIVERY_2, no_o_id, w_id, d_id);
		elog(DEBUG1, "%s", query);
		ret = SPI_exec(query, 0);
		if (ret != SPI_OK_DELETE) {
			SPI_finish();
			PG_RETURN_INT32(-1);
		}

		sprintf(query, DELIVERY_3, no_o_id, w_id, d_id);
		elog(DEBUG1, "%s", query);
		ret = SPI_exec(query, 0);
		if (ret == SPI_OK_SELECT && SPI_processed > 0) {
			tupdesc = SPI_tuptable->tupdesc;
			tuptable = SPI_tuptable;
			tuple = tuptable->vals[0];

			o_c_id = SPI_getvalue(tuple, tupdesc, 1);
			elog(DEBUG1, "o_c_id = %s", no_o_id);
		} else {
			SPI_finish();
			PG_RETURN_INT32(-1);
		}

		sprintf(query, DELIVERY_4, o_carrier_id, no_o_id, w_id, d_id);
		elog(DEBUG1, "%s", query);
		ret = SPI_exec(query, 0);
		if (ret != SPI_OK_UPDATE) {
			SPI_finish();
			PG_RETURN_INT32(-1);
		}

		sprintf(query, DELIVERY_5, no_o_id, w_id, d_id);
		elog(DEBUG1, "%s", query);
		ret = SPI_exec(query, 0);
		if (ret != SPI_OK_UPDATE) {
			SPI_finish();
			PG_RETURN_INT32(-1);
		}

		sprintf(query, DELIVERY_6, no_o_id, w_id, d_id);
		elog(DEBUG1, "%s", query);
		ret = SPI_exec(query, 0);
		if (ret == SPI_OK_SELECT && SPI_processed > 0) {
			tupdesc = SPI_tuptable->tupdesc;
			tuptable = SPI_tuptable;
			tuple = tuptable->vals[0];

			ol_amount = SPI_getvalue(tuple, tupdesc, 1);
			elog(DEBUG1, "ol_amount = %s", no_o_id);
		} else {
			SPI_finish();
			PG_RETURN_INT32(-1);
		}

		sprintf(query, DELIVERY_7, ol_amount, o_c_id, w_id, d_id);
		elog(DEBUG1, "%s", query);
		ret = SPI_exec(query, 0);
		if (ret != SPI_OK_UPDATE) {
			SPI_finish();
			PG_RETURN_INT32(-1);
		}
	}

	SPI_finish();
	PG_RETURN_INT32(1);
}
