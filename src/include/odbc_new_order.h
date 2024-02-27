/*
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright The DBT-2 Authors
 *
 * Based on TPC-C Standard Specification Revision 5.0.
 */

#ifndef _ODBC_NEW_ORDER_H_
#define _ODBC_NEW_ORDER_H_

#include "db.h"

#define STMT_NEW_ORDER                                                         \
  "CALL new_order (?, ?, ?, ?, ?, "                                            \
  "?, ?, ?, ?, ?, ?, ?, "                                                      \
  "?, ?, ?, ?, ?, ?, ?, "                                                      \
  "?, ?, ?, ?, ?, ?, ?, "                                                      \
  "?, ?, ?, ?, ?, ?, ?, "                                                      \
  "?, ?, ?, ?, ?, ?, ?, "                                                      \
  "?, ?, ?, ?, ?, ?, ?, "                                                      \
  "?, ?, ?, ?, ?, ?, ?, "                                                      \
  "?, ?, ?, ?, ?, ?, ?, "                                                      \
  "?, ?, ?, ?, ?, ?, ?, "                                                      \
  "?, ?, ?, ?, ?, ?, ?, "                                                      \
  "?, ?, ?, ?, ?, ?, ?, "                                                      \
  "?, ?, ?, ?, ?, ?, ?, "                                                      \
  "?, ?, ?, ?, ?, ?, ?, "                                                      \
  "?, ?, ?, ?, ?, ?, ?, "                                                      \
  "?, ?, ?, ?, ?, ?, ?, "                                                      \
  "?, ?, "                                                                     \
  "?, ?, "                                                                     \
  "?, ?, ?, ?)"

int execute_new_order(struct db_context_t *odbcc, struct new_order_t *data);

#endif /* _ODBC_NEW_ORDER_H_ */
