---------------
Developer Guide
---------------

This document is for detailing any related to the development of this test kit.

Building the Kit
================

CMake is build system used for this kit.  A `Makefile.cmake` is provided to
automate some of the tasks.

Building for debugging::

    make -f Makefile.cmake debug

Building for release::

    make -f Makefile.cmake release

Building source packages::

    make -f Makefile.cmake package

See the **AppImage** section for details on building an AppImage.  There are
additional requirements for the `appimage` target in the `Makefile.cmake`.
Alternatively, the kit provides scripts to create a container that can create
an AppImage.

Testing the Kit
===============

The CMake testing infrastructure is used with shUnit2 to provide some testing.

datagen
-------

Tests are provided to verify that partitioning does not generate different data
than if the data was not partitioned.  There are some data that is generated
with the time stamp of when the data is created, so those columns are ignored
when comparing data since they are not likely to be the same time stamps.

post-process
------------

A test is provided to make sure that the post-process output continue to work
with multiple mix files.

AppImage
========

AppImages are only for Linux based systems:

    https://appimage.org/

The AppImageKit AppImage can be downloaded from:

    https://github.com/AppImage/AppImageKit/releases

It is recommended to build AppImages on older distributions:

    https://docs.appimage.org/introduction/concepts.html#build-on-old-systems-run-on-newer-systems

At the time of this document, CentOS 7 is the one of the oldest supported Linux
distributions with the oldest libc version.

The logo used is the number "2" from the Freeware Metal On Metal Font.

See the `README.rst` in the `tools/` directory for an example of creating
an AppImage with a Podman container.

Building the AppImage
---------------------

Use a custom configured PostgreSQL build with minimal options enabled to reduce
library dependency support.  Part of this reason is to make it easier to
include libraries with compatible licences.  At least version PostgreSQL 11
should be used for the `pg_type_d.h` header file.

At the time of this document, PostgreSQL 11 was configured with the following
options::

    ./configure --without-ldap --without-readline --without-zlib \
          --without-gssapi --with-openssl

Don't forget that both `PATH` and `LD_LIBRARY_PATH` may need to be set
appropriately depending on where the custom build of PostgreSQL is installed.
