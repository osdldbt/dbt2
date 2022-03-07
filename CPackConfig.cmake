SET(CPACK_CMAKE_GENERATOR "Unix Makefiles")
SET(CPACK_GENERATOR "TGZ;TBZ2;TXZ")

SET(CPACK_PACKAGE_NAME "dbt2")
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Database Test 2")
SET(CPACK_PACKAGE_VENDOR "dbt")

SET(CPACK_PACKAGE_VERSION_MAJOR "0")
SET(CPACK_PACKAGE_VERSION_MINOR "46")
SET(CPACK_PACKAGE_VERSION_PATCH "0")
SET(CPACK_PACKAGE_VERSION "${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}")
SET(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}")

SET(CPACK_INSTALL_CMAKE_PROJECTS "")

SET(CPACK_PACKAGE_DESCRIPTION_FILE "README")
SET(CPACK_RESOURCE_FILE_LICENSE "LICENSE")

SET(CPACK_IGNORE_FILES "CPackConfig.cmake;_CPack_Packages/;\\.swp$;/\\.git/;\\.tar.Z$;\\.tar.gz$;\\.tar.bz2$;README-CMAKE")
