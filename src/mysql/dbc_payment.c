/*
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright (C) 2002 Mark Wong & Open Source Development Labs, Inc.
 * Copyright (C) 2004 Alexey Stroganov & MySQL AB.
 *
 */

#include "common.h"
#include "logging.h"
#include "mysql_payment.h"

#include <stdio.h>

int execute_payment(struct db_context_t *dbc, struct payment_t *data)
{
	char stmt[512];

	/* Create the query and execute it. */
	sprintf(stmt, "call payment(%d, %d, %d, %d, %d, '%s', %f)",
		data->w_id, data->d_id, data->c_id, data->c_w_id, data->c_d_id,
		data->c_last, data->h_amount);

#ifdef DEBUG_QUERY
        LOG_ERROR_MESSAGE("execute_payment stmt: %s\n", stmt);
#endif

        if (mysql_query(dbc->mysql, stmt))
        {
          LOG_ERROR_MESSAGE("mysql reports SQL STMT: stmt ERROR: %d %s", mysql_errno(dbc->mysql) ,
                            mysql_error(dbc->mysql));
          return ERROR;
        }
	return OK;
}

