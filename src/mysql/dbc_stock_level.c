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
#include "mysql_stock_level.h"

#include <stdio.h>

int execute_stock_level(struct db_context_t *dbc, struct stock_level_t *data)
{
	char stmt[512];

        /* Create the query and execute it. */
	sprintf(stmt, "call stock_level(%d, %d, %d, @low_stock)", data->w_id, data->d_id, data->threshold);

#ifdef DEBUG_QUERY
        LOG_ERROR_MESSAGE("execute_stock_level stmt: %s\n", stmt);
#endif

        if (mysql_query(dbc->mysql, stmt))
        {
          LOG_ERROR_MESSAGE("mysql reports: %d %s", mysql_errno(dbc->mysql) ,
                            mysql_error(dbc->mysql));
          return ERROR;
        }
        return OK;
}

