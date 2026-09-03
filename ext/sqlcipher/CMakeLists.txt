# SQLCipher vendored static library
#
# Builds a static library providing the SQLCipher encryption extension to
# SQLite.  The amalgamation in this directory is generated from the upstream
# SQLCipher repository (https://github.com/sqlcipher/sqlcipher), currently
# tracking v4.16.0.
#
# The library links OpenSSL for its crypto backend.

add_library(
    sqlcipher STATIC
    sqlite3.c)

target_compile_definitions(
    sqlcipher PRIVATE
    # Enable SQLCipher codec
    SQLITE_HAS_CODEC
    # Use OpenSSL as the crypto provider
    SQLCIPHER_CRYPTO_OPENSSL
    # Required SQLCipher init/shutdown hooks
    SQLITE_EXTRA_INIT=sqlcipher_extra_init
    SQLITE_EXTRA_SHUTDOWN=sqlcipher_extra_shutdown
    # Use memory for temp storage (required by SQLCipher)
    SQLITE_TEMP_STORE=2
    # Omit loadable extension support
    SQLITE_OMIT_LOAD_EXTENSION)

target_include_directories(
    sqlcipher PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR})

# SQLCipher needs OpenSSL for AES-256-CBC, PBKDF2-HMAC-SHA512
find_package(OpenSSL REQUIRED)
target_link_libraries(
    sqlcipher PUBLIC
    OpenSSL::SSL
    OpenSSL::Crypto)

# Suppress warnings in the vendored amalgamation -- not our code to fix.
if(NOT MSVC)
    target_compile_options(
        sqlcipher PRIVATE
        -Wno-pointer-arith
        -Wno-error)
endif()

# Must be position-independent to link into a shared library.
set_target_properties(sqlcipher PROPERTIES POSITION_INDEPENDENT_CODE ON)
