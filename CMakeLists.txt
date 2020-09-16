# libdjinterop CMake file
#
# This minimal CMake build script is provided for simpler integration with
# projects that wish to include libdjinterop in an "in source tree" fashion.
#
# The meson/ninja build should be preferred in all other cases.
#
cmake_minimum_required(VERSION 3.10)
project(libdjinterop VERSION 0.13.0)

# Require C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Require sqlite3 >= 3.11.0
find_package(SQLite3 3.11.0 REQUIRED)

# Require zlib >= 1.2.8
find_package(ZLIB 1.2.8 REQUIRED)

# Generate config.hpp based on build-time environment.
include(CheckIncludeFileCXX)
CHECK_INCLUDE_FILE_CXX(optional DJINTEROP_STD_OPTIONAL)
CHECK_INCLUDE_FILE_CXX(experimental/optional DJINTEROP_STD_EXPERIMENTAL_OPTIONAL)
configure_file(
    include/djinterop/config.hpp.in
    include/djinterop/config.hpp)

add_library(
    djinterop
    src/djinterop/enginelibrary/el_crate_impl.cpp
    src/djinterop/enginelibrary/el_database_impl.cpp
    src/djinterop/enginelibrary/el_storage.cpp
    src/djinterop/enginelibrary/el_track_impl.cpp
    src/djinterop/enginelibrary/el_transaction_guard_impl.cpp
    src/djinterop/enginelibrary/encode_decode_utils.cpp
    src/djinterop/enginelibrary/performance_data_format.cpp
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
    src/djinterop/crate.cpp
    src/djinterop/database.cpp
    src/djinterop/enginelibrary.cpp
    src/djinterop/track.cpp
    src/djinterop/transaction_guard.cpp
    src/djinterop/impl/crate_impl.cpp
    src/djinterop/impl/database_impl.cpp
    src/djinterop/impl/track_impl.cpp
    src/djinterop/impl/transaction_guard_impl.cpp
        src/djinterop/util.cpp)

target_compile_definitions(
    djinterop PUBLIC
    DJINTEROP_SOURCE)

target_include_directories(
    djinterop PUBLIC
    ${SQLite3_INCLUDE_DIRS}
    ${ZLIB_INCLUDE_DIRS}
    ext/sqlite_modern_cpp
    ${CMAKE_CURRENT_BINARY_DIR}/include
    include src)

target_link_libraries(
    djinterop
    ${SQLite3_LIBRARIES}
    ${ZLIB_LIBRARIES})

install(TARGETS djinterop DESTINATION lib)
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
    DESTINATION include/djinterop)

