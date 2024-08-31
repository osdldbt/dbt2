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

set (PROGRAMFILES "")
foreach (FILE dbt2
              dbt2-build
              dbt2-post-process.sqlite3
              dbt2-report
              dbt2-run
              dbt2-test-connection-scaling
              dbt2-test-db-parameter
        )
    configure_file(${FILE}.in ${CMAKE_BINARY_DIR}/${FILE} @ONLY)
    set (PROGRAMFILES ${PROGRAMFILES} ${CMAKE_BINARY_DIR}/${FILE})
endforeach()

install (
	PROGRAMS
    ${PROGRAMFILES}
    ${CMAKE_SOURCE_DIR}/src/scripts/dbt2-generate-report
    ${CMAKE_SOURCE_DIR}/src/scripts/dbt2-get-os-info
    ${CMAKE_SOURCE_DIR}/src/scripts/dbt2-post-process.r
	DESTINATION "bin"
)

add_subdirectory (test)
