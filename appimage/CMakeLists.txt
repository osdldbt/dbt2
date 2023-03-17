configure_file (AppRun.in ${CMAKE_BINARY_DIR}/AppRun @ONLY)
install (PROGRAMS ${CMAKE_BINARY_DIR}/AppRun DESTINATION "bin")
