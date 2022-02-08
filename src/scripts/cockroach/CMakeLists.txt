install (
    PROGRAMS
    ${CMAKE_SOURCE_DIR}/src/scripts/cockroach/dbt2-cockroach-build-db
    ${CMAKE_SOURCE_DIR}/src/scripts/cockroach/dbt2-cockroach-create-tables
    ${CMAKE_SOURCE_DIR}/src/scripts/cockroach/dbt2-cockroach-create-indexes
    DESTINATION "bin"
)
