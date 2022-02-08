if (DBMS STREQUAL "pgsql")
	add_subdirectory (pgsql)
elseif (DBMS STREQUAL "sqlite")
	add_subdirectory (sqlite)
elseif (DBMS STREQUAL "mysql")
	add_subdirectory (mysql)
elseif (DBMS STREQUAL "drizzle")
	add_subdirectory (drizzle)
elseif (DBMS STREQUAL "cockroach")
	add_subdirectory (cockroach)
	add_subdirectory (pgsql)
endif (DBMS STREQUAL "pgsql")

install (
	PROGRAMS
    ${CMAKE_SOURCE_DIR}/src/scripts/dbt2-generate-report
    ${CMAKE_SOURCE_DIR}/src/scripts/dbt2-get-os-info
    ${CMAKE_SOURCE_DIR}/src/scripts/dbt2-plot-transaction-rate
    ${CMAKE_SOURCE_DIR}/src/scripts/dbt2-post-process
    ${CMAKE_SOURCE_DIR}/src/scripts/dbt2-run-workload
    ${CMAKE_SOURCE_DIR}/src/scripts/dbt2-run-workload-autoscale
    ${CMAKE_SOURCE_DIR}/src/scripts/dbt2-sysstats
	DESTINATION "bin"
)
