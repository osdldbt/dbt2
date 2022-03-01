/*
 * odbc_stock_level.h
 *
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright (C) 2002      Open Source Development Lab, Inc.
 *               2002-2022 Mark Wong
 *
 * 23 july 2002
 * Based on TPC-C Standard Specification Revision 5.0.
 */

#ifndef _ODBC_STOCK_LEVEL_H_
#define _ODBC_STOCK_LEVEL_H_

#include "db.h"

#define STMT_STOCK_LEVEL \
	"CALL stock_level (?, ?, ?, ?)"

int execute_stock_level(struct db_context_t *odbcc,
	struct stock_level_t *data);

#endif /* _ODBC_STOCK_LEVEL_H_ */
