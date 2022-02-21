/*
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright (C) 2002      Open Source Development Labs, Inc.
 *               2002-2022 Mark Wong
 *
 * 16 June 2002
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "db.h"
#include "logging.h"

#ifdef ODBC
#include "odbc_delivery.h"
#include "odbc_order_status.h"
#include "odbc_payment.h"
#include "odbc_stock_level.h"
#include "odbc_new_order.h"
#include "odbc_integrity.h"
#endif /* ODBC */

#ifdef HAVE_LIBPQ
#include "libpq_delivery.h"
#include "libpq_order_status.h"
#include "libpq_payment.h"
#include "libpq_stock_level.h"
#include "libpq_new_order.h"
#include "libpq_integrity.h"
#endif /* HAVE_LIBPQ */


#ifdef LIBMYSQL
#include "mysql_delivery.h"
#include "mysql_order_status.h"
#include "mysql_payment.h"
#include "mysql_stock_level.h"
#include "mysql_new_order.h"
#include "mysql_integrity.h"
#endif /* LIBMYSQL */


#ifdef LIBSQLITE
#include "nonsp_delivery.h"
#include "nonsp_order_status.h"
#include "nonsp_payment.h"
#include "nonsp_stock_level.h"
#include "nonsp_new_order.h"
#include "nonsp_integrity.h"
#endif /* LIBSQLITE */

#ifdef LIBDRIZZLE
#include "drizzle_delivery.h"
#include "drizzle_order_status.h"
#include "drizzle_payment.h"
#include "drizzle_stock_level.h"
#include "drizzle_new_order.h"
#include "drizzle_integrity.h"
#endif /* LIBDRIZZLE */

int connect_to_db(struct db_context_t *dbc) {
	int rc;

	rc = (*dbc->connect)(dbc);
	if (rc != OK) {
		return ERROR;
	}

	return OK;
}

int disconnect_from_db(struct db_context_t *dbc) {
	int rc;

	rc = (*dbc->disconnect)(dbc);
	if (rc != OK) {
		return ERROR;
	}

	return OK;
}

int process_transaction(int transaction, struct db_context_t *dbc,
	union transaction_data_t *td)
{
	int rc;
	int i;
	int status;

	switch (transaction) {
	case INTEGRITY:
		rc = (*dbc->execute_integrity)(dbc, &td->integrity);
		break;
	case DELIVERY:
		rc = (*dbc->execute_delivery)(dbc, &td->delivery);
		break;
	case NEW_ORDER:
		td->new_order.o_all_local = 1;
		for (i = 0; i < td->new_order.o_ol_cnt; i++) {
			if (td->new_order.order_line[i].ol_supply_w_id !=
					td->new_order.w_id) {
				td->new_order.o_all_local = 0;
				break;
			}
		}
		rc = (*dbc->execute_new_order)(dbc, &td->new_order);
		if (rc != ERROR && td->new_order.rollback == 0) {
			/*
			 * Calculate the adjusted total_amount here to work
			 * around an issue with SAP DB stored procedures that
			 * does not allow any statements to execute after a
			 * SUBTRANS ROLLBACK without throwing an error.
	 		 */
			td->new_order.total_amount =
				td->new_order.total_amount *
				(1 - td->new_order.c_discount) *
				(1 + td->new_order.w_tax + td->new_order.d_tax);
		} else {
			rc = ERROR;
		}
		break;
	case ORDER_STATUS:
		rc = (*dbc->execute_order_status)(dbc, &td->order_status);
		break;
	case PAYMENT:
		rc = (*dbc->execute_payment)(dbc, &td->payment);
		break;
	case STOCK_LEVEL:
		rc = (*dbc->execute_stock_level)(dbc, &td->stock_level);
		break;
	default:
		LOG_ERROR_MESSAGE("unknown transaction type %d", transaction);
		return ERROR;
	}

	/* Commit or rollback the transaction. */
	if (rc == OK) {
		status = (*dbc->commit_transaction)(dbc);
	} else {
		status = (*dbc->rollback_transaction)(dbc);
	}

	return status;
}
