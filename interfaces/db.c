/*
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright (C) 2002 Mark Wong & Open Source Development Labs, Inc.
 *
 * 16 June 2002
 */

#include "db.h"
#include "logging.h"

#ifdef ODBC
#include "odbc_delivery.h"
#include "odbc_order_status.h"
#include "odbc_payment.h"
#include "odbc_stock_level.h"
#include "odbc_new_order.h"
#endif /* ODBC */

#ifdef LIBPQ
#include "libpq_delivery.h"
#include "libpq_order_status.h"
#include "libpq_payment.h"
#include "libpq_stock_level.h"
#include "libpq_new_order.h"
#endif /* LIBPQ */

int connect_to_db(struct db_context_t *dbc) {
	int rc;

	rc = _connect_to_db(dbc);
	if (rc != OK) {
		return ERROR;
	}

	return OK;
}

#ifdef ODBC
int db_init(char *sname, char *uname, char *auth)
#endif /* ODBC */
#ifdef LIBPQ
int db_init(char *_dbname, char *_pghost, char *_pgport)
#endif /* LIBPQ */
{
	int rc;

#ifdef ODBC
	rc = _db_init(sname, uname, auth);
#endif /* ODBC */

#ifdef LIBPQ
	rc = _db_init(_dbname, _pghost, _pgport);
#endif /* LIBPQ */

	return OK;
}

int disconnect_from_db(struct db_context_t *dbc) {
	int rc;

#ifdef ODBC
	/* ODBC _disconnect_from_db() is halting for some reason. */
	return OK;
#endif /* ODBC */
	rc = _disconnect_from_db(dbc);
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
	case DELIVERY:
		rc = execute_delivery(dbc, &td->delivery);
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
		rc = execute_new_order(dbc, &td->new_order);
		if (td->new_order.rollback == 0) {
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
		rc = execute_order_status(dbc, &td->order_status);
		break;
	case PAYMENT:
		rc = execute_payment(dbc, &td->payment);
		break;
	case STOCK_LEVEL:
		rc = execute_stock_level(dbc, &td->stock_level);
		break;
	default:
		LOG_ERROR_MESSAGE("unknown transaction type %d", transaction);
		return ERROR;
	}
#ifdef ODBC
	if (rc == OK) {
		/* Commit. */
		i = SQLEndTran(SQL_HANDLE_DBC, dbc->hdbc, SQL_COMMIT);
		status = OK;
	} else {
		/* Rollback. */
		i = SQLEndTran(SQL_HANDLE_DBC, dbc->hdbc, SQL_ROLLBACK);
		status = STATUS_ROLLBACK;
	}
	if (i != SQL_SUCCESS && i != SQL_SUCCESS_WITH_INFO) {
		LOG_ODBC_ERROR(SQL_HANDLE_STMT, dbc->hstmt);
		status = ERROR;
	}
#endif /* ODBC */

#ifdef LIBPQ
	/* Placeholder until I figure out how to handle rollbacks. */
	status = OK;
#endif /* LIBPQ */

	return status;
}
