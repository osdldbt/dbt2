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

There are two general ways to use this kit:

1. The **Manual Test Execution** section is not just how to run a test with
   minimal assistance, but also minimal data collection.  In other words, the
   kit only produces the test metrics and data to validate the correctness of
   the test.  It is left to the tester to do any additional data collection to
   characterize system behavior.  But there are no limitations on how one may
   actually execute a test.

2. The **Comprehensive Test Execution** section details how to use the provided
   shell scripts to fully characterize system behavior in addition to executing
   a test.  This includes collecting system statistics as well and software
   profiles.  There is not as much flexibility as running a test manually,
   while intending to help ease system characterization work.  Scripts are also
   provided to generate a report with all sorts of charts using gnuplot, in
   HTML (using Docutils) and PDFs (using pandoc in conjunction with Docutils).

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

External Software Dependencies
------------------------------

`SQLite3 <https://www.sqlite.org/index.html>`_ is required to do basic
statistical calculations in order to report test metrics.

System and database statistic collection and post processing are handled by
`Touchstone Tool <https://gitlab.com/touchstone/touchstone-tools>`_.  Install
this package, or its AppImage, in order to characterize system performance.
This package is included in the DBT-2 AppImage.

A test report can be generated with the aid of `DBT Tools
<https://github.com/osdldbt/dbttools>`_.  This package is included in the DBT-2
AppImage.

`gnuplot <https://www.gnuplot.info/>`_ is used when generating charts.  This
package is also included in the DBT-2 AppImage.

`jq <https://jqlang.github.io/jq/>`_ and `toml-cli
<https://github.com/gnprice/toml-cli>`_ are required to use TOML configuration
files for executing tests.

Linux AppImage
--------------

A Linux AppImage is available, intended for simplified use, albeit with reduced
functionality, on Linux based systems.  See **Database Management System
Notes** for specific usage and for any database management system specific
limitations.  The DBT-2 AppImage may not always be as capable as installing the
kit from source or from other pre-packaged distributions.

The DBT-2 AppImage must be installed on each system.  It is also recommended to
install the Touchstone Tools AppImage on each system when there are multiple
systems in use.

For ease of use, we recommend renaming the DBT-2 AppImage binary to `dbt2`, if
it hasn't already been done.  Examples in the documentation will assume it has
been renamed to `dbt2`.

If FUSE is not available, the AppImage is self-extracting and provides a script
`AppRun` that can be executed as if running the AppImage itself, when the
**APPDIR** environment variable is set to the absolute path of the extracted
`squashfs-root` directory.::

    dbt2-*.AppImage --extract-appimage
    export APPDIR="$(pwd)/squashfs-root"
    squashfs-root/AppRun

Alternatively, one could also set `PATH` and `LD_LIBRARTY_PATH` manually to
include the extracted environment.

DBT-2 Test Kit Source
---------------------

The latest stable version of the kit can be found on SourceForge at:
https://github.com/osdldbt/dbt2

Environment Configuration
=========================

The section explains how to configure various parts of the system before a test
can be executed.

The individual driver and client binaries (e.g. `dbt2-driver` and
`dbt2-client`) can be run directly (and you may want to depending on the
situation), but scripts, such as `dbt2-run`, are provided to make some testing
scenarios easier to execute.  It does this by taking advantage of password-less
`ssh` usage, but also requires additional environmental setup.  This **User
Guide** will not where applicable and the **Database Management System Notes**
may have additional notes depending on the database management system.

Multi-tier System Testing
-------------------------

DBT-2 supports a 1-, 2-, and 3-tier client-server system configurations.
Whether the binaries are run by hand, or whether the supplied helper scripts
are used determine how much additional environmental setup is needed.

When using the `dbt2-run` script, which also supports helping execute a 1-, 2-,
and 3-tier system configuration tests, additional environment variables may
need to be set because a remote ssh environment may be more limited than a
normal login environment.  For example, if the scripts and binaries are
installed in `/usr/local/bin`, this path may not be in the path when using
`ssh` to execute commands locally.  

Also password-less ssh keys needs to be set up to keep the scripts simple, the
kit needs to be installed in the same location on each system.  Each user must
also have sudo privileges, also without requiring a password.

The ssh server on each system may need to allow user environment processing.
To enable this, make the following change to the ssh server configuration file
(typically `/etc/ssh/sshd_config`) and restart the `sshd` daemon (see specific
instructions from the operating system in use)::

    PermitUserEnvironment yes

Some database specific examples are provided in the `examples/` directory.
Also see **Database Management System Notes** for more specific details per
database management system, if available.

Building the Database
=====================

The database needs to be created, and data needs to be loaded before testing
can begin.  It is recommended to build the database from the system itself, as
opposed to from a remote system, for performance reasons if possible.  See
**Database Management System Notes** for more specific instructions.  There may
be several options available depending on the database management system used.
For example, this kit may include scripts that help create the database
instance if one does not exist, install the database from source if the
software is not already installed, and even scripts to help configure the
database.

Sizing Consideration
--------------------

The database can be built with as few as 1 warehouse.  A 1,000 warehouse
generates about 100 GiB of raw text data.  The size of the database ultimately
depends on the schema definition and the number of indexes built.

Not all of the warehouses need to be used.  For example, if the database is
built with 100 warehouses, 80 can be specified.  Specifying more than available
warehouses will results errors.  The test will continue to run, but will log
errors if a warehouse referenced does not exist.

If DBT-2 is used with standard transaction mix ratios, keying, and thinking
times, etc. then the maximum throughput is limited to `12.86 x the number of
warehouses`.  Thus if you want more throughput, the database size also needs to
be increased.

If non-standard ratios, times, etc. are used, the database should be resized to
match the measured throughput.  This could take multiple revisions if database
is tested to be undersized compared to the measured throughput.

Of course if you have any reasons to test outside of these recommendations, you
should feel free to do so.

Running the Test Kit
====================

There are a numbers of ways that DBT-2 can be executed.  In each of these
scenarios, it is required that the database has been created and loaded before
any test can run successfully.  There are several factors that may influence
the execution method you may want to use.  This section will outline some of
the scenarios and provide examples.

The general test execution plan is:

1. Start the database management system
2. Start the client (e.g. `dbt2-client`), this can be skipped if running with
   the combined client-driver program `dbt2-driver3` (See examples to
   understand whether not this is the method you want to use.)
3. Start the driver (e.g. `dbt2-driver`)
4. Process the test results

There are multiple variants of the client and driver.

Client only:

1. `dbt2-client` - a pthread based multi-threaded program where 1 thread is
   created per database connection opened, and 1 thread is created per remote
   terminal connection.
2. `dbt2-client2` - an event-based multi-process program that is a work in
   progress. (This is still in development and won't be mentioned elsewhere
   until it is functionally complete.)

Driver (remote terminal emulator) only:

1. `dbt2-driver` - a pthread based multi-threaded program where 1 thread is
   created per warehouse and district pair.
2. `dbt2-driver2` - an event-based multi-process program that is a work in
   progress. (This is still in development and won't be mentioned elsewhere
   until it is functionally complete.)

Client-Driver combined:

1. `dbt2-driver3` - an event-based multi-progress program that spawns N (user
   defined) number of processes per detected processor, where warehouses are
   evenly partitioned between each process and an event timer is spawned for
   every warehouse and district pair in each process.

Using `dbt2-client` in conjunction with `dbt2-driver` is considered the most
traditional way to run the test.  `dbt2-driver3` was developed as a more
efficient and easier to use program to drive the workload.

The "Easy" Way
--------------

There are many ways that this kit can be used.  What *easy* means here is that
many of the decisions are made for you:

1. Use the DBT-2 AppImage because it is packaged with database management
   system client libraries and post processing analytical packages, thus
   minimizing system software setup.
2. Use a system where the database is already running and you already created
   a database, because these steps don't help with database installation or
   configuration.
3. Use the event-driven multi-process driver, which opens 1 database connection
   per processor on the system by default and minimizes the number of tiers
   used for testing.
4. Do not use any keying or thinking time, thus letting the system be driven as
   hard as possible depending on the number of available processors on the
   system.

The number of warehouses and the length of the test can still be specified.

The **Database Management System Notes** section may have additional database
management system specific notes.

The examples in this section assume that the DBT-2 AppImage has been renamed to
`dbt2` and is in the user's `PATH`.

PostgreSQL
~~~~~~~~~~

This example will connect to PostgreSQL based on what is in the user's
environment, as one would normally expect with core PostgreSQL utilities, but
additional arguments can be used to change the connection information.

Execute the following commands with default parameters to build a 1 warehouse
database with pl/pgsql stored functions, and run a 3 minute test::

    dbt2 build pgsql
    dbt2 run pgsql /tmp/results

Manual Test Execution
---------------------

This method involves starting each of the components manually without any help
from any of the scripts.

The database needs to be manually started.

The next step is to start the client.  The command line parameters depends on
the database management system tested so please review the help (`-h`) and
**Database Management System Notes** for details.  Here is an example for
starting the client with 10 connections opened to PostgreSQL::

    dbt2-client -a pgsql -d db.hostname -b dbt2 -c 10 -o .

The client will log errors, as well as its processor ID (pid) into the current
directory, as specified by the `-o .` parameter.  

The output from the client should look something like::

    setting locale: en_US.utf8
    listening to port 30000
    opening 10 connection(s) to localhost...
    listening to port 30000
    10 DB worker threads have started
    client has started

The next step is to start the driver.  To get sane results from a 1 warehouse
database, we should run the driver for at least 4 minutes (240 seconds)::

    dbt2-driver -d client.hostname -w 1 -l 240 -outdir .

The driver will log error and results, as well as its process ID (pid) into the
current directory.

The output from the driver should look something like::

    setting locale: en_US.utf8
    connecting to client at 'db.hostname'

    database table cardinalities:
    warehouses = 1
    districts = 10
    customers = 3000
    items = 100000
    orders = 3000
    stock = 100000
    new-orders = 900

     transaction  mix threshold keying thinking
    new order    0.45      0.45     18    12000
    payment      0.43      0.88      3    12000
    order status 0.04      0.92      2    10000
    delivery     0.04      0.96      2     5000
    stock level  0.04      1.00      2     5000

    w_id range 0 to 0
    10 terminals per warehouse
    240 second steady state duration

    driver is starting to ramp up at time 1675394297
    driver will ramp up in  10 seconds
    will stop test at time 1675394307
    seed for 212536:7f9eca271700 : 10962933948494954280
    seed for 212536:7f9eca234700 : 6320917737120767790
    seed for 212536:7f9eca213700 : 6590945454066933208
    seed for 212536:7f9eca1f2700 : 1675724396147333855
    seed for 212536:7f9eca1d1700 : 15221135594039080856
    seed for 212536:7f9eca1b0700 : 11698084064519635828
    seed for 212536:7f9eca18f700 : 12013746617097863687
    seed for 212536:7f9eca16e700 : 1937451735529826674
    seed for 212536:7f9eca14d700 : 10201147048873733402
    seed for 212536:7f9eca12c700 : 11758382826843355753
    terminals started...
    driver is exiting normally

The last step is to process the test data to see what the results are::

    dbt2-post-process mix.log

The resulting output should look something like::

    ============  =====  =========  =========  ===========  ===========  =====
              ..     ..    Response Time (s)            ..           ..     ..
    ------------  -----  --------------------  -----------  -----------  -----
     Transaction      %   Average     90th %        Total    Rollbacks      %
    ============  =====  =========  =========  ===========  ===========  =====
        Delivery   3.81      0.000      0.000            4            0   0.00
       New Order  47.62      0.001      0.001           50            1   2.00
    Order Status   5.71      0.001      0.001            6            0   0.00
         Payment  40.00      0.004      0.001           42            0   0.00
     Stock Level   2.86      0.000      0.000            3            0   0.00
    ============  =====  =========  =========  ===========  ===========  =====

    * Throughput: 12.99 new-order transactions per minute (NOTPM)
    * Duration: 3.9 minute(s)
    * Unknown Errors: 0
    * Ramp Up Time: 0.1 minute(s)

Congratulations, you've run a test!

Comprehensive Test Execution
----------------------------

The `dbt2-run` is a wrapper script that will attempt to collect system
statistics and database statistics, as well as start all components of the
test.  It can optionally profile a Linux system with readprofile, oprofile, or
perf.  See **Database Management System Notes** for any database management
system specific notes as there may be additional system specific flags.

The shell script `dbt2-run` is used to execute a test.  For example, run a 4
minutes (480 second) test against a default sized 1 warehouse database locally
and save the results to `/tmp/results`::

    dbt2 run -d 480 pgsql /tmp/results

See the help output from `dbt2 run --help` a brief description of all options.

This script will also process the results and output the same information as if
you were running `dbt2 report` manually like the last section's example.
Additional, the `dbt2 report --html` command is for building an HTML report
based on all of the data that is saved to `/tmp/results` by running::

    dbt2 report --html /tmp/results

The HTML report uses Docutils.  gnuplot is also required to generate any
charts.  This will create an `index.html` file in the `<directory>`.

An example of the HTML report is available online:
https://osdldbt.github.io/dbt-reports/dbt2/3-tier/report.html

Executing with multiple tiers
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

To execute the test where the database is on another tier, pass the `--db-host
<address>` flag to the `dbt2 run` command.  The address can be a hostname or IP
address.

To execute the test where the client is on another tier, pass the
`--client-host <address>` flag to the `dbt2 run` command.  The address can also
be a hostname or IP address.

Multi-process driver execution
------------------------------

Default behavior for the driver is to create 10 threads per warehouse under a
single process.  At some point (depends on hardware and resource limitations)
the driver, specifically `dbt2-driver` as a multi-threaded progress, will
become a bottleneck.  We can increase the load by starting multiple
multi-threaded drivers.  The `-b #` flag can be passed to the `dbt2 run`
command to specify how many warehouses to be created per process.  The script
will calculate how many driver processes to start.

Keying and Thinking Time
------------------------

The driver is supposed to emulate the thinking time of a person as well as the
time a person takes to enter information into the terminal.  This introduces a
limit on the rate of transaction that can be executed by the database.

Each of the DBT-2 drivers allows the tester to specify different delays for
each transaction's keying and thinking time.  The most common scenario is not
factor in any time for keying or thinking.  For example::

    -ktd 0 -ktn 0 -kto 0 -ktp 0 -kts 0 -ttd 0 -ttn 0 -tto 0 -ttp 0 -tts 0

See the help from the driver binaries to see which flag controls which
transaction's thinking and keying times if you want to varying the delays
differently.

The `dbt2 run` script sets each of the thinking and keying time flags to 0 by
default and does not offer any finer grained controls at this time.

Transaction Mix
---------------

The transaction mix can be altered with the driver using the following flags,
where the percentages are represented as a decimal number:

==  ===========================================================================
-q  percentage of Payment transaction, default 0.43
-r  percentage of Order Status transaction, default 0.04
-e  percentage of Delivery transaction, default 0.04
-t  percentage of Stock Level transaction, default 0.04
==  ===========================================================================

The percentage for the New Order transaction is the difference after the other
4 transactions such that the sum adds to 1 (i.e. 100%.)

Complex Test Configurations
---------------------------

The `run` script can use a TOML formatted configuration file to execute the
workload in more complex configuration than what the command line arguments can
provide.  Note that running the binaries by hand still offer the most
flexibility.  For example:

* Using multiple client programs across multiple systems
* Using multiple driver programs across multiple systems
* Specifying the client system and port per driver
* Specifying the database system to use per client (for distributed database
  systems)
* Specifying the warehouse range per driver

See the following subsections for specific scenarios.

Example 1: 1-tier Threaded Driver & Client
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

This the traditional way to run the workload with three components, the
database, a client, and a driver.  In this example all three components are run
on a single system::

    mode = 1
    database_name = "dbt2"
    warehouses = 1
    duration = 120

    [[client]]
    client_addr = "localhost"
    database_addr = "localhost"
    connections = 1

    [[driver]]
    driver_addr = "localhost"
    client_addr = "localhost"

Example 2: 1-tier Multiple Threaded Drivers & Clients
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

This example is similar to Example 1, except it illustrates how to start
mutiple drivers and clients on the same system::

    mode = 1
    database_name = "dbt2"
    warehouses = 2
    duration = 120

    [[client]]
    client_addr = "localhost"
    database_addr = "localhost"
    connections = 1

    [[client]]
    client_addr = "localhost"
    database_addr = "localhost"
    connections = 1
    client_port = 30001

    [[driver]]
    driver_addr = "localhost"
    client_addr = "localhost"
    wmin = 1
    wmax = 1

    [[driver]]
    driver_addr = "localhost"
    client_addr = "localhost"
    wmin = 2
    wmax = 2
    client_port = 30001

Example 3: 3-tier Threaded Driver & Client
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

This example is similar to Example 1, except it illustrates how to start
each component on separate systems, where the `run` script is executed on the
driver system::

    mode = 1
    database_name = "dbt2"
    warehouses = 1
    duration = 120

    [[client]]
    client_addr = "sodium"
    database_addr = "lithium"
    connections = 1

    [[driver]]
    driver_addr = "localhost"
    client_addr = "sodium"

Example 4: 3-tier Threaded Driver & Client
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

This example is similar to Example 3, except it illustrates how to start
multiple drivers and clients::

    mode = 1
    database_name = "dbt2"
    warehouses = 2
    duration = 120

    [[client]]
    client_addr = "sodium"
    database_addr = "lithium"
    connections = 1

    [[client]]
    client_addr = "sodium"
    database_addr = "lithium"
    connections = 1
    client_port = 30001

    [[driver]]
    driver_addr = "localhost"
    client_addr = "sodium"
    wmin = 1
    wmax = 1

    [[driver]]
    driver_addr = "localhost"
    client_addr = "sodium"
    wmin = 2
    wmax = 2
    client_port = 30001

Example 5: 1-tier Event-Driven Driver
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

This is a simplified and low resource way to run the workload, where the client
has been combined with the driver.  This example is single system where the
event-driven driver and database are on the same system::

    mode = 3
    database_name = "dbt2"
    warehouses = 1
    duration = 120

    [[driver]]
    driver_addr = "localhost"
    database_addr = "localhost"

Example 6: 1-tier multiple event-driven drivers
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

A single system where multiple event-driven drivers and database are on the
same system.  This example illustrates how to start multiple drivers that use a
different and distinct warehouse range::

    mode = 3
    database_name = "dbt2"
    warehouses = 2
    duration = 120

    [[driver]]
    driver_addr = "localhost"
    database_addr = "localhost"
    wmin = 1
    wmax = 1

    [[driver]]
    driver_addr = "localhost"
    database_addr = "localhost"
    wmin = 2
    wmax = 2

Example 7: 2-tier Event-Driven Driver
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

This is similar to Example 5 except the driver is on a separate system.  The
`run` script is executed on the driver system::

    mode = 3
    database_name = "dbt2"
    warehouses = 1
    duration = 120
    [[driver]]
    driver_addr = "localhost"
    database_addr = "lithium"

Example 8: 2-tier Multiple Event-Driven Drivers
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

This example expands on Example 7 where there are multiple event-driven drivers
started on the same system that are configured on distinct warehouse ranges::

    mode = 3
    database_name = "dbt2"
    warehouses = 2
    duration = 120

    [[driver]]
    driver_addr = "localhost"
    database_addr = "lithium"
    wmin = 1
    wmax = 1

    [[driver]]
    driver_addr = "localhost"
    database_addr = "lithium"
    wmin = 2
    wmax = 2
