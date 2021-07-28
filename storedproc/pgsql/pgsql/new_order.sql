/*
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright (C) 2003      Open Source Development Lab, Inc.
 *               2003-2021 Mark Wong
 *
 * Based on TPC-C Standard Specification Revision 5.11 Clause 2.4.2.
 */

CREATE TYPE new_order_info
AS (ol_i_id INTEGER, ol_supply_w_id INTEGER, ol_quantity INTEGER);

CREATE OR REPLACE FUNCTION new_order (
    w_id INTEGER,
    d_id INTEGER,
    c_id INTEGER,
    o_all_local INTEGER,
    o_ol_cnt INTEGER,
    order_line_1 new_order_info,
    order_line_2 new_order_info,
    order_line_3 new_order_info,
    order_line_4 new_order_info,
    order_line_5 new_order_info,
    order_line_6 new_order_info,
    order_line_7 new_order_info,
    order_line_8 new_order_info,
    order_line_9 new_order_info,
    order_line_10 new_order_info,
    order_line_11 new_order_info,
    order_line_12 new_order_info,
    order_line_13 new_order_info,
    order_line_14 new_order_info,
    order_line_15 new_order_info
) RETURNS TABLE (
    ol_supply_w_id INTEGER,
    ol_i_id INTEGER,
    i_name TEXT,
    ol_quantity INTEGER,
    s_quantity INTEGER,
    i_price REAL,
    ol_amount REAL,
    brand_generic CHAR
) AS $$
DECLARE
    i INTEGER;

    ol new_order_info[15];

    out_w_tax NUMERIC;

    out_d_tax NUMERIC;
    out_d_next_o_id INTEGER;

    out_c_discount NUMERIC;
    out_c_last VARCHAR;
    out_c_credit VARCHAR;

    tmp_i_data VARCHAR(50);
    tmp_s_dist VARCHAR(24);
    tmp_s_data VARCHAR(50);

    tmp_ol_amount NUMERIC;
    tmp_ol_supply_w_id INTEGER;
    tmp_ol_quantity INTEGER;

    tmp_total_amount NUMERIC;

    decr_quantity REAL;

    t1 BOOLEAN;
    t2 BOOLEAN;
BEGIN
    ol[0] := order_line_1;
    ol[1] := order_line_2;
    ol[2] := order_line_3;
    ol[3] := order_line_4;
    ol[4] := order_line_5;
    ol[5] := order_line_6;
    ol[6] := order_line_7;
    ol[7] := order_line_8;
    ol[8] := order_line_9;
    ol[9] := order_line_10;
    ol[10] := order_line_11;
    ol[11] := order_line_12;
    ol[12] := order_line_13;
    ol[13] := order_line_14;
    ol[14] := order_line_15;

    SELECT w_tax
    INTO out_w_tax
    FROM warehouse
    WHERE warehouse.w_id = new_order.w_id;

    UPDATE district
    SET d_next_o_id = d_next_o_id + 1
    WHERE d_w_id = w_id
      AND district.d_id = new_order.d_id
    RETURNING d_tax, d_next_o_id
    INTO out_d_tax, out_d_next_o_id;

    SELECT c_discount, c_last, c_credit
    INTO out_c_discount, out_c_last, out_c_credit
    FROM customer
    WHERE c_w_id = w_id
      AND c_d_id = d_id
      AND customer.c_id = new_order.c_id;

    INSERT INTO new_order (no_o_id, no_d_id, no_w_id)
    VALUES (out_d_next_o_id, d_id, w_id);

    SELECT d_tax, d_next_o_id
    INTO out_d_tax, out_d_next_o_id
    FROM district
    WHERE d_w_id = w_id
      AND district.d_id = new_order.d_id;

    INSERT INTO orders (o_id, o_d_id, o_w_id, o_c_id, o_entry_d,
                        o_carrier_id, o_ol_cnt, o_all_local)
    VALUES (out_d_next_o_id, d_id, w_id, c_id,
            CURRENT_TIMESTAMP, NULL, o_ol_cnt, o_all_local);

    tmp_total_amount = 0;

    FOR i IN 0..(o_ol_cnt - 1) LOOP
        ol_i_id := ol[i].ol_i_id;
        ol_supply_w_id := ol[i].ol_supply_w_id;
        ol_quantity := ol[i].ol_quantity;

        SELECT item.i_price, item.i_name, i_data
        INTO new_order.i_price, new_order.i_name, tmp_i_data
        FROM item
        WHERE i_id = ol_i_id;

        IF NOT FOUND THEN
            RAISE EXCEPTION 'item not found';
        END IF;

        ol_amount := i_price * ol_quantity;
        tmp_total_amount := tmp_total_amount + ol_amount;

        EXECUTE 'SELECT stock.s_quantity, s_dist_' || lpad(d_id::TEXT, 2, '0')
            || ', s_data FROM stock WHERE s_i_id = $1 AND s_w_id = $2'
        INTO new_order.s_quantity, tmp_s_dist, tmp_s_data
        USING ol_i_id, w_id;

        IF s_quantity > ol_quantity THEN
            decr_quantity :=  ol_quantity;
        ELSE
            decr_quantity :=  ol_quantity - 91;
        END IF;

        SELECT tmp_i_data ~ 'ORIGINAL', tmp_s_data ~ 'ORIGINAL'
        INTO t1, t2;

        IF t1 AND t2 THEN
            brand_generic := 'B';
        ELSE
            brand_generic := 'G';
        END IF;

        UPDATE stock
        SET s_quantity = new_order.s_quantity - decr_quantity
        WHERE s_i_id = ol_i_id
          AND s_w_id = w_id;

        INSERT INTO order_line (ol_o_id, ol_d_id, ol_w_id, ol_number, ol_i_id,
                                ol_supply_w_id, ol_delivery_d, ol_quantity,
                                ol_amount, ol_dist_info)
        VALUES (out_d_next_o_id, d_id, w_id, i + 1, ol_i_id, ol_supply_w_id,
                NULL, ol_quantity, ol_amount, tmp_s_dist);

        RETURN NEXT;
    END LOOP;

    RETURN;
END;
$$ LANGUAGE 'plpgsql';
