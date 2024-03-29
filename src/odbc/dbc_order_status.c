/*
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright The DBT-2 Authors
 *
 * Based on TPC-C Standard Specification Revision 5.0.
 */

#include <odbc_order_status.h>

int execute_order_status(
		struct db_context_t *odbcc, struct order_status_t *data) {
	SQLRETURN rc;
	int i = 1;
	int j;

	/* Perpare statement for New Order transaction. */
	rc = SQLPrepare(
			odbcc->library.odbc.hstmt, (unsigned char *) STMT_ORDER_STATUS,
			SQL_NTS);
	if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO) {
		LOG_ODBC_ERROR(SQL_HANDLE_STMT, odbcc->library.odbc.hstmt);
		return ERROR;
	}

	/* Bind variables for New Order transaction. */
	rc = SQLBindParameter(
			odbcc->library.odbc.hstmt, i++, SQL_PARAM_INPUT_OUTPUT, SQL_C_SLONG,
			SQL_INTEGER, 0, 0, &data->c_id, 0, NULL);
	if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO) {
		LOG_ODBC_ERROR(SQL_HANDLE_STMT, odbcc->library.odbc.hstmt);
		return ERROR;
	}
	rc = SQLBindParameter(
			odbcc->library.odbc.hstmt, i++, SQL_PARAM_INPUT, SQL_C_SLONG,
			SQL_INTEGER, 0, 0, &data->c_w_id, 0, NULL);
	if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO) {
		LOG_ODBC_ERROR(SQL_HANDLE_STMT, odbcc->library.odbc.hstmt);
		return ERROR;
	}
	rc = SQLBindParameter(
			odbcc->library.odbc.hstmt, i++, SQL_PARAM_INPUT, SQL_C_SLONG,
			SQL_INTEGER, 0, 0, &data->c_d_id, 0, NULL);
	if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO) {
		LOG_ODBC_ERROR(SQL_HANDLE_STMT, odbcc->library.odbc.hstmt);
		return ERROR;
	}
	rc = SQLBindParameter(
			odbcc->library.odbc.hstmt, i++, SQL_PARAM_OUTPUT, SQL_C_CHAR,
			SQL_VARCHAR, 0, 0, data->c_first, sizeof(data->c_first), NULL);
	if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO) {
		LOG_ODBC_ERROR(SQL_HANDLE_STMT, odbcc->library.odbc.hstmt);
		return ERROR;
	}
	rc = SQLBindParameter(
			odbcc->library.odbc.hstmt, i++, SQL_PARAM_OUTPUT, SQL_C_CHAR,
			SQL_CHAR, 0, 0, data->c_middle, sizeof(data->c_middle), NULL);
	if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO) {
		LOG_ODBC_ERROR(SQL_HANDLE_STMT, odbcc->library.odbc.hstmt);
		return ERROR;
	}
	rc = SQLBindParameter(
			odbcc->library.odbc.hstmt, i++, SQL_PARAM_INPUT_OUTPUT, SQL_C_CHAR,
			SQL_VARCHAR, 0, 0, data->c_last, sizeof(data->c_last), NULL);
	if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO) {
		LOG_ODBC_ERROR(SQL_HANDLE_STMT, odbcc->library.odbc.hstmt);
		return ERROR;
	}
	rc = SQLBindParameter(
			odbcc->library.odbc.hstmt, i++, SQL_PARAM_OUTPUT, SQL_C_DOUBLE,
			SQL_DOUBLE, 0, 0, &data->c_balance, 0, NULL);
	if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO) {
		LOG_ODBC_ERROR(SQL_HANDLE_STMT, odbcc->library.odbc.hstmt);
		return ERROR;
	}
	rc = SQLBindParameter(
			odbcc->library.odbc.hstmt, i++, SQL_PARAM_OUTPUT, SQL_C_SLONG,
			SQL_INTEGER, 0, 0, &data->o_id, 0, NULL);
	if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO) {
		LOG_ODBC_ERROR(SQL_HANDLE_STMT, odbcc->library.odbc.hstmt);
		return ERROR;
	}
	rc = SQLBindParameter(
			odbcc->library.odbc.hstmt, i++, SQL_PARAM_OUTPUT, SQL_C_SLONG,
			SQL_INTEGER, 0, 0, &data->o_carrier_id, 0, NULL);
	if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO) {
		LOG_ODBC_ERROR(SQL_HANDLE_STMT, odbcc->library.odbc.hstmt);
		return ERROR;
	}
	rc = SQLBindParameter(
			odbcc->library.odbc.hstmt, i++, SQL_PARAM_OUTPUT, SQL_C_CHAR,
			SQL_VARCHAR, 0, 0, data->o_entry_d, sizeof(data->o_entry_d), NULL);
	if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO) {
		LOG_ODBC_ERROR(SQL_HANDLE_STMT, odbcc->library.odbc.hstmt);
		return ERROR;
	}
	rc = SQLBindParameter(
			odbcc->library.odbc.hstmt, i++, SQL_PARAM_OUTPUT, SQL_C_SLONG,
			SQL_INTEGER, 0, 0, &data->o_ol_cnt, 0, NULL);
	if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO) {
		LOG_ODBC_ERROR(SQL_HANDLE_STMT, odbcc->library.odbc.hstmt);
		return ERROR;
	}
	for (j = 0; j < O_OL_CNT_MAX; j++) {
		rc = SQLBindParameter(
				odbcc->library.odbc.hstmt, i++, SQL_PARAM_OUTPUT, SQL_C_SLONG,
				SQL_INTEGER, 0, 0, &data->order_line[j].ol_supply_w_id, 0,
				NULL);
		if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO) {
			LOG_ODBC_ERROR(SQL_HANDLE_STMT, odbcc->library.odbc.hstmt);
			return ERROR;
		}
		rc = SQLBindParameter(
				odbcc->library.odbc.hstmt, i++, SQL_PARAM_OUTPUT, SQL_C_SLONG,
				SQL_INTEGER, 0, 0, &data->order_line[j].ol_i_id, 0, NULL);
		if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO) {
			LOG_ODBC_ERROR(SQL_HANDLE_STMT, odbcc->library.odbc.hstmt);
			return ERROR;
		}
		rc = SQLBindParameter(
				odbcc->library.odbc.hstmt, i++, SQL_PARAM_OUTPUT, SQL_C_SLONG,
				SQL_INTEGER, 0, 0, &data->order_line[j].ol_quantity, 0, NULL);
		if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO) {
			LOG_ODBC_ERROR(SQL_HANDLE_STMT, odbcc->library.odbc.hstmt);
			return ERROR;
		}
		rc = SQLBindParameter(
				odbcc->library.odbc.hstmt, i++, SQL_PARAM_OUTPUT, SQL_C_DOUBLE,
				SQL_DOUBLE, 0, 0, &data->order_line[j].ol_amount, 0, NULL);
		if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO) {
			LOG_ODBC_ERROR(SQL_HANDLE_STMT, odbcc->library.odbc.hstmt);
			return ERROR;
		}
		rc = SQLBindParameter(
				odbcc->library.odbc.hstmt, i++, SQL_PARAM_OUTPUT, SQL_C_CHAR,
				SQL_VARCHAR, 0, 0, data->order_line[j].ol_delivery_d,
				sizeof(data->order_line[j].ol_delivery_d), NULL);
		if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO) {
			LOG_ODBC_ERROR(SQL_HANDLE_STMT, odbcc->library.odbc.hstmt);
			return ERROR;
		}
	}

	/* Execute stored procedure. */
	rc = SQLExecute(odbcc->library.odbc.hstmt);
	if (check_odbc_rc(SQL_HANDLE_STMT, odbcc->library.odbc.hstmt, rc) ==
		ERROR) {
		LOG_ODBC_ERROR(SQL_HANDLE_STMT, odbcc->library.odbc.hstmt);
		return ERROR;
	}

	return OK;
}
