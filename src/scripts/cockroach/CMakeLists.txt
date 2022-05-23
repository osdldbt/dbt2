install (
    PROGRAMS
    ${CMAKE_SOURCE_DIR}/src/scripts/cockroach/dbt2-cockroach-build-db
    ${CMAKE_SOURCE_DIR}/src/scripts/cockroach/dbt2-cockroach-create-tables
    ${CMAKE_SOURCE_DIR}/src/scripts/cockroach/dbt2-cockroach-create-indexes
    ${CMAKE_SOURCE_DIR}/src/scripts/cockroach/dbt2-cockroach-db-stat
    ${CMAKE_SOURCE_DIR}/src/scripts/cockroach/dbt2-cockroach-load-db
    DESTINATION "bin"
)
