SAP DB
======

Setup
-----

After installing SAP DB create the user id *sapdb*, assigning the user to group
*sapdb*.

Create the database stats collection tool, x_cons, by executing the following::

    cp -p /opt/sapdb/indep_prog/bin/x_server /opt/sapdb/indep_prog/bin/x_cons

The test kit requires unixODBC drivers to connect with SAP DB.  The driver can
be install from the source available at:
http://www.unixodbc.org/unixODBC-2.2.3.tar.gz

ODBC
----

An `.odbc.ini` file must reside in the home directory of the user
attempting to run the program.  The format of the file is::

    [alias]
    ServerDB = database_name
    ServerNode = address
    Driver = /opt/sapdb/interfaces/odbc/lib/libsqlod.so
    Description = any text

For example::

    [dbt2]
    ServerDB = DBT2
    ServerNode = 192.168.0.1
    Driver = /opt/sapdb/interfaces/odbc/lib/libsqlod.so
    Description = OSDL DBT-2

is a valid `.odbc.ini` file where dbt2 can be used as the server name to
connect to the database.  Note that the location of _libsqlod.so_ may vary
depending on how your database is installed on your system.

Building the Database
---------------------

The script dbt2/scripts/sapdb/create_db_sample.sh must be edited to configure
the SAP DB devspaces.  The param_adddevspaces commands need to be changed to
match your system configuration.  Similarly, the backup_media_put commands also
need to be changed to match your system configuration.  The script assumes the
instance name to be `DBT2`.  Change the line `SID=DBT2` if you need to change
the instance name (in this an all the scripts).  Many other SAP DB parameters
(e.g. MAXCPU) will need to be adjusted based on your system.

As user sapdb, execute the following script, in dbt2/scripts/sapdb, to generate
the database from scratch (which will execute create_db_sample.sh)::

    ./db_setup.sh <warehouses> <outputdir>

where <warehouses> is the number of warehouses to build for and <outputdir> is
the directory in which the data files will be generated.  This script will
create the data files, create the database, the tables, load the database,
create indexes, update database statistics, load the stored procedures, and
backup the database.  This script assumes the instance name is `DBT2` so you
need to edit the line ''SID=DBT2'' if you need to change the instance name.

Although the create script starts the remote communication server, it is a good
idea to include the same thing in your system startup scripts /etc/rc.local::

    # start remote communication server:
    x_server start > /dev/null 2>&1:

There is an option for ''migration'' that needs to be documented, or else
removed from the create_db.sh scripts.

It is highly recommended for performance that you use raw partitions for the
LOG and DATA files.  Here is a sample of the adddevices when using 1 LOG (the
limit is one)  of 1126400 -8k pages and 5 raw DATA devices of 2044800 -8k pages
each::

    param_adddevspace 1 LOG  /dev/raw/raw1 R 1126400
    param_adddevspace 1 DATA /dev/raw/raw2 R 204800
    param_adddevspace 2 DATA /dev/raw/raw3 R 204800
    param_adddevspace 3 DATA /dev/raw/raw4 R 204800
    param_adddevspace 4 DATA /dev/raw/raw5 R 204800
    param_adddevspace 5 DATA /dev/raw/raw6 R 204800

Results
-------

Each output files starting with `m_*.out`, refers to a monitor table in the
SAP DB database.  For example:

===========  ==================================================================
m_cache.out  MONITOR_CACHE contains information about the operations performed
             on the different caches.
m_lock.out   MONITOR_LOCK contains information on the database's lock
             management operations.
m_pages.out  MONITOR_PAGES contains information on page accesses.
m_trans.out  MONITOR_TRANS contains information on database transaction.
m_load.out   MONITOR_LOAD contains information on the executed SQL statements
             and access methods.
m_log.out    MONITOR_LOG contains information on the database logging
             operations.
m_row.out    MONITOR_ROW contains information on operations at the row level.
===========  ==================================================================

Additional database locking statistics are collected in `lockstats.csv`.  The
data and log devspaces statistics are collected in `datadev0.txt`,
`datadev1.txt`, `logdev0.txt`, and `logdev1.txt`, respectively.
