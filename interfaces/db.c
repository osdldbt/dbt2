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

#ifdef ODBC
int process_transaction(int transaction, struct odbc_context_t *odbcc,
	union odbc_transaction_t *odbct)
{
	switch (transaction)
	{
		case DELIVERY:
			execute_delivery(odbcc, &odbct->delivery);
			break;
		case NEW_ORDER:
			execute_new_order(odbcc, &odbct->new_order);
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
			execute_order_status(odbcc, &odbct->order_status);
			break;
		case PAYMENT:
			execute_payment(odbcc, &odbct->payment);
			break;
		case STOCK_LEVEL:
			execute_stock_level(odbcc, &odbct->stock_level);
			break;
	}

	return OK;
}
#endif /* ODBC */
