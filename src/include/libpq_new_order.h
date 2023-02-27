/*
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright The DBT-2 Authors
 */

#ifndef _LIBPQ_NEW_ORDER_H_
#define _LIBPQ_NEW_ORDER_H_

#include "db.h"

int execute_new_order_libpq(struct db_context_t *, struct new_order_t *);

#endif /* _LIBPQ_NEW_ORDER_H_ */
