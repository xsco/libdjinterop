# libdjinterop CMake file
#
# This minimal CMake build script is provided for simpler integration with
# projects that wish to include libdjinterop in an "in source tree" fashion.
#
# The meson/ninja build should be preferred in all other cases.
#
cmake_minimum_required(VERSION 3.13)
project(libdjinterop
        VERSION 0.14.6
        DESCRIPTION "C++ library providing access to DJ record libraries"
        HOMEPAGE_URL "https://github.com/xsco/libdjinterop")

# Require C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
if (MSVC)
    # Ask MSVC to populate the __cplusplus macro properly.
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zc:__cplusplus")
endif()

# Build shared a library by default.
option(BUILD_SHARED_LIBS "Build shared library" ON)

# Option to use either system SQLite or embedded SQLite.
option(SYSTEM_SQLITE "Use system installation of SQLite" ON)

# Require zlib >= 1.2.8
find_package(ZLIB 1.2.8 REQUIRED)

add_library(
    djinterop
    src/djinterop/impl/crate_impl.cpp
    src/djinterop/impl/database_impl.cpp
    src/djinterop/impl/track_impl.cpp
    src/djinterop/impl/transaction_guard_impl.cpp
    src/djinterop/enginelibrary/schema/schema_1_6_0.cpp
    src/djinterop/enginelibrary/schema/schema_1_7_1.cpp
    src/djinterop/enginelibrary/schema/schema_1_9_1.cpp
    src/djinterop/enginelibrary/schema/schema_1_11_1.cpp
    src/djinterop/enginelibrary/schema/schema_1_13_0.cpp
    src/djinterop/enginelibrary/schema/schema_1_13_1.cpp
    src/djinterop/enginelibrary/schema/schema_1_13_2.cpp
    src/djinterop/enginelibrary/schema/schema_1_15_0.cpp
    src/djinterop/enginelibrary/schema/schema_1_17_0.cpp
    src/djinterop/enginelibrary/schema/schema_1_18_0.cpp
    src/djinterop/enginelibrary/schema/schema.cpp
    src/djinterop/enginelibrary/el_crate_impl.cpp
    src/djinterop/enginelibrary/el_database_impl.cpp
    src/djinterop/enginelibrary/el_storage.cpp
    src/djinterop/enginelibrary/el_track_impl.cpp
    src/djinterop/enginelibrary/el_transaction_guard_impl.cpp
    src/djinterop/enginelibrary/encode_decode_utils.cpp
    src/djinterop/enginelibrary/performance_data_format.cpp
    src/djinterop/crate.cpp
    src/djinterop/database.cpp
    src/djinterop/enginelibrary.cpp
    src/djinterop/track.cpp
    src/djinterop/transaction_guard.cpp
    src/djinterop/util.cpp)

target_compile_definitions(djinterop PUBLIC DJINTEROP_SOURCE)

get_target_property(djinterop_library_type djinterop TYPE)
if(djinterop_library_type STREQUAL "STATIC_LIBRARY")
	set(DJINTEROP_STATIC ON)
endif()

# Generate config.hpp based on build-time environment.
include(CheckIncludeFileCXX)
CHECK_INCLUDE_FILE_CXX(optional DJINTEROP_STD_OPTIONAL)
CHECK_INCLUDE_FILE_CXX(experimental/optional DJINTEROP_STD_EXPERIMENTAL_OPTIONAL)
configure_file(
    include/djinterop/config.hpp.in
    include/djinterop/config.hpp)

target_include_directories(
    djinterop PUBLIC
    ${ZLIB_INCLUDE_DIRS}
    ext/sqlite_modern_cpp
    ${CMAKE_CURRENT_BINARY_DIR}/include
    include src)

target_link_libraries(
    djinterop PUBLIC
    ${ZLIB_LIBRARIES})


if(SYSTEM_SQLITE)
    # Search for system installation of SQLite and use that.
    find_package(SQLite3 3.11.0 REQUIRED)
    target_include_directories(
        djinterop PUBLIC
        ${SQLite3_INCLUDE_DIRS})
    target_link_libraries(
        djinterop PUBLIC
        ${SQLite3_LIBRARIES})
else()
    # Use embedded SQLite amalgamation sources.
    message(STATUS "Using embedded SQLite")
    target_sources(
        djinterop PUBLIC
        ext/sqlite-amalgamation/sqlite3.c)
    target_compile_definitions(
        djinterop PUBLIC
        SQLITE_OMIT_LOAD_EXTENSION)
    target_include_directories(
        djinterop PUBLIC
        ext/sqlite-amalgamation)
endif()

set_target_properties(djinterop PROPERTIES C_VISIBILITY_PRESET hidden)
set_target_properties(djinterop PROPERTIES CXX_VISIBILITY_PRESET hidden)

include(GNUInstallDirs)

install(TARGETS djinterop
  ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
  LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
  RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
)

install(FILES
    include/djinterop/album_art.hpp
    ${CMAKE_CURRENT_BINARY_DIR}/include/djinterop/config.hpp
    include/djinterop/crate.hpp
    include/djinterop/database.hpp
    include/djinterop/djinterop.hpp
    include/djinterop/exceptions.hpp
    include/djinterop/enginelibrary.hpp
    include/djinterop/musical_key.hpp
    include/djinterop/optional.hpp
    include/djinterop/pad_color.hpp
    include/djinterop/performance_data.hpp
    include/djinterop/semantic_version.hpp
    include/djinterop/track.hpp
    include/djinterop/transaction_guard.hpp
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/djinterop")


include(InstallRequiredSystemLibraries)
set(CPACK_PACKAGE_VENDOR "XSCO")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Adam Szmigin <adam.szmigin@xsco.net>")
set(CPACK_DEBIAN_PACKAGE_SECTION "sound")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "zlib1g (>= 1.2.8), libsqlite3-0 (>= 3.11)")
include(CPack)
