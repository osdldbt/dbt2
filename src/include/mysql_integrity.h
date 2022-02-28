/*
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright (C) 2002      Open Source Development Labs, Inc.
 *               2002-2022 Mark Wong
 *
 * 13 May 2003
 */

#ifndef _MYSQL_INTEGRITY_H_
#define _MYSQL_INTEGRITY_H_

#include "db.h"

int execute_integrity_mysql(struct db_context_t *, struct integrity_t *);

#endif /* _MYSQL_INTEGRITY_H_ */
