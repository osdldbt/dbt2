/*
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright (C) 2002 Mark Wong & Open Source Development Labs, Inc.
 *
 * 13 May 2003
 */

#ifndef _LIBPQ_ORDER_STATUS_H_
#define _LIBPQ_ORDER_STATUS_H_

#include "libpq_common.h"

int execute_order_status(struct db_context_t *dbc, struct order_status_t *data);

#endif /* _LIBPQ_ORDER_STATUS_H_ */
