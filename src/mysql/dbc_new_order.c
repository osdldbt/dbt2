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
#include "mysql_new_order.h"

#include <stdio.h>
#include <string.h>

int execute_new_order(struct db_context_t *dbc, struct new_order_t *data)
{
	char stmt[512];
        int rc;

        MYSQL_RES * result;
        MYSQL_ROW row;

	/* Create the query and execute it. */
	sprintf(stmt,
                 "call new_order(%d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d,\
                                 %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d,\
                                 %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d,\
                          %d, %d, @rc)",
		data->w_id, data->d_id, data->c_id, data->o_all_local,
		data->o_ol_cnt,
		data->order_line[0].ol_i_id,
		data->order_line[0].ol_supply_w_id,
		data->order_line[0].ol_quantity,
		data->order_line[1].ol_i_id,
		data->order_line[1].ol_supply_w_id,
		data->order_line[1].ol_quantity,
		data->order_line[2].ol_i_id,
		data->order_line[2].ol_supply_w_id,
		data->order_line[2].ol_quantity,
		data->order_line[3].ol_i_id,
		data->order_line[3].ol_supply_w_id,
		data->order_line[3].ol_quantity,
		data->order_line[4].ol_i_id,
		data->order_line[4].ol_supply_w_id,
		data->order_line[4].ol_quantity,
		data->order_line[5].ol_i_id,
		data->order_line[5].ol_supply_w_id,
		data->order_line[5].ol_quantity,
		data->order_line[6].ol_i_id,
		data->order_line[6].ol_supply_w_id,
		data->order_line[6].ol_quantity,
		data->order_line[7].ol_i_id,
		data->order_line[7].ol_supply_w_id,
		data->order_line[7].ol_quantity,
		data->order_line[8].ol_i_id,
		data->order_line[8].ol_supply_w_id,
		data->order_line[8].ol_quantity,
		data->order_line[9].ol_i_id,
		data->order_line[9].ol_supply_w_id,
		data->order_line[9].ol_quantity,
		data->order_line[10].ol_i_id,
		data->order_line[10].ol_supply_w_id,
		data->order_line[10].ol_quantity,
		data->order_line[11].ol_i_id,
		data->order_line[11].ol_supply_w_id,
		data->order_line[11].ol_quantity,
		data->order_line[12].ol_i_id,
		data->order_line[12].ol_supply_w_id,
		data->order_line[12].ol_quantity,
		data->order_line[13].ol_i_id,
		data->order_line[13].ol_supply_w_id,
		data->order_line[13].ol_quantity,
		data->order_line[14].ol_i_id,
		data->order_line[14].ol_supply_w_id,
		data->order_line[14].ol_quantity);


#ifdef DEBUG_QUERY
        LOG_ERROR_MESSAGE("execute_new_order stmt: %s\n", stmt);
#endif

        if (mysql_query(dbc->mysql, stmt))
        {

          LOG_ERROR_MESSAGE("mysql reports: SQL: %s,  ERROR: %d %s", stmt, mysql_errno(dbc->mysql) ,
                            mysql_error(dbc->mysql));
          return ERROR;
        }

        rc= ERROR;

        if (mysql_query(dbc->mysql, "select @rc"))
        {
          LOG_ERROR_MESSAGE("mysql reports: %d %s", mysql_errno(dbc->mysql) ,
                            mysql_error(dbc->mysql));
        }
        else
        {
          if ((result = mysql_store_result(dbc->mysql)))
          {
            if ((row = mysql_fetch_row(result)) && (row[0]))
            {
              data->rollback=atoi(row[0]);
              if  (data->rollback)
              {
                LOG_ERROR_MESSAGE("NEW_ORDER ROLLBACK RC %d\n",data->rollback);
              }
              rc= OK;
            }
            else
            {
              fprintf(stderr, "Error: %s\n", mysql_error(dbc->mysql));
            }
            mysql_free_result(result);
          }
        }

	return rc;
}

