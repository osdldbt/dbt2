/*
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright (C) 2003-2008 Mark Wong
 *               2003-2008 Open Source Development Labs, Inc.
 *               2015      2ndQuadrant, Ltd.
 */

\echo Use "CREATE EXTENSION dbt2" to load this file. \quit

CREATE OR REPLACE FUNCTION delivery (
    w_id INTEGER,
    o_carrier_id INTEGER
) RETURNS TABLE (
    d_id INTEGER,
    o_id INTEGER
) AS 'MODULE_PATHNAME', 'delivery'
LANGUAGE C VOLATILE;

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
) AS 'MODULE_PATHNAME', 'new_order'
LANGUAGE C VOLATILE;

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
)AS 'MODULE_PATHNAME'
LANGUAGE C VOLATILE;

CREATE OR REPLACE FUNCTION payment (
    w_id INTEGER,
    d_id INTEGER,
    c_id INTEGER,
    c_w_id INTEGER,
    c_d_id INTEGER,
    c_last TEXT,
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
) AS 'MODULE_PATHNAME', 'payment'
LANGUAGE C VOLATILE;

CREATE OR REPLACE FUNCTION stock_level (
    w_id INTEGER,
    d_id INTEGER,
    threshold INTEGER
) RETURNS INTEGER AS
'MODULE_PATHNAME', 'stock_level'
LANGUAGE C VOLATILE;
