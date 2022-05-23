These scripts and Docker files are for build testing, and evaluating DBT-2 with
a small 1 warehouse scale factor.  While DBT-2 compatible with multiple
database systems, this initial set of scripts works only for a the subset
described here..

* `build-client` - Build a Docker image for the client transaction manager.
* `build-database` - Build a Docker image with a 1 warehouse database.
* `build-driver` - Build a docker image for the driver.
* `compile-dbt2` - Simply attempt to compile the local source code.
* `prepare-image` - Build a Docker image to be further expanded by the other
                    components in the kit.
* `run3` - Run a 2-tier test using the run-workload script.
* `shell` - Helper script to open a shell in any of the DBT-2 Docker images.
* `start-client` - Script to start the client, and create the Docker image if
                   not done already.
* `start-database` - Script to start the database, and create the Docker image
                     if not done already.
* `start-driver` - Script to execute the driver for a short 2 minute test, and
                   create the image if not done already.

CockroachDB
===========

Create a 1 warehouse database and run a test in a 2-tier configuration::

    docker/build-driver
    docker/build-database 1 cockroach
    # run `docker inspect dbt2-cockroach-1` to get the IP address
    docker/run3 <database address> cockroach 1

PostgreSQL
==========

Create a 1 warehouse database and run a test in a 3-tier configuration::

    docker/start-database
    docker/start-client <database address>
    docker/start-driver <client address>

Create a 1 warehouse database and run a test in a 2-tier configuration::

    docker/start-database
    docker/run3 <database address> pgsql 1
