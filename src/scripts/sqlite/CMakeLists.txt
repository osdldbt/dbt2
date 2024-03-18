set (PROGRAMFILES "")
foreach (FILE dbt2-sqlite-build-db
        )
    configure_file(${FILE}.in ${CMAKE_BINARY_DIR}/${FILE} @ONLY)
    set (PROGRAMFILES ${PROGRAMFILES} ${CMAKE_BINARY_DIR}/${FILE})
endforeach()

install (
    PROGRAMS
    ${PROGRAMFILES}
    ${CMAKE_SOURCE_DIR}/src/scripts/sqlite/dbt2-sqlite-plans
    ${CMAKE_SOURCE_DIR}/src/scripts/sqlite/dbt2-sqlite-start-db
    ${CMAKE_SOURCE_DIR}/src/scripts/sqlite/dbt2-sqlite-stop-db
    DESTINATION "bin"
)
