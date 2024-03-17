This document covers installing the kit from source.

Perquisites
-----------

Required software:

* C compiler
* `CMake <https://cmake.org/>`_ is the build system used

Recommended software:

* `Make` can be used with the supplied `Makefile.cmake` for running some of the
  common build tasks

The kit will build support with the following database management system client
libraries if they can be detected by pkg-config.  The following client
libraries that are support are:

* libpq (CockroachDB, PostgreSQL, YubabyteDB)
* libmysqlclient (MySQL)
* sqlite3 (SQLite)

Building
--------

::

	make -f Makefile.cmake release

If `make` is not available, the `cmake` commands can be reviewed in the
`Makefile.cmake` file.

Installing
----------

::

	cd builds/release
	cmake --install . --prefix /usr/local

Uninstalling
------------

::

    xargs rm < install_manifest.txt

The file `install_manifest.txt` will be created after running `make install`.
