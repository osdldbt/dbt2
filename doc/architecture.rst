------------
Architecture
------------

This section briefly recaps the TPC-C, and then describes how DBT-2 implements
the TPC-C specification.

TPC-C
=====

The TPC-C represents the database activities of any industry that manages,
sells, and distributes a product or service, such as car rental agencies,
food distribution companies, and parts suppliers.  The simulated business
model mimics a wholesale parts supplier that operates out of a number of
warehouses and their associated sales districts. Each warehouse has ten sales
districts and each district servers three thousand customers.  A user from a
sales district can select at any time one of five operations from the order
entry system:  entering new orders, delivering orders, tracking payment for
orders, checking the status of the orders, or monitoring the level of stock
at a specified warehouse.

The most frequent transaction consists of entering a new order that is
comprised of an average of ten line items.  Each warehouse maintains stock
for 100,000 items and attempts to fill orders from that stock.  To simulate
realistic events, such as the case where a particular warehouse may not have
the item in stock, the TPC-C benchmark requires that close to 10% of all
orders must be supplied by another warehouse (i.e. 10% of all orders are not
in stock at the warehouse where the order is entered).

Another heavily weighted transaction is the recording of payments from
customers.  Delivering orders, stock level checks, and inquiring about the
status of certain orders are less frequently executed transactions.

The level of throughput is a result of the activity of the users executing
database transactions.  For each warehouse, ten terminals are simulated to
access the database.  The final throughput of the benchmark is directly
related to the number of warehouses the database is scaled to.  A remote
terminal emulator (RTE) is used to maintain the required mix of transactions
over run duration of the workload.  The mix represents the complete business
processing of an order as it is entered, paid for, checked , and delivered.
The primary metric of a TPC-C benchmark is the number of New-Order
transactions executed per minute, designated as tpmC.

TPC-C consists of five transactions of varying complexity.  These
transactions exorcises a database's ability to maintain data integrity,
accesses data of varying sizes, and handles contention on accesses and
updates.  The transactions are called New-Order, Payment, Order-Status,
Delivery, and Stock-Level.

For more information on the TPC, see their web site at: http://www.tpc.org/.
Further information on the TPC-C can be found on the web at:
http://www.tpc.org/tpcc/.

DBT-2
=====

DBT-2 is a derivative of the TPC-C designed to produce a real-world
on-line OLTP workload, similar to the TPC-C, to stress the Linux operation
system without the complexity and expense of running a TPC benchmark.

TPC benchmarks are intended as competitive marketing tools.  The TPC requires
all published results to comply with strict publication and auditing rules to
ensure fair comparisons between competitors.  The TPC also requires the
general availability and disclosure of the pricing for all products used for
the benchmark.  It is impractical for open source development projects to
adhere to these rules; thus, the results reported by the DBT-2 test kit
do not constitute a TPC-C result, and are incomparable with any TPC-C
benchmark.

The primary metric reported by the DBT-2 workload is the number of New-Order
transactions executed per minute and is expressed as NOTPM (New Order
Transactions per Minute).  However, NOTPM's do not and should not be compared
to tpmC measurements in any way since the DBT-2 workload does not constitute a
compliant TPC-C benchmark.

Improper Comparisons
====================

If you discover any usage of DBT-2 in drawing conclusions about TPC-C
performance, this inappropriate usage should be reported to both the TPC and
to the OSDL:

* TPC - admin@tpc.org
* OSDL - wookie@osdl.org

Design
======

This kit is composed of three main components, as illustrated in Figure 1: a
database, remote terminal emulators, and clients.  There can be multiple
terminals that connect to multiple terminal concentrators, which connect to a
single database.  Each component is described in the following sub-sections.

.. figure:: component.png
   :align: center
   :width: 100%

   DBT-2 Component Diagram

Database
--------

The database consists of nine tables with supportint five transactions.  While
this test kit is currently primarily maintained for PostgreSQL, it can be
adapted to any other database.  The data represents a company that is a
wholesale supplier with a number of distributed sales districts and associated
warehouses covering a wide geographic range.  The database can be scaled to any
number of warehouses to simulate businesses of varying sizes.  By default, a
warehouse covers 10 districts, each district serving 3,000 customers, with each
warehouse maintaining stock for a complete inventory of 100,000 items.  DBT-2
allows the rest of the database to be scaled as defined by the user.  The five
transactions supported are: New-Order, Payment, Order-Status, Delivery, and
Stock-Level.

New-Order Transaction
~~~~~~~~~~~~~~~~~~~~~

The New-Order transaction is a mid-weight, read-write single database
transaction designed to reflect on-line database activity typically found in
production environments.  The transaction performs seven to seventeen row
selections, six to sixteen row selections with updates, and seven to
seventeen row insertions, and is executed 45% of the time.

Payment Transaction
~~~~~~~~~~~~~~~~~~~

The Payment transaction is a light-weight, read-write database transaction
that updates a customer's balance and reflects payment on a district's and
warehouse's sales statistics.  The transaction performs an average of two row
selections, six row selections with updates, and two row insertions, and is
executed 43% of the time.

Order-Status Transaction
~~~~~~~~~~~~~~~~~~~~~~~~

The Order-Status transaction is a mid-weight read-only data transaction that
queries the status of a customer's most recent order.  The transaction
performs two row selections and nine to nineteen row selections with updates,
and is executed 4% of the time.

Delivery Transaction
~~~~~~~~~~~~~~~~~~~~

The Delivery transaction is a database transaction that processes up to ten
new orders.  The transaction performs two row selections, six to sixteen row
selections with updates, and one row deletion, and is executed 4% of the time.

Stock-Level Transaction
~~~~~~~~~~~~~~~~~~~~~~~

The Stock-Level transaction is a heavy read-only database transaction that
determines the number of recently sold items that have a stock level below a
specific threshold.  The transaction performs up to 900 row selections and is
executed 4% of the time.

Remote Terminal Emulators
-------------------------

A remote terminal emulator (RTE) simulates the activities of a person at a
terminal console executing one of the five transactions supported by the
database.  The RTE is designed to either connect to a client system in order
to access the database in a three-tier model.  The RTE is also designed so
that it can be controlled by an external process.  The external process is a
monitoring program that manages drivers across multiple systems.

The RTE is designed as a multi-threaded program where each thread of activity
represents a single terminal accessing the database.  Ten terminals are
simulated for every warehouse that the database is configured for.  Each
terminal records every interaction attempted and the response time from the
point where the request is sent to when the response has been received.

Clients
-------

The clients are terminal concentrators that allows more than one terminal to
share a connection to the database system.  The client program starts up a
listener to handle terminal requests and a pool of threads to process
transaction requests.  A new thread is created for each terminal connecting
to the client to.
