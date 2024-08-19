MySQL
=====

COMMENTS
--------

There are two versions of DBT2(TPC-C) test:

- pure C based version of the test(nonSP)
- server side SP based version of the test(default)

It is possible to run C based(nonSP) test with any version of MySQL 
server.

To run test you have to build test with includes and libraries 
from MySQL 5.0 or higher.

See examples/mysql/mysql-dbt2.cnf for example MySQL defaults file.

MULTI-TIER TESTING
------------------

- Enable 'PermitUserEnvironment' in sshd_config.

- Set DBT2DATADIR in the database system user's '.ssh/environment' file:

  DBT2DATADIR=/tmp/mydbt2

- Enalbe tcp/ip listening in the defaults file (see example from above).  This
  binds to all network interfaces::

    bind-address = 0.0.0.0

- The build scripts automatically grants remote access to DB_USER
  (default root) for everything from any ip address.  If there are other users
  that require access, the grant will have to be done manually:

  grant all on *.* to ${DB_USER}@'%';

PREPARATION FOR TEST
--------------------

0. Build test binaries

NOTE:

   If you want to compile with MySQL support, you must either make sure 
   that path to 'mysql_config' is listed in your PATH environment variable.

::

    cmake CMakeLists.txt
    make
    make install

1. How to generate data files for test?

   One has to specify::

     -w - number of warehouses (example: -w 3)
     -d - output path for data files (example: -d /tmp/dbt2-w3)
     - mode (example: --mysql)

   datagen -w 3 -d /tmp/dbt2-w3 --mysql

   Please note that output directory for data file should exist.

2. How to load test database?

   You should run shell script which will create database scheme
   and will load all data files.

::

    dbt2-mysql-build-db -d dbt2 -f /tmp/dbt2-w3 -v -w 2 -s /tmp/mydbt2.sock

   usage: dbt2-mysql-build-db [options]
   options::

       -d <database name>
       -f <path to dataset files>
       -m <database scheme [OPTIMIZED|ORIG] (default scheme OPTIMIZED)>
       -c <mysql.cnf defaults file>
       -s <database socket>
       -h <database host>
       -u <database user>
       -p <database password>
       -e <storage engine: [MYISAM|INNODB|BDB]. (default INNODB)>
       -l <to use LOCAL keyword while loading dataset>
       -v <verbose output>

   Example: dbt2-mysql-build-db -d dbt2 -f /tmp/dbt2-w3 -s /tmp/mysql.sock -p ~/src/dbt2/examples/mysql/mysql-dbt2.cnf

3. How to load SP procedures?

   dbt2-mysql-load-stored-procs

   usage: dbt2-mysql-load-sp [options]
   options::

       -d <database name>
       -f <path to SPs>
       -h <database host (default: localhost)>
       -s <database socket>
       -u <database user>
       -p <database password>
       -t <database port>

   Example: dbt2-mysql-load-stored-procs -d dbt2 -f ~/src/dbt2/storedproc/mysql/ -s /tmp/mysql.sock

RUN TEST
--------

   dbt2-run

   usage: dbt2-run -a mysql -c <number of database connections> -t <duration of test> -w <number of warehouses>
   other options::

       -D <database name. (default dbt2)>
       -h <database host name. (default localhost)>
       -l <database port number. (default 3306)>
       -S <database socket>
       -u <database user>
       -x <database password>
       -s <delay of starting of new thread in milliseconds(default 300ms)>
       -t <terminals per warehouse. [1..10] (default 10)>
       -z <comments for the test>
       -n <use zero delays for test (default no)>
       -v <verbose output>
       -o <output dir>
       -p <mysql defaults file

   Example: dbt2-run -a mysql -D dbt2 -c 20 -t 300 -w 3 -o results -S /tmp/mysql.sock -p ~/src/dbt2/examples/mysql/mysql-dbt2.cnf

   Test will be run for 300 seconds with 20 database connections and 
   scale factor(num of warehouses) 3::

    -c number of database connections 
    -d duration of test in seconds
    -w number of warehouses (scale factor)

WARNING: If you break test (by Control-C for instance) or some kind of error
happened during running of test and you want to start test again please be sure 
that 'client' and 'driver' programms are not running anymore otherwise test 
will fail.

WARNING: Please ensure that number of warehouses (option -w) is less of equal
(not greater) to the real number of warehouses that exist in your test
database.

POSTRUNNING ANALYSES
--------------------

Results can be found in bin/output/<number>

some of the usefull log files::

    results/client/${HOSTNAME}.*/error.log - errors from backend C|SP based
    results/driver/${HOSTNAME}.*/error.log - errors from terminals(driver)
    results/mix.log - info about performed transactions
  r  esults/report.txt - results of the test
