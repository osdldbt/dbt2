set (PROGRAMFILES "")
foreach (FILE dbt2-yugabyte-build-db
        )
    configure_file(${FILE}.in ${CMAKE_BINARY_DIR}/${FILE} @ONLY)
    set (PROGRAMFILES ${PROGRAMFILES} ${CMAKE_BINARY_DIR}/${FILE})
endforeach()

install (
    PROGRAMS
    ${PROGRAMFILES}
    ${CMAKE_SOURCE_DIR}/src/scripts/yugabyte/dbt2-yugabyte-create-indexes
    ${CMAKE_SOURCE_DIR}/src/scripts/yugabyte/dbt2-yugabyte-load-db
    DESTINATION "bin"
)
