if (PG_CONFIG_FOUND)
	add_subdirectory (cockroach)
	add_subdirectory (pgsql)
	add_subdirectory (yugabyte)
endif (PG_CONFIG_FOUND)

if (DBMS STREQUAL "sqlite")
	add_subdirectory (sqlite)
elseif (DBMS STREQUAL "mysql")
	add_subdirectory (mysql)
endif (DBMS STREQUAL "sqlite")

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
