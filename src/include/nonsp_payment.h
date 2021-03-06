/*
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright (C) 2002      Open Source Development Lab, Inc.
 *               2002-2022 Mark Wong
 *
 * 9 july 2002
 * Based on TPC-C Standard Specification Revision 5.0.
 */

#ifndef _NONSP_PAYMENT_H_
#define _NONSP_PAYMENT_H_

#include <transaction_data.h>
#include <nonsp_common.h>

int execute_payment_nonsp(struct db_context_t *, struct payment_t *);
int  payment_nonsp(struct db_context_t *, struct payment_t *, char **, int);

#endif /* _NONSP_PAYMENT_H_ */
