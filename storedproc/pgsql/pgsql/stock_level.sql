/*
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright (C) 2003      Open Source Development Lab, Inc.
 *               2003-2021 Mark Wong
 *
 * Based on TPC-C Standard Specification Revision 5.11 Clause 2.8.2.
 */

CREATE OR REPLACE FUNCTION stock_level (
    sl_w_id INTEGER,
    sl_d_id INTEGER,
    sl_threshold INTEGER)
RETURNS INTEGER AS $$
DECLARE
	sl_d_next_o_id INTEGER;

	sl_low_stock INTEGER;
BEGIN
	SELECT d_next_o_id
	INTO sl_d_next_o_id
	FROM district
	WHERE d_w_id = sl_w_id
	AND d_id = sl_d_id;

    WITH
    ol AS (
        SELECT DISTINCT ol_i_id
        FROM order_line
	    WHERE ol_w_id = sl_w_id
	      AND ol_d_id = sl_d_id
	      AND ol_o_id BETWEEN (sl_d_next_o_id - 20)
	                      AND (sl_d_next_o_id - 1)
    )
	SELECT count(*)
	INTO sl_low_stock
	FROM ol, stock
	WHERE s_w_id = sl_w_id
	  AND s_i_id = ol_i_id
	  AND s_quantity < sl_threshold;

	RETURN sl_low_stock;
END;
$$ LANGUAGE 'plpgsql';
