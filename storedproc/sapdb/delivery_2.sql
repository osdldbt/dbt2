/* This file is released under the terms of the Artistic License.  Please see
/* the file LICENSE, included in this package, for details.
/*
/* Copyright (C) 2002 Mark Wong & Open Source Development Lab, Inc.
/*
/* Based on TPC-C Standard Specification Revision 5.0 Clause 2.7.4.
/* July 22, 2002
/*     Not capturing the case where the case arises when there are no
/*     deliveries to be made for a district.
CREATE DBPROC delivery_2(IN w_id FIXED(9), IN o_carrier_id FIXED(2),
IN d_id FIXED(2))
AS
  VAR o_id FIXED(8); c_id FIXED(5); ol_amount FIXED(8, 4);
BEGIN
  SET o_id = -1;
  SELECT no_o_id
  FROM dbt.new_order
  WHERE no_w_id = :w_id
    AND no_d_id = :d_id
  ORDER BY no_o_id ASC;
  IF $rc = 0 THEN
    BEGIN
      FETCH INTO :o_id;
      IF o_id <> -1 THEN
        BEGIN
          DELETE FROM dbt.new_order
          WHERE no_o_id = :o_id
            AND no_w_id = :w_id
            AND no_d_id = :d_id;
          SELECT o_c_id
          INTO :c_id
          FROM dbt.orders
          WHERE o_id = :o_id
            AND o_w_id = :w_id
            AND o_d_id = :d_id;
          UPDATE dbt.orders
          SET o_carrier_id = :o_carrier_id
          WHERE o_id = :o_id
            AND o_w_id = :w_id
            AND o_d_id = :d_id;
          UPDATE dbt.order_line
          SET ol_delivery_d = TIMESTAMP
          WHERE ol_o_id = :o_id
            AND ol_w_id = :w_id
            AND ol_d_id = :d_id;
          SELECT SUM(ol_amount * ol_quantity)
          INTO :ol_amount
          FROM dbt.order_line
          WHERE ol_w_id = :w_id
            AND ol_d_id = :d_id
            AND ol_o_id = :o_id;
          UPDATE dbt.customer
          SET c_delivery_cnt = c_delivery_cnt + 1,
              c_balance = c_balance + :ol_amount
          WHERE c_id = :c_id 
            AND c_w_id = :w_id
            AND c_d_id = :d_id;
        END;
    END;
END;;
