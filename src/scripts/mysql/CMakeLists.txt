set (PROGRAMFILES "")
foreach (FILE dbt2-mysql-build-db
              dbt2-mysql-load-stored-procs
              dbt2-mysql-plans
        )
    configure_file(${FILE}.in ${CMAKE_BINARY_DIR}/${FILE} @ONLY)
    set (PROGRAMFILES ${PROGRAMFILES} ${CMAKE_BINARY_DIR}/${FILE})
endforeach()

install (
    PROGRAMS
    ${PROGRAMFILES}
    ${CMAKE_SOURCE_DIR}/src/scripts/mysql/dbt2-mysql-plans
    ${CMAKE_SOURCE_DIR}/src/scripts/mysql/dbt2-mysql-start-db
    ${CMAKE_SOURCE_DIR}/src/scripts/mysql/dbt2-mysql-stop-db
    DESTINATION "bin"
)
