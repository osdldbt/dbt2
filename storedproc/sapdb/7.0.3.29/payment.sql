// This file is released under the terms of the Artistic License.  Please see
// the file LICENSE, included in this package, for details.
//
// Copyright (C) 2002 Mark Wong & Open Source Development Lab, Inc.
//
// Based on TPC-C Standard Specification Revision 5.0 Clause 2.5.2.
// July 10, 2002
//     Not selecting n/2 for customer search by c_last.
// July 12, 2002
//     Not using c_d_id and c_w_id when searching for customers by last name
//     since there are cases with 1 warehouse where no customers are found.
// August 13, 2002
//     Not appending c_data to c_data when credit is bad.
CREATE DBPROC payment(IN w_id FIXED(9), IN d_id FIXED(2),
INOUT c_id FIXED(5), IN c_w_id FIXED(9), IN c_d_id FIXED(2),
INOUT c_last VARCHAR(16), IN h_amount FIXED(12, 6),
OUT w_name VARCHAR(10), OUT w_street_1 VARCHAR(20), OUT w_street_2 VARCHAR(20),
OUT w_city VARCHAR(20), OUT w_state CHAR(2), OUT w_zip char(9),
OUT d_name VARCHAR(10), OUT d_street_1 VARCHAR(20), OUT d_street_2 VARCHAR(20),
OUT d_city VARCHAR(20), OUT d_state CHAR(2), OUT d_zip char(9),
OUT c_first VARCHAR(16), OUT c_middle char(2),
OUT c_street_1 VARCHAR(20), OUT c_street_2 VARCHAR(20), OUT c_city VARCHAR(20),
OUT c_state CHAR(2), OUT c_zip CHAR(9), OUT c_phone CHAR(16),
OUT c_since VARCHAR(28), OUT c_credit CHAR(2), OUT c_credit_lim FIXED(24, 12),
OUT c_discount FIXED(8, 4), OUT c_balance FIXED(24, 12),
OUT c_data VARCHAR(500))
AS
  VAR c_ytd_payment FIXED(24, 12); h_data VARCHAR(24);
SUBTRANS BEGIN;
  SELECT w_name, w_street_1, w_street_2, w_city, w_state, w_zip
  INTO :w_name, :w_street_1, :w_street_2, :w_city, :w_state, :w_zip
  FROM dbt.warehouse
  WHERE w_id = :w_id;
  UPDATE dbt.warehouse
  SET w_ytd = w_ytd + :h_amount
  WHERE w_id = :w_id;
  SELECT d_name, d_street_1, d_street_2, d_city, d_state, d_zip
  INTO :d_name, :d_street_1, :d_street_2, :d_city, :d_state, :d_zip
  FROM dbt.district
  WHERE d_id = :d_id
    AND d_w_id = :w_id;
  UPDATE dbt.district
  SET d_ytd = d_ytd + :h_amount
  WHERE d_id = :d_id
    AND d_w_id = :w_id;
  IF c_id = 0 THEN
    BEGIN
      SELECT c_id
      FROM dbt.customer
      WHERE c_w_id = :c_w_id
        AND c_d_id = :c_d_id
        AND c_last = :c_last
      ORDER BY c_first ASC;
      FETCH INTO :c_id;
    END;
  SELECT c_first, c_middle, c_last, c_street_1, c_street_2, c_city,
         c_state, c_zip, c_phone, CHAR(c_since, ISO), c_credit,
         c_credit_lim, c_discount, c_balance, c_data, c_ytd_payment
  INTO :c_first, :c_middle, :c_last, :c_street_1, :c_street_2, :c_city,
       :c_state, :c_zip, :c_phone, :c_since, :c_credit,
       :c_credit_lim, :c_discount, :c_balance, :c_data, :c_ytd_payment
  FROM dbt.customer
  WHERE c_w_id = :c_w_id
    AND c_d_id = :c_d_id
    AND c_id = :c_id;
  SET c_balance = c_balance - h_amount;
  SET c_ytd_payment = c_ytd_payment + 1;
  IF c_credit = 'BC' THEN
    BEGIN
      SET c_data = CHR(c_id) & CHR(c_d_id) & CHR(c_w_id) & CHR(d_id) &
                   CHR(w_id) & CHR(h_amount);
      UPDATE dbt.customer(c_balance, c_ytd_payment, c_data)
      VALUES (:c_balance, :c_ytd_payment, :c_data)
      WHERE c_id = :c_id
        AND c_w_id = :c_w_id
        AND c_d_id = :c_d_id;
    END
  ELSE
    UPDATE dbt.customer(c_balance, c_ytd_payment)
    VALUES (:c_balance, :c_ytd_payment)
    WHERE c_id = :c_id
      AND c_w_id = :c_w_id
      AND c_d_id = :c_d_id;
  set h_data = w_name & '    ' & d_name;
  INSERT INTO dbt.history(h_c_id, h_c_d_id, h_c_w_id, h_d_id, h_w_id, 
                          h_date, h_amount, h_data)
  VALUES (:c_id, :c_d_id, :c_w_id, :d_id, :w_id, TIMESTAMP, :h_amount, :h_data);
SUBTRANS END;
