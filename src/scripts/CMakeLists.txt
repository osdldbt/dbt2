if (LIBPQ_FOUND)
    if ("${LIBPQ_VERSION}" VERSION_GREATER_EQUAL "11.0")
	    add_subdirectory (cockroach)
    endif ("${LIBPQ_VERSION}" VERSION_GREATER_EQUAL "11.0")
	add_subdirectory (pgsql)
	add_subdirectory (yugabyte)
endif (LIBPQ_FOUND)

if (SQLITE3_FOUND)
	add_subdirectory (sqlite)
endif (SQLITE3_FOUND)

if (MYSQLCLIENT_FOUND)
	add_subdirectory (mysql)
endif (MYSQLCLIENT_FOUND)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set (SQLITEOPTIONS ".echo on
.eqp on")
endif ()

configure_file(
    dbt2.in
    ${CMAKE_BINARY_DIR}/dbt2
    @ONLY
)
configure_file(
    dbt2-post-process.sql.in
    ${CMAKE_BINARY_DIR}/dbt2-post-process.sql
    @ONLY
)
configure_file(
    dbt2-run.in
    ${CMAKE_BINARY_DIR}/dbt2-run
    @ONLY
)

install (
	PROGRAMS
    ${CMAKE_BINARY_DIR}/dbt2
    ${CMAKE_BINARY_DIR}/dbt2-post-process.sql
    ${CMAKE_BINARY_DIR}/dbt2-run
    ${CMAKE_SOURCE_DIR}/src/scripts/dbt2-generate-report
    ${CMAKE_SOURCE_DIR}/src/scripts/dbt2-get-os-info
    ${CMAKE_SOURCE_DIR}/src/scripts/dbt2-post-process
    ${CMAKE_SOURCE_DIR}/src/scripts/dbt2-post-process.r
    ${CMAKE_SOURCE_DIR}/src/scripts/dbt2-run-workload-autoscale
	DESTINATION "bin"
)

add_subdirectory (test)
