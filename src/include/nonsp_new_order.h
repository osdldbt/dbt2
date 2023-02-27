/*
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright The DBT-2 Authors
 *
 * Based on TPC-C Standard Specification Revision 5.0.
 */

#ifndef _NONSP_NEW_ORDER_H_
#define _NONSP_NEW_ORDER_H_

#include <transaction_data.h>
#include <nonsp_common.h>

int execute_new_order_nonsp(struct db_context_t *, struct new_order_t *);
int new_order_nonsp(struct db_context_t *, struct new_order_t *, char **, int);

#endif /* _NONSP_NEW_ORDER_H_ */
