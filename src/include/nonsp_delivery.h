/*
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright (C) 2002      Open Source Development Lab, Inc.
 *               2002-2022 Mark Wong
 *
 * 22 july 2002
 * Based on TPC-C Standard Specification Revision 5.0.
 */

#ifndef _NONSP_DELIVERY_H_
#define _NONSP_DELIVERY_H_

#include <transaction_data.h>
#include <nonsp_common.h>

int execute_delivery_nonsp(struct db_context_t *, struct delivery_t *);
int delivery(struct db_context_t *dbc, struct delivery_t *data, char ** vals, int nvals);

#endif /* _NONSP_DELIVERY_H_ */
