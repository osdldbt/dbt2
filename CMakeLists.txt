CMAKE_MINIMUM_REQUIRED(VERSION 2.4)
PROJECT(DBT2 C)

#
# Defining binaries so to minimized typoes.
#

SET(DBT2_CLIENT bin/dbt2-client)
SET(DBT2_DATAGEN bin/dbt2-datagen)
SET(DBT2_DRIVER bin/dbt2-driver)
SET(DBT2_TXN_TEST bin/dbt2-transaction-test)

#
# Check for large file support by using 'getconf'.
#

FIND_PROGRAM(GETCONF getconf)
IF (GETCONF)
    SET(GETCONF_FOUND TRUE)
ENDIF (GETCONF)

IF (GETCONF_FOUND)
    EXECUTE_PROCESS(COMMAND ${GETCONF} LFS_CFLAGS COMMAND ${GETCONF}
        LFS64_CFLAGS OUTPUT_VARIABLE LFS_CFLAGS
        OUTPUT_STRIP_TRAILING_WHITESPACE)
ENDIF (GETCONF_FOUND)

#
# Checking for PostgreSQL specific configuration.
#

FIND_PROGRAM(PG_CONFIG pg_config)

IF (PG_CONFIG)
    SET(PG_CONFIG_FOUND TRUE)
ENDIF (PG_CONFIG)

IF (PG_CONFIG_FOUND)
    MESSAGE(STATUS "Found pg_config for PostgreSQL: ${PG_CONFIG}")
    SET(LIBPQ_SOURCE src/libpq/dbc_common.c src/libpq/dbc_new_order.c
        src/libpq/dbc_payment.c src/libpq/dbc_order_status.c
        src/libpq/dbc_delivery.c src/libpq/dbc_stock_level.c
        src/libpq/dbc_integrity.c)
    EXECUTE_PROCESS(COMMAND ${PG_CONFIG} --includedir OUTPUT_VARIABLE
        PG_INCLUDEDIR OUTPUT_STRIP_TRAILING_WHITESPACE)
    SET(LIBPQ_FLAGS "-DLIBPQ -I${PG_INCLUDEDIR}")
    SET(PGSQL_SCRIPTS bin/pgsql/dbt2-pgsql-analyze-stats
        bin/pgsql/dbt2-pgsql-build-db bin/pgsql/dbt2-pgsql-check-db
        bin/pgsql/dbt2-pgsql-create-db bin/pgsql/dbt2-pgsql-create-indexes
        bin/pgsql/dbt2-pgsql-create-tables bin/pgsql/dbt2-pgsql-db-stat
        bin/pgsql/dbt2-pgsql-drop-db bin/pgsql/dbt2-pgsql-drop-tables
        bin/pgsql/dbt2-pgsql-load-db bin/pgsql/dbt2-pgsql-load-stored-procs
        bin/pgsql/dbt2-pgsql-plans bin/pgsql/dbt2-pgsql-start-db
        bin/pgsql/dbt2-pgsql-stop-db)
ENDIF (PG_CONFIG_FOUND)

#
# Defining commong groups of source files.
#

SET(COMMON_SOURCE src/common.c src/logging.c src/_socket.c 
    src/client_interface.c src/transaction_data.c src/_semaphore.c)

#
# Files to build.
#

ADD_EXECUTABLE(${DBT2_CLIENT} src/client.c src/transaction_queue.c
    src/listener.c src/db_threadpool.c src/db.c ${COMMON_SOURCE}
    ${LIBPQ_SOURCE})
ADD_EXECUTABLE(${DBT2_DATAGEN} src/datagen.c src/common.c)
ADD_EXECUTABLE(${DBT2_DRIVER} ${COMMON_SOURCE} src/driver_main.c src/driver.c
    src/input_data_generator.c)
ADD_EXECUTABLE(${DBT2_TXN_TEST} src/transaction_test.c src/db.c
    src/input_data_generator.c ${COMMON_SOURCE} ${LIBPQ_SOURCE})

#      
# Set compiler flags.
#

SET_SOURCE_FILES_PROPERTIES(src/datagen.c ${COMMON_SOURCE} src/driver_main.c
	src/driver.c src/input_data_generator.c src/transaction_queue.c
    src/listener.c src/input_data_generator.c
    COMPILE_FLAGS "-g -Wall -Isrc/include ${LFS_CFLAGS}")
SET_SOURCE_FILES_PROPERTIES(src/db_threadpool.c src/db.c src/client.c
    src/transaction_test.c
    COMPILE_FLAGS "-g -Wall -Isrc/include ${LIBPQ_FLAGS} ${LFS_CFLAGS}")
SET_SOURCE_FILES_PROPERTIES(${LIBPQ_SOURCE}
    COMPILE_FLAGS "-g -Wall -Isrc/include ${LFS_CFLAGS}")

#
# Set linker flags.
#

SET_TARGET_PROPERTIES(${DBT2_CLIENT} PROPERTIES LINK_FLAGS "-lm -lpq")
SET_TARGET_PROPERTIES(${DBT2_DATAGEN} PROPERTIES LINK_FLAGS -lm)
SET_TARGET_PROPERTIES(${DBT2_DRIVER} PROPERTIES LINK_FLAGS "-lm -lpthread")
SET_TARGET_PROPERTIES(${DBT2_TXN_TEST} PROPERTIES LINK_FLAGS "-lm -lpq")

#
# Install rules.
#

INSTALL(PROGRAMS ${DBT2_CLIENT} ${DBT2_DATAGEN} ${DBT2_DRIVER} ${DBT2_TXN_TEST}
    bin/dbt2-generate-report bin/dbt2-get-os-info bin/dbt2-post-process
    bin/dbt2-run-workload bin/dbt2-sysstats
    ${PGSQL_SCRIPTS} DESTINATION "/bin")
