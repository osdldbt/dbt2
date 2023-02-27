/*
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright The DBT-2 Authors
 */

#ifndef _MYSQL_DELIVERY_H_
#define _MYSQL_DELIVERY_H_

#include "db.h"

int execute_delivery_mysql(struct db_context_t *, struct delivery_t *);

#endif /* _MYSQL_DELIVERY_H_ */
