cmake_minimum_required (VERSION 3.1.0)
project (dbt2 C)

set (PROJECT_VERSION_MAJOR 0)
set (PROJECT_VERSION_MINOR 53)
set (PROJECT_VERSION_PATCH 6)
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

# dbt2-driver3 libraries should overlap with psql needs.
set (DRIVER3 "src/${PROJECT_NAME}-driver3")
set (GNUPLOT "AppDir/usr/bin/gnuplot")

add_custom_target(appimage-podman
    COMMAND cp -aLr /usr/local/AppDir .
    COMMAND mkdir -p AppDir/etc AppDir/usr/share/metainfo AppDir/usr/lib
    COMMAND cp -aLr /etc/fonts AppDir/etc
    COMMAND cp -aL ${CMAKE_BINARY_DIR}/AppRun
                   ${APPIMAGE_SOURCE_DIR}/activate
                   ${APPIMAGE_SOURCE_DIR}/dbt2.desktop
                   ${APPIMAGE_SOURCE_DIR}/dbt2.svg
            AppDir
    COMMAND cp -aL ${APPIMAGE_SOURCE_DIR}/dbt2.appdata.xml
            AppDir/usr/share/metainfo
    COMMAND cp -aL `ldd ${DRIVER3} | grep libcrypto\\\\\\\\. | cut -d \" \" -f 3`
                   `ldd ${DRIVER3} | grep libev\\\\\\\\. | cut -d \" \" -f 3`
                   `ldd ${DRIVER3} | grep libnsl\\\\\\\\. | cut -d \" \" -f 3`
                   `ldd ${DRIVER3} | grep libssl\\\\\\\\. | cut -d \" \" -f 3`
                   `ldd ${GNUPLOT} | grep libcairo\\\\\\\\. | cut -d \" \" -f 3`
                   `ldd ${GNUPLOT} | grep libdatrie\\\\\\\\. | cut -d \" \" -f 3`
                   `ldd ${GNUPLOT} | grep libfontconfig\\\\\\\\. | cut -d \" \" -f 3`
                   `ldd ${GNUPLOT} | grep libfreetype\\\\\\\\. | cut -d \" \" -f 3`
                   `ldd ${GNUPLOT} | grep libfribidi\\\\\\\\. | cut -d \" \" -f 3`
                   `ldd ${GNUPLOT} | grep libgraphite2\\\\\\\\. | cut -d \" \" -f 3`
                   `ldd ${GNUPLOT} | grep harfbuzz\\\\\\\\. | cut -d \" \" -f 3`
                   `ldd ${GNUPLOT} | grep libpango-1\\\\\\\\.0\\\\\\\\. | cut -d \" \" -f 3`
                   `ldd ${GNUPLOT} | grep libpangoft2-1\\\\\\\\.0\\\\\\\\. | cut -d \" \" -f 3`
                   `ldd ${GNUPLOT} | grep libpangocairo-1\\\\\\\\.0\\\\\\\\. | cut -d \" \" -f 3`
                   `ldd ${GNUPLOT} | grep libpixman-1\\\\\\\\. | cut -d \" \" -f 3`
                   `ldd ${GNUPLOT} | grep libpng12\\\\\\\\. | cut -d \" \" -f 3`
                   `ldd ${GNUPLOT} | grep libthai\\\\\\\\. | cut -d \" \" -f 3`
                   `ldd ${GNUPLOT} | grep libxcb\\\\\\\\. | cut -d \" \" -f 3`
                   `ldd ${GNUPLOT} | grep libxcb-render\\\\\\\\. | cut -d \" \" -f 3`
                   `ldd ${GNUPLOT} | grep libxcb-shm\\\\\\\\. | cut -d \" \" -f 3`
                   `ldd ${GNUPLOT} | grep libX11\\\\\\\\. | cut -d \" \" -f 3`
                   `ldd ${GNUPLOT} | grep libXau\\\\\\\\. | cut -d \" \" -f 3`
                   `ldd ${GNUPLOT} | grep libXdmcp\\\\\\\\. | cut -d \" \" -f 3`
                   `ldd ${GNUPLOT} | grep libXext\\\\\\\\. | cut -d \" \" -f 3`
                   `ldd ${GNUPLOT} | grep libXrender\\\\\\\\. | cut -d \" \" -f 3`
            AppDir/usr/lib
    COMMAND VERSION=${PROJECT_VERSION} /usr/local/squashfs-root/AppRun
            --comp=xz AppDir
)
