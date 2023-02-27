/*
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright The DBT-2 Authors
 */

#ifndef _MYSQL_ORDER_STATUS_H_
#define _MYSQL_ORDER_STATUS_H_

#include "db.h"

int execute_order_status_mysql(struct db_context_t *, struct order_status_t *);

#endif /* _MYSQL_ORDER_STATUS_H_ */
