CMAKE_MINIMUM_REQUIRED(VERSION 2.4)
PROJECT(dbt2 C)

INCLUDE(CheckLibraryExists)

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

# Check for libraries.

# Solaris links nanosleep via librt.  Linux does not.  Check if the library
# exists and set the linker flags below if it does.  If we don't fint this
# library, then assume we don't need to link anything.
CHECK_LIBRARY_EXISTS(rt nanosleep "" HAVE_NANOSLEEP)
IF (HAVE_NANOSLEEP)
    SET (NEED_LIBRT TRUE)
ENDIF (HAVE_NANOSLEEP)

# FIXME: If one of these passes, then hopefully the rest will pass.  libsocket
# is required on Solaris.
CHECK_LIBRARY_EXISTS(socket bind "" HAVE_BIND)
CHECK_LIBRARY_EXISTS(socket recv "" HAVE_RECV)
CHECK_LIBRARY_EXISTS(socket send "" HAVE_SEND)
CHECK_LIBRARY_EXISTS(socket accept "" HAVE_ACCEPT)
CHECK_LIBRARY_EXISTS(socket listen "" HAVE_LISTEN)
CHECK_LIBRARY_EXISTS(socket getprotobyname "" HAVE_GETPROTOBYNAME)
CHECK_LIBRARY_EXISTS(socket socket "" HAVE_SOCKET)
CHECK_LIBRARY_EXISTS(socket setsockopt "" HAVE_SETSOCKOPT)
CHECK_LIBRARY_EXISTS(socket connect "" HAVE_CONNECT)
IF (HAVE_BIND AND HAVE_RECV AND HAVE_SEND AND HAVE_ACCEPT AND
    HAVE_LISTEN AND HAVE_GETPROTOBYNAME AND HAVE_SOCKET AND
    HAVE_SETSOCKOPT AND HAVE_CONNECT)
    SET (NEED_LIBSOCKET TRUE)
ENDIF (HAVE_BIND AND HAVE_RECV AND HAVE_SEND AND HAVE_ACCEPT AND
       HAVE_LISTEN AND HAVE_GETPROTOBYNAME AND HAVE_SOCKET AND
       HAVE_SETSOCKOPT AND HAVE_CONNECT)

# FIXME: If one of these passes, then hopefully the rest will pass.  libnsl
# is required on Solaris.
CHECK_LIBRARY_EXISTS(nsl gethostbyaddr "" HAVE_GETHOSTBYADDR)
CHECK_LIBRARY_EXISTS(nsl gethostbyname "" HAVE_GETHOSTBYNAME)
CHECK_LIBRARY_EXISTS(nsl inet_addr "" HAVE_INET_ADDR)
IF (HAVE_GETHOSTBYADDR AND HAVE_GETHOSTBYNAME AND HAVE_INET_ADDR)
    SET (NEED_LIBNSL TRUE)
ENDIF (HAVE_GETHOSTBYADDR AND HAVE_GETHOSTBYNAME AND HAVE_INET_ADDR)

IF (DBMS STREQUAL "pgsql")
    #
    # Create PostgreSQL specific configuration.
    #

    FIND_PROGRAM(PG_CONFIG pg_config)

    IF (PG_CONFIG)
       SET(PG_CONFIG_FOUND TRUE)
    ENDIF (PG_CONFIG)

    IF (PG_CONFIG_FOUND)
        MESSAGE(STATUS "Found pg_config for PostgreSQL: ${PG_CONFIG}")
        SET(DB_SOURCE src/libpq/dbc_common.c src/libpq/dbc_new_order.c
                src/libpq/dbc_payment.c src/libpq/dbc_order_status.c
                src/libpq/dbc_delivery.c src/libpq/dbc_stock_level.c
                src/libpq/dbc_integrity.c)
        EXECUTE_PROCESS(COMMAND ${PG_CONFIG} --includedir OUTPUT_VARIABLE
                PG_INCLUDEDIR OUTPUT_STRIP_TRAILING_WHITESPACE)
        EXECUTE_PROCESS(COMMAND ${PG_CONFIG} --libdir OUTPUT_VARIABLE
                PG_LIBDIR OUTPUT_STRIP_TRAILING_WHITESPACE)
        SET(DBCFLAGS "-DLIBPQ -I${PG_INCLUDEDIR}")
        SET(DB_LIBS "-L${PG_LIBDIR} -lpq")
        SET(DB_SCRIPTS bin/pgsql/dbt2-pgsql-build-db
                bin/pgsql/dbt2-pgsql-check-db
                bin/pgsql/dbt2-pgsql-create-db
                bin/pgsql/dbt2-pgsql-create-indexes
                bin/pgsql/dbt2-pgsql-create-tables bin/pgsql/dbt2-pgsql-db-stat
                bin/pgsql/dbt2-pgsql-drop-db bin/pgsql/dbt2-pgsql-drop-tables
                bin/pgsql/dbt2-pgsql-load-db
                bin/pgsql/dbt2-pgsql-load-stored-procs
                bin/pgsql/dbt2-pgsql-plans bin/pgsql/dbt2-pgsql-start-db
                bin/pgsql/dbt2-pgsql-stop-db)
    ELSE (NOT DEFINED PKG_CONFIG_FOUND)
        MESSAGE(FATAL_ERROR "pg_config not found in path")
    ENDIF (PG_CONFIG_FOUND)
ELSEIF (DBMS STREQUAL "sqlite")
    FIND_PROGRAM(PKG_CONFIG pkg-config)

    IF (PKG_CONFIG)
       SET(PKG_CONFIG_FOUND TRUE)
    ENDIF (PKG_CONFIG)

    IF (PKG_CONFIG_FOUND)
        SET(DB_SOURCE src/sqlite/dbc_common.c src/nonsp/dbc_new_order.c
                src/nonsp/dbc_payment.c src/nonsp/dbc_order_status.c
                src/nonsp/dbc_delivery.c src/nonsp/dbc_stock_level.c
                src/nonsp/dbc_integrity.c src/nonsp/dbc_nonsp.c)
        EXECUTE_PROCESS(COMMAND ${PKG_CONFIG} --cflags sqlite3 OUTPUT_VARIABLE
                SQLITE_CFLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)
        EXECUTE_PROCESS(COMMAND ${PKG_CONFIG} --libs sqlite3 OUTPUT_VARIABLE
                DB_LIBS OUTPUT_STRIP_TRAILING_WHITESPACE)
        SET(DBCFLAGS "-DLIBSQLITE ${SQLITE_CFLAGS}")
        SET(DB_SCRIPTS bin/sqlite/dbt2-sqlite-build-db
                bin/sqlite/dbt2-sqlite-plans bin/sqlite/dbt2-sqlite-start-db
                bin/sqlite/dbt2-sqlite-stop-db)
    ENDIF (PKG_CONFIG_FOUND)
ELSEIF (DBMS STREQUAL "mysql")
    #
    # Create MySQL specific configuration.
    #

    FIND_PROGRAM(MYSQL_CONFIG mysql_config)

    IF (MYSQL_CONFIG)
        SET(MYSQL_CONFIG_FOUND TRUE)
    ENDIF (MYSQL_CONFIG)

    IF(NOT DEFINED MYSQL)
        SET(MYSQL /usr/bin/sql )
    ENDIF(NOT DEFINED MYSQL)

    IF (MYSQL_CONFIG_FOUND)
        MESSAGE(STATUS "Found mysql_config for MySQL: ${MYSQL_CONFIG}")
        SET(DB_SOURCE src/mysql/dbc_common.c src/mysql/dbc_new_order.c
                src/mysql/dbc_payment.c src/mysql/dbc_order_status.c
                src/mysql/dbc_delivery.c src/mysql/dbc_stock_level.c
                src/mysql/dbc_integrity.c)
        EXECUTE_PROCESS(COMMAND ${MYSQL_CONFIG} --include OUTPUT_VARIABLE
                MYSQL_INCLUDEDIR OUTPUT_STRIP_TRAILING_WHITESPACE)
        EXECUTE_PROCESS(COMMAND ${MYSQL_CONFIG} --libs OUTPUT_VARIABLE
                DB_LIBS OUTPUT_STRIP_TRAILING_WHITESPACE)
        SET(DBCFLAGS "-DLIBMYSQL ${MYSQL_INCLUDEDIR}")
        SET(DB_SCRIPTS
                bin/mysql/dbt2-mysql-build-db
                bin/mysql/dbt2-mysql-load-stored-procs
                bin/mysql/dbt2-mysql-plans
                bin/mysql/dbt2-mysql-start-db
                bin/mysql/dbt2-mysql-stop-db)
    ENDIF (MYSQL_CONFIG_FOUND)
ELSEIF (DBMS STREQUAL "drizzle")
    #
    # Create Drizzle specific configuration.
    #

    SET(DB_SOURCE src/drizzle/dbc_common.c
                       src/nonsp/dbc_new_order.c
                       src/nonsp/dbc_payment.c
                       src/nonsp/dbc_order_status.c
                       src/nonsp/dbc_delivery.c
                       src/nonsp/dbc_stock_level.c
                       src/nonsp/dbc_nonsp.c
                       src/nonsp/dbc_integrity.c)

    IF(NOT DEFINED DRIZZLE_INCLUDEDIR)
        SET(DRIZZLE_INCLUDEDIR /usr/local/include )
    ENDIF(NOT DEFINED DRIZZLE_INCLUDEDIR)

    IF(NOT DEFINED DRIZZLE_LIBS)
        SET(DB_LIBS "-ldrizzle  -lpthread")
    ENDIF(NOT DEFINED DRIZZLE_LIBS)

    SET(DBCFLAGS "-DLIBDRIZZLE -I${DRIZZLE_INCLUDEDIR}")

    MESSAGE(STATUS "Drizzle configuration build flags :  ${DBCFLAGS}")
    MESSAGE(STATUS "Drizzle configuration library flags:  ${DB_LIBS}")

    SET(DB_SCRIPTS bin/drizzle/dbt2-drizzle-admin
            bin/drizzle/dbt2-drizzle-load-db
            bin/drizzle/dbt2-drizzle-start-db
            bin/drizzle/dbt2-drizzle-stop-db)
ELSE (NOT DEFINED DBMS)
    MESSAGE(FATAL_ERROR "Need to specify -DDBMS=drizzle|mysql|pgsql|sqlite")

ENDIF (DBMS STREQUAL "pgsql")

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
        ${DB_SOURCE})
IF (NEED_LIBRT)
    TARGET_LINK_LIBRARIES(${DBT2_CLIENT} rt)
ENDIF (NEED_LIBRT)
IF (NEED_LIBSOCKET)
    TARGET_LINK_LIBRARIES(${DBT2_CLIENT} socket)
ENDIF (NEED_LIBSOCKET)
IF (NEED_LIBNSL)
    TARGET_LINK_LIBRARIES(${DBT2_CLIENT} nsl)
ENDIF (NEED_LIBNSL)

ADD_EXECUTABLE(${DBT2_DATAGEN} src/datagen.c src/common.c)

ADD_EXECUTABLE(${DBT2_DRIVER} ${COMMON_SOURCE} src/driver_main.c src/driver.c
        src/input_data_generator.c)
IF (NEED_LIBRT)
    TARGET_LINK_LIBRARIES(${DBT2_DRIVER} rt)
ENDIF (NEED_LIBRT)
IF (NEED_LIBSOCKET)
    TARGET_LINK_LIBRARIES(${DBT2_DRIVER} socket)
ENDIF (NEED_LIBSOCKET)
IF (NEED_LIBNSL)
    TARGET_LINK_LIBRARIES(${DBT2_DRIVER} nsl)
ENDIF (NEED_LIBNSL)

ADD_EXECUTABLE(${DBT2_TXN_TEST} src/transaction_test.c src/db.c
        src/input_data_generator.c ${COMMON_SOURCE} ${DB_SOURCE})
IF (NEED_LIBSOCKET)
    TARGET_LINK_LIBRARIES(${DBT2_TXN_TEST} socket)
ENDIF (NEED_LIBSOCKET)
IF (NEED_LIBNSL)
    TARGET_LINK_LIBRARIES(${DBT2_TXN_TEST} nsl)
ENDIF (NEED_LIBNSL)

#
# Set compiler flags.
#

SET(CMAKE_C_FLAGS "-g -Wall -Isrc/include ${LFS_CFLAGS}")

SET_SOURCE_FILES_PROPERTIES(src/db_threadpool.c src/db.c src/client.c
        src/transaction_test.c src/common.c COMPILE_FLAGS "${DBCFLAGS}")
SET_SOURCE_FILES_PROPERTIES(${DB_SOURCE} COMPILE_FLAGS "${DBCFLAGS}")

#
# Set linker flags.
#

SET_TARGET_PROPERTIES(${DBT2_CLIENT} PROPERTIES LINK_FLAGS
        "-lm ${DB_LIBS} -lpthread")
SET_TARGET_PROPERTIES(${DBT2_DATAGEN} PROPERTIES LINK_FLAGS -lm)
SET_TARGET_PROPERTIES(${DBT2_DRIVER} PROPERTIES LINK_FLAGS "-lm -lpthread")
SET_TARGET_PROPERTIES(${DBT2_TXN_TEST} PROPERTIES LINK_FLAGS "-lm ${DB_LIBS}")

#
# Install rules.
#

INSTALL(PROGRAMS ${DBT2_CLIENT} ${DBT2_DATAGEN} ${DBT2_DRIVER} ${DBT2_TXN_TEST}
        bin/dbt2-generate-report bin/dbt2-get-os-info bin/dbt2-post-process
        bin/dbt2-run-workload bin/dbt2-sysstats bin/dbt2-plot-transaction-rate
        ${DB_SCRIPTS} DESTINATION "/bin")
