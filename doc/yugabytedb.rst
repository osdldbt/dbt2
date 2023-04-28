YugabyteDB
==========

A really quick howto.

The YugabyteDB scripts makes use of PostgreSQL's psql client program and libpq
C client library interface, thus any environment variables that libpq would
use.  See dbt2_profile for examples of what environment variables that may be
used.

The *run* script currently only support handling YugabyteDB installations that
are already setup.  i.e. script do not handle installing or initialization a
database instance.

Create a 1 warehouse database by running dbt2-yugabyte-build-db::

	dbt2-yugabyte-build-db -w 1 -l 26257

Flag description::

	-d <dir> -  Location for data files to load.
	-w #     -  Set the warehouse scale factor.

Additional flags::

	-h            -  Help.
	-r            -  Drop the database before loading, for rebuilding the
                     database.

Run a test a 2 minute, 1 warehouse test, specifying a PostgreSQL database
management system::

    dbt2-run -a pgsql -c 10 -d 120 -w 1 -o /tmp/results
