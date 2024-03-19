cmake_minimum_required (VERSION 3.5.0)
project (dbt2 C)

set (PROJECT_VERSION_MAJOR 0)
set (PROJECT_VERSION_MINOR 61)
set (PROJECT_VERSION_PATCH 1)
set (CMAKE_C_STANDARD 99)

set (PROJECT_VERSION
     ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH})

enable_testing()

include (CheckLibraryExists)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set (SHELLOPTIONS "set -x")
endif ()

set (
    HOMEPAGE
    "Database Test 2 (DBT-2) project page: https://github.com/osdldbt/dbt2"
)

add_subdirectory (appimage)
add_subdirectory (man)
add_subdirectory (src)
add_subdirectory (storedproc)

# CPack rules for creating distribution files.

include (InstallRequiredSystemLibraries)

set (CPACK_CMAKE_GENERATOR "Unix Makefiles")

set (CPACK_PACKAGE_NAME "dbt2")
set (CPACK_PACKAGE_DESCRIPTION_SUMMARY "Database Test 2")
set (CPACK_PACKAGE_VENDOR "dbt")

set (CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set (CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set (CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")
set (CPACK_PACKAGE_VERSION
    "${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}")
set (CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}")

set (CPACK_INSTALL_CMAKE_PROJECTS "")

set (CPACK_PACKAGE_DESCRIPTION_FILE "README")
set (CPACK_RESOURCE_FILE_LICENSE "LICENSE")

set (CPACK_SOURCE_GENERATOR "TBZ2;TXZ;ZIP")
set (
    CPACK_PACKAGE_DESCRIPTION_SUMMARY
    "Database Test 2"
)
set (CPACK_PACKAGE_VENDOR "osdl")
set (CPACK_SOURCE_PACKAGE_FILE_NAME "${PROJECT_NAME}-${PROJECT_VERSION}")

set (CPACK_SOURCE_IGNORE_FILES
     "\\\\.bz2\\$;\\\\.gz\\$;\\\\.xz\\$;\\\\.Z\\$;\\\\.zip\\$"
)
set (CPACK_SOURCE_IGNORE_FILES
     "${CPACK_SOURCE_IGNORE_FILES};/CMakeCache\\\\.txt\\$"
)
set (CPACK_SOURCE_IGNORE_FILES
     "${CPACK_SOURCE_IGNORE_FILES};/CMakeFiles/"
)
set (CPACK_SOURCE_IGNORE_FILES
     "${CPACK_SOURCE_IGNORE_FILES};/cmake_install\\\\.cmake\\$"
)
set (CPACK_SOURCE_IGNORE_FILES
     "${CPACK_SOURCE_IGNORE_FILES};/CPackConfig\\\\.cmake\\$"
)
set (CPACK_SOURCE_IGNORE_FILES
     "${CPACK_SOURCE_IGNORE_FILES};/\\\\.github\\$"
)
set (CPACK_SOURCE_IGNORE_FILES
     "${CPACK_SOURCE_IGNORE_FILES};/\\\\.gitignore\\$"
)
set (CPACK_SOURCE_IGNORE_FILES
     "${CPACK_SOURCE_IGNORE_FILES};/\\\\_CPack\\\\_Packages/"
)
set (CPACK_SOURCE_IGNORE_FILES
     "${CPACK_SOURCE_IGNORE_FILES};/CPackSourceConfig\\\\.cmake\\$"
)
set (CPACK_SOURCE_IGNORE_FILES
     "${CPACK_SOURCE_IGNORE_FILES};/CTestTestfile\\\\.cmake\\$"
)

include (CPack)

set (APPIMAGE_SOURCE_DIR ${CMAKE_SOURCE_DIR}/appimage)

add_custom_target(appimage-podman
    COMMAND cp -aLr /usr/local/AppDir .
    COMMAND VERSION=${PROJECT_VERSION} /usr/local/squashfs-root/AppRun
            --appdir AppDir
            --custom-apprun=${CMAKE_BINARY_DIR}/AppRun
            --desktop-file=${APPIMAGE_SOURCE_DIR}/dbt2.desktop
            --executable=/root/.cargo/bin/toml
            --executable=/usr/bin/jq
            --executable=/usr/bin/perl
            --executable=/usr/bin/psql
            --executable=/usr/local/bin/gnuplot
            --executable=/usr/local/bin/sqlite3
            --icon-file=${APPIMAGE_SOURCE_DIR}/dbt2.svg
            --output appimage
)
