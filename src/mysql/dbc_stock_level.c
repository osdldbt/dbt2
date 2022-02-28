/*
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright (C) 2002      Open Source Development Labs, Inc.
 *               2004      Alexey Stroganov & MySQL AB.
 *               2002-2022 Mark Wong
 *
 */

#include "common.h"
#include "logging.h"
#include "mysql_stock_level.h"

#include <stdio.h>

int execute_stock_level_mysql(struct db_context_t *dbc, struct stock_level_t *data)
{
	char stmt[512];

        /* Create the query and execute it. */
	sprintf(stmt, "call stock_level(%d, %d, %d, @low_stock)", data->w_id, data->d_id, data->threshold);

#ifdef DEBUG_QUERY
        LOG_ERROR_MESSAGE("execute_stock_level stmt: %s\n", stmt);
#endif

        if (mysql_query(dbc->library.mysql.mysql, stmt))
        {
          LOG_ERROR_MESSAGE("mysql reports: %d %s", mysql_errno(dbc->library.mysql.mysql) ,
                            mysql_error(dbc->library.mysql.mysql));
          return ERROR;
        }
        return OK;
}

