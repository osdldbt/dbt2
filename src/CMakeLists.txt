#
# Defining binaries so to minimized typos.
#

set (CLIENT ${PROJECT_NAME}-client)
set (CLIENT2 ${PROJECT_NAME}-client2)
set (DATAGEN ${PROJECT_NAME}-datagen)
set (DRIVER ${PROJECT_NAME}-driver)
set (DRIVER2 ${PROJECT_NAME}-driver2)
set (DRIVER3 ${PROJECT_NAME}-driver3)
set (RAND ${PROJECT_NAME}-rand)
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

# Determine what DBMS connectivity libraries to build against.

find_package(PkgConfig REQUIRED)

# libpq covers CockroachDB, PostgreSQL and YugabyteDB

pkg_check_modules(LIBPQ libpq)
if (LIBPQ_FOUND)
    set (DB_SOURCE
            ${DB_SOURCE}
            libpq/dbc_common.c
            libpq/dbc_delivery.c
            libpq/dbc_integrity.c
            libpq/dbc_new_order.c
            libpq/dbc_order_status.c
            libpq/dbc_payment.c
            libpq/dbc_stock_level.c
    )
    if ("${LIBPQ_VERSION}" VERSION_GREATER_EQUAL "11.0")
        set (DB_SOURCE
                ${DB_SOURCE}
                cockroach/dbc_common.c
                cockroach/dbc_delivery.c
                cockroach/dbc_integrity.c
                cockroach/dbc_new_order.c
                cockroach/dbc_order_status.c
                cockroach/dbc_payment.c
                cockroach/dbc_stock_level.c
        )
    endif ("${LIBPQ_VERSION}" VERSION_GREATER_EQUAL "11.0")
    execute_process (COMMAND pg_config --includedir-server
            OUTPUT_VARIABLE PG_INCLUDEDIR_SERVER
            OUTPUT_STRIP_TRAILING_WHITESPACE)
    set (DBCFLAGS "${DBCFLAGS} -DHAVE_LIBPQ ${LIBPQ_CFLAGS} -I${PG_INCLUDEDIR_SERVER}")
endif (LIBPQ_FOUND)

#
# Check for mysqlclient
#

pkg_check_modules(MYSQLCLIENT mysqlclient)
if (MYSQLCLIENT_FOUND)
    #
    # Create MySQL specific configuration.
    #

    set (DB_SOURCE
            ${DB_SOURCE}
            mysql/dbc_common.c
            mysql/dbc_delivery.c
            mysql/dbc_integrity.c
            mysql/dbc_new_order.c
            mysql/dbc_order_status.c
            mysql/dbc_payment.c
            mysql/dbc_stock_level.c
    )
    set (DBCFLAGS "${DBCFLAGS} -DHAVE_MYSQL ${MYSQLCLIENT_CFLAGS}")
endif (MYSQLCLIENT_FOUND)

#
# Check for unixODBC
#

pkg_check_modules(ODBC odbc)
if (ODBC_FOUND)
    set (DB_SOURCE
        ${DB_SOURCE}
        odbc/dbc_common.c
        odbc/dbc_new_order.c
        odbc/dbc_payment.c
        odbc/dbc_order_status.c
        odbc/dbc_delivery.c
        odbc/dbc_stock_level.c
        odbc/dbc_integrity.c
    )
    set (DBCFLAGS "${DBCFLAGS} -DHAVE_ODBC ${ODBC_CFLAGS}")
endif (ODBC_FOUND)

#
# Check for sqlite3
#

pkg_check_modules(SQLITE3 sqlite3)
if (SQLITE3_FOUND)
    set (DB_SOURCE
        ${DB_SOURCE}
        sqlite/dbc_common.c
        nonsp/dbc_new_order.c
        nonsp/dbc_payment.c
        nonsp/dbc_order_status.c
        nonsp/dbc_delivery.c
        nonsp/dbc_stock_level.c
        nonsp/dbc_integrity.c
        nonsp/dbc_nonsp.c
    )
    set (DBCFLAGS "${DBCFLAGS} -DHAVE_SQLITE3 ${SQLITE3_CFLAGS}")
endif (SQLITE3_FOUND)

#
# Defining common groups of source files.
#

set (COMMON_SOURCE
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

add_executable (${RAND} ${PCG_SOURCE} rand.c)

add_executable (${CLIENT}
                ${COMMON_SOURCE}
                ${DB_SOURCE}
                ${PCG_SOURCE}
                _semaphore.c
                client.c
                client_common.c
                client_main.c
                db.c
                db_threadpool.c
                listener.c
                transaction_queue.c
)
target_compile_definitions(${CLIENT} PRIVATE -DCLIENT1)
add_executable (${CLIENT2}
                ${COMMON_SOURCE}
                ${DB_SOURCE}
                ${PCG_SOURCE}
                client_common.c
                client_main.c
                client2.c
                db.c
)
target_compile_definitions(${CLIENT2} PRIVATE -DCLIENT2)
target_link_libraries (${CLIENT} m pthread)
target_link_libraries (${CLIENT2} m ev rt)
if (NEED_LIBRT)
    target_link_libraries (${CLIENT} rt)
endif (NEED_LIBRT)
if (NEED_LIBSOCKET)
    target_link_libraries (${CLIENT} socket)
    target_link_libraries (${CLIENT2} socket)
endif (NEED_LIBSOCKET)
if (NEED_LIBNSL)
    target_link_libraries (${CLIENT} nsl)
    target_link_libraries (${CLIENT2} nsl)
endif (NEED_LIBNSL)

add_executable (${DATAGEN} ${PCG_SOURCE} datagen.c common.c)
target_link_libraries (${DATAGEN} m)

add_executable (${DRIVER}
                ${COMMON_SOURCE}
                ${PCG_SOURCE}
                driver.c
                driver_common.c
                driver_main.c
                input_data_generator.c
)
target_compile_definitions(${DRIVER} PRIVATE -DDRIVER1)
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

add_executable (${DRIVER2}
                ${COMMON_SOURCE}
                ${PCG_SOURCE}
                driver2.c
                driver_common.c
                driver_main.c
                input_data_generator.c
)
target_compile_definitions(${DRIVER2} PRIVATE -DDRIVER2)
target_link_libraries (${DRIVER2} m ev)
if (NEED_LIBRT)
    target_link_libraries (${DRIVER2} rt)
endif (NEED_LIBRT)
if (NEED_LIBSOCKET)
    target_link_libraries (${DRIVER2} socket)
endif (NEED_LIBSOCKET)
if (NEED_LIBNSL)
    target_link_libraries (${DRIVER2} nsl)
endif (NEED_LIBNSL)

add_executable (${DRIVER3}
                ${COMMON_SOURCE}
                ${DB_SOURCE}
                ${PCG_SOURCE}
                client_common.c
                db.c
                driver_common.c
                driver_main.c
                driver3.c
                input_data_generator.c
)
target_compile_definitions(${DRIVER3} PRIVATE -DDRIVER3)
target_link_libraries (${DRIVER3} m ev)
if (NEED_LIBRT)
    target_link_libraries (${DRIVER3} rt)
endif (NEED_LIBRT)
if (NEED_LIBSOCKET)
    target_link_libraries (${DRIVER3} socket)
endif (NEED_LIBSOCKET)
if (NEED_LIBNSL)
    target_link_libraries (${DRIVER3} nsl)
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

if (LIBPQ_FOUND)
    target_link_libraries (${CLIENT} ${LIBPQ_LDFLAGS})
    target_link_libraries (${CLIENT2} ${LIBPQ_LDFLAGS})
    target_link_libraries (${DRIVER3} ${LIBPQ_LDFLAGS})
    target_link_libraries (${TXN_TEST} ${LIBPQ_LDFLAGS})
endif (LIBPQ_FOUND)
if (MYSQLCLIENT_FOUND)
    target_link_libraries (${CLIENT} ${MYSQLCLIENT_LIBRARIES})
    target_link_libraries (${CLIENT2} ${MYSQLCLIENT_LIBRARIES})
    target_link_libraries (${DRIVER3} ${MYSQLCLIENT_LIBRARIES})
    target_link_libraries (${TXN_TEST} ${MYSQLCLIENT_LIBRARIES})
endif (MYSQLCLIENT_FOUND)
if (ODBC_FOUND)
    target_link_libraries (${CLIENT} ${ODBC_LIBRARIES})
    target_link_libraries (${CLIENT2} ${ODBC_LIBRARIES})
    target_link_libraries (${DRIVER3} ${ODBC_LIBRARIES})
    target_link_libraries (${TXN_TEST} ${ODBC_LIBRARIES})
endif (ODBC_FOUND)
if (SQLITE3_FOUND)
    target_link_libraries (${CLIENT} ${SQLITE3_LIBRARIES})
    target_link_libraries (${CLIENT2} ${SQLITE3_LIBRARIES})
    target_link_libraries (${DRIVER3} ${SQLITE3_LIBRARIES})
    target_link_libraries (${TXN_TEST} ${SQLITE3_LIBRARIES})
endif (SQLITE3_FOUND)

#
# Set compiler flags.
#

set (CMAKE_INCLUDE_CURRENT_DIR ON)
set (CMAKE_C_FLAGS_DEBUG "-g -Wall")

set_source_files_properties (
        ${PCG_SOURCE}
        rand.c
        COMPILE_FLAGS "-std=c99 -DHAVE_DEV_RANDOM -I${CMAKE_HOME_DIRECTORY}/src/pcg -I${CMAKE_HOME_DIRECTORY}/src/pcg/extras"
)
set_source_files_properties (
        datagen.c
        COMPILE_FLAGS "-std=c99 -I${CMAKE_HOME_DIRECTORY}/src/include ${LFS_CFLAGS} -I${CMAKE_HOME_DIRECTORY}/src/pcg -I${CMAKE_HOME_DIRECTORY}/src/pcg/extras"
)
set_source_files_properties (
        client_interface.c
        common.c
        driver.c
        driver_common.c
        driver2.c
        driver3.c
        input_data_generator.c
        listener.c
        logging.c
        transaction_data.c
        transaction_queue.c
        COMPILE_FLAGS "-std=c99 -I${CMAKE_HOME_DIRECTORY}/src/include -I${CMAKE_HOME_DIRECTORY}/src/pcg -I${CMAKE_HOME_DIRECTORY}/src/pcg/extras"
)
set_source_files_properties (
        driver_main.c
        COMPILE_FLAGS "-std=c99 ${DBCFLAGS} -I${CMAKE_HOME_DIRECTORY}/src/include -I${CMAKE_HOME_DIRECTORY}/src/pcg -I${CMAKE_HOME_DIRECTORY}/src/pcg/extras"
)
set_source_files_properties (
        _semaphore.c
        _socket.c
        COMPILE_FLAGS "-std=c99 -I${CMAKE_HOME_DIRECTORY}/src/include ${LFS_CFLAGS}")
set_source_files_properties (
        ${DB_SOURCE}
        client.c
        client_common.c
        client_main.c
        client2.c
        db.c
        db_threadpool.c
        transaction_test.c
        COMPILE_FLAGS "-std=c99 ${DBCFLAGS} -I${CMAKE_HOME_DIRECTORY}/src/include -I${CMAKE_HOME_DIRECTORY}/src/pcg -I${CMAKE_HOME_DIRECTORY}/src/pcg/extras"
)

#
# Set linker flags.
#

set_target_properties (${CLIENT} PROPERTIES LINK_FLAGS "${DB_LIBS}")
set_target_properties (${CLIENT2} PROPERTIES LINK_FLAGS "${DB_LIBS}")
set_target_properties (${TXN_TEST} PROPERTIES LINK_FLAGS "${DB_LIBS}")

#
# Install rules.
#

install (PROGRAMS
         ${CMAKE_BINARY_DIR}/src/${CLIENT}
         ${CMAKE_BINARY_DIR}/src/${CLIENT2}
         ${CMAKE_BINARY_DIR}/src/${DATAGEN}
         ${CMAKE_BINARY_DIR}/src/${DRIVER}
         ${CMAKE_BINARY_DIR}/src/${DRIVER2}
         ${CMAKE_BINARY_DIR}/src/${DRIVER3}
         ${CMAKE_BINARY_DIR}/src/${RAND}
         ${CMAKE_BINARY_DIR}/src/${TXN_TEST}
         DESTINATION "bin"
)

add_subdirectory (scripts)
add_subdirectory (test)
