/* This file is released under the terms of the Artistic License.  Please see
/* the file LICENSE, included in this package, for details.
/*
/* Copyright (C) 2002 Mark Wong & Open Source Development Lab, Inc.
/*
/* Based on TPC-C Standard Specification Revision 5.0 Clause 2.8.2.
CREATE DBPROC stock_level(IN w_id FIXED(9), IN d_id FIXED(2),
IN threshold FIXED(4), OUT low_stock FIXED(9))
AS
  VAR d_next_o_id FIXED(8); s_quantity FIXED(4); d_next_high_o_id FIXED(8); d_next_low_o_id FIXED(8);
SUBTRANS BEGIN;
  SELECT d_next_o_id
  INTO :d_next_o_id
  FROM dbt.district
  WHERE d_w_id = :w_id
    AND d_id = :d_id
  WITH LOCK ISOLATION LEVEL 0;
  SET low_stock = 0;
  SET d_next_high_o_id = d_next_o_id - 20;
  SET d_next_low_o_id = d_next_o_id - 1;
  SELECT count(DISTINCT s_i_id)
  INTO :low_stock
  FROM dbt.order_line, dbt.stock, dbt.district
  WHERE d_id = :d_id
    AND d_w_id = :w_id
    AND d_id = ol_d_id
    AND d_w_id = ol_w_id
    AND ol_i_id = s_i_id
    AND ol_w_id = s_w_id
    AND s_quantity < :threshold
    AND ol_o_id BETWEEN (:d_next_high_o_id)
                    AND (:d_next_low_o_id)
  WITH LOCK ISOLATION LEVEL 0;
SUBTRANS END;;
