/*
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright (C) 2002 Open Source Development Lab, Inc.
 *               2002-2022 Mark Wong
 *
 * 23 july 2002
 * Based on TPC-C Standard Specification Revision 5.0.
 */

#ifndef _LIBPQ_INTEGRITY_H_
#define _LIBPQ_INTEGRITY_H_

#include "db.h"

int execute_integrity_libpq(struct db_context_t *, struct integrity_t *);

#endif /* _LIBPQ_INTEGRITY_H_ */
