/*
 * db.c
 *
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright (C) 2002 Mark Wong & Open Source Development Lab, Inc.
 *
 * 16 june 2002
 */

#include <db.h>
#include <odbc_delivery.h>
#include <odbc_order_status.h>
#include <odbc_payment.h>
#include <odbc_stock_level.h>
#include <odbc_new_order.h>

#ifdef ODBC
int process_transaction(int transaction, struct odbc_context_t *odbcc,
	union transaction_data_t *odbct)
{
	int rc;
	int i;

	switch (transaction)
	{
		case DELIVERY:
			rc = execute_delivery(odbcc, &odbct->delivery);
			break;
		case NEW_ORDER:
			odbct->new_order.o_all_local = 1;
			for (i = 0; i < odbct->new_order.o_ol_cnt; i++)
			{
				if (odbct->new_order.order_line[i].ol_supply_w_id !=
					odbct->new_order.w_id)
				{
					odbct->new_order.o_all_local = 0;
					break;
				}
			}
			rc = execute_new_order(odbcc, &odbct->new_order);
			/*
			 * Calculate the adjusted total_amount here to work around
			 * an issue with SAP DB stored procedures that does not allow
			 * any statements to execute after a SUBTRANS ROLLBACK without
			 * throwing an error.
			 */
			odbct->new_order.total_amount =
				odbct->new_order.total_amount *
				(1 - odbct->new_order.c_discount) *
				(1 + odbct->new_order.w_tax + odbct->new_order.d_tax);
			break;
		case ORDER_STATUS:
			rc = execute_order_status(odbcc, &odbct->order_status);
			break;
		case PAYMENT:
			rc = execute_payment(odbcc, &odbct->payment);
			break;
		case STOCK_LEVEL:
			rc = execute_stock_level(odbcc, &odbct->stock_level);
			break;
		default:
			LOG_ERROR_MESSAGE("unknown transaction type %d", transaction);
			return ERROR;
	}
	if (rc == OK)
	{
		/* Commit. */
		i = SQLEndTran(SQL_HANDLE_DBC, odbcc->hdbc, SQL_COMMIT);
	}
	else
	{
		/* Rollback. */
		i = SQLEndTran(SQL_HANDLE_DBC, odbcc->hdbc, SQL_ROLLBACK);
	}
	if (i != SQL_SUCCESS && i != SQL_SUCCESS_WITH_INFO)
	{
		LOG_ODBC_ERROR(SQL_HANDLE_STMT, odbcc->hstmt);
		return ERROR;
	}

	return OK;
}
#endif /* ODBC */
