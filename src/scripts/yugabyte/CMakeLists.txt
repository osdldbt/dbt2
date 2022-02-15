install (
    PROGRAMS
    ${CMAKE_SOURCE_DIR}/src/scripts/yugabyte/dbt2-yugabyte-build-db
    ${CMAKE_SOURCE_DIR}/src/scripts/yugabyte/dbt2-yugabyte-create-indexes
    DESTINATION "bin"
)
