Remember to review the README-APPIMAGE.rst file for general comments about
AppImage usage.

Limitations
===========

Using the AppImage has some limitations with PostgreSQL:

1. The AppImage cannot alone be used to build a database with C stored
   functions for the database transactions.  The full kit still needs to be
   downloaded on the PostgreSQL server so that the C stored functions can be
   built and installed onto the system.
2. Use a custom configured PostgreSQL build with minimal options enabled to
   reduce library dependency support.  Part of this reason is to make it easier
   to include libraries with compatible licences.  At least version PostgreSQL
   11 should be used for the `pg_type_d.h` header file.

Creating the AppImage
=====================

At the time of this document, PostgreSQL 11 was configured with the following
options::

    ./configure --without-ldap --without-readline --without-zlib \
          --without-gssapi --with-openssl

Don't forget that both PATH and LD_LIBRARY_PATH may need to be set
appropriately depending on where the custom build of PostgreSQL is installed.

Using the AppImage
==================

Here are example of creating the database with pl/pgsql stored functions and
using the event-driven multi-process combined client-driver to execute a test.
For simplicity, the AppImage is assumed to have been renamed to `dbt2` from
dbt2-X.Y.Z-ARCH.AppImage.

Create a 1 warehouse database (note that the database name is exported into
the DBT2NAME environment) variable::

    export DBT2NAME="dbt2"
    dbt2 pgsql-build-db -s plpgsql -u -w 1

Run a 2 minute test::

    dbt2 driver3 -a pgsql -sleep 100 -wmin 1 -wmax 1 -w 1 -altered 1 -b dbt2 \
            -d localhost -l 120 -outdir /results

Generate a summary of the results, note this still requires R to be installed
on the host system::

    dbt2 post-process /results/mix-*.log
