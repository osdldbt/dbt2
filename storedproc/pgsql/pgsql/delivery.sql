/*
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright The DBT-2 Authors
 *
 * Based on TPC-C Standard Specification Revision 5.11 Clause 2.7.4.
 */

CREATE OR REPLACE FUNCTION delivery (
    in_w_id INTEGER,
    in_o_carrier_id INTEGER
) RETURNS TABLE (
    d_id INTEGER,
    o_id INTEGER
) AS $$
DECLARE
	i INTEGER;
	tmp_c_id INTEGER;
	tmp_ol_amount NUMERIC;
BEGIN
	FOR i IN 1..10 LOOP
        d_id := i;

		SELECT no_o_id
		INTO o_id
		FROM new_order
		WHERE no_w_id = in_w_id
		  AND no_d_id = d_id
		ORDER BY no_o_id ASC
		LIMIT 1;

		IF FOUND THEN
			DELETE FROM new_order
			WHERE no_o_id = delivery.o_id
			  AND no_w_id = in_w_id
			  AND no_d_id = d_id;

			UPDATE orders
			SET o_carrier_id = in_o_carrier_id
			WHERE orders.o_id = delivery.o_id
			  AND orders.o_w_id = in_w_id
			  AND orders.o_d_id = d_id
            RETURNING o_c_id INTO tmp_c_id;

			UPDATE order_line
			SET ol_delivery_d = current_timestamp
			WHERE ol_o_id = delivery.o_id
			  AND ol_w_id = in_w_id
			  AND ol_d_id = d_id;

			SELECT SUM(ol_amount * ol_quantity)
			INTO tmp_ol_amount
			FROM order_line
			WHERE ol_o_id = delivery.o_id
			  AND ol_w_id = in_w_id
			  AND ol_d_id = d_id;

			UPDATE customer
			SET c_delivery_cnt = c_delivery_cnt + 1,
			    c_balance = c_balance + tmp_ol_amount
			WHERE c_id = tmp_c_id
			  AND c_w_id = in_w_id
			  AND c_d_id = d_id;

            RETURN NEXT;
		END IF;
	END LOOP;
END;
$$ LANGUAGE 'plpgsql';
