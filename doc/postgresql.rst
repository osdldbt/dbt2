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

The `make install` command will need to be run by the owner of the database
installation.

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

Create a 1 warehouse database by running::

    dbt2 build pgsql

Run a 5 minute (300 second) test by running::

    dbt2 run -d 300 pgsql /tmp/result

Building the Database
---------------------

The `dbt2-pgsql-build-db` script is designed to handle the following steps:

1. create the database
2. create the tables
3. generate and load the tables
4. create the indexes
5. vacuum the database

The `dbt2-pgsql-build-db` script is also designed to generate data in parallel
based on the number of processors detected on the system, and will stream the
data into the database.  The script currently do not let you control the degree
of parallelism, or whether the data should be created as files first, but these
can be controlled manually by running the `dbt2-datagen` binary directly.

The other significant choices available are:

* `-r` drop the existing database first
* `-s <c | plpgsql>` use C or pl/pgsql stored functions, where plpgsql is the
  default
* `-t` use tablespaces for tables and indexes
* `-u` the executing user has privileges to restart the database system, and
  drop and create a database

See the usage output with the `-h` for the complete list of options.

The following command will create a default sized 1 warehouse database::

    dbt2 build pgsql

Environment Configuration
-------------------------

The DBT-2 scripts required environment variables to be set in order to work
properly (e.g. `examples/dbt2_profile`) in order for the scripts to work
properly.  For example::

    DBT2PORT=5432; export DBT2PORT
    DBT2DBNAME=dbt2; export DBT2DBNAME
    DBT2PGDATA=/tmp/pgdata; export DBT2PGDATA

An optional environment variable can be set to specify a different location for
the transaction logs (i.e. `pg_xlog` or `pg_wal`) when the `dbt2-pgsql-init-db`
script::

    DBT2XLOGDIR=/tmp/pgxlogdbt2; export DBT2XLOGDIR

The environment variables may need to be defined in `~/.ssh/environment` file
on each system for multi-tier environment for `ssh`.  The ssh daemon may need
to be configured to enable the use of user environment files.  Make sure `PATH`
is set to include the location where the DBT-2 executables and PostgreSQL
binaries are installed, if not in the default `PATH`.  For example::

    DBT2PORT=5432
    DBT2DBNAME=dbt2
    DBT2PGDATA=/tmp/pgdata
    PATH=/usr/local/bin:/usr/bin:/bin:/opt/bin

Tablespace Notes
----------------

The scripts assumes a specific tablespace layout for keeping the scripts
simple.

The `${DBT2TSDIR}` environment variable defines the directory where all
tablespace devices will be mounted.  Directories or symlinks can be substituted
for what is assumed to be a mount point from this point forward.

`dbt2-pgsql-create-tables` and `dbt2-pgsql-create-indexes` are where the
tablespaces are created.

The expected mount points or symlinks, which must also be writeable by the
database owner, need to be at::

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
   built and installed onto the system.  Thus the default behavior is to use
   the pl/pgsql stored functions.
