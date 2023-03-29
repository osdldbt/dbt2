Overview
========

These scripts and container files are for build testing, and evaluating DBT-2
at a small warehouse scale factor.  While DBT-2 is compatible with multiple
database systems, this initial set of scripts works only for the subset
described here.  Note that these scripts prefer Podman over Docker but you can
specify the engine by setting the `ENGINE` environment variable to `podman` or
`docker`, resp.

* `appimage-build` - Build the DBT-2 lite AppImage.
* `appimage-prepare` - Build a container image to be used for creating an
                       AppImage.
* `build-all` - Run all of the build scripts to prepare all container images.
* `build-client` - Build a container image for the client transaction manager.
* `build-database` - Build a container image with a 1 warehouse database.
* `build-driver` - Build a container image for the driver.
* `compile-dbt2` - Simply attempt to compile the local source code.
* `prepare-image` - Build a container image to be further expanded by the other
                    components in the kit.
* `run-test` - Run a test using the run-workload script.
* `shell` - Helper script to open a shell in any of the DBT-2 container images.
* `start-client` - Script to start the client, and create the container image
                   if not done already.
* `start-database` - Script to start the database, and create the container
                     image if not done already.
* `start-driver` - Script to execute the driver for a short 2 minute test, and
                   create the image if not done already.

Testing binaries
================

These brief steps are intended to describe how to test the bare minimal
individual binaries of the kit.  In other words, how to run the workload
without capturing any additional data such at system statistics, software
profiles, or database statistics.

PostgreSQL
----------

Create a 1 warehouse database and run a test in a 3-tier configuration::

    container/start-database
    container/start-client <database address>
    container/start-driver <client address>

Testing the complete kit
========================

These brief steps are intended to describe how to set up containers such that
they can be treated with full virtual machines.  In other words, this allows
tests scripts to collect system stats, system profiles, and generating reports.

The results will be copied to `container/results`.

Testing the complete kit in containers may need more advanced knowledge of
Docker or Podman, such as have a host environment capable of running containers
that use systemd as entry point of a container.

Testing options
---------------

The `EXTRAARGS` environment variable can be set to pass extra arguments to the
dbt2-run-workload script.  Normally one would just set the arguments directly
when running the `dbt2-run-workload` script.

Set `TIERS=2` in the environment to test the event-driven multi-process altered
mode driver.  It is also recommended to also set `EXTRAARGS` to use the flags
`-A -t 1`.  See the `dbt2-run-workload` script help for more details on the
options.

Notes for collecting profiles
-----------------------------

The containers expect a Linux host to also set `kernel.perf_event_paranoid =
-1` on the host operating system, which will be inherited by the containers.

CockroachDB
-----------

Create all the container images for each tier with a 1 warehouse database::

    container/build-all 1 pgsql

Execute a test::

    container/run-3tier-test 1 pgsql

PostgreSQL
----------

Create all the container images for each tier with a 1 warehouse database::

    container/build-all 1 pgsql

Execute a test::

    container/run-3tier-test 1 pgsql

YugabyteDB
----------

Create all the container images for each tier with a 1 warehouse database::

    container/build-all 1 yugabyte

Execute a test::

    container/run-3tier-test 1 yugabyte
