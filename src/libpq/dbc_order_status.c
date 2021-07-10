/*
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright (C) 2002 Open Source Development Labs, Inc.
 *               2002-2021 Mark Wong
 *
 * 13 May 2003
 */

#include <stdio.h>
#include <wchar.h>

#include "common.h"
#include "logging.h"
#include "libpq_order_status.h"

const static int g_debug = 0;

int execute_order_status(struct db_context_t *dbc, struct order_status_t *data)
{
        PGresult *res;
        wchar_t wcstmt[512];
        char stmt[2048];

        int nFields;
        int i, j;

        /* Start a transaction block. */
        res = PQexec(dbc->conn, "BEGIN");
        if (!res || PQresultStatus(res) != PGRES_COMMAND_OK) {
                LOG_ERROR_MESSAGE("%s", PQerrorMessage(dbc->conn));
                PQclear(res);
                return ERROR;
        }
        PQclear(res);

        /* Create the query and execute it. */
        swprintf(wcstmt, 512, L"SELECT * FROM order_status(%d, %d, %d, '%ls')",
                data->c_id, data->c_w_id, data->c_d_id, data->c_last);
		wcstombs(stmt, wcstmt, 2048);
        res = PQexec(dbc->conn, stmt);
        if (!res || (PQresultStatus(res) != PGRES_COMMAND_OK &&
                PQresultStatus(res) != PGRES_TUPLES_OK)) {
                LOG_ERROR_MESSAGE("%s", PQerrorMessage(dbc->conn));
                PQclear(res);
                return ERROR;
        }
        /* first, print out the attribute names */
        nFields = PQnfields(res);
        for (i = 0; i < nFields; i++) {
            char* tmp = PQfname(res, i);
            if(g_debug) printf("%-15s", tmp);
        }
        if(g_debug) printf("\n\n");

        /* next, print out the rows */
        for (i = 0; i < PQntuples(res); i++) {
            for (j = 0; j < nFields; j++) {
                char* tmp = PQgetvalue(res, i, j);
                if(g_debug) printf("%-15s", tmp);
            }
            if(g_debug) printf("\n");
        }
        PQclear(res);

        return OK;
}
