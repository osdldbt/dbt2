/*
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright The DBT-2 Authors
 *
 * Based on TPC-C Standard Specification Revision 5.0.
 */

#ifndef _NONSP_INTEGRITY_H_
#define _NONSP_INTEGRITY_H_

#include <nonsp_common.h>
#include <transaction_data.h>

int execute_integrity_nonsp(struct db_context_t *, struct integrity_t *);
int integrity(struct db_context_t *dbc, struct integrity_t *data, char **vals,
              int nvals);

#endif /* _NONSP_INTEGRITY_H_ */
