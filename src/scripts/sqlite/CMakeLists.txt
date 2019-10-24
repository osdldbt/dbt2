install (
    PROGRAMS
    ${CMAKE_SOURCE_DIR}/src/scripts/sqlite/dbt2-sqlite-build-db
    ${CMAKE_SOURCE_DIR}/src/scripts/sqlite/dbt2-sqlite-plans
    ${CMAKE_SOURCE_DIR}/src/scripts/sqlite/dbt2-sqlite-start-db
    ${CMAKE_SOURCE_DIR}/src/scripts/sqlite/dbt2-sqlite-stop-db
    DESTINATION "bin"
)
