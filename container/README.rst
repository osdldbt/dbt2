Overview
========

These scripts and container files are for build testing, and evaluating DBT-2
at a small warehouse scale factor.  While DBT-2 compatible with multiple
database systems, this initial set of scripts works only for the subset
described here.  Note that these scripts prefer Podman over Docker.

* `build-client` - Build a container image for the client transaction manager.
* `build-database` - Build a container image with a 1 warehouse database.
* `build-driver` - Build a container image for the driver.
* `compile-dbt2` - Simply attempt to compile the local source code.
* `prepare-image` - Build a container image to be further expanded by the other
                    components in the kit.
* `run3` - Run a 2-tier test using the run-workload script.
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

CockroachDB
-----------

Create a 1 warehouse database and run a test in a 2-tier configuration::

    container/build-driver
    container/build-database 1 cockroach
    # run `docker|podman inspect dbt2-cockroach-1` to get the IP address
    container/run3 <database address> cockroach 1

PostgreSQL
----------

Create a 1 warehouse database and run a test in a 3-tier configuration::

    container/start-database
    container/start-client <database address>
    container/start-driver <client address>

Create a 1 warehouse database and run a test in a 2-tier configuration::

    container/start-database
    container/run3 <database address> pgsql 1

YugabyteDB
----------

Create a 1 warehouse database and run a test in a 2-tier configuration::

    container/build-driver
    container/build-database 1 yugabyteDB
    # run `docker|podman inspect dbt2-yugabyteDB-1` to get the IP address
    container/run3 <database address> cockroach 1

Testing the complete kit
========================

These brief steps are intended to describe how to set up containers such that
they can be treated with full virtual machines.  In other words, this allows
tests scripts to collect system stats, system profiles, and generating reports.

While this needs more advanced knowledge of Docker or Podman, these scripts
need more work to be easier to use.

Additional options for Podman
-----------------------------

These steps probably only apply to Linux hosts for collecting system profiles.

`--cap-add SYS_ADMIN` needs to be used with the `run` command in order to grant
permissions to the container.  Linux also needs to set
`kernel.perf_event_paranoid = -1`.

PostgreSQL
----------

Create images for each tier::
    container/build-database
    container/build-client
    container/build-driver

Start a container for each tier (note additional options in a previous section
for collection software profiles)::

    podman container run --network=podman -d --name db0 dbt2-database-pgsql-1w
    podman container run --network=podman -d --name cli dbt2-client
    podman container run --network=podman -d --name dri dbt2-driver

The IP address needs to be extracted from the client node and the database node
either by using the `inspect` command or by getting on each node and asking the
operating system.

Open a shell on the driver node, run a test and generate a report::

    podman exec -it -u postgres dri /bin/bash
    # Add -r flag to dbt2-run-workload to collect software profiles
    dbt2-run-workload \
            -a pgsql \
            -c 10 \
            -C 10.10.10.1 \ # IP address of client container
            -d 120 \
            -D dbt2 \
            -H 10.10.10.1 \ # IP address of database container
            -n \
            -o /tmp/results \
            -s 100 \
            -w 1
    dbt2-generate-report -i /tmp/results
