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
#include "string.h"
#include "mysql_integrity.h"

#include <stdio.h>

int execute_integrity(struct db_context_t *dbc, struct integrity_t *data)
{
	char stmt[512];
        int rc;

        MYSQL_RES * result;
        MYSQL_ROW row;

	/* Create the query and execute it. */
	sprintf(stmt, "select count(w_id) as w from warehouse");

#ifdef DEBUG_QUERY
        LOG_ERROR_MESSAGE("execute_delivery stmt: %s\n", stmt);
#endif

        rc= OK;

        if (mysql_query(dbc->mysql, stmt))
        {
          LOG_ERROR_MESSAGE("mysql reports: %d %s", mysql_errno(dbc->mysql) , 
                            mysql_error(dbc->mysql));
          rc= ERROR;
        }
        else 
        {
          if ((result = mysql_store_result(dbc->mysql)))
          {
            if ((row = mysql_fetch_row(result)) && (row[0]))
            {
              if (atoi(row[0]) != data->w_id)
              {
                LOG_ERROR_MESSAGE("Wrong number of warehouses. Should be %d Database reports %d", data->w_id, atoi(row[0]));
                rc= ERROR;
              }
            }
            else  
            {
              fprintf(stderr, "Error: %s\n", mysql_error(dbc->mysql));
              rc= ERROR;
            }
            mysql_free_result(result);
          }
          else
          {
            rc= ERROR;
          }
        }

	return rc;
}

