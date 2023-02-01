PostgreSQL
==========

Setup
-----

The DBT-2 test kit has been ported to work with PostgreSQL starting with
version 7.3.  It has be updated to work with later version and backwards
compatibility may vary.  Source code for PostgreSQL can be obtained from their
website at: https://www.postgresql.org/

To install PostgreSQL from source::

    ./configure --prefix=<installation dir>
    make
    make install

Prior to PostgreSQL 8.0 this additional make command is required to ensure the
server include files get installed::

    make install-all-headers

After installing PostgreSQL, the DBT-2 C stored functions need to be compiled
and installed, if they are to be used instead of pl/pgsql stored functions or
other client side transaction logic::

    cd storedproc/pgsql/c
    make
    make install

The 'make install' command will need to be run by the owner of the
database installation.

When testing PostgreSQL in a multi-tier system configuration, verify that the
database has been configuration to accept TCP/IP connections.  For example, the
`listen_addresses` parameter must be set to listen for connections on the
appropriate interfaces.  Remote connections must also be allowed in the
`pg_hba.conf` file.  The simplest (and insecure) way would be to trust all
connections on all interfaces.

Also prior to PostgreSQL 8.0, `pg_autovacuum` should be installed.  If
installing from source, it is located in the `contrib/pg_autovacuum` directory.

The following subsections have additional PostgreSQL version specific notes.

v7.3
~~~~

With PostgreSQL 7.3, it needs to be built with a change in `pg_config.h.in`
where `INDEX_MAX_KEYS` must be set to 64.  Be sure to make this change before
running the configure script for PostgreSQL.

v7.4
~~~~

With PostgreSQL 7.4, it needs to be built with a change in
`src/include/pg_config_manual.h` where `INDEX_MAX_KEYS` must be set to 64.

Edit the parameter in `postgresql.conf` that says `tcpip_socket = false`,
uncomment, set to `true`, and restart the daemon.

v8.0
~~~~

For PostgreSQL 8.0 and later, run `configure` with the `--enable-thread-safety`
to avoid `SIGPIPE` handling for the multi-thread DBT-2 client program.  This is
a significant performance benefit.

A really quick howto
--------------------

Edit `examples/dbt2_profile` and follow the notes for the `DBT2PGDATA`
directory.  `DBT2PGDATA` is where the database directory will be created.

Create a 1 warehouse database by running `dbt2-pgsql-build-db`::

    dbt2-pgsql-build-db -w 1

Run a 5 minute (300 second) test by running `dbt2-run-workload`::

    dbt2-run-workload -a pgsql -d 300 -w 1 -o /tmp/result -c 10

Building the Database
---------------------

These scripts will generate data in parallel based on the number of processors
detected on the system, and will stream the data into the database.  The
scripts currently do not let you control the degree of parallelism, or whether
the data should be created as files first, but this can be run manually.

The following command will create a 1 warehouse database in the `DBT2PGDATA`
location specified in the user's environment::

    dbt2-pgsql-build-db -w 1

By default the table data is streamed into the database as opposed to loading
it from files.  See the usage help for additional options::

    dbt2-pgsql-build-db -h

Environment Configuration
-------------------------

The DBT-2 scripts required environment variables to be set in order to work
properly (e.g. `examples/dbt2_profile`) in order for the scripts to work
properly.  For example::

    DBT2PORT=5432; export DBT2PORT
    DBT2DBNAME=dbt2; export DBT2DBNAME
    DBT2PGDATA=/tmp/pgdata; export DBT2PGDATA

An optional environment variable can be set to specify a different location for
the transaction logs (i.e. `pg_xlog`)::

    DBT2XLOGDIR=/tmp/pgxlogdbt2; export DBT2XLOGDIR

The environment variables must be defined in `~/.ssh/environment` file on each
system for multi-tier environment for `ssh`.  Make sure `PATH` is set to cover
the location where the DBT-2 executables and PostgreSQL binaries are installed.
For example::

    DBT2PORT=5432
    DBT2DBNAME=dbt2
    DBT2PGDATA=/tmp/pgdata
    PATH=/usr/local/bin:/usr/bin:/bin:/opt/bin

Tablespace Notes
----------------

The scripts assumes a specific tablespace layout.

The `${DBT2TSDIR}` variable in `dbt2_profile` defines the directory where all
tablespace devices will be mounted.  Directories or symlinks can be substituted
for what is assumed to be a mount point from this point forward.

`dbt2-pgsql-create-tables` is where the tablespaces are created.

The mount points that need to be created, and must be owned by the user running
the scripts, at::

    ${DBT2TSDIR}/warehouse
    ${DBT2TSDIR}/district
    ${DBT2TSDIR}/customer
    ${DBT2TSDIR}/history
    ${DBT2TSDIR}/new_order
    ${DBT2TSDIR}/orders
    ${DBT2TSDIR}/order_line
    ${DBT2TSDIR}/item
    ${DBT2TSDIR}/stock
    ${DBT2TSDIR}/index1
    ${DBT2TSDIR}/index2
    ${DBT2TSDIR}/pk_customer
    ${DBT2TSDIR}/pk_district
    ${DBT2TSDIR}/pk_item
    ${DBT2TSDIR}/pk_new_order
    ${DBT2TSDIR}/pk_order_line
    ${DBT2TSDIR}/pk_orders
    ${DBT2TSDIR}/pk_stock
    ${DBT2TSDIR}/pk_warehouse

AppImage Notes
--------------

Limitations
~~~~~~~~~~~

Using the AppImage has some limitations with PostgreSQL:

1. The AppImage cannot alone be used to build a database with C stored
   functions for the database transactions.  The full kit still needs to be
   downloaded on the PostgreSQL server so that the C stored functions can be
   built and installed onto the system.

AppImage Usage
~~~~~~~~~~~~~~

Here are examples of creating the database with pl/pgsql stored functions and
using the event-driven multi-process combined client-driver to execute a test.
For simplicity, the AppImage is assumed to have been renamed to `dbt2` from
`dbt2-X.Y.Z-ARCH.AppImage`.

Create a 1 warehouse database (note that the database name is exported into
the `DBT2NAME` environment) variable::

    export DBT2NAME="dbt2"
    dbt2 pgsql-build-db -s plpgsql -u -w 1

Run a 2 minute test::

    dbt2 driver3 -a pgsql -sleep 100 -wmin 1 -wmax 1 -w 1 -altered 1 -b dbt2 \
            -d localhost -l 120 -outdir /results

Generate a summary of the results, note this still requires R to be installed
on the host system::

    dbt2 post-process /results/mix-*.log
