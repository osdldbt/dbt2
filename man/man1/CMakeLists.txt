execute_process(
    COMMAND sed -n "s/^.*(\\(.*\\)).*$/\\1/p;q" ${CMAKE_HOME_DIRECTORY}/ChangeLog
    OUTPUT_VARIABLE MANDATE
)

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
endforeach()

install(
    FILES ${CMAKE_BINARY_DIR}/man/man1/dbt2.1
          ${CMAKE_BINARY_DIR}/man/man1/dbt2-build.1
          ${CMAKE_BINARY_DIR}/man/man1/dbt2-report.1
          ${CMAKE_BINARY_DIR}/man/man1/dbt2-run.1
    DESTINATION "share/man/man1"
)
