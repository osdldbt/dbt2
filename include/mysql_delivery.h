/*
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright (C) 2002 Mark Wong & Open Source Development Labs, Inc.
 *
 * 13 May 2003
 */

#ifndef _MYSQL_DELIVERY_H_
#define _MYSQL_DELIVERY_H_

#include "mysql_common.h"

int execute_delivery(struct db_context_t *dbc, struct delivery_t *data);

#endif /* _MYSQL_DELIVERY_H_ */
