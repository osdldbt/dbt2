--------------------------
Differences from the TPC-C
--------------------------

Introduction
============

The purpose of this document is to list the places where the OSDL Database Test
2 (DBT-2) test kit deviates from the TPC-C Standard Specification Revision 5.0
(http://www.tpc.org/tpcc/).  DBT-2 may use terminology similar to benchmarks
from the TPC or other, but such similarity does not in any way imply any
comparable relationship with any other benchmark.

Database Design
===============

* All required fields are not returned to the terminal.
* The kit currently does not use foreign keys to enforce any integrity
  constraints.  (Clause 1.3)
* Storage space for a 60-day period is not determined.  (Clause 4.2.3)

Database Scaling
================

* By default the data generator for the database scales the database properly
  based on the number of warehouses built.  The scale for all the tables,
  except `DISTRICT` and `ORDER_LINE`, can be manually overridden.  (Clause
  1.4.3)

Transactions
============

Payment
-------

* In the case where the selected customer has bad credit, the original c_data
  is not appended to the new `c_data` updated for the customer.  (Clause
  2.5.2.2) [SAP DB]

Delivery
--------

* Delivery transactions are not queued and deferred, but are executed
  immediately.  (Clause 2.7.2.1)

* Statistics are not collected for the case when deliveries are not made for a
  district.  (Clause 2.7.2.3)

Acid Properties
===============

* Test have not been designed to test the ACID properties of the database.
  (Clause 3)

Performance Metrics
===================

* The reported throughput does not have a minimum required throughput.
  (Clause 4.1.3)
* DBT-2 does not require reporting (Clause 5.6) nor does it require a full
  disclosure report be published (Clause 8).  An independent audit is also not
  required.  (Clause 9)
* The pricing of the system under test is not calculated.  (Clause 7)

Driver
======

* The driver is currently designed to work with a client program to reduce the
  number of connections to the database.  The driver can be used in a way such
  that it implements most of the client code directly to reduce the number of
  connections to the database from the driver program directly, instead of
  opening a database connection per terminal emulated.  (Clause 6.1)
* The mix of the transactions conforms to the TPC-C specification by default,
  but it can be adjusted outside the restrictions placed in the specification.
  (Clause 5.2.3)
* The Keying Time and Thinking Time is constant but can be user defined.
  (Clause 5.2.5.7)
* A maximum 90th percentile response time is not required.  (Clause 5.2.5.3)
