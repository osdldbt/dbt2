PostgreSQL
==========

A really quick howto
--------------------

Edit examples/dbt2_profile and follow the notes for the DBT2PGDATA
and DBDATA directory.  DBT2PGDATA is where the database directory will
be created and DBDATA is where the database table data will be
generated.

Create a 1 warehouse database by running bin/pgsql/dbt2-pgsql-build-db
and put the data files in '/tmp/data'::

    dbt2-pgsql-build-db -w 1

Run a 5 minute (300 second) test by running dbt2-run-workload::

    dbt2-run-workload -a pgsql -d 300 -w 1 -o /tmp/result -c 10

Tablespace Notes
~~~~~~~~~~~~~~~~

The scripts assumes a specific tablespace layout.

The ${DBT2TSDIR} variable in dbt2_profile defines the directory where all
tablespace devices will be mounted.  Directories or symlinks can be substituted
for what is assumed to be a mount point from this point forward.

In create_tables.sh is where the tablespaces are created.

The mount points that need to be created, and must be owned by the user running
the scripts are::

    ${DBT2TSDIR}/warehouse
    ${DBT2TSDIR}/district
    ${DBT2TSDIR}/customer
    ${DBT2TSDIR}/history
    ${DBT2TSDIR}/new_order
    ${DBT2TSDIR}/orders
    ${DBT2TSDIR}/order_line
    ${DBT2TSDIR}/item
    ${DBT2TSDIR}/stock
    ${DBT2TSDIR}/index1
    ${DBT2TSDIR}/index2
    ${DBT2TSDIR}/pk_customer
    ${DBT2TSDIR}/pk_district
    ${DBT2TSDIR}/pk_item
    ${DBT2TSDIR}/pk_new_order
    ${DBT2TSDIR}/pk_order_line
    ${DBT2TSDIR}/pk_orders
    ${DBT2TSDIR}/pk_stock
    ${DBT2TSDIR}/pk_warehouse

A (slightly less) quick howto run the test (Thanks Min!)
--------------------------------------------------------

* small db 2 warehouse;
* big db 20 warehouse,
* tiny db 500 warehouse (scaling other factors)

generated from: (tpcc only allow scaling warehouse)::

    mkdir DB.small
    dbt2-datagen --pgsql -w 2 -d DB.small -c 300 -i 10000 -o 300 -n 90
    mkdir DB.big
    dbt2-datagen --pgsql -w 20 -d DB.big -c 300 -i 10000 -o 300 -n 90
    
    # scaling the other factor
    mkdir DB.tiny
    dbt2-datagen --pgsql -w 500 -d DB.tiny -c 3 -i 10 -o 3 -n 9

You can get number of warehouse from psql: `select count(*) from warehouse;`

Then load database data using load_db.sh, make sure DBDATA variable
is pointing to the correct directory you just generated above.

Then run `dbt2-client` program manually,
you can monitor the query queue length by type "status" command::

    cd <dbt2_home>/bin/pgsql
    dbt2-client -d localhost -c 2 -l 5432 -o ../output/0

Then run `dbt2-driver` program manually,
you can control tpw (terminal per warehouse) and think time etc.::

    dbt2-src/driver -d localhost -l 360 -wmin 1 -wmax 20 -w 20 \
      -c 3 -i 10 -o 3 -n 9 \
      -ktd 0 -ktn 0 -kto 0 -ktp 0 -kts 0 \
      -ttd 0 -ttn 0 -tto 0 -ttp 0 -tts 0 -tpw 80 -outdir ../output/0

Finally, look in `../output/0 directory` for the possible error output

tuning
~~~~~~

1. If you have fsync on then the daemon will be waiting for disk I/O
   for writing log (WAL)
2. For 1 warehouse, increase currency hurt per cpu utilization.

   * 1 daemon 1 terminal: 12% in 8 way system
   * 2 daemon 2 terminal: 9.2%
   * 3 daemon 3 terminal: 6.7%
   * 4 daemon 4 terminal: 5.5%

Note the tpc-c model:

* company scales with number of warehouses
* each warehouse supports 10 districts
* each district serves 3000 customers
* each warehouse maintains stock level of 100,000 items
* require ~10% orders are fulfilled from other warehouses due to not have
  all items in the company's catalog
