install (
    PROGRAMS
    ${CMAKE_SOURCE_DIR}/src/scripts/pgsql/dbt2-pgsql-backup
    ${CMAKE_SOURCE_DIR}/src/scripts/pgsql/dbt2-pgsql-build-db
    ${CMAKE_SOURCE_DIR}/src/scripts/pgsql/dbt2-pgsql-check-db
    ${CMAKE_SOURCE_DIR}/src/scripts/pgsql/dbt2-pgsql-create-db
    ${CMAKE_SOURCE_DIR}/src/scripts/pgsql/dbt2-pgsql-create-indexes
    ${CMAKE_SOURCE_DIR}/src/scripts/pgsql/dbt2-pgsql-create-tables
    ${CMAKE_SOURCE_DIR}/src/scripts/pgsql/dbt2-pgsql-db-stat
    ${CMAKE_SOURCE_DIR}/src/scripts/pgsql/dbt2-pgsql-destroy-installation
    ${CMAKE_SOURCE_DIR}/src/scripts/pgsql/dbt2-pgsql-drop-db
    ${CMAKE_SOURCE_DIR}/src/scripts/pgsql/dbt2-pgsql-drop-tables
    ${CMAKE_SOURCE_DIR}/src/scripts/pgsql/dbt2-pgsql-install-source
    ${CMAKE_SOURCE_DIR}/src/scripts/pgsql/dbt2-pgsql-load-db
    ${CMAKE_SOURCE_DIR}/src/scripts/pgsql/dbt2-pgsql-load-stored-procs
    ${CMAKE_SOURCE_DIR}/src/scripts/pgsql/dbt2-pgsql-plans
    ${CMAKE_SOURCE_DIR}/src/scripts/pgsql/dbt2-pgsql-restore
    ${CMAKE_SOURCE_DIR}/src/scripts/pgsql/dbt2-pgsql-start-db
    ${CMAKE_SOURCE_DIR}/src/scripts/pgsql/dbt2-pgsql-stop-db
    ${CMAKE_SOURCE_DIR}/src/scripts/pgsql/dbt2-pgsql-test
    DESTINATION "bin"
)
