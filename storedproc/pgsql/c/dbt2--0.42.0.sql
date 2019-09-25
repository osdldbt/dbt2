/*
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright (C) 2003-2008 Mark Wong
 *               2003-2008 Open Source Development Labs, Inc.
 *               2015      2ndQuadrant, Ltd.
 */

\echo Use "CREATE EXTENSION dbt2" to load this file. \quit

CREATE OR REPLACE FUNCTION delivery (INTEGER, INTEGER)
RETURNS INTEGER
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT;

CREATE TYPE new_order_info
AS (ol_i_id INTEGER, ol_supply_w_id INTEGER, ol_quantity INTEGER);

CREATE OR REPLACE FUNCTION make_new_order_info (INTEGER, INTEGER, INTEGER)
RETURNS new_order_info
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT;

CREATE OR REPLACE FUNCTION new_order (INTEGER, INTEGER, INTEGER, INTEGER, INTEGER, new_order_info, new_order_info, new_order_info, new_order_info, new_order_info, new_order_info, new_order_info, new_order_info, new_order_info, new_order_info, new_order_info, new_order_info, new_order_info, new_order_info, new_order_info)
RETURNS INTEGER
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT;

CREATE TYPE status_info
AS (ol_i_id INTEGER, ol_supply_w_id INTEGER, ol_quantity REAL, ol_amount REAL, ol_delivery_d TIMESTAMP);

CREATE OR REPLACE FUNCTION order_status (INTEGER, INTEGER, INTEGER, TEXT)
RETURNS SETOF status_info
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT;

CREATE OR REPLACE FUNCTION payment (INTEGER, INTEGER, INTEGER, INTEGER, INTEGER, TEXT, REAL)
RETURNS INTEGER AS
'MODULE_PATHNAME'
LANGUAGE C STRICT;

CREATE OR REPLACE FUNCTION stock_level (INTEGER, INTEGER, INTEGER)
RETURNS INTEGER
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT;
