cmake_minimum_required(VERSION 3.10)
project(libdjinterop
        VERSION 0.27.3
        DESCRIPTION "C++ library providing access to DJ record libraries")
set(PROJECT_HOMEPAGE_URL "https://github.com/xsco/libdjinterop")

if(POLICY CMP0074)
    # CMP0074: find_package() uses <PackageName>_ROOT variables.
    cmake_policy(SET CMP0074 NEW)
endif()
if(POLICY CMP0076)
    # CMP0076: target_sources() command converts relative paths to absolute.
    cmake_policy(SET CMP0076 NEW)
endif()
if(POLICY CMP0167)
    # CMP0167: Don't use FindBoost() any longer.
    cmake_policy(SET CMP0167 NEW)
endif()

# Require C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS OFF)
if(MSVC)
    add_compile_options("/W4" "/wd4251" "/wd4275" "$<$<CONFIG:RELEASE>:/O2>")

    # Ask MSVC to populate the __cplusplus macro properly.
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zc:__cplusplus")
else()
    # Disabling -Wunused-variable needed as the library implementation doesn't
    # use every variable in the public headers.  Is there a better way?
    # Disabling -Wunused-but-set-variable owing to XCode triggering on this
    # in the bundled SQLite amalgamation.
    add_compile_options("-Wall" "-Wpedantic" "-Wno-unused-variable" "-Wno-unused-but-set-variable" "-Werror")
endif()

# Build shared a library by default.
option(BUILD_SHARED_LIBS "Build shared library" ON)

# libdjinterop is dependent on several other libraries, some of which are
# readily available on all major OSes, and others which are not so easy to
# find.  For the less readily-available dependencies, these are bundled
# alongside the main code, in the `ext/` directory, for convenience.
#
# The below options offer the choice of using the bundled dependencies,
# or attempting to use a system installation (for a so-called 'devendored'
# build).
option(SYSTEM_DATE_H "Use system installation of date.h" OFF)
option(SYSTEM_SQLITE "Use system installation of SQLite" ON)
option(SYSTEM_SQLITE_MODERN_CPP "Use system installation of sqlite_modern_cpp" OFF)


# The processor's own AES instructions are worth two orders of magnitude over
# software AES when reading an encrypted OneLibrary database, so use them where
# the compiler can be persuaded to emit them.  Whether the processor running the
# built library has them is a separate question, decided at run time; settled
# here is only whether the intrinsics compile, and with which flags.  They are
# applied to `aes_hardware.cpp` alone, so no other translation unit can pick up
# an instruction the target may not have.
include(CheckCXXSourceCompiles)

set(DJINTEROP_AES_X86_SOURCE "
#if defined(_MSC_VER)
#include <intrin.h>
#else
#include <immintrin.h>
#endif
int main()
{
    __m128i x = _mm_setzero_si128();
    x = _mm_aesenc_si128(x, x);
    x = _mm_aesenclast_si128(x, x);
    x = _mm_aesdec_si128(x, x);
    x = _mm_aesdeclast_si128(x, x);
    return _mm_cvtsi128_si32(x);
}
")

set(DJINTEROP_AES_ARM64_SOURCE "
#include <arm_neon.h>
int main()
{
    uint8x16_t x = vdupq_n_u8(0);
    x = vaesmcq_u8(vaeseq_u8(x, x));
    x = vaesimcq_u8(vaesdq_u8(x, x));
    return vgetq_lane_u8(x, 0);
}
")

set(DJINTEROP_AES_INTRINSICS_DEFINE "")
set(DJINTEROP_AES_INTRINSICS_FLAGS "")

# Try one architecture, unless another has already answered.  Bare comes first:
# MSVC needs no flags at all, and neither does a compiler whose default target
# already includes the extension, such as Apple's on arm64.
macro(djinterop_try_aes arch source flags)
    if(NOT DJINTEROP_AES_INTRINSICS_DEFINE)
        check_cxx_source_compiles("${source}" DJINTEROP_AES_${arch}_BARE)
        if(NOT DJINTEROP_AES_${arch}_BARE)
            set(CMAKE_REQUIRED_FLAGS "${flags}")
            check_cxx_source_compiles(
                "${source}" DJINTEROP_AES_${arch}_FLAGGED)
            unset(CMAKE_REQUIRED_FLAGS)
            if(DJINTEROP_AES_${arch}_FLAGGED)
                set(DJINTEROP_AES_INTRINSICS_FLAGS "${flags}")
            endif()
        endif()
        if(DJINTEROP_AES_${arch}_BARE OR DJINTEROP_AES_${arch}_FLAGGED)
            set(DJINTEROP_AES_INTRINSICS_DEFINE
                "DJINTEROP_AES_INTRINSICS_${arch}")
        endif()
    endif()
endmacro()

djinterop_try_aes(X86 "${DJINTEROP_AES_X86_SOURCE}" "-maes -msse2")
djinterop_try_aes(ARM64 "${DJINTEROP_AES_ARM64_SOURCE}" "-march=armv8-a+crypto")

if(DJINTEROP_AES_INTRINSICS_DEFINE)
    message(STATUS "Using AES instructions where available at run time")
    set_source_files_properties(
        src/djinterop/util/crypto/aes_hardware.cpp PROPERTIES
        COMPILE_DEFINITIONS "${DJINTEROP_AES_INTRINSICS_DEFINE}"
        COMPILE_FLAGS "${DJINTEROP_AES_INTRINSICS_FLAGS}")
else()
    message(STATUS "No AES instructions for this target; using AES tables")
endif()

add_library(
    DjInterop
    include/djinterop/album_art.hpp
    include/djinterop/crate.hpp
    include/djinterop/database.hpp
    include/djinterop/djinterop.hpp
    include/djinterop/engine/engine.hpp
    include/djinterop/engine/engine_schema.hpp
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
    include/djinterop/engine/v3/beat_data_blob.hpp
    include/djinterop/engine/v3/engine_library.hpp
    include/djinterop/engine/v3/information_table.hpp
    include/djinterop/engine/v3/loops_blob.hpp
    include/djinterop/engine/v3/overview_waveform_data_blob.hpp
    include/djinterop/engine/v3/performance_data_table.hpp
    include/djinterop/engine/v3/playlist_entity_table.hpp
    include/djinterop/engine/v3/playlist_table.hpp
    include/djinterop/engine/v3/quick_cues_blob.hpp
    include/djinterop/engine/v3/track_data_blob.hpp
    include/djinterop/engine/v3/track_table.hpp
    include/djinterop/exceptions.hpp
    include/djinterop/musical_key.hpp
    include/djinterop/onelibrary/onelibrary.hpp
    include/djinterop/pad_color.hpp
    include/djinterop/performance_data.hpp
    include/djinterop/playlist.hpp
    include/djinterop/semantic_version.hpp
    include/djinterop/stream_helper.hpp
    include/djinterop/track.hpp
    include/djinterop/track_snapshot.hpp
    src/djinterop/crate.cpp
    src/djinterop/database.cpp
    src/djinterop/engine/base_engine_library.cpp
    src/djinterop/engine/base_engine_impl.hpp
    src/djinterop/engine/encode_decode_utils.cpp
    src/djinterop/engine/encode_decode_utils.hpp
    src/djinterop/engine/engine.cpp
    src/djinterop/engine/engine_library_context.hpp
    src/djinterop/engine/engine_library_dir_utils.cpp
    src/djinterop/engine/engine_library_dir_utils.hpp
    src/djinterop/engine/metadata_types.hpp
    src/djinterop/engine/schema/schema_1_6_0.cpp
    src/djinterop/engine/schema/schema_1_6_0.hpp
    src/djinterop/engine/schema/schema_1_7_1.cpp
    src/djinterop/engine/schema/schema_1_7_1.hpp
    src/djinterop/engine/schema/schema_1_9_1.cpp
    src/djinterop/engine/schema/schema_1_9_1.hpp
    src/djinterop/engine/schema/schema_1_11_1.cpp
    src/djinterop/engine/schema/schema_1_11_1.hpp
    src/djinterop/engine/schema/schema_1_13_0.cpp
    src/djinterop/engine/schema/schema_1_13_0.hpp
    src/djinterop/engine/schema/schema_1_13_1.cpp
    src/djinterop/engine/schema/schema_1_13_1.hpp
    src/djinterop/engine/schema/schema_1_13_2.cpp
    src/djinterop/engine/schema/schema_1_13_2.hpp
    src/djinterop/engine/schema/schema_1_15_0.cpp
    src/djinterop/engine/schema/schema_1_15_0.hpp
    src/djinterop/engine/schema/schema_1_17_0.cpp
    src/djinterop/engine/schema/schema_1_17_0.hpp
    src/djinterop/engine/schema/schema_1_18_0_desktop.cpp
    src/djinterop/engine/schema/schema_1_18_0_desktop.hpp
    src/djinterop/engine/schema/schema_1_18_0_os.cpp
    src/djinterop/engine/schema/schema_1_18_0_os.hpp
    src/djinterop/engine/schema/schema_2_18_0.cpp
    src/djinterop/engine/schema/schema_2_18_0.hpp
    src/djinterop/engine/schema/schema_2_20_1.cpp
    src/djinterop/engine/schema/schema_2_20_1.hpp
    src/djinterop/engine/schema/schema_2_20_2.cpp
    src/djinterop/engine/schema/schema_2_20_2.hpp
    src/djinterop/engine/schema/schema_2_20_3.cpp
    src/djinterop/engine/schema/schema_2_20_3.hpp
    src/djinterop/engine/schema/schema_2_21_0.cpp
    src/djinterop/engine/schema/schema_2_21_0.hpp
    src/djinterop/engine/schema/schema_2_21_1.cpp
    src/djinterop/engine/schema/schema_2_21_1.hpp
    src/djinterop/engine/schema/schema_2_21_2.cpp
    src/djinterop/engine/schema/schema_2_21_2.hpp
    src/djinterop/engine/schema/schema_3_0_0.cpp
    src/djinterop/engine/schema/schema_3_0_0.hpp
    src/djinterop/engine/schema/schema_3_0_1.cpp
    src/djinterop/engine/schema/schema_3_0_1.hpp
    src/djinterop/engine/schema/schema_3_0_2.cpp
    src/djinterop/engine/schema/schema_3_0_2.hpp
    src/djinterop/engine/schema/schema.cpp
    src/djinterop/engine/schema/schema.hpp
    src/djinterop/engine/schema/schema_validate_utils.hpp
    src/djinterop/engine/v1/engine_crate_impl.cpp
    src/djinterop/engine/v1/engine_crate_impl.hpp
    src/djinterop/engine/v1/engine_database_impl.cpp
    src/djinterop/engine/v1/engine_database_impl.hpp
    src/djinterop/engine/v1/engine_playlist_impl.cpp
    src/djinterop/engine/v1/engine_playlist_impl.hpp
    src/djinterop/engine/v1/engine_storage.cpp
    src/djinterop/engine/v1/engine_storage.hpp
    src/djinterop/engine/v1/engine_track_impl.cpp
    src/djinterop/engine/v1/engine_track_impl.hpp
    src/djinterop/engine/v1/performance_data_format.cpp
    src/djinterop/engine/v1/performance_data_format.hpp
    src/djinterop/engine/v2/beat_data_blob.cpp
    src/djinterop/engine/v2/change_log_table.cpp
    src/djinterop/engine/v2/convert_beatgrid.hpp
    src/djinterop/engine/v2/convert_hot_cues.hpp
    src/djinterop/engine/v2/convert_loops.hpp
    src/djinterop/engine/v2/convert_track.hpp
    src/djinterop/engine/v2/convert_waveform.hpp
    src/djinterop/engine/v2/crate_impl.cpp
    src/djinterop/engine/v2/crate_impl.hpp
    src/djinterop/engine/v2/database_impl.cpp
    src/djinterop/engine/v2/database_impl.hpp
    src/djinterop/engine/v2/engine_library.cpp
    src/djinterop/engine/v2/information_table.cpp
    src/djinterop/engine/v2/loops_blob.cpp
    src/djinterop/engine/v2/overview_waveform_data_blob.cpp
    src/djinterop/engine/v2/playlist_entity_table.cpp
    src/djinterop/engine/v2/playlist_impl.cpp
    src/djinterop/engine/v2/playlist_impl.hpp
    src/djinterop/engine/v2/playlist_table.cpp
    src/djinterop/engine/v2/quick_cues_blob.cpp
    src/djinterop/engine/v2/track_data_blob.cpp
    src/djinterop/engine/v2/track_table.cpp
    src/djinterop/engine/v2/track_impl.cpp
    src/djinterop/engine/v2/track_impl.hpp
    src/djinterop/engine/v3/convert_beatgrid.hpp
    src/djinterop/engine/v3/convert_hot_cues.hpp
    src/djinterop/engine/v3/convert_loops.hpp
    src/djinterop/engine/v3/convert_track.hpp
    src/djinterop/engine/v3/convert_waveform.hpp
    src/djinterop/engine/v3/crate_impl.cpp
    src/djinterop/engine/v3/crate_impl.hpp
    src/djinterop/engine/v3/database_impl.cpp
    src/djinterop/engine/v3/database_impl.hpp
    src/djinterop/engine/v3/engine_library.cpp
    src/djinterop/engine/v3/performance_data_table.cpp
    src/djinterop/engine/v3/playlist_impl.cpp
    src/djinterop/engine/v3/playlist_impl.hpp
    src/djinterop/engine/v3/track_impl.cpp
    src/djinterop/engine/v3/track_impl.hpp
    src/djinterop/engine/v3/track_table.cpp
    src/djinterop/impl/crate_impl.cpp
    src/djinterop/impl/crate_impl.hpp
    src/djinterop/impl/database_impl.hpp
    src/djinterop/impl/playlist_impl.hpp
    src/djinterop/impl/track_impl.cpp
    src/djinterop/impl/track_impl.hpp
    src/djinterop/onelibrary/content_table.cpp
    src/djinterop/onelibrary/content_table.hpp
    src/djinterop/onelibrary/crate_impl.cpp
    src/djinterop/onelibrary/crate_impl.hpp
    src/djinterop/onelibrary/database_impl.cpp
    src/djinterop/onelibrary/database_impl.hpp
    src/djinterop/onelibrary/onelibrary.cpp
    src/djinterop/onelibrary/onelibrary_context.hpp
    src/djinterop/onelibrary/playlist_impl.cpp
    src/djinterop/onelibrary/playlist_impl.hpp
    src/djinterop/onelibrary/playlist_table.cpp
    src/djinterop/onelibrary/playlist_table.hpp
    src/djinterop/onelibrary/track_impl.cpp
    src/djinterop/onelibrary/track_impl.hpp
    src/djinterop/playlist.cpp
    src/djinterop/track.cpp
    src/djinterop/util/chrono.cpp
    src/djinterop/util/chrono.hpp
    src/djinterop/util/crypto/aes.cpp
    src/djinterop/util/crypto/aes.hpp
    src/djinterop/util/crypto/aes_hardware.cpp
    src/djinterop/util/crypto/aes_hardware.hpp
    src/djinterop/util/crypto/sha512.cpp
    src/djinterop/util/crypto/sha512.hpp
    src/djinterop/util/crypto/sqlcipher_codec.cpp
    src/djinterop/util/crypto/sqlcipher_codec.hpp
    src/djinterop/util/crypto/sqlcipher_wal.cpp
    src/djinterop/util/crypto/sqlcipher_wal.hpp
    src/djinterop/util/filesystem.cpp
    src/djinterop/util/filesystem.hpp
    src/djinterop/util/random.cpp
    src/djinterop/util/random.hpp
    src/djinterop/util/sqlite_transaction.hpp
)

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

# Decrypting the pages of a database is spread over the processors available,
# which is what the standard threading library is needed for.
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(
    DjInterop PRIVATE
    Threads::Threads)

# Always rely on system installation of zlib.
set(ZLIB_MIN_VERSION 1.2.8)
find_package(ZLIB ${ZLIB_MIN_VERSION} REQUIRED)
target_include_directories(
    DjInterop PRIVATE SYSTEM
    ${ZLIB_INCLUDE_DIRS})
target_link_libraries(
    DjInterop PUBLIC
    ${ZLIB_LIBRARIES})

if(SYSTEM_DATE_H)
    # Search for system installation of date.h and use that.
    find_package(date CONFIG REQUIRED)
    target_link_libraries(
        DjInterop PUBLIC
        date::date)
else()
    # Use bundled date.h (a header-only library).
    message(STATUS "Using bundled date.h...")
    target_include_directories(
        DjInterop PRIVATE SYSTEM
        ext/date)
endif()

if(SYSTEM_SQLITE)
    # Search for system installation of SQLite and use that.
    set(SQLITE_MIN_VERSION 3.11)
    find_package(SQLite3 ${SQLITE_MIN_VERSION} REQUIRED)
    target_include_directories(
        DjInterop PRIVATE SYSTEM
        ${SQLite3_INCLUDE_DIRS})
    target_link_libraries(
        DjInterop PUBLIC
        ${SQLite3_LIBRARIES})
    set(DJINTEROP_SQLITE_VERSION "${SQLite3_VERSION}")
else()
    # Use bundled SQLite amalgamation sources.
    message(STATUS "Using bundled SQLite...")
    target_sources(
        DjInterop PRIVATE
        ext/sqlite-amalgamation/sqlite3.c)
    target_compile_definitions(
        DjInterop PUBLIC
        SQLITE_OMIT_LOAD_EXTENSION)
    target_include_directories(
        DjInterop PRIVATE SYSTEM
        ext/sqlite-amalgamation)

    # Read the version out of the amalgamation rather than restating it here,
    # so that bumping the bundled copy is a matter of replacing two files.
    file(
        STRINGS ext/sqlite-amalgamation/sqlite3.h DJINTEROP_SQLITE_VERSION
        REGEX "^#define SQLITE_VERSION[ \t]+\"")
    string(
        REGEX REPLACE "^#define SQLITE_VERSION[ \t]+\"([^\"]+)\".*" "\\1"
        DJINTEROP_SQLITE_VERSION "${DJINTEROP_SQLITE_VERSION}")
    message(STATUS "Bundled SQLite is version ${DJINTEROP_SQLITE_VERSION}")
endif()

if(SYSTEM_SQLITE_MODERN_CPP)
    # Search for system installation of sqlite-modern-cpp and use that.
    find_path(SQLITE_MODERN_CPP_INCLUDE_DIRS "sqlite_modern_cpp.h")
    message(STATUS "Found sqlite-modern-cpp at ${SQLITE_MODERN_CPP_INCLUDE_DIRS}")
    target_include_directories(
        DjInterop PRIVATE SYSTEM
        ${SQLITE_MODERN_CPP_INCLUDE_DIRS})
else()
    # Use bundled sqlite-modern-cpp (a header-only library).
    message(STATUS "Using bundled sqlite-modern-cpp...")
    target_include_directories(
        DjInterop PRIVATE SYSTEM
        ext/sqlite_modern_cpp)
endif()

set_target_properties(DjInterop PROPERTIES C_VISIBILITY_PRESET hidden)
set_target_properties(DjInterop PROPERTIES CXX_VISIBILITY_PRESET hidden)

install(TARGETS DjInterop
        EXPORT DjInteropTargets
        ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}")

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/include/djinterop/config.hpp
    include/djinterop/album_art.hpp
    include/djinterop/crate.hpp
    include/djinterop/database.hpp
    include/djinterop/djinterop.hpp
    include/djinterop/exceptions.hpp
    include/djinterop/musical_key.hpp
    include/djinterop/pad_color.hpp
    include/djinterop/performance_data.hpp
    include/djinterop/playlist.hpp
    include/djinterop/semantic_version.hpp
    include/djinterop/stream_helper.hpp
    include/djinterop/track.hpp
    include/djinterop/track_snapshot.hpp
    DESTINATION "${DJINTEROP_INSTALL_INCLUDEDIR}")
install(FILES
    include/djinterop/engine/base_engine_library.hpp
    include/djinterop/engine/engine.hpp
    include/djinterop/engine/engine_schema.hpp
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
install(FILES
    include/djinterop/engine/v3/beat_data_blob.hpp
    include/djinterop/engine/v3/engine_library.hpp
    include/djinterop/engine/v3/information_table.hpp
    include/djinterop/engine/v3/loops_blob.hpp
    include/djinterop/engine/v3/overview_waveform_data_blob.hpp
    include/djinterop/engine/v3/playlist_entity_table.hpp
    include/djinterop/engine/v3/playlist_table.hpp
    include/djinterop/engine/v3/quick_cues_blob.hpp
    include/djinterop/engine/v3/track_data_blob.hpp
    include/djinterop/engine/v3/track_table.hpp
    DESTINATION "${DJINTEROP_INSTALL_INCLUDEDIR}/engine/v3")
install(FILES
    include/djinterop/onelibrary/onelibrary.hpp
    DESTINATION "${DJINTEROP_INSTALL_INCLUDEDIR}/onelibrary")


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
        DESTINATION "${DJINTEROP_INSTALL_CMAKEDIR}")

# Build example programs by default.
option(BUILD_EXAMPLES "Build example programs" ON)

if (BUILD_EXAMPLES)
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
    add_djinterop_example(onelibrary)
endif()

# Unit tests.
include(CTest)
find_package(Boost 1.71.0 QUIET COMPONENTS filesystem system)
if (Boost_FOUND)
    set(TESTDATA_DIR "${CMAKE_CURRENT_SOURCE_DIR}/testdata")

    function(add_djinterop_test test_dir test_name)
        string(REPLACE "/" "_" test_executable_name ${test_dir}${test_name})
        add_executable(${test_executable_name}
                test/djinterop/${test_dir}${test_name}.cpp)

        target_compile_definitions(${test_executable_name} PUBLIC
                -DTESTDATA_DIR=${TESTDATA_DIR})
        if (MSVC)
            # Some unit tests are rather large and exceed max sections on VC++.
            target_compile_options(${test_executable_name} PRIVATE /bigobj)
        endif()

        target_include_directories(${test_executable_name} PUBLIC
                ${Boost_INCLUDE_DIRS}
                ${CMAKE_CURRENT_BINARY_DIR}/include
                include)
        # A test that compiles library sources into itself, rather than only
        # linking, needs the headers that those sources include.
        target_include_directories(${test_executable_name} PRIVATE SYSTEM
                $<TARGET_PROPERTY:DjInterop,INCLUDE_DIRECTORIES>)
        target_link_libraries(${test_executable_name} PUBLIC
                DjInterop
                ${Boost_LIBRARIES})
        add_test(NAME ${test_executable_name} COMMAND ${test_executable_name})
    endfunction()

    add_djinterop_test("" semantic_version_test)
    add_djinterop_test(engine/ crate_test)
    add_djinterop_test(engine/ database_reference_test)
    add_djinterop_test(engine/ database_test)
    add_djinterop_test(engine/ engine_test)
    add_djinterop_test(engine/ playlist_test)
    add_djinterop_test(engine/ track_test)
    add_djinterop_test(engine/v2/ playlist_entity_table_test)
    add_djinterop_test(engine/v2/ playlist_table_test)
    add_djinterop_test(engine/v2/ track_table_test)
    add_djinterop_test(engine/v3/ performance_data_table_test)
    add_djinterop_test(engine/v3/ track_table_test)

    # Some parts of the library are internal, and its symbols are hidden, so
    # tests of them compile those sources into themselves rather than linking.
    #
    # A test that talks to SQLite directly cannot borrow it from the library
    # either, for the same reason: with the bundled amalgamation, SQLite is
    # compiled into the library and hidden along with everything else, leaving
    # nothing to link against.  Such a test compiles SQLite as well.  A system
    # installation is a library in its own right, and linking it is enough.
    if(SYSTEM_SQLITE)
        set(DJINTEROP_TEST_SQLITE_SOURCES "")
    else()
        set(DJINTEROP_TEST_SQLITE_SOURCES ext/sqlite-amalgamation/sqlite3.c)
    endif()

    add_djinterop_test(onelibrary/ content_table_test)
    target_sources(onelibrary_content_table_test PRIVATE
            src/djinterop/onelibrary/content_table.cpp
            ${DJINTEROP_TEST_SQLITE_SOURCES})

    # A decrypted database is handed to SQLite through `sqlite3_deserialize`,
    # which arrived in 3.36.  Built against anything older the library reports
    # the format unsupported, and this test would only be watching it say so.
    if(NOT DJINTEROP_SQLITE_VERSION VERSION_LESS 3.36)
        add_djinterop_test(onelibrary/ database_test)
        target_sources(onelibrary_database_test PRIVATE
                src/djinterop/util/crypto/aes.cpp
                src/djinterop/util/crypto/aes_hardware.cpp
                src/djinterop/util/crypto/sha512.cpp
                src/djinterop/util/crypto/sqlcipher_codec.cpp
                src/djinterop/util/filesystem.cpp
                ${DJINTEROP_TEST_SQLITE_SOURCES})
    else()
        message(
            STATUS
            "OneLibrary database test not available, as reading the format "
            "needs SQLite 3.36 or newer")
    endif()

    add_djinterop_test(onelibrary/ playlist_table_test)
    target_sources(onelibrary_playlist_table_test PRIVATE
            src/djinterop/onelibrary/playlist_table.cpp
            ${DJINTEROP_TEST_SQLITE_SOURCES})

    add_djinterop_test(util/ crypto_test)
    target_sources(util_crypto_test PRIVATE
            src/djinterop/util/crypto/aes.cpp
            src/djinterop/util/crypto/aes_hardware.cpp
            src/djinterop/util/crypto/sha512.cpp
            src/djinterop/util/crypto/sqlcipher_codec.cpp)
else()
    message(STATUS "Unit tests not available, as Boost cannot be found")
endif()


include(InstallRequiredSystemLibraries)
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
include(CPack)

# Add documentation option
option(BUILD_DOC "Build documentation" ON)

# Check if Doxygen is installed
find_package(Doxygen OPTIONAL_COMPONENTS dot dia)
if (DOXYGEN_FOUND)
    set(DOXYGEN_HTML_OUTPUT docs)
    set(DOXYGEN_USE_MDFILE_AS_MAINPAGE README.md)

    message("Doxygen build started")

    doxygen_add_docs(
            docs
            include
            README.md
            COMMENT "Generate docs"
    )
else (DOXYGEN_FOUND)
    message("Doxygen need to be installed to generate the doxygen documentation")
endif (DOXYGEN_FOUND)
