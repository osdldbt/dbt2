-----------
Quick Start
-----------

The fastest way to get started is to use a Linux x86_64 based system and to
download the latest AppImage from GitHub:
https://github.com/osdldbt/dbt2/releases

Rename the AppImage file to **dbt2** and make sure it is in your `PATH` and is
executable.

The rest of this section describes the easiest way to use this test kit.  See
the rest of the documentation for more advanced uses.

PostgreSQL
==========

Start a PostgreSQL instance on the local system and make sure the local user
can connect to PostgreSQL.

Next build a database::

    dbt2 build pgsql

Next run a test and save the results to directory named `results`::

    dbt2 run pgsql results
