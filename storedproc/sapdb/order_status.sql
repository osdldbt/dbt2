/* This file is released under the terms of the Artistic License.  Please see
/* the file LICENSE, included in this package, for details.
/*
/* Copyright (C) 2002 Mark Wong & Open Source Development Lab, Inc.
/*
/* Based on TPC-C Standard Specification Revision 5.0 Clause 2.6.2.
/* July 16, 2002
/*    Having problems with the case of searching for a customer by c_last
/*    similar to the Payment transaction.  The data generator will not execute
/*    that case.
CREATE DBPROC order_status(
INOUT c_id FIXED(5), IN c_w_id FIXED(9), IN c_d_id FIXED(2),
OUT c_first VARCHAR(16), OUT c_middle char(2), INOUT c_last VARCHAR(16),
OUT c_balance FIXED(24, 12), OUT o_id FIXED(8), OUT o_carrier_id FIXED(2),
OUT o_entry_d VARCHAR(28), OUT o_ol_cnt FIXED(2),
OUT ol_supply_w_id1 FIXED(9), OUT ol_i_id1 FIXED(6),
OUT ol_quantity1 FIXED(4), OUT ol_amount1 FIXED(12, 6),
OUT ol_delivery_d1 VARCHAR(28),
OUT ol_supply_w_id2 FIXED(9), OUT ol_i_id2 FIXED(6),
OUT ol_quantity2 FIXED(4), OUT ol_amount2 FIXED(12, 6),
OUT ol_delivery_d2 VARCHAR(28),
OUT ol_supply_w_id3 FIXED(9), OUT ol_i_id3 FIXED(6),
OUT ol_quantity3 FIXED(4), OUT ol_amount3 FIXED(12, 6),
OUT ol_delivery_d3 VARCHAR(28),
OUT ol_supply_w_id4 FIXED(9), OUT ol_i_id4 FIXED(6),
OUT ol_quantity4 FIXED(4), OUT ol_amount4 FIXED(12, 6),
OUT ol_delivery_d4 VARCHAR(28),
OUT ol_supply_w_id5 FIXED(9), OUT ol_i_id5 FIXED(6),
OUT ol_quantity5 FIXED(4), OUT ol_amount5 FIXED(12, 6),
OUT ol_delivery_d5 VARCHAR(28),
OUT ol_supply_w_id6 FIXED(9), OUT ol_i_id6 FIXED(6),
OUT ol_quantity6 FIXED(4), OUT ol_amount6 FIXED(12, 6),
OUT ol_delivery_d6 VARCHAR(28),
OUT ol_supply_w_id7 FIXED(9), OUT ol_i_id7 FIXED(6),
OUT ol_quantity7 FIXED(4), OUT ol_amount7 FIXED(12, 6),
OUT ol_delivery_d7 VARCHAR(28),
OUT ol_supply_w_id8 FIXED(9), OUT ol_i_id8 FIXED(6),
OUT ol_quantity8 FIXED(4), OUT ol_amount8 FIXED(12, 6),
OUT ol_delivery_d8 VARCHAR(28),
OUT ol_supply_w_id9 FIXED(9), OUT ol_i_id9 FIXED(6),
OUT ol_quantity9 FIXED(4), OUT ol_amount9 FIXED(12, 6),
OUT ol_delivery_d9 VARCHAR(28),
OUT ol_supply_w_id10 FIXED(9), OUT ol_i_id10 FIXED(6),
OUT ol_quantity10 FIXED(4), OUT ol_amount10 FIXED(12, 6),
OUT ol_delivery_d10 VARCHAR(28),
OUT ol_supply_w_id11 FIXED(9), OUT ol_i_id11 FIXED(6),
OUT ol_quantity11 FIXED(4), OUT ol_amount11 FIXED(12, 6),
OUT ol_delivery_d11 VARCHAR(28),
OUT ol_supply_w_id12 FIXED(9), OUT ol_i_id12 FIXED(6),
OUT ol_quantity12 FIXED(4), OUT ol_amount12 FIXED(12, 6),
OUT ol_delivery_d12 VARCHAR(28),
OUT ol_supply_w_id13 FIXED(9), OUT ol_i_id13 FIXED(6),
OUT ol_quantity13 FIXED(4), OUT ol_amount13 FIXED(12, 6),
OUT ol_delivery_d13 VARCHAR(28),
OUT ol_supply_w_id14 FIXED(9), OUT ol_i_id14 FIXED(6),
OUT ol_quantity14 FIXED(4), OUT ol_amount14 FIXED(12, 6),
OUT ol_delivery_d14 VARCHAR(28),
OUT ol_supply_w_id15 FIXED(9), OUT ol_i_id15 FIXED(6),
OUT ol_quantity15 FIXED(4), OUT ol_amount15 FIXED(12, 6),
OUT ol_delivery_d15 VARCHAR(28))
AS
SUBTRANS BEGIN;
  SET o_ol_cnt = 0;
  SET o_id = 0;
  SET o_carrier_id = 0;
  SET o_entry_d = '';
  SET ol_supply_w_id1 = 0;
  SET ol_i_id1 = 0;
  SET ol_quantity1 = 0;
  SET ol_amount1 = 0;
  SET ol_delivery_d1 = '';
  SET ol_supply_w_id2 = 0;
  SET ol_i_id2 = 0;
  SET ol_quantity2 = 0;
  SET ol_amount2 = 0;
  SET ol_delivery_d2 = '';
  SET ol_supply_w_id3 = 0;
  SET ol_i_id3 = 0;
  SET ol_quantity3 = 0;
  SET ol_amount3 = 0;
  SET ol_delivery_d3 = '';
  SET ol_supply_w_id4 = 0;
  SET ol_i_id4 = 0;
  SET ol_quantity4 = 0;
  SET ol_amount4 = 0;
  SET ol_delivery_d4 = '';
  SET ol_supply_w_id5 = 0;
  SET ol_i_id5 = 0;
  SET ol_quantity5 = 0;
  SET ol_amount5 = 0;
  SET ol_delivery_d5 = '';
  SET ol_supply_w_id6 = 0;
  SET ol_i_id6 = 0;
  SET ol_quantity6 = 0;
  SET ol_amount6 = 0;
  SET ol_delivery_d6 = '';
  SET ol_supply_w_id7 = 0;
  SET ol_i_id7 = 0;
  SET ol_quantity7 = 0;
  SET ol_amount7 = 0;
  SET ol_delivery_d7 = '';
  SET ol_supply_w_id8 = 0;
  SET ol_i_id8 = 0;
  SET ol_quantity8 = 0;
  SET ol_amount8 = 0;
  SET ol_delivery_d8 = '';
  SET ol_supply_w_id9 = 0;
  SET ol_i_id9 = 0;
  SET ol_quantity9 = 0;
  SET ol_amount9 = 0;
  SET ol_delivery_d9 = '';
  SET ol_supply_w_id10 = 0;
  SET ol_i_id10 = 0;
  SET ol_quantity10 = 0;
  SET ol_amount10 = 0;
  SET ol_delivery_d10 = '';
  SET ol_supply_w_id11 = 0;
  SET ol_i_id11 = 0;
  SET ol_quantity11 = 0;
  SET ol_amount11 = 0;
  SET ol_delivery_d11 = '';
  SET ol_supply_w_id12 = 0;
  SET ol_i_id12 = 0;
  SET ol_quantity12 = 0;
  SET ol_amount12 = 0;
  SET ol_delivery_d12 = '';
  SET ol_supply_w_id13 = 0;
  SET ol_i_id13 = 0;
  SET ol_quantity13 = 0;
  SET ol_amount13 = 0;
  SET ol_delivery_d13 = '';
  SET ol_supply_w_id14 = 0;
  SET ol_i_id14 = 0;
  SET ol_quantity14 = 0;
  SET ol_amount14 = 0;
  SET ol_delivery_d14 = '';
  SET ol_supply_w_id15 = 0;
  SET ol_i_id15 = 0;
  SET ol_quantity15 = 0;
  SET ol_amount15 = 0;
  SET ol_delivery_d15 = '';
  IF c_id = 0 THEN
    BEGIN
      SELECT c_id
      FROM dbt.customer
      WHERE c_w_id = :c_w_id
        AND c_d_id = :c_d_id
        AND c_last = :c_last;
      FETCH INTO :c_id;
    END;
  SELECT c_first, c_middle, c_last, c_balance
  INTO :c_first, :c_middle, :c_last, :c_balance
  FROM dbt.customer
  WHERE c_w_id = :c_w_id
    AND c_d_id = :c_d_id
    AND c_id = :c_id;
  SELECT o_id, o_carrier_id, CHAR(o_entry_d, ISO), o_ol_cnt
  FROM dbt.orders
  WHERE o_w_id = :c_w_id
    AND o_d_id = :c_d_id
    AND o_c_id = :c_id
  ORDER BY o_id DESC;
  IF $rc = 0 THEN
    BEGIN
      FETCH INTO :o_id, :o_carrier_id, :o_entry_d, :o_ol_cnt;
      SELECT ol_i_id, ol_supply_w_id, ol_quantity, ol_amount,
             CHAR(ol_delivery_d, ISO)
      FROM dbt.order_line
      WHERE ol_w_id = :c_w_id
        AND ol_d_id = :c_d_id
        AND ol_o_id = :o_id;
      IF $rc = 0 THEN
        BEGIN
          FETCH INTO :ol_i_id1, :ol_supply_w_id1, :ol_quantity1, :ol_amount1,
                     :ol_delivery_d1;
        END;
      IF $rc = 0 THEN
        BEGIN
          FETCH INTO :ol_i_id2, :ol_supply_w_id2, :ol_quantity2, :ol_amount2,
                     :ol_delivery_d2;
        END;
      IF $rc = 0 THEN
        BEGIN
          FETCH INTO :ol_i_id3, :ol_supply_w_id3, :ol_quantity3, :ol_amount3,
                     :ol_delivery_d3;
        END;
      IF $rc = 0 THEN
        BEGIN
          FETCH INTO :ol_i_id4, :ol_supply_w_id4, :ol_quantity4, :ol_amount4,
                     :ol_delivery_d4;
        END;
      IF $rc = 0 THEN
        BEGIN
          FETCH INTO :ol_i_id5, :ol_supply_w_id5, :ol_quantity5, :ol_amount5,
                     :ol_delivery_d5;
        END;
      IF $rc = 0 THEN
        BEGIN
          FETCH INTO :ol_i_id6, :ol_supply_w_id6, :ol_quantity6, :ol_amount6,
                     :ol_delivery_d6;
        END;
      IF $rc = 0 THEN
        BEGIN
          FETCH INTO :ol_i_id7, :ol_supply_w_id7, :ol_quantity7, :ol_amount7,
                     :ol_delivery_d7;
        END;
      IF $rc = 0 THEN
        BEGIN
          FETCH INTO :ol_i_id8, :ol_supply_w_id8, :ol_quantity8, :ol_amount8,
                     :ol_delivery_d8;
        END;
      IF $rc = 0 THEN
        BEGIN
          FETCH INTO :ol_i_id9, :ol_supply_w_id9, :ol_quantity9, :ol_amount9,
                     :ol_delivery_d9;
        END;
      IF $rc = 0 THEN
        BEGIN
          FETCH INTO :ol_i_id10, :ol_supply_w_id10, :ol_quantity10,
                     :ol_amount10, :ol_delivery_d10;
        END;
      IF $rc = 0 THEN
        BEGIN
          FETCH INTO :ol_i_id11, :ol_supply_w_id11, :ol_quantity11,
                     :ol_amount11, :ol_delivery_d11;
        END;
      IF $rc = 0 THEN
        BEGIN
          FETCH INTO :ol_i_id12, :ol_supply_w_id12, :ol_quantity12,
                     :ol_amount12, :ol_delivery_d12;
        END;
      IF $rc = 0 THEN
        BEGIN
          FETCH INTO :ol_i_id13, :ol_supply_w_id13, :ol_quantity13,
                     :ol_amount13, :ol_delivery_d13;
        END;
      IF $rc = 0 THEN
        BEGIN
          FETCH INTO :ol_i_id14, :ol_supply_w_id14, :ol_quantity14,
                     :ol_amount14, :ol_delivery_d14;
        END;
      IF $rc = 0 THEN
        BEGIN
          FETCH INTO :ol_i_id15, :ol_supply_w_id15, :ol_quantity15,
                     :ol_amount15, :ol_delivery_d15;
        END;
    END;
    IF o_carrier_id not between 0 and 9 THEN
      BEGIN
        SET o_carrier_id = 0;
        SET ol_delivery_d1 = '';
        SET ol_delivery_d2 = '';
        SET ol_delivery_d3 = '';
        SET ol_delivery_d4 = '';
        SET ol_delivery_d5 = '';
        SET ol_delivery_d6 = '';
        SET ol_delivery_d7 = '';
        SET ol_delivery_d8 = '';
        SET ol_delivery_d9 = '';
        SET ol_delivery_d10 = '';
        SET ol_delivery_d11 = '';
        SET ol_delivery_d12 = '';
        SET ol_delivery_d13 = '';
        SET ol_delivery_d14 = '';
        SET ol_delivery_d15 = '';
      END;
SUBTRANS END;;
