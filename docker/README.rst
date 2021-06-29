These scripts and Docker files are for build testing, and evaluating DBT-2 with
a small 1 warehouse scale factor.  While DBT-2 compatible with multiple
database systems, this initial set of scripts is just for PostgreSQL.

The quickest way to try out the kit is to run::

    start-database-pgsql
    start-client-pgsql <database address>
    start-driver <client address>

* `build-client-pgsql` - Build a Docker image for the client transaction
                         manager.
* `build-database-pgsql` - Build a Docker image with a 1 warehouse database.
* `build-driver` - Build a docker image for the driver.
* `compile-dbt2` - Simply attempt to compile the local source code.
* `prepare-image` - Build a Docker image to be further expanded by the other
                    components in the kit.
* `shell` - Helper script to open a shell in any of the DBT-2 Docker images.
* `start-client-pgsql` - Script to start the client, and create the Docker
                         image if not done already.
* `start-database-pgsql` - Script to start the database, and create the Docker
                           image if not done already.
* `start-driver` - Script to execute the driver for a short 2 minute test, and
                   create the image if not done already.
