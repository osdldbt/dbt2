/* This file is released under the terms of the Artistic License.  Please see
/* the file LICENSE, included in this package, for details.
/*
/* Copyright (C) 2002 Mark Wong & Open Source Development Lab, Inc.
/*
/* Based on TPC-C Standard Specification Revision 5.0 Clause 2.7.4.
CREATE DBPROC delivery(IN w_id FIXED(9), IN o_carrier_id FIXED(2))
AS
SUBTRANS BEGIN;
  CALL delivery_2(:w_id, :o_carrier_id, 1);
  CALL delivery_2(:w_id, :o_carrier_id, 2);
  CALL delivery_2(:w_id, :o_carrier_id, 3);
  CALL delivery_2(:w_id, :o_carrier_id, 4);
  CALL delivery_2(:w_id, :o_carrier_id, 5);
  CALL delivery_2(:w_id, :o_carrier_id, 6);
  CALL delivery_2(:w_id, :o_carrier_id, 7);
  CALL delivery_2(:w_id, :o_carrier_id, 8);
  CALL delivery_2(:w_id, :o_carrier_id, 9);
  CALL delivery_2(:w_id, :o_carrier_id, 10);
SUBTRANS END;;
