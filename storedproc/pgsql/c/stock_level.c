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
#include <executor/spi.h> /* this should include most necessary APIs */
#include <executor/executor.h>  /* for GetAttributeByName() */
#include <funcapi.h> /* for returning set of rows in order_status */

#ifdef PG_MODULE_MAGIC
PG_MODULE_MAGIC;
#endif

/*
 * Stock Level transaction SQL Statement.
 */

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
Datum stock_level(PG_FUNCTION_ARGS);

PG_FUNCTION_INFO_V1(stock_level);

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
	elog(DEBUG1, "%s", query);
	ret = SPI_exec(query, 0);
	if (ret == SPI_OK_SELECT && SPI_processed > 0) {
		tupdesc = SPI_tuptable->tupdesc;
		tuptable = SPI_tuptable;
		tuple = tuptable->vals[0];

		buf = SPI_getvalue(tuple, tupdesc, 1);
		elog(DEBUG1, "d_next_o_id = %s", buf);
		d_next_o_id = atoi(buf);
	} else {
		SPI_finish();
		PG_RETURN_INT32(-1);
	}

	sprintf(query, STOCK_LEVEL_2, w_id, d_id, threshold, d_next_o_id - 20,
			d_next_o_id - 1);
	elog(DEBUG1, "%s", query);
	ret = SPI_exec(query, 0);
	if (ret == SPI_OK_SELECT && SPI_processed > 0) {
		tupdesc = SPI_tuptable->tupdesc;
		tuptable = SPI_tuptable;
		tuple = tuptable->vals[0];

		buf = SPI_getvalue(tuple, tupdesc, 1);
		elog(DEBUG1, "low_stock = %s", buf);
		low_stock = atoi(buf);
	} else {
		SPI_finish();
		PG_RETURN_INT32(-1);
	}

	SPI_finish();
	PG_RETURN_INT32(low_stock);
}
