install (
    PROGRAMS
    ${CMAKE_SOURCE_DIR}/src/scripts/drizzle/dbt2-drizzle-admin
    ${CMAKE_SOURCE_DIR}/src/scripts/drizzle/dbt2-drizzle-load-db
    ${CMAKE_SOURCE_DIR}/src/scripts/drizzle/dbt2-drizzle-start-db
    ${CMAKE_SOURCE_DIR}/src/scripts/drizzle/dbt2-drizzle-stop-db
    DESTINATION "bin"
)
