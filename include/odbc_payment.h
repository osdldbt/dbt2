/*
 * odbc_payment.h
 *
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright (C) 2002 Mark Wong & Open Source Development Lab, Inc.
 *
 * 9 july 2002
 * Based on TPC-C Standard Specification Revision 5.0.
 */

#ifndef _ODBC_PAYMENT_H_
#define _ODBC_PAYMENT_H_

#include <transaction_data.h>
#include <odbc_common.h>

#define STMT_PAYMENT \
	"CALL payment (?, ?, ?, ?, ?, ?, ?," \
	"?, ?, ?, ?, ?, ?, " \
	"?, ?, ?, ?, ?, ?, " \
	"?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)"

int execute_payment(struct db_context_t *odbcc, struct payment_t *data);

#endif /* _ODBC_PAYMENT_H_ */
