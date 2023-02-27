/*
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright The DBT-2 Authors
 *
 * Based on TPC-C Standard Specification Revision 5.11 Clause 2.6.2.
 */

CREATE OR REPLACE FUNCTION order_status (
    c_id INTEGER,
    c_w_id INTEGER,
    c_d_id INTEGER,
    c_last TEXT
) RETURNS TABLE (
    ol_i_id INTEGER,
    ol_supply_w_id INTEGER,
    ol_quantity REAL,
    ol_amount REAL,
    ol_delivery_d TIMESTAMP
) AS $$
DECLARE
	tmp_c_first VARCHAR;
	tmp_c_middle VARCHAR;
	tmp_c_last VARCHAR;
	tmp_c_balance NUMERIC;

	tmp_o_id INTEGER;
	tmp_o_carrier_id INTEGER;
	tmp_o_entry_d VARCHAR;
	tmp_o_ol_cnt INTEGER;

	ol RECORD;

    tmp_c_id INTEGER;
BEGIN
	/*
	 * Pick a customer by searching for c_last, should pick the one in the
	 * middle, not the first one.
	 */
	IF c_id = 0 THEN
		SELECT customer.c_id
		INTO tmp_c_id
		FROM customer
		WHERE customer.c_w_id = order_status.c_w_id
		  AND customer.c_d_id = order_status.c_d_id
		  AND customer.c_last = order_status.c_last
		ORDER BY c_first ASC;
	ELSE
		tmp_c_id = c_id;
	END IF;

	SELECT c_first, c_middle, customer.c_last, c_balance
	INTO tmp_c_first, tmp_c_middle, tmp_c_last, tmp_c_balance
	FROM customer
	WHERE customer.c_w_id = order_status.c_w_id
	  AND customer.c_d_id = order_status.c_d_id
	  AND customer.c_id = tmp_c_id;

	SELECT o_id, o_carrier_id, o_entry_d, o_ol_cnt
	INTO tmp_o_id, tmp_o_carrier_id, tmp_o_entry_d, tmp_o_ol_cnt
	FROM orders
	WHERE o_w_id = c_w_id
  	AND o_d_id = c_d_id
  	AND o_c_id = tmp_c_id
	ORDER BY o_id DESC
    LIMIT 1;

	FOR ol IN
		SELECT order_line.ol_i_id, order_line.ol_supply_w_id,
		       order_line.ol_quantity, order_line.ol_amount,
               order_line.ol_delivery_d
		FROM order_line
		WHERE ol_w_id = c_w_id
		  AND ol_d_id = c_d_id
		  AND ol_o_id = tmp_o_id
	LOOP
        ol_i_id := ol.ol_i_id;
        ol_supply_w_id := ol.ol_supply_w_id;
        ol_quantity := ol.ol_quantity;
        ol_amount := ol.ol_amount;
        ol_delivery_d := ol.ol_delivery_d;
		RETURN NEXT;
	END LOOP;
END;
$$ LANGUAGE 'plpgsql';
