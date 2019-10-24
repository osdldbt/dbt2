install (
    PROGRAMS
    ${CMAKE_SOURCE_DIR}/src/scripts/mysql/dbt2-mysql-build-db
    ${CMAKE_SOURCE_DIR}/src/scripts/mysql/dbt2-mysql-load-sp
    ${CMAKE_SOURCE_DIR}/src/scripts/mysql/dbt2-mysql-load-stored-procs
    ${CMAKE_SOURCE_DIR}/src/scripts/mysql/dbt2-mysql-plans
    ${CMAKE_SOURCE_DIR}/src/scripts/mysql/dbt2-mysql-start-db
    ${CMAKE_SOURCE_DIR}/src/scripts/mysql/dbt2-mysql-stop-db
    DESTINATION "bin"
)
