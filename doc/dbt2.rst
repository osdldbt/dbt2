=====================================
Database Test 2 (DBT-2) Documentation
=====================================

::

    Open Source Development Labs, Inc.
    12725 SW Millikan Way, Suite 400
    Beaverton, OR 97005
    Phone: (503) 626-2455
    Fax: (503) 626-2436
    Email: info@osdl.org

Copyright (c) 2002 by The Open Source Development Laboratory, Inc. This
material may be distributed only subject to the terms and conditions set forth
in the Open Publication License, v1.0 or later (the latest version is currently
available at http://www.opencontent.org/openpub/). Distribution of
substantively modified versions of this document is prohibited without the
explicit permission of the copyright holder.

Other company, product or service names may be trademarks or service marks of
others.

.. contents:: Table of Contents

------------
Introduction
------------

The OSDL Database Test 2 (DBT-2) workload test kit provides an on-line
transaction processing (OLTP) workload using an open source database and a set
of defined transactions.  This document gives an overview of the DBT-2 test
kit.

The DBT-2 test kit is a derivative of a benchmark specification released by the
Transaction Processing Performance Council (TPC).  The TPC Benchmark(TM) C
(TPC-C) is briefly described before the DBT-2 test kit is discussed.

.. include:: tpc.rst
.. include:: architecture.rst
.. include:: quick-start.rst
.. include:: user-guide.rst
.. include:: testing.rst

--------------------------------
Database Management System Notes
--------------------------------

.. include:: cockroachdb.rst
.. include:: mysql.rst
.. include:: postgresql.rst
.. include:: sqlite.rst
.. include:: yugabytedb.rst

----------------------
Operating System Notes
----------------------

.. include:: linux.rst
.. include:: solaris.rst

.. include:: dev-guide.rst
