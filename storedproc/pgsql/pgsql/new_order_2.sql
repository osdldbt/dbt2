/*
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright (C) 2003      Open Source Development Lab, Inc.
 *               2003-2021 Mark Wong
 *
 * Based on TPC-C Standard Specification Revision 5.0 Clause 2.8.2.
 */

CREATE OR REPLACE FUNCTION make_new_order_info (
    _ol_i_id INTEGER,
    _ol_supply_w_id INTEGER,
    _ol_quantity INTEGER
) RETURNS TABLE (
    ol_i_id INTEGER,
    ol_supply_w_id INTEGER,
    ol_quantity INTEGER
)
AS $$
BEGIN
    ol_i_id := _ol_i_id;
    ol_supply_w_id := _ol_supply_w_id;
    ol_quantity := _ol_quantity;
    RETURN NEXT;
END;
$$ LANGUAGE 'plpgsql';
