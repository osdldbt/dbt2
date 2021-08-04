add_subdirectory (scripts)

#
# Defining binaries so to minimized typos.
#

set (CLIENT ${PROJECT_NAME}-client)
set (DATAGEN ${PROJECT_NAME}-datagen)
set (DRIVER ${PROJECT_NAME}-driver)
set (TXN_TEST ${PROJECT_NAME}-transaction-test)

#
# Check for large file support by using 'getconf'.
#

find_program (GETCONF getconf)
if (GETCONF)
    set (GETCONF_FOUND TRUE)
endif (GETCONF)

if (GETCONF_FOUND)
    execute_process (COMMAND ${GETCONF} LFS_CFLAGS COMMAND ${GETCONF}
            LFS64_CFLAGS OUTPUT_VARIABLE LFS_CFLAGS
            OUTPUT_STRIP_TRAILING_WHITESPACE)
endif (GETCONF_FOUND)

# Check for libraries.

# Solaris links nanosleep via librt.  Linux does not.  Check if the library
# exists and set the linker flags below if it does.  If we don't find this
# library, then assume we don't need to link anything.
check_library_exists (rt nanosleep "" HAVE_NANOSLEEP)
if (HAVE_NANOSLEEP)
    set (NEED_LIBRT TRUE)
endif (HAVE_NANOSLEEP)

# FIXME: If one of these passes, then hopefully the rest will pass.  libsocket
# is required on Solaris.
check_library_exists (socket bind "" HAVE_BIND)
check_library_exists (socket recv "" HAVE_RECV)
check_library_exists (socket send "" HAVE_SEND)
check_library_exists (socket accept "" HAVE_ACCEPT)
check_library_exists (socket listen "" HAVE_LISTEN)
check_library_exists (socket getprotobyname "" HAVE_GETPROTOBYNAME)
check_library_exists (socket socket "" HAVE_SOCKET)
check_library_exists (socket setsockopt "" HAVE_SETSOCKOPT)
check_library_exists (socket connect "" HAVE_CONNECT)
if (HAVE_BIND AND HAVE_RECV AND HAVE_SEND AND HAVE_ACCEPT AND
    HAVE_LISTEN AND HAVE_GETPROTOBYNAME AND HAVE_SOCKET AND
    HAVE_SETSOCKOPT AND HAVE_CONNECT)
    set (NEED_LIBSOCKET TRUE)
endif (HAVE_BIND AND HAVE_RECV AND HAVE_SEND AND HAVE_ACCEPT AND
       HAVE_LISTEN AND HAVE_GETPROTOBYNAME AND HAVE_SOCKET AND
       HAVE_SETSOCKOPT AND HAVE_CONNECT)

# FIXME: If one of these passes, then hopefully the rest will pass.  libnsl
# is required on Solaris.
check_library_exists (nsl gethostbyaddr "" HAVE_GETHOSTBYADDR)
check_library_exists (nsl gethostbyname "" HAVE_GETHOSTBYNAME)
check_library_exists (nsl inet_addr "" HAVE_INET_ADDR)
if (HAVE_GETHOSTBYADDR AND HAVE_GETHOSTBYNAME AND HAVE_INET_ADDR)
    set (NEED_LIBNSL TRUE)
endif (HAVE_GETHOSTBYADDR AND HAVE_GETHOSTBYNAME AND HAVE_INET_ADDR)

if (DBMS STREQUAL "pgsql")
    #
    # Create PostgreSQL specific configuration.
    #

    find_program (PG_CONFIG pg_config)

    if (PG_CONFIG)
       set (PG_CONFIG_FOUND TRUE)
    endif (PG_CONFIG)

    if (PG_CONFIG_FOUND)
        message (STATUS "Found pg_config for PostgreSQL: ${PG_CONFIG}")
        set (DB_SOURCE
                libpq/dbc_common.c
                libpq/dbc_delivery.c
                libpq/dbc_integrity.c
                libpq/dbc_new_order.c
                libpq/dbc_order_status.c
                libpq/dbc_payment.c
                libpq/dbc_stock_level.c
        )
        execute_process (COMMAND ${PG_CONFIG} --includedir OUTPUT_VARIABLE
                PG_INCLUDEDIR OUTPUT_STRIP_TRAILING_WHITESPACE)
        execute_process (COMMAND ${PG_CONFIG} --libdir OUTPUT_VARIABLE
                PG_LIBDIR OUTPUT_STRIP_TRAILING_WHITESPACE)
        set (DBCFLAGS "-DLIBPQ -I${PG_INCLUDEDIR}")
        set (DB_LIBS "-L${PG_LIBDIR}")
    else (NOT DEFINED PKG_CONFIG_FOUND)
        message (FATAL_ERROR "pg_config not found in path")
    endif (PG_CONFIG_FOUND)
elseif (DBMS STREQUAL "sqlite")
    find_program (PKG_CONFIG pkg-config)

    if (PKG_CONFIG)
       set (PKG_CONFIG_FOUND TRUE)
    endif (PKG_CONFIG)

    if (PKG_CONFIG_FOUND)
        set (DB_SOURCE src/sqlite/dbc_common.c src/nonsp/dbc_new_order.c
                src/nonsp/dbc_payment.c src/nonsp/dbc_order_status.c
                src/nonsp/dbc_delivery.c src/nonsp/dbc_stock_level.c
                src/nonsp/dbc_integrity.c src/nonsp/dbc_nonsp.c)
        execute_process (COMMAND ${PKG_CONFIG} --cflags sqlite3 OUTPUT_VARIABLE
                SQLITE_CFLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)
        execute_process (COMMAND ${PKG_CONFIG} --libs sqlite3 OUTPUT_VARIABLE
                DB_LIBS OUTPUT_STRIP_TRAILING_WHITESPACE)
        set (DBCFLAGS "-DLIBSQLITE ${SQLITE_CFLAGS}")
    endif (PKG_CONFIG_FOUND)
elseif (DBMS STREQUAL "mysql")
    #
    # Create MySQL specific configuration.
    #

    find_program (MYSQL_CONFIG mysql_config)

    if (MYSQL_CONFIG)
        set (MYSQL_CONFIG_FOUND TRUE)
    endif (MYSQL_CONFIG)

    if (NOT DEFINED MYSQL)
        set (MYSQL /usr/bin/sql )
    endif (NOT DEFINED MYSQL)

    if (MYSQL_CONFIG_FOUND)
        message (STATUS "Found mysql_config for MySQL: ${MYSQL_CONFIG}")
        set (DB_SOURCE src/mysql/dbc_common.c src/mysql/dbc_new_order.c
                src/mysql/dbc_payment.c src/mysql/dbc_order_status.c
                src/mysql/dbc_delivery.c src/mysql/dbc_stock_level.c
                src/mysql/dbc_integrity.c)
        execute_process (COMMAND ${MYSQL_CONFIG} --include OUTPUT_VARIABLE
                MYSQL_INCLUDEDIR OUTPUT_STRIP_TRAILING_WHITESPACE)
        execute_process (COMMAND ${MYSQL_CONFIG} --libs OUTPUT_VARIABLE
                DB_LIBS OUTPUT_STRIP_TRAILING_WHITESPACE)
        set (DBCFLAGS "-DLIBMYSQL ${MYSQL_INCLUDEDIR}")
    endif (MYSQL_CONFIG_FOUND)
elseif (DBMS STREQUAL "drizzle")
    #
    # Create Drizzle specific configuration.
    #

    set (DB_SOURCE src/drizzle/dbc_common.c
                       src/nonsp/dbc_new_order.c
                       src/nonsp/dbc_payment.c
                       src/nonsp/dbc_order_status.c
                       src/nonsp/dbc_delivery.c
                       src/nonsp/dbc_stock_level.c
                       src/nonsp/dbc_nonsp.c
                       src/nonsp/dbc_integrity.c)

    if (NOT DEFINED DRIZZLE_INCLUDEDIR)
        set (DRIZZLE_INCLUDEDIR /usr/local/include )
    endif (NOT DEFINED DRIZZLE_INCLUDEDIR)

    if (NOT DEFINED DRIZZLE_LIBS)
        set (DB_LIBS "-ldrizzle")
    endif (NOT DEFINED DRIZZLE_LIBS)

    set (DBCFLAGS "-DLIBDRIZZLE -I${DRIZZLE_INCLUDEDIR}")

    message (STATUS "Drizzle configuration build flags :  ${DBCFLAGS}")
    message (STATUS "Drizzle configuration library flags:  ${DB_LIBS}")
elseif (DBMS STREQUAL "cpack")
    message (STATUS "prepared for packaging")
else (NOT DEFINED DBMS)
    message (FATAL_ERROR "Need to specify -DDBMS=drizzle|mysql|pgsql|sqlite")
endif (DBMS STREQUAL "pgsql")

#
# Defining common groups of source files.
#

set (COMMON_SOURCE
     _semaphore.c
     _socket.c
     client_interface.c
     common.c
     logging.c
     transaction_data.c
)

set (PCG_SOURCE
    pcg/extras/entropy.c
    pcg/pcg-advance-8.c
    pcg/pcg-advance-16.c
    pcg/pcg-advance-32.c
    pcg/pcg-advance-64.c
    pcg/pcg-advance-128.c
    pcg/pcg-global-32.c
    pcg/pcg-global-64.c
    pcg/pcg-output-8.c
    pcg/pcg-output-16.c
    pcg/pcg-output-32.c
    pcg/pcg-output-64.c
    pcg/pcg-output-128.c
    pcg/pcg-rngs-8.c
    pcg/pcg-rngs-16.c
    pcg/pcg-rngs-32.c
    pcg/pcg-rngs-64.c
    pcg/pcg-rngs-128.c
    pcg/pcg-rngs-128.c
)

#
# Files to build.
#

add_executable (${CLIENT}
                ${COMMON_SOURCE}
                ${DB_SOURCE}
                ${PCG_SOURCE}
                client.c
                db.c
                db_threadpool.c
                listener.c
                transaction_queue.c
)
target_link_libraries (${CLIENT} m pthread)
if (NEED_LIBRT)
    target_link_libraries (${CLIENT} rt)
endif (NEED_LIBRT)
if (NEED_LIBSOCKET)
    target_link_libraries (${CLIENT} socket)
endif (NEED_LIBSOCKET)
if (NEED_LIBNSL)
    target_link_libraries (${CLIENT} nsl)
endif (NEED_LIBNSL)
if (DBMS STREQUAL "pgsql")
    target_link_libraries (${CLIENT} pq)
endif (DBMS STREQUAL "pgsql")

add_executable (${DATAGEN} ${PCG_SOURCE} datagen.c common.c)
target_link_libraries (${DATAGEN} m)

add_executable (${DRIVER}
                ${COMMON_SOURCE}
                ${PCG_SOURCE}
                driver.c
                driver_main.c
                input_data_generator.c
)
target_link_libraries (${DRIVER} m pthread)
if (NEED_LIBRT)
    target_link_libraries (${DRIVER} rt)
endif (NEED_LIBRT)
if (NEED_LIBSOCKET)
    target_link_libraries (${DRIVER} socket)
endif (NEED_LIBSOCKET)
if (NEED_LIBNSL)
    target_link_libraries (${DRIVER} nsl)
endif (NEED_LIBNSL)

add_executable(${TXN_TEST}
               ${COMMON_SOURCE}
               ${DB_SOURCE}
               ${PCG_SOURCE}
               db.c
               input_data_generator.c
               transaction_test.c
)
target_link_libraries (${TXN_TEST} m)
if (NEED_LIBSOCKET)
    target_link_libraries (${TXN_TEST} socket)
endif (NEED_LIBSOCKET)
if (NEED_LIBNSL)
    target_link_libraries (${TXN_TEST} nsl)
endif (NEED_LIBNSL)
if (DBMS STREQUAL "pgsql")
    target_link_libraries (${TXN_TEST} pq)
endif (DBMS STREQUAL "pgsql")

#
# Set compiler flags.
#

set (CMAKE_INCLUDE_CURRENT_DIR ON)
set (CMAKE_C_FLAGS_DEBUG "-g -Wall")

set_source_files_properties (
        ${PCG_SOURCE}
        COMPILE_FLAGS "-I${CMAKE_HOME_DIRECTORY}/src/pcg -I${CMAKE_HOME_DIRECTORY}/src/pcg/extras"
)
set_source_files_properties (
        datagen.c
        COMPILE_FLAGS "-I${CMAKE_HOME_DIRECTORY}/src/include ${LFS_CFLAGS} -I${CMAKE_HOME_DIRECTORY}/src/pcg -I${CMAKE_HOME_DIRECTORY}/src/pcg/extras"
)
set_source_files_properties (
        client_interface.c
        common.c
        driver.c
        driver_main.c
        input_data_generator.c
        listener.c
        logging.c
        transaction_data.c
        transaction_queue.c
        COMPILE_FLAGS "-I${CMAKE_HOME_DIRECTORY}/src/include -I${CMAKE_HOME_DIRECTORY}/src/pcg -I${CMAKE_HOME_DIRECTORY}/src/pcg/extras"
)
set_source_files_properties (
        _semaphore.c
        _socket.c
        COMPILE_FLAGS "-I${CMAKE_HOME_DIRECTORY}/src/include ${LFS_CFLAGS}")
set_source_files_properties (
        ${DB_SOURCE}
        client.c
        db.c
        db_threadpool.c
        transaction_test.c
        COMPILE_FLAGS "${DBCFLAGS} -I${CMAKE_HOME_DIRECTORY}/src/include -I${CMAKE_HOME_DIRECTORY}/src/pcg -I${CMAKE_HOME_DIRECTORY}/src/pcg/extras"
)

#
# Set linker flags.
#

set_target_properties (${CLIENT} PROPERTIES LINK_FLAGS "${DB_LIBS}")
set_target_properties (${TXN_TEST} PROPERTIES LINK_FLAGS "${DB_LIBS}")

#
# Install rules.
#

install (PROGRAMS
         ${CMAKE_BINARY_DIR}/src/${CLIENT}
         ${CMAKE_BINARY_DIR}/src/${DATAGEN}
         ${CMAKE_BINARY_DIR}/src/${DRIVER}
         ${CMAKE_BINARY_DIR}/src/${TXN_TEST}
         DESTINATION "bin"
)
