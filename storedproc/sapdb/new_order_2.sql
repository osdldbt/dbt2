/* This file is released under the terms of the Artistic License.  Please see
/* the file LICENSE, included in this package, for details.
/*
/* Copyright (C) 2002 Mark Wong & Open Source Development Lab, Inc.
/*
/* Based on TPC-C Standard Specification Revision 5.0 Clause 2.4.2.
CREATE DBPROC new_order_2(IN w_id FIXED(9), IN d_id FIXED(2),
IN ol_i_id FIXED(6), IN ol_quantity FIXED(2), IN i_price FIXED(10, 5),
IN i_name VARCHAR(24), IN i_data VARCHAR(50), IN ol_o_id FIXED(8),
IN ol_amount FIXED(12, 6), IN ol_supply_w_id FIXED(9), IN ol_number FIXED(2),
OUT s_quantity FIXED(8, 4))
AS
  VAR s_dist VARCHAR(24); s_data VARCHAR(50);
BEGIN
  IF d_id = 1 THEN
    SELECT s_quantity, s_dist_01, s_data
    INTO :s_quantity, :s_dist, :s_data
    FROM dbt.stock
    WHERE s_i_id = :ol_i_id
    AND s_w_id = :w_id
  ELSE IF d_id = 2 THEN
    SELECT s_quantity, s_dist_02, s_data
    INTO :s_quantity, :s_dist, :s_data
    FROM dbt.stock
    WHERE s_i_id = :ol_i_id
    AND s_w_id = :w_id
  ELSE IF d_id = 3 THEN
    SELECT s_quantity, s_dist_03, s_data
    INTO :s_quantity, :s_dist, :s_data
    FROM dbt.stock
    WHERE s_i_id = :ol_i_id
      AND s_w_id = :w_id
  ELSE IF d_id = 4 THEN
    SELECT s_quantity, s_dist_04, s_data
    INTO :s_quantity, :s_dist, :s_data
    FROM dbt.stock
    WHERE s_i_id = :ol_i_id
      AND s_w_id = :w_id
  ELSE IF d_id = 5 THEN
    SELECT s_quantity, s_dist_05, s_data
    INTO :s_quantity, :s_dist, :s_data
    FROM dbt.stock
    WHERE s_i_id = :ol_i_id
      AND s_w_id = :w_id
  ELSE IF d_id = 6 THEN
    SELECT s_quantity, s_dist_06, s_data
    INTO :s_quantity, :s_dist, :s_data
    FROM dbt.stock
    WHERE s_i_id = :ol_i_id
      AND s_w_id = :w_id
  ELSE IF d_id = 7 THEN
    SELECT s_quantity, s_dist_07, s_data
    INTO :s_quantity, :s_dist, :s_data
    FROM dbt.stock
    WHERE s_i_id = :ol_i_id
      AND s_w_id = :w_id
  ELSE IF d_id = 8 THEN
    SELECT s_quantity, s_dist_08, s_data
    INTO :s_quantity, :s_dist, :s_data
    FROM dbt.stock
    WHERE s_i_id = :ol_i_id
      AND s_w_id = :w_id
  ELSE IF d_id = 9 THEN
    SELECT s_quantity, s_dist_09, s_data
    INTO :s_quantity, :s_dist, :s_data
    FROM dbt.stock
    WHERE s_i_id = :ol_i_id
      AND s_w_id = :w_id
  ELSE IF d_id = 10 THEN
    SELECT s_quantity, s_dist_10, s_data
    INTO :s_quantity, :s_dist, :s_data
    FROM dbt.stock
    WHERE s_i_id = :ol_i_id
      AND s_w_id = :w_id;
  IF s_quantity > ol_quantity + 10 THEN
    BEGIN
      SET s_quantity = s_quantity - ol_quantity;
      UPDATE dbt.stock
      SET s_quantity = :s_quantity
      WHERE s_i_id = :ol_i_id
        AND s_w_id = :w_id;
    END
  ELSE
    BEGIN
      SET s_quantity = s_quantity - ol_quantity + 91;
      UPDATE dbt.stock
      SET s_quantity = :s_quantity
      WHERE s_i_id = :ol_i_id
        AND s_w_id = :w_id;
    END;
  INSERT INTO dbt.order_line (ol_o_id, ol_d_id, ol_w_id, ol_number, ol_i_id,
                              ol_supply_w_id, ol_delivery_d, ol_quantity,
                              ol_amount, ol_dist_info)
  VALUES (:ol_o_id, :d_id, :w_id, :ol_number, :ol_i_id,
          :ol_supply_w_id, NULL, :ol_quantity,
          :ol_amount, :s_dist);
END;;
