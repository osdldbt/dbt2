/* This file is released under the terms of the Artistic License.  Please see
/* the file LICENSE, included in this package, for details.
/*
/* Copyright (C) 2002 Mark Wong & Open Source Development Lab, Inc.
/*
/* Based on TPC-C Standard Specification Revision 5.0 Clause 2.4.2.
CREATE DBPROC new_order(IN w_id FIXED(9), IN d_id FIXED(2),
IN c_id FIXED(5), IN o_all_local FIXED(1), IN o_ol_cnt FIXED(2),
IN ol_i_id1 FIXED(6), IN ol_supply_w_id1 FIXED(9), IN ol_quantity1 FIXED(2),
OUT i_name1 VARCHAR(24), OUT i_price1 FIXED(10, 5),
OUT s_quantity1 FIXED(4), OUT ol_amount1 FIXED(12, 6),
IN ol_i_id2 FIXED(6), IN ol_supply_w_id2 FIXED(9), IN ol_quantity2 FIXED(2),
OUT i_name2 VARCHAR(24), OUT i_price2 FIXED(10, 5),
OUT s_quantity2 FIXED(4), OUT ol_amount2 FIXED(12, 6),
IN ol_i_id3 FIXED(6), IN ol_supply_w_id3 FIXED(9), IN ol_quantity3 FIXED(2),
OUT i_name3 VARCHAR(24), OUT i_price3 FIXED(10, 5),
OUT s_quantity3 FIXED(4), OUT ol_amount3 FIXED(12, 6),
IN ol_i_id4 FIXED(6), IN ol_supply_w_id4 FIXED(9), IN ol_quantity4 FIXED(2),
OUT i_name4 VARCHAR(24), OUT i_price4 FIXED(10, 5),
OUT s_quantity4 FIXED(4), OUT ol_amount4 FIXED(12, 6),
IN ol_i_id5 FIXED(6), IN ol_supply_w_id5 FIXED(9), IN ol_quantity5 FIXED(2),
OUT i_name5 VARCHAR(24), OUT i_price5 FIXED(10, 5),
OUT s_quantity5 FIXED(4), OUT ol_amount5 FIXED(12, 6),
IN ol_i_id6 FIXED(6), IN ol_supply_w_id6 FIXED(9), IN ol_quantity6 FIXED(2),
OUT i_name6 VARCHAR(24), OUT i_price6 FIXED(10, 5),
OUT s_quantity6 FIXED(4), OUT ol_amount6 FIXED(12, 6),
IN ol_i_id7 FIXED(6), IN ol_supply_w_id7 FIXED(9), IN ol_quantity7 FIXED(2),
OUT i_name7 VARCHAR(24), OUT i_price7 FIXED(10, 5),
OUT s_quantity7 FIXED(4), OUT ol_amount7 FIXED(12, 6),
IN ol_i_id8 FIXED(6), IN ol_supply_w_id8 FIXED(9), IN ol_quantity8 FIXED(2),
OUT i_name8 VARCHAR(24), OUT i_price8 FIXED(10, 5),
OUT s_quantity8 FIXED(4), OUT ol_amount8 FIXED(12, 6),
IN ol_i_id9 FIXED(6), IN ol_supply_w_id9 FIXED(9), IN ol_quantity9 FIXED(2),
OUT i_name9 VARCHAR(24), OUT i_price9 FIXED(10, 5),
OUT s_quantity9 FIXED(4), OUT ol_amount9 FIXED(12, 6),
IN ol_i_id10 FIXED(6), IN ol_supply_w_id10 FIXED(9), IN ol_quantity10 FIXED(2),
OUT i_name10 VARCHAR(24), OUT i_price10 FIXED(10, 5),
OUT s_quantity10 FIXED(4), OUT ol_amount10 FIXED(12, 6),
IN ol_i_id11 FIXED(6), IN ol_supply_w_id11 FIXED(9), IN ol_quantity11 FIXED(2),
OUT i_name11 VARCHAR(24), OUT i_price11 FIXED(10, 5),
OUT s_quantity11 FIXED(4), OUT ol_amount11 FIXED(12, 6),
IN ol_i_id12 FIXED(6), IN ol_supply_w_id12 FIXED(9), IN ol_quantity12 FIXED(2),
OUT i_name12 VARCHAR(24), OUT i_price12 FIXED(10, 5),
OUT s_quantity12 FIXED(4), OUT ol_amount12 FIXED(12, 6),
IN ol_i_id13 FIXED(6), IN ol_supply_w_id13 FIXED(9), IN ol_quantity13 FIXED(2),
OUT i_name13 VARCHAR(24), OUT i_price13 FIXED(10, 5),
OUT s_quantity13 FIXED(4), OUT ol_amount13 FIXED(12, 6),
IN ol_i_id14 FIXED(6), IN ol_supply_w_id14 FIXED(9), IN ol_quantity14 FIXED(2),
OUT i_name14 VARCHAR(24), OUT i_price14 FIXED(10, 5),
OUT s_quantity14 FIXED( 4), OUT ol_amount14 FIXED(12, 6),
IN ol_i_id15 FIXED(6), IN ol_supply_w_id15 FIXED(9), IN ol_quantity15 FIXED(2),
OUT i_name15 VARCHAR(24), OUT i_price15 FIXED(10, 5),
OUT s_quantity15 FIXED( 4), OUT ol_amount15 FIXED(12, 6),
OUT o_id FIXED(8), OUT total_amount FIXED(12, 6),
OUT w_tax FIXED(8, 4), OUT d_tax FIXED(8, 4),
OUT c_last VARCHAR(16), OUT c_credit CHAR(2), OUT c_discount FIXED(8, 4),
OUT rollback FIXED(1))
AS
  VAR d_next_o_id FIXED(8); i_data VARCHAR(50);
SUBTRANS BEGIN;
  SET i_name1 = '';
  SET i_name2 = '';
  SET i_name3 = '';
  SET i_name4 = '';
  SET i_name5 = '';
  SET i_name6 = '';
  SET i_name7 = '';
  SET i_name8 = '';
  SET i_name9 = '';
  SET i_name10 = '';
  SET i_name11 = '';
  SET i_name12 = '';
  SET i_name13 = '';
  SET i_name14 = '';
  SET i_name15 = '';
  SET i_price1 = 0;
  SET i_price2 = 0;
  SET i_price3 = 0;
  SET i_price4 = 0;
  SET i_price5 = 0;
  SET i_price6 = 0;
  SET i_price7 = 0;
  SET i_price8 = 0;
  SET i_price9 = 0;
  SET i_price10 = 0;
  SET i_price11 = 0;
  SET i_price12 = 0;
  SET i_price13 = 0;
  SET i_price14 = 0;
  SET i_price15 = 0;
  SET s_quantity1 = 0;
  SET s_quantity2 = 0;
  SET s_quantity3 = 0;
  SET s_quantity4 = 0;
  SET s_quantity5 = 0;
  SET s_quantity6 = 0;
  SET s_quantity7 = 0;
  SET s_quantity8 = 0;
  SET s_quantity9 = 0;
  SET s_quantity10 = 0;
  SET s_quantity11 = 0;
  SET s_quantity12 = 0;
  SET s_quantity13 = 0;
  SET s_quantity14 = 0;
  SET s_quantity15 = 0;
  SET ol_amount1 = 0;
  SET ol_amount2 = 0;
  SET ol_amount3 = 0;
  SET ol_amount4 = 0;
  SET ol_amount5 = 0;
  SET ol_amount6 = 0;
  SET ol_amount7 = 0;
  SET ol_amount8 = 0;
  SET ol_amount9 = 0;
  SET ol_amount10 = 0;
  SET ol_amount11 = 0;
  SET ol_amount12 = 0;
  SET ol_amount13 = 0;
  SET ol_amount14 = 0;
  SET ol_amount15 = 0;
  SET o_id = 0;
  SET total_amount = 0;
  SET rollback = 0;
  SELECT w_tax
  INTO :w_tax
  FROM dbt.warehouse
  WHERE w_id = :w_id;
  SELECT d_tax, d_next_o_id
  INTO :d_tax, :d_next_o_id
  FROM dbt.district
  WHERE d_w_id = :w_id
    AND d_id = :d_id;
  SET o_id = d_next_o_id;
  SET d_next_o_id = o_id + 1;
  UPDATE dbt.district
  SET d_next_o_id = :d_next_o_id
  WHERE d_id = :d_id
    AND d_w_id = :w_id;
  SELECT c_discount, c_last, c_credit
  INTO :c_discount, :c_last, :c_credit
  FROM dbt.customer
  WHERE c_w_id = :w_id
    AND c_d_id = :d_id
    AND c_id = :c_id;
  INSERT INTO dbt.new_order(no_o_id, no_d_id, no_w_id)
  VALUES (:o_id, :d_id, :w_id);
  INSERT INTO dbt.orders(o_id, o_d_id, o_w_id, o_c_id, o_entry_d, o_carrier_id,
                         o_ol_cnt, o_all_local)
  VALUES (:o_id, :d_id, :w_id, :c_id, TIMESTAMP, NULL, :o_ol_cnt, :o_all_local);
  IF o_ol_cnt > 0 THEN
    BEGIN
      SELECT i_price, i_name, i_data
      FROM dbt.item
      WHERE i_id = :ol_i_id1;
      IF $rc = 0 THEN
        BEGIN
          FETCH INTO :i_price1, :i_name1, :i_data;
          SET ol_amount1 = i_price1 * ol_quantity1;
          CALL new_order_2(:ol_supply_w_id1, :d_id, :ol_i_id1, :ol_quantity1,
                           :i_price1, :i_name1, :i_data, :o_id,
                           :ol_amount1, :ol_supply_w_id1, 1, :s_quantity1);
          SET total_amount = ol_amount1;
        END
      ELSE
        BEGIN
          SET rollback = 1;
          SUBTRANS ROLLBACK;
        END;
    END;
  IF o_ol_cnt > 1 THEN
    BEGIN
      SELECT i_price, i_name, i_data
      FROM dbt.item
      WHERE i_id = :ol_i_id2;
      IF $rc = 0 THEN
        BEGIN
          FETCH INTO :i_price2, :i_name2, :i_data;
          SET ol_amount2 = i_price2 * ol_quantity2;
          CALL new_order_2(:ol_supply_w_id2, :d_id, :ol_i_id2, :ol_quantity2,
                           :i_price2, :i_name2, :i_data, :o_id, :ol_amount2,
                           :ol_supply_w_id2, 2, :s_quantity2);
          SET total_amount = total_amount + ol_amount2;
        END
      ELSE
        BEGIN
          SET rollback = 1;
          SUBTRANS ROLLBACK;
        END;
    END;
  IF o_ol_cnt > 2 THEN
    BEGIN
      SELECT i_price, i_name, i_data
      FROM dbt.item
      WHERE i_id = :ol_i_id3;
      IF $rc = 0 THEN
        BEGIN
          FETCH INTO :i_price3, :i_name3, :i_data;
          SET ol_amount3 = i_price3 * ol_quantity3;
          CALL new_order_2(:ol_supply_w_id3, :d_id, :ol_i_id3, :ol_quantity3,
                           :i_price3, :i_name3, :i_data, :o_id, :ol_amount3,
                           :ol_supply_w_id3, 3, :s_quantity3);
          SET total_amount = total_amount + ol_amount3;
        END
      ELSE
        BEGIN
          SET rollback = 1;
          SUBTRANS ROLLBACK;
        END;
    END;
  IF o_ol_cnt > 3 THEN
    BEGIN
      SELECT i_price, i_name, i_data
      FROM dbt.item
      WHERE i_id = :ol_i_id4;
      IF $rc = 0 THEN
        BEGIN
          FETCH INTO :i_price4, :i_name4, :i_data;
          SET ol_amount4 = i_price4 * ol_quantity4;
          CALL new_order_2(:ol_supply_w_id4, :d_id, :ol_i_id4, :ol_quantity4,
                           :i_price4, :i_name4, :i_data, :o_id, :ol_amount4,
                           :ol_supply_w_id4, 4, :s_quantity4);
          SET total_amount = total_amount + ol_amount4;
        END
      ELSE
        BEGIN
          SET rollback = 1;
          SUBTRANS ROLLBACK;
        END;
    END;
  IF o_ol_cnt > 4 THEN
    BEGIN
      SELECT i_price, i_name, i_data
      FROM dbt.item
      WHERE i_id = :ol_i_id5;
      IF $rc = 0 THEN
        BEGIN
          FETCH INTO :i_price5, :i_name5, :i_data;
          SET ol_amount5 = i_price5 * ol_quantity5;
          CALL new_order_2(:ol_supply_w_id5, :d_id, :ol_i_id5, :ol_quantity5,
                           :i_price5, :i_name5, :i_data, :o_id, :ol_amount5,
                           :ol_supply_w_id5, 5, :s_quantity5);
          SET total_amount = total_amount + ol_amount5;
        END
      ELSE
        BEGIN
          SET rollback = 1;
          SUBTRANS ROLLBACK;
        END;
    END;
  IF o_ol_cnt > 5 THEN
    BEGIN
      SELECT i_price, i_name, i_data
      FROM dbt.item
      WHERE i_id = :ol_i_id6;
      IF $rc = 0 THEN
        BEGIN
          FETCH INTO :i_price6, :i_name6, :i_data;
          SET ol_amount6 = i_price6 * ol_quantity6;
          CALL new_order_2(:ol_supply_w_id6, :d_id, :ol_i_id6, :ol_quantity6,
                           :i_price6, :i_name6, :i_data, :o_id, :ol_amount6,
                           :ol_supply_w_id6, 6, :s_quantity6);
          SET total_amount = total_amount + ol_amount6;
        END
      ELSE
        BEGIN
          SET rollback = 1;
          SUBTRANS ROLLBACK;
        END;
    END;
  IF o_ol_cnt > 6 THEN
    BEGIN
      SELECT i_price, i_name, i_data
      FROM dbt.item
      WHERE i_id = :ol_i_id7;
      IF $rc = 0 THEN
        BEGIN
          FETCH INTO :i_price7, :i_name7, :i_data;
          SET ol_amount7 = i_price7 * ol_quantity7;
          CALL new_order_2(:ol_supply_w_id7, :d_id, :ol_i_id7, :ol_quantity7,
                           :i_price7, :i_name7, :i_data, :o_id, :ol_amount7,
                           :ol_supply_w_id7, 7, :s_quantity7);
          SET total_amount = total_amount + ol_amount7;
        END
      ELSE
        BEGIN
          SET rollback = 1;
          SUBTRANS ROLLBACK;
        END;
    END;
  IF o_ol_cnt > 7 THEN
    BEGIN
      SELECT i_price, i_name, i_data
      FROM dbt.item
      WHERE i_id = :ol_i_id8;
      IF $rc = 0 THEN
        BEGIN
          FETCH INTO :i_price8, :i_name8, :i_data;
          SET ol_amount8 = i_price8 * ol_quantity8;
          CALL new_order_2(:ol_supply_w_id8, :d_id, :ol_i_id8, :ol_quantity8,
                           :i_price8, :i_name8, :i_data, :o_id, :ol_amount8,
                           :ol_supply_w_id8, 8, :s_quantity8);
          SET total_amount = total_amount + ol_amount8;
        END
      ELSE
        BEGIN
          SET rollback = 1;
          SUBTRANS ROLLBACK;
        END;
    END;
  IF o_ol_cnt > 8 THEN
    BEGIN
      SELECT i_price, i_name, i_data
      FROM dbt.item
      WHERE i_id = :ol_i_id9;
      IF $rc = 0 THEN
        BEGIN
          FETCH INTO :i_price9, :i_name9, :i_data;
          SET ol_amount9 = i_price9 * ol_quantity9;
          CALL new_order_2(:ol_supply_w_id9, :d_id, :ol_i_id9, :ol_quantity9,
                           :i_price9, :i_name9, :i_data, :o_id, :ol_amount9,
                           :ol_supply_w_id9, 9, :s_quantity9);
          SET total_amount = total_amount + ol_amount9;
        END
      ELSE
        BEGIN
          SET rollback = 1;
          SUBTRANS ROLLBACK;
        END;
    END;
  IF o_ol_cnt > 9 THEN
    BEGIN
      SELECT i_price, i_name, i_data
      FROM dbt.item
      WHERE i_id = :ol_i_id10;
      IF $rc = 0 THEN
        BEGIN
          FETCH INTO :i_price10, :i_name10, :i_data;
          SET ol_amount10 = i_price10 * ol_quantity10;
          CALL new_order_2(:ol_supply_w_id10, :d_id, :ol_i_id10, :ol_quantity10,
                           :i_price10, :i_name10, :i_data, :o_id, :ol_amount10,
                           :ol_supply_w_id10, 10, :s_quantity10);
          SET total_amount = total_amount + ol_amount10;
        END
      ELSE
        BEGIN
          SET rollback = 1;
          SUBTRANS ROLLBACK;
        END;
    END;
  IF o_ol_cnt > 10 THEN
    BEGIN
      SELECT i_price, i_name, i_data
      FROM dbt.item
      WHERE i_id = :ol_i_id11;
      IF $rc = 0 THEN
        BEGIN
          FETCH INTO :i_price11, :i_name11, :i_data;
          SET ol_amount11 = i_price11 * ol_quantity11;
          CALL new_order_2(:ol_supply_w_id11, :d_id, :ol_i_id11, :ol_quantity11,
                           :i_price11, :i_name11, :i_data, :o_id, :ol_amount11,
                           :ol_supply_w_id11, 11, :s_quantity11);
          SET total_amount = total_amount + ol_amount11;
        END
      ELSE
        BEGIN
          SET rollback = 1;
          SUBTRANS ROLLBACK;
        END;
    END;
  IF o_ol_cnt > 11 THEN
    BEGIN
      SELECT i_price, i_name, i_data
      FROM dbt.item
      WHERE i_id = :ol_i_id12;
      IF $rc = 0 THEN
        BEGIN
          FETCH INTO :i_price12, :i_name12, :i_data;
          SET ol_amount12 = i_price12 * ol_quantity12;
          CALL new_order_2(:ol_supply_w_id12, :d_id, :ol_i_id12, :ol_quantity12,
                           :i_price12, :i_name12, :i_data, :o_id, :ol_amount12,
                           :ol_supply_w_id12, 12, :s_quantity12);
          SET total_amount = total_amount + ol_amount12;
        END
      ELSE
        BEGIN
          SET rollback = 1;
          SUBTRANS ROLLBACK;
        END;
    END;
  IF o_ol_cnt > 12 THEN
    BEGIN
      SELECT i_price, i_name, i_data
      FROM dbt.item
      WHERE i_id = :ol_i_id13;
      IF $rc = 0 THEN
        BEGIN
          FETCH INTO :i_price13, :i_name13, :i_data;
          SET ol_amount13 = i_price13 * ol_quantity13;
          CALL new_order_2(:ol_supply_w_id13, :d_id, :ol_i_id13, :ol_quantity13,
                           :i_price13, :i_name13, :i_data, :o_id, :ol_amount13,
                           :ol_supply_w_id13, 13, :s_quantity13);
          SET total_amount = total_amount + ol_amount13;
        END
      ELSE
        BEGIN
          SET rollback = 1;
          SUBTRANS ROLLBACK;
        END;
    END;
  IF o_ol_cnt > 13 THEN
    BEGIN
      SELECT i_price, i_name, i_data
      FROM dbt.item
      WHERE i_id = :ol_i_id14;
      IF $rc = 0 THEN
        BEGIN
          FETCH INTO :i_price14, :i_name14, :i_data;
          SET ol_amount14 = i_price14 * ol_quantity14;
          CALL new_order_2(:ol_supply_w_id14, :d_id, :ol_i_id14, :ol_quantity14,
                           :i_price14, :i_name14, :i_data, :o_id, :ol_amount14,
                           :ol_supply_w_id14, 14, :s_quantity14);
          SET total_amount = total_amount + ol_amount14;
        END
      ELSE
        BEGIN
          SET rollback = 1;
          SUBTRANS ROLLBACK;
        END;
    END;
  IF o_ol_cnt > 14 THEN
    BEGIN
      SELECT i_price, i_name, i_data
      FROM dbt.item
      WHERE i_id = :ol_i_id15;
      IF $rc = 0 THEN
        BEGIN
          FETCH INTO :i_price15, :i_name15, :i_data;
          SET ol_amount15 = i_price15 * ol_quantity15;
          CALL new_order_2(:ol_supply_w_id15, :d_id, :ol_i_id15, :ol_quantity15,
                           :i_price15, :i_name15, :i_data, :o_id, :ol_amount15,
                           :ol_supply_w_id15, 15, :s_quantity15);
          SET total_amount = total_amount + ol_amount15;
        END
      ELSE
        BEGIN
          SET rollback = 1;
          SUBTRANS ROLLBACK;
        END;
    END;
SUBTRANS END;;
