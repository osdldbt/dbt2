install (
    PROGRAMS
    ${CMAKE_SOURCE_DIR}/src/scripts/yugabyte/dbt2-yugabyte-build-db
    ${CMAKE_SOURCE_DIR}/src/scripts/yugabyte/dbt2-yugabyte-create-indexes
    ${CMAKE_SOURCE_DIR}/src/scripts/yugabyte/dbt2-yugabyte-load-db
    DESTINATION "bin"
)
