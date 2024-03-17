set (PROGRAMFILES "")
foreach (FILE dbt2-cockroach-build-db
        )
    configure_file(${FILE}.in ${CMAKE_BINARY_DIR}/${FILE} @ONLY)
    set (PROGRAMFILES ${PROGRAMFILES} ${CMAKE_BINARY_DIR}/${FILE})
endforeach()

install (
    PROGRAMS
    ${PROGRAMFILES}
    ${CMAKE_SOURCE_DIR}/src/scripts/cockroach/dbt2-cockroach-create-tables
    ${CMAKE_SOURCE_DIR}/src/scripts/cockroach/dbt2-cockroach-create-indexes
    ${CMAKE_SOURCE_DIR}/src/scripts/cockroach/dbt2-cockroach-db-stat
    ${CMAKE_SOURCE_DIR}/src/scripts/cockroach/dbt2-cockroach-load-db
    DESTINATION "bin"
)
