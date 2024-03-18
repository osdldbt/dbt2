execute_process(
    COMMAND sed -n "s/^.*(\\(.*\\)).*$/\\1/p;q" ${CMAKE_HOME_DIRECTORY}/ChangeLog
    OUTPUT_VARIABLE MANDATE
)

set (MANFILES "")
foreach (file dbt2
              dbt2-build
              dbt2-report
              dbt2-run
        )
    configure_file(${file}.1.rst.in ${CMAKE_BINARY_DIR}/man/man1/${file}.1.rst)
    execute_process(
        COMMAND rst2man.py ${CMAKE_BINARY_DIR}/man/man1/${file}.1.rst
        OUTPUT_FILE ${CMAKE_BINARY_DIR}/man/man1/${file}.1
    )
    set (MANFILES ${MANFILES} ${CMAKE_BINARY_DIR}/man/man1/${file}.1)
endforeach()

install(FILES ${MANFILES} DESTINATION "share/man/man1")
