/ This file is released under the terms of the Artistic License.  Please see
/ the file LICENSE, included in this package, for details.
/
/ Copyright (C) 2002 Mark Wong & Open Source Development Lab, Inc.
/
/ Based on TPC-C Standard Specification Revision 5.0 Clause 2.8.2.
CREATE DBPROC stock_level_2(IN w_id FIXED(9), IN d_id FIXED(2),
IN d_next_o_id FIXED(8), IN threshold FIXED(4), INOUT low_stock FIXED(9))
AS
  VAR s_quantity FIXED(4); ol_i_id FIXED(6);
BEGIN
  SELECT distinct(ol_i_id)
  FROM dbt.order_line
  WHERE ol_o_id = :d_next_o_id
    AND ol_w_id = :w_id
    AND ol_d_id = :d_id;
  WHILE $rc = 0 DO
    BEGIN
      FETCH INTO :ol_i_id;
      SELECT s_quantity
      FROM dbt.stock
      WHERE s_i_id = :ol_i_id
        AND s_w_id = :w_id
        AND s_quantity < :threshold;
      IF $rc = 0 THEN
        BEGIN
          FETCH INTO :s_quantity;
          SET low_stock = low_stock + s_quantity;
        END;
    END;
END;;
