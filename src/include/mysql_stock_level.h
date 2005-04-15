/*
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright (C) 2002 Mark Wong & Open Source Development Labs, Inc.
 *
 * 13 May 2003
 */

#ifndef _MYSQL_STOCK_LEVEL_H_
#define _MYSQL_STOCK_LEVEL_H_

#include "mysql_common.h"

int execute_stock_level(struct db_context_t *dbc, struct stock_level_t *data);

#endif /* _MYSQL_STOCK_LEVEL_H_ */
