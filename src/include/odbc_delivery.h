/*
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright The DBT-2 Authors
 *
 * Based on TPC-C Standard Specification Revision 5.0.
 */

#ifndef _ODBC_DELIVERY_H_
#define _ODBC_DELIVERY_H_

#include "db.h"

#define STMT_DELIVERY \
	"CALL delivery (?, ?)"

int execute_delivery(struct db_context_t *odbcc, struct delivery_t *data);

#endif /* _ODBC_DELIVERY_H_ */
