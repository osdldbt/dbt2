/*
 * odbc_common.h
 *
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *  
 * Copyright (C) 2002 Mark Wong & Open Source Development Lab, Inc.
 *
 * 11 june 2002
 */

#ifndef _ODBC_COMMON_H_
#define _ODBC_COMMON_H_

#include <WINDOWS.H>
#include <sqltypes.h>
#include <sql.h>
#include <sqlext.h>

#include <common.h>
#include <logging.h>
#include <transaction_data.h>

#define LOG_ODBC_ERROR(type, handle) log_odbc_error(__FILE__, __LINE__, type, handle)

#define COMMIT "COMMIT"
#define ROLLBACK "ROLLBACK"

struct odbc_context_t
{
	SQLHDBC hdbc;
	SQLHSTMT hstmt;
};

union odbc_transaction_t
{
	struct delivery_t delivery;
	struct new_order_t new_order;
	struct order_status_t order_status;
	struct payment_t payment;
	struct stock_level_t stock_level;
};

int log_odbc_error(char *filename, int line, SQLSMALLINT handle_type,
	SQLHANDLE handle);
int odbc_connect(struct odbc_context_t *odbcc);
int odbc_disconnect(struct odbc_context_t *odbcc);
int odbc_init(char *sname, char *uname, char *auth);

#endif /* _ODBC_COMMON_H_ */
