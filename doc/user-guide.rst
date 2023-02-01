----------
User Guide
----------

Introduction
============

This document provides instructions on how to set up and use the Open Source
Development Lab's Database Test 2 (DBT-2) test kit.  Database management
systems can be installed from source or packages, or a DBaaS option can
possibly be used.  This section provides the high level overview of how to use
the test kit.  See **Database Management System Notes** for more specific
details as it pertains to the database management system being tested, if
available.

The database management systems that are currently supported are:

* PostgreSQL
* SQLite

These database management systems are a work-in-progress:

* CockroachDB
* YugabyteDB

Databases management systems that have worked in the past but are not current:

* MySQL
* SAP DB

Setup
=====

Linux AppImage
--------------

A Linux AppImage is available.  See **Database Management System Notes** for
specific usage and for any database management system specific limitations.
The DBT-2 AppImage may not always be as capable as installing the kit from
source or from other pre-packaged distributions.

For ease of use, we recommend renaming the DBT-2 AppImage binary to `dbt2`, if
it hasn't already be done.  Examples in the documentation will assume it has
been renamed to `dbt2`.

Linux Distribution Packages
---------------------------

There may be packaging available for your Linux distribution.  See this project
for more details: https://github.com/osdldbt/dbt2-packaging

DBT-2 Test Kit Source
---------------------

The latest stable version of the kit can be found on SourceForge at:
https://github.com/osdldbt/dbt2

Environment Configuration
=========================

The section explains how to configure various parts of the system before a test
can be executed.

Multi-tier System Testing
-------------------------

The DBT-2 scripts support a 2-tier system configuration where the driver and
client components are run on one tier and the database management system is
running on a separate tier.

The user environment on each system needs to have the same environment
variables defined.  Some database specific examples are provided in the
`examples/` directory.  See **Database Management System Notes** for more
specific details per database management system, if available.

Also password-less ssh keys needs to be set up to keep the scripts simple, the
kit needs to be installed in the same location on each system.  Each user must
also have sudo privileges, also without requiring a password.

The ssh server on each system needs to allow user environment processing.  This
is because by default ssh usually has a limited set of environment variables
set.  For example, `/usr/local/bin` is usually not in the path when using ssh
to execute commands locally.  This is disabled by default.  To enable, make the
following change to the sshd config file (typically `/etc/ssh/sshd_config`) and
restart the sshd server::

    PermitUserEnvironment yes

Building the Database
---------------------

The database needs to be created, and data needs to be loaded before testing.
It is recommended to run these on the database system itself, if possible.  See
**Database Management System Notes** for more specific instructions.

Running the Test Kit
--------------------

The `dbt2-run-workload` is a wrapper script that will attempt to collect system
statistics and database statistics, as well as start all components of the
test.  See **Database Management System Notes** for any database management
system specific notes as there may be additional system specific flags.

The shell script `dbt2-run-workload` is used to execute a test.  For
example, run a 2 minutes (120 second) test against a 1 warehouse database
and save the results to `/tmp/results`::

    dbt2-run-workload -a pgsql -d 120 -w 1 -o /tmp/results -c 10

See the help output from `dbt2-run-workload -h` a brief description of all
options.

Executing with multiple tiers
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

To execute the test in a multi-tier configuration pass the `-H <address>` flag
to the `dbt2-run-workload` script.  The address can be a hostname or IP
address.

Multi-process driver execution
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Default behavior for the driver is to create 10 threads per warehouse under a
single process.  The `-b #` flag can be passed to the `dbt2-run-workload`
script to specify how many warehouses to be created per process.  The script
will calculate how many driver processes to start.

Test Results
============

Under the output directory, specified by the `dbt2-run-workload` script's `-o`
flag, there are a number of subdirectories and files.

::

    |===============================================================
    |client/           |Output and error logging for dbt2-client
    |db/               |DB stats, OS stats of DB system, db log file
    |driver/           |Output and error logging for dbt2-driver
    |mix.log           |Aggregated transaction response time data
    |perf-annotate.txt |Linux perf annotated source
    |perf.data         |Raw Linux perf data
    |perf-report.txt   |Linux perf report
    |perf-trace.txt    |Linux perf trace
    |readme.txt        |Test details
    |report.txt        |Transaction response time stats
    |===============================================================

The Linux perf files are only created if the `-r` flag is used with
`dbt2-run-workload` to collect that data.

The `readme.txt` displays the comment for the test, `uname` output, changes in
kernel parameters from the previous test, and database version information.

Report
------

An example of a report is available online:
https://osdldbt.github.io/dbt-reports/dbt2/3-tier/report.html

A reStructuredText report with charts and links to the many of the files in the
results directory can be created by running the following command::

    dbt2-generate-report -i <directory>

Where `<directory>` is the path specified by the `-o` flag when running
`dbt2-run-workload`.

R is required to generate any charts.

An HTML report is also generated if Docutils are available on the system. This
will create an `index.html` file in the `<directory>`.

A PDF report is also generated if pandoc is available on the system.
