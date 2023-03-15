# libdjinterop CMake file
#
# This CMake build script is provided for simpler integration with projects
# that wish to include libdjinterop via CMake's ExternalProject module.
#
# It is also automatically-detected as the preferred build system to use when
# building `.deb` packages for Debian/Ubuntu and derivatives.
#
# The meson/ninja build should be preferred in all other cases.
#
cmake_minimum_required(VERSION 3.10)
project(libdjinterop
        VERSION 0.18.0
        DESCRIPTION "C++ library providing access to DJ record libraries")
set(PROJECT_HOMEPAGE_URL "https://github.com/xsco/libdjinterop")

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

if(POLICY CMP0074)
    # CMP0074: find_package() uses <PackageName>_ROOT variables.
    cmake_policy(SET CMP0074 NEW)
endif()
if(POLICY CMP0076)
    # CMP0076: target_sources() command converts relative paths to absolute.
    cmake_policy(SET CMP0076 NEW)
endif()

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
set(ZLIB_MIN_VERSION 1.2.8)
find_package(ZLIB ${ZLIB_MIN_VERSION} REQUIRED)

add_library(
    DjInterop
    src/djinterop/impl/crate_impl.cpp
    src/djinterop/impl/database_impl.cpp
    src/djinterop/impl/track_impl.cpp
    src/djinterop/impl/transaction_guard_impl.cpp
    src/djinterop/engine/encode_decode_utils.cpp
    src/djinterop/engine/engine.cpp
    src/djinterop/engine/schema/schema_1_6_0.cpp
    src/djinterop/engine/schema/schema_1_7_1.cpp
    src/djinterop/engine/schema/schema_1_9_1.cpp
    src/djinterop/engine/schema/schema_1_11_1.cpp
    src/djinterop/engine/schema/schema_1_13_0.cpp
    src/djinterop/engine/schema/schema_1_13_1.cpp
    src/djinterop/engine/schema/schema_1_13_2.cpp
    src/djinterop/engine/schema/schema_1_15_0.cpp
    src/djinterop/engine/schema/schema_1_17_0.cpp
    src/djinterop/engine/schema/schema_1_18_0_desktop.cpp
    src/djinterop/engine/schema/schema_1_18_0_os.cpp
    src/djinterop/engine/schema/schema_2_18_0.cpp
    src/djinterop/engine/schema/schema.cpp
    src/djinterop/engine/v1/engine_crate_impl.cpp
    src/djinterop/engine/v1/engine_database_impl.cpp
    src/djinterop/engine/v1/engine_storage.cpp
    src/djinterop/engine/v1/engine_track_impl.cpp
    src/djinterop/engine/v1/engine_transaction_guard_impl.cpp
    src/djinterop/engine/v1/performance_data_format.cpp
    src/djinterop/engine/v2/beat_data_blob.cpp
    src/djinterop/engine/v2/change_log_table.cpp
    src/djinterop/engine/v2/database_impl.cpp
    src/djinterop/engine/v2/engine_library.cpp
    src/djinterop/engine/v2/information_table.cpp
    src/djinterop/engine/v2/loops_blob.cpp
    src/djinterop/engine/v2/overview_waveform_data_blob.cpp
    src/djinterop/engine/v2/playlist_table.cpp
    src/djinterop/engine/v2/playlist_entity_table.cpp
    src/djinterop/engine/v2/quick_cues_blob.cpp
    src/djinterop/engine/v2/track_data_blob.cpp
    src/djinterop/engine/v2/track_table.cpp
    src/djinterop/engine/v2/track_impl.cpp
    src/djinterop/crate.cpp
    src/djinterop/database.cpp
    src/djinterop/track.cpp
    src/djinterop/transaction_guard.cpp
    src/djinterop/util.cpp)

set_target_properties(DjInterop PROPERTIES
    OUTPUT_NAME "djinterop"
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR})

target_compile_definitions(DjInterop PUBLIC DJINTEROP_SOURCE)

get_target_property(DJINTEROP_LIBRARY_TYPE DjInterop TYPE)
if(DJINTEROP_LIBRARY_TYPE STREQUAL "STATIC_LIBRARY")
	set(DJINTEROP_STATIC ON)
endif()

# Generate config.hpp based on build-time environment.
include(CheckIncludeFileCXX)
CHECK_INCLUDE_FILE_CXX(optional DJINTEROP_STD_OPTIONAL)
CHECK_INCLUDE_FILE_CXX(experimental/optional DJINTEROP_STD_EXPERIMENTAL_OPTIONAL)
configure_file(
    include/djinterop/config.hpp.in
    include/djinterop/config.hpp)

include(GNUInstallDirs)
set(DJINTEROP_INSTALL_INCLUDEDIR "${CMAKE_INSTALL_INCLUDEDIR}/djinterop")

target_include_directories(
        DjInterop PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${DJINTEROP_INSTALL_INCLUDEDIR}>)
target_include_directories(
        DjInterop PRIVATE
        ${ZLIB_INCLUDE_DIRS}
        ext/sqlite_modern_cpp
        ext/date
        src)

target_link_libraries(
    DjInterop PUBLIC
    ${ZLIB_LIBRARIES})


if(SYSTEM_SQLITE)
    # Search for system installation of SQLite and use that.
    set(SQLITE_MIN_VERSION 3.11)
    find_package(SQLite3 ${SQLITE_MIN_VERSION} REQUIRED)
    target_include_directories(
        DjInterop PRIVATE
        ${SQLite3_INCLUDE_DIRS})
    target_link_libraries(
        DjInterop PUBLIC
        ${SQLite3_LIBRARIES})
else()
    # Use embedded SQLite amalgamation sources.
    message(STATUS "Using embedded SQLite")
    target_sources(
        DjInterop PRIVATE
        ext/sqlite-amalgamation/sqlite3.c)
    target_compile_definitions(
        DjInterop PUBLIC
        SQLITE_OMIT_LOAD_EXTENSION)
    target_include_directories(
        DjInterop PRIVATE
        ext/sqlite-amalgamation)
endif()

set_target_properties(DjInterop PROPERTIES C_VISIBILITY_PRESET hidden)
set_target_properties(DjInterop PROPERTIES CXX_VISIBILITY_PRESET hidden)

install(TARGETS DjInterop
        EXPORT DjInteropTargets
        ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}")

install(FILES
    include/djinterop/album_art.hpp
    ${CMAKE_CURRENT_BINARY_DIR}/include/djinterop/config.hpp
    include/djinterop/crate.hpp
    include/djinterop/database.hpp
    include/djinterop/djinterop.hpp
    include/djinterop/exceptions.hpp
    include/djinterop/musical_key.hpp
    include/djinterop/optional.hpp
    include/djinterop/pad_color.hpp
    include/djinterop/performance_data.hpp
    include/djinterop/semantic_version.hpp
    include/djinterop/stream_helper.hpp
    include/djinterop/track.hpp
    include/djinterop/track_snapshot.hpp
    include/djinterop/transaction_guard.hpp
    DESTINATION "${DJINTEROP_INSTALL_INCLUDEDIR}")
install(FILES
    include/djinterop/engine/engine.hpp
    include/djinterop/engine/engine_version.hpp
    DESTINATION "${DJINTEROP_INSTALL_INCLUDEDIR}/engine")
install(FILES
    include/djinterop/engine/v2/beat_data_blob.hpp
    include/djinterop/engine/v2/change_log_table.hpp
    include/djinterop/engine/v2/engine_library.hpp
    include/djinterop/engine/v2/information_table.hpp
    include/djinterop/engine/v2/loops_blob.hpp
    include/djinterop/engine/v2/overview_waveform_data_blob.hpp
    include/djinterop/engine/v2/playlist_entity_table.hpp
    include/djinterop/engine/v2/playlist_table.hpp
    include/djinterop/engine/v2/quick_cues_blob.hpp
    include/djinterop/engine/v2/track_data_blob.hpp
    include/djinterop/engine/v2/track_table.hpp
    DESTINATION "${DJINTEROP_INSTALL_INCLUDEDIR}/engine/v2")


if (UNIX)
    set(PKGCONFIG_TARGET djinterop)
    if (SYSTEM_SQLITE)
        set(PKGCONFIG_REQUIRES "zlib >= ${ZLIB_MIN_VERSION}, sqlite3 >= ${SQLITE_MIN_VERSION}")
    else()
        set(PKGCONFIG_REQUIRES "zlib >= ${ZLIB_MIN_VERSION}")
    endif()
    configure_file(djinterop.pc.in djinterop.pc @ONLY)
    install(FILES
            ${CMAKE_CURRENT_BINARY_DIR}/djinterop.pc
            DESTINATION "${CMAKE_INSTALL_LIBDIR}/pkgconfig")
endif()

# CMake package config export.
include(CMakePackageConfigHelpers)
set(DJINTEROP_INSTALL_CMAKEDIR "lib/cmake/DjInterop")
install(EXPORT DjInteropTargets
        FILE DjInteropTargets.cmake
        NAMESPACE DjInterop::
        DESTINATION "${DJINTEROP_INSTALL_CMAKEDIR}")
configure_package_config_file(
        DjInteropConfig.cmake.in
        "${CMAKE_CURRENT_BINARY_DIR}/DjInteropConfig.cmake"
        INSTALL_DESTINATION "${DJINTEROP_INSTALL_CMAKEDIR}")
write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/DjInteropConfigVersion.cmake"
        VERSION "${CMAKE_PROJECT_VERSION}"
        COMPATIBILITY SameMajorVersion)
install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/DjInteropConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/DjInteropConfigVersion.cmake"
        cmake/FindPackageHandleStandardArgs.cmake
        cmake/FindPackageMessage.cmake
        cmake/FindSQLite3.cmake
        DESTINATION "${DJINTEROP_INSTALL_CMAKEDIR}")

# Example programs demonstrating usage.
function(add_djinterop_example example_name)
    add_executable(example_${example_name} example/${example_name}.cpp)
    target_include_directories(example_${example_name} PUBLIC
            ${CMAKE_CURRENT_BINARY_DIR}/include
            include)
    target_link_libraries(example_${example_name} PUBLIC DjInterop)
endfunction()

add_djinterop_example(engine_prime)
add_djinterop_example(engine_library_v2_low_level)

# Unit tests.
include(CTest)
find_package(Boost 1.65.1 QUIET COMPONENTS filesystem system)
if (Boost_FOUND)
    set(TESTDATA_DIR "${CMAKE_CURRENT_SOURCE_DIR}/testdata")

    function(add_djinterop_test test_dir test_name)
        string(REPLACE "/" "_" test_executable_name ${test_dir}${test_name})
        add_executable(${test_executable_name}
                test/djinterop/${test_dir}${test_name}.cpp)
        target_compile_definitions(${test_executable_name} PUBLIC
                -DTESTDATA_DIR=${TESTDATA_DIR})
        target_include_directories(${test_executable_name} PUBLIC
                ${Boost_INCLUDE_DIRS}
                ${CMAKE_CURRENT_BINARY_DIR}/include
                include)
        target_link_libraries(${test_executable_name} PUBLIC
                DjInterop
                ${Boost_LIBRARIES})
        add_test(NAME ${test_executable_name} COMMAND ${test_executable_name})
    endfunction()

    add_djinterop_test("" semantic_version_test)
    add_djinterop_test("" track_snapshot_test)
    add_djinterop_test(engine/ engine_test)
    add_djinterop_test(engine/v1/ crate_test)
    add_djinterop_test(engine/v1/ database_test)
    add_djinterop_test(engine/v1/ engine_test)
    add_djinterop_test(engine/v1/ track_test)
    add_djinterop_test(engine/v2/ engine_test)
    add_djinterop_test(engine/v2/ track_table_test)
    add_djinterop_test(engine/v2/ track_test)

else()
    message(STATUS "Unit tests not available, as Boost cannot be found")
endif()


include(InstallRequiredSystemLibraries)
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
include(CPack)
