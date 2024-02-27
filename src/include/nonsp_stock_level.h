/*
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright The DBT-2 Authors
 *
 * Based on TPC-C Standard Specification Revision 5.0.
 */

#ifndef _NONSP_STOCK_LEVEL_H_
#define _NONSP_STOCK_LEVEL_H_

#include <nonsp_common.h>
#include <transaction_data.h>

int execute_stock_level_nonsp(struct db_context_t *, struct stock_level_t *);
int stock_level_nonsp(struct db_context_t *, struct stock_level_t *, char **,
                      int);

#endif /* _NONSP_STOCK_LEVEL_H_ */
