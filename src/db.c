/*
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright The DBT-2 Authors
 *
 * 16 June 2002
 */

#define _POSIX_C_SOURCE 199309L

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <errno.h>
#include <string.h>
#include <unistd.h>

#include "db.h"
#include "logging.h"

#ifdef HAVE_ODBC
#include "odbc_delivery.h"
#include "odbc_order_status.h"
#include "odbc_payment.h"
#include "odbc_stock_level.h"
#include "odbc_new_order.h"
#include "odbc_integrity.h"
#endif /* HAVE_ODBC */

#ifdef HAVE_LIBPQ
#include "libpq_delivery.h"
#include "libpq_order_status.h"
#include "libpq_payment.h"
#include "libpq_stock_level.h"
#include "libpq_new_order.h"
#include "libpq_integrity.h"
#endif /* HAVE_LIBPQ */

#ifdef HAVE_MYSQL
#include "mysql_delivery.h"
#include "mysql_integrity.h"
#include "mysql_new_order.h"
#include "mysql_order_status.h"
#include "mysql_payment.h"
#include "mysql_stock_level.h"
#endif /* HAVE_MYSQL */

#ifdef HAVE_SQLITE3
#include "nonsp_delivery.h"
#include "nonsp_order_status.h"
#include "nonsp_payment.h"
#include "nonsp_stock_level.h"
#include "nonsp_new_order.h"
#include "nonsp_integrity.h"
#endif /* HAVE_SQLITE3 */

const char s_dist[10][11] = {
	"s_dist_01", "s_dist_02", "s_dist_03", "s_dist_04", "s_dist_05",
	"s_dist_06", "s_dist_07", "s_dist_08", "s_dist_09", "s_dist_10"
};

int connect_to_db(struct db_context_t *dbc) {
	int rc;
	struct timespec ts0, rem0;
	int retry_count = 0;

	while (1) {
		time_t tt = time(NULL);
		if (dbc->stop_time != 0 && tt > dbc->stop_time) return ERROR;
		++retry_count;
		rc = (*dbc->connect)(dbc);
		if (rc == OK) {
			LOG_ERROR_MESSAGE("%d retries to reconnect", retry_count);
			break;
		}
		ts0.tv_sec = dbc->ts_retry.tv_sec;
		ts0.tv_nsec = dbc->ts_retry.tv_nsec;
		while (nanosleep(&ts0, &rem0) == -1) {
			extern int errno;
			if (errno == EINTR) {
				memcpy(&ts0, &rem0, sizeof(struct timespec));
			} else {
				LOG_ERROR_MESSAGE(
						"sleep time invalid %ld s %ld ns",
						ts0.tv_sec, ts0.tv_nsec);
			}
		}
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
	int status = 0;
	int txn_count = 0;
	struct timespec ts0, rem0;

	while (txn_count++ < 1) {
		int rc2;
		int retry_count = 0;

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

		/* Commit or rollback the transaction on expected OK or ERROR. */
		if (rc == OK) {
			status = (*dbc->commit_transaction)(dbc);
			if (status == OK) break;
		} else if (rc == ERROR) {
			status = (*dbc->rollback_transaction)(dbc);
			if (status == STATUS_ROLLBACK) break;
		}

		/* Reconnect to the database on unexpected return codes. */
		LOG_ERROR_MESSAGE("cleaning up connection before retry");
		disconnect_from_db(dbc);
		/* Retry until the calculated stop time. */
		while (1) {
			time_t tt = time(NULL);
			if (dbc->stop_time == 0 || tt > dbc->stop_time) return ERROR;

			++retry_count;
			LOG_ERROR_MESSAGE("retrying database connection");
			rc2 = connect_to_db(dbc);
			if (rc2 == OK) {
				LOG_ERROR_MESSAGE("%d retries to reconnect", retry_count);
				break;
			}
			ts0.tv_sec = dbc->ts_retry.tv_sec;
			ts0.tv_nsec = dbc->ts_retry.tv_nsec;
			while (nanosleep(&ts0, &rem0) == -1) {
				extern int errno;
				if (errno == EINTR) {
					memcpy(&ts0, &rem0, sizeof(struct timespec));
				} else {
					LOG_ERROR_MESSAGE(
							"sleep time invalid %ld s %ld ns",
							ts0.tv_sec, ts0.tv_nsec);
				}
			}
		}
		if (rc2 != OK) {
			LOG_ERROR_MESSAGE("unable to reconnect to database");
			/* Don't know what to do...  Keep looping until better idea... */
		}
	}

	return status;
}
