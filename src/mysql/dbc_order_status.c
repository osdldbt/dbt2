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
#include "mysql_order_status.h"

#include <stdio.h>

int execute_order_status(struct db_context_t *dbc, struct order_status_t *data)
{
	char stmt[128];

	/* Create the query and execute it. */
	sprintf(stmt, "call order_status(%d, %d, %d, '%s')",
		data->c_id, data->c_w_id, data->c_d_id, data->c_last);

#ifdef DEBUG_QUERY
        LOG_ERROR_MESSAGE("execute_order_status stmt: %s\n", stmt);
#endif
        if (mysql_query(dbc->mysql, stmt))
        {
          LOG_ERROR_MESSAGE("mysql reports: SQL STMT %s ERROR: %d %s", stmt,  mysql_errno(dbc->mysql) ,
                            mysql_error(dbc->mysql));
          return ERROR;
        }
	return OK;
}

