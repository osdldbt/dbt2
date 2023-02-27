/*
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright The DBT-2 Authors
 */

#ifndef _LIBPQ_STOCK_LEVEL_H_
#define _LIBPQ_STOCK_LEVEL_H_

#include "db.h"

int execute_stock_level_libpq(struct db_context_t *, struct stock_level_t *);

#endif /* _LIBPQ_STOCK_LEVEL_H_ */
