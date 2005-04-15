/*
 * odbc_order_status.h
 *
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright (C) 2002 Mark Wong & Open Source Development Lab, Inc.
 *
 * 16 july 2002
 * Based on TPC-C Standard Specification Revision 5.0.
 */

#ifndef _ODBC_ORDER_STATUS_H_
#define _ODBC_ORDER_STATUS_H_

#include <transaction_data.h>
#include <odbc_common.h>

#define STMT_ORDER_STATUS \
	"CALL order_status (?, ?, ?, " \
	"?, ?, ?, " \
	"?, ?, ?, " \
	"?, ?, " \
	"?, ?, ?, ?, ?, " \
	"?, ?, ?, ?, ?, " \
	"?, ?, ?, ?, ?, " \
	"?, ?, ?, ?, ?, " \
	"?, ?, ?, ?, ?, " \
	"?, ?, ?, ?, ?, " \
	"?, ?, ?, ?, ?, " \
	"?, ?, ?, ?, ?, " \
	"?, ?, ?, ?, ?, " \
	"?, ?, ?, ?, ?, " \
	"?, ?, ?, ?, ?, " \
	"?, ?, ?, ?, ?, " \
	"?, ?, ?, ?, ?, " \
	"?, ?, ?, ?, ?, " \
	"?, ?, ?, ?, ?)"

int execute_order_status(struct db_context_t *odbcc,
	struct order_status_t *data);

#endif /* _ODBC_ORDER_STATUS_H_ */
