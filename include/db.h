/*
 * db.h
 *
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright (C) 2002 Mark Wong & Open Source Development Lab, Inc.
 *
 * 16 june 2002
 * Based on TPC-C Standard Specification Revision 5.0.
 */

#ifndef _DB_H_
#define _DB_H_

#ifdef ODBC
#include <odbc_common.h>
#include <client_interface.h>

int process_transaction(int transaction, struct odbc_context_t *odbcc,
	union transaction_data_t *odbct);
#endif /* ODBC */

#endif /* _DB_H_ */
