/*
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright (C) 2003      Open Source Development Lab, Inc.
 *               2003-2021 Mark Wong
 *
 * Based on TPC-C Standard Specification Revision 5.11 Clause 2.5.2.
 */

CREATE OR REPLACE FUNCTION payment (
    w_id INTEGER,
    d_id INTEGER,
    c_id INTEGER,
    c_w_id INTEGER,
    c_d_id INTEGER,
    in_c_last TEXT,
    h_amount REAL
) RETURNS TABLE (
    w_street_1 TEXT,
    w_street_2 TEXT,
    w_city TEXT,
    w_state TEXT,
    w_zip TEXT,
    d_street_1 TEXT,
    d_street_2 TEXT,
    d_city TEXT,
    d_state TEXT,
    d_zip TEXT,
    c_first TEXT,
    c_middle TEXT,
    c_last TEXT,
    c_street_1 TEXT,
    c_street_2 TEXT,
    c_city TEXT,
    c_state TEXT,
    c_zip TEXT,
    c_phone TEXT,
    c_since TIMESTAMP,
    c_credit TEXT,
    c_credit_lim NUMERIC,
    c_discount REAL,
    c_balance NUMERIC,
    c_data TEXT,
    h_date TIMESTAMP
) AS $$
DECLARE
    d_name VARCHAR;
    w_name VARCHAR;

    tmp_c_id INTEGER;
BEGIN
	UPDATE warehouse
	SET w_ytd = w_ytd + h_amount
	WHERE warehouse.w_id = payment.w_id
    RETURNING warehouse.w_name, warehouse.w_street_1, warehouse.w_street_2,
              warehouse.w_city, warehouse.w_state, warehouse.w_zip
    INTO w_name, w_street_1, w_street_2, w_city, w_state, w_zip;

	UPDATE district
	SET d_ytd = d_ytd + h_amount
	WHERE district.d_id = payment.d_id
	  AND d_w_id = payment.w_id
	RETURNING district.d_name, district.d_street_1, district.d_street_2,
              district.d_city, district.d_state, district.d_zip
	INTO d_name, d_street_1, d_street_2, d_city, d_state, d_zip;

	/*
	 * Pick a customer by searching for c_last, supposed to pick the one in the
	 * middle, not the first one.
	 */
	IF c_id = 0 THEN
		SELECT customer.c_id
		INTO tmp_c_id
		FROM customer
		WHERE customer.c_w_id = payment.c_w_id
		  AND customer.c_d_id = payment.c_d_id
		  AND customer.c_last = in_c_last
		ORDER BY customer.c_first ASC;
	ELSE
		tmp_c_id = c_id;
	END IF;

	SELECT customer.c_first, customer.c_middle, customer.c_last,
           customer.c_street_1, customer.c_street_2, customer.c_city,
           customer.c_state, customer.c_zip, customer.c_phone,
           customer.c_since, customer.c_credit, customer.c_credit_lim,
           customer.c_discount, customer.c_balance
    INTO c_first, c_middle, c_last, c_street_1, c_street_2, c_city, c_state,
         c_zip, c_phone, c_since, c_credit, c_credit_lim, c_discount,
         c_balance
	FROM customer
	WHERE customer.c_w_id = payment.c_w_id
	  AND customer.c_d_id = payment.c_d_id
	  AND customer.c_id = tmp_c_id;

	/* Check credit rating. */
	IF c_credit = 'BC' THEN
		UPDATE customer
		SET c_balance = customer.c_balance - h_amount,
		    c_ytd_payment = c_ytd_payment + 1,
            c_data = substring(tmp_c_id || ' ' || payment.c_d_id || ' '
                               || payment.c_w_id || ' ' || d_id || ' ' || w_id
                               || ' ' || h_amount || ' ' || customer.c_data, 1,
                               500)
		WHERE customer.c_id = payment.c_id
		  AND customer.c_w_id = payment.c_w_id
		  AND customer.c_d_id = payment.c_d_id
        RETURNING substring(customer.c_data, 1, 200)
        INTO payment.c_data;
	ELSE
		UPDATE customer
		SET c_balance = customer.c_balance - h_amount,
		    c_ytd_payment = c_ytd_payment + 1
		WHERE customer.c_id = payment.c_id
		  AND customer.c_w_id = payment.c_w_id
		  AND customer.c_d_id = payment.c_d_id;
	END IF;

	INSERT INTO history (h_c_id, h_c_d_id, h_c_w_id, h_d_id, h_w_id,
	                     h_date, h_amount, h_data)
	VALUES (c_id, c_d_id, c_w_id, d_id, w_id,
		    CURRENT_TIMESTAMP, h_amount,
            substring(w_name || '    ' || d_name, 1, 24))
    RETURNING history.h_date
    INTO payment.h_date;

    RETURN NEXT;
END;
$$ LANGUAGE 'plpgsql';
