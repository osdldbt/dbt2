/ This file is released under the terms of the Artistic License.  Please see
/ the file LICENSE, included in this package, for details.
/
/ Copyright (C) 2002 Mark Wong & Open Source Development Lab, Inc.
/
/ Based on TPC-C Standard Specification Revision 5.0 Clause 2.8.2.
CREATE DBPROC stock_level(IN w_id FIXED(9), IN d_id FIXED(2),
IN threshold FIXED(4), OUT low_stock FIXED(9))
AS
  VAR d_next_o_id FIXED(8);
SUBTRANS BEGIN;
  SELECT d_next_o_id
  INTO :d_next_o_id
  FROM dbt.district
  WHERE d_w_id = :w_id
    AND d_id = :d_id;
  SET low_stock = 0;
  SET d_next_o_id = d_next_o_id - 1;
  CALL stock_level_2(:w_id, :d_id, :d_next_o_id, :threshold, :low_stock);
  SET d_next_o_id = d_next_o_id - 1;
  CALL stock_level_2(:w_id, :d_id, :d_next_o_id, :threshold, :low_stock);
  SET d_next_o_id = d_next_o_id - 1;
  CALL stock_level_2(:w_id, :d_id, :d_next_o_id, :threshold, :low_stock);
  SET d_next_o_id = d_next_o_id - 1;
  CALL stock_level_2(:w_id, :d_id, :d_next_o_id, :threshold, :low_stock);
  SET d_next_o_id = d_next_o_id - 1;
  CALL stock_level_2(:w_id, :d_id, :d_next_o_id, :threshold, :low_stock);
  SET d_next_o_id = d_next_o_id - 1;
  CALL stock_level_2(:w_id, :d_id, :d_next_o_id, :threshold, :low_stock);
  SET d_next_o_id = d_next_o_id - 1;
  CALL stock_level_2(:w_id, :d_id, :d_next_o_id, :threshold, :low_stock);
  SET d_next_o_id = d_next_o_id - 1;
  CALL stock_level_2(:w_id, :d_id, :d_next_o_id, :threshold, :low_stock);
  SET d_next_o_id = d_next_o_id - 1;
  CALL stock_level_2(:w_id, :d_id, :d_next_o_id, :threshold, :low_stock);
  SET d_next_o_id = d_next_o_id - 1;
  CALL stock_level_2(:w_id, :d_id, :d_next_o_id, :threshold, :low_stock);
  SET d_next_o_id = d_next_o_id - 1;
  CALL stock_level_2(:w_id, :d_id, :d_next_o_id, :threshold, :low_stock);
  SET d_next_o_id = d_next_o_id - 1;
  CALL stock_level_2(:w_id, :d_id, :d_next_o_id, :threshold, :low_stock);
  SET d_next_o_id = d_next_o_id - 1;
  CALL stock_level_2(:w_id, :d_id, :d_next_o_id, :threshold, :low_stock);
  SET d_next_o_id = d_next_o_id - 1;
  CALL stock_level_2(:w_id, :d_id, :d_next_o_id, :threshold, :low_stock);
  SET d_next_o_id = d_next_o_id - 1;
  CALL stock_level_2(:w_id, :d_id, :d_next_o_id, :threshold, :low_stock);
  SET d_next_o_id = d_next_o_id - 1;
  CALL stock_level_2(:w_id, :d_id, :d_next_o_id, :threshold, :low_stock);
  SET d_next_o_id = d_next_o_id - 1;
  CALL stock_level_2(:w_id, :d_id, :d_next_o_id, :threshold, :low_stock);
  SET d_next_o_id = d_next_o_id - 1;
  CALL stock_level_2(:w_id, :d_id, :d_next_o_id, :threshold, :low_stock);
  SET d_next_o_id = d_next_o_id - 1;
  CALL stock_level_2(:w_id, :d_id, :d_next_o_id, :threshold, :low_stock);
  SET d_next_o_id = d_next_o_id - 1;
  CALL stock_level_2(:w_id, :d_id, :d_next_o_id, :threshold, :low_stock);
SUBTRANS END;;
