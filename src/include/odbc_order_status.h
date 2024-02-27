/*
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright The DBT-2 Authors
 *
 * Based on TPC-C Standard Specification Revision 5.0.
 */

#ifndef _ODBC_ORDER_STATUS_H_
#define _ODBC_ORDER_STATUS_H_

#include "db.h"

#define STMT_ORDER_STATUS                                                      \
  "CALL order_status (?, ?, ?, "                                               \
  "?, ?, ?, "                                                                  \
  "?, ?, ?, "                                                                  \
  "?, ?, "                                                                     \
  "?, ?, ?, ?, ?, "                                                            \
  "?, ?, ?, ?, ?, "                                                            \
  "?, ?, ?, ?, ?, "                                                            \
  "?, ?, ?, ?, ?, "                                                            \
  "?, ?, ?, ?, ?, "                                                            \
  "?, ?, ?, ?, ?, "                                                            \
  "?, ?, ?, ?, ?, "                                                            \
  "?, ?, ?, ?, ?, "                                                            \
  "?, ?, ?, ?, ?, "                                                            \
  "?, ?, ?, ?, ?, "                                                            \
  "?, ?, ?, ?, ?, "                                                            \
  "?, ?, ?, ?, ?, "                                                            \
  "?, ?, ?, ?, ?, "                                                            \
  "?, ?, ?, ?, ?, "                                                            \
  "?, ?, ?, ?, ?)"

int execute_order_status(struct db_context_t *odbcc,
                         struct order_status_t *data);

#endif /* _ODBC_ORDER_STATUS_H_ */
