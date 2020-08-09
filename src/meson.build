sources = [
    'djinterop/enginelibrary/el_crate_impl.cpp',
    'djinterop/enginelibrary/el_database_impl.cpp',
    'djinterop/enginelibrary/el_storage.cpp',
    'djinterop/enginelibrary/el_track_impl.cpp',
    'djinterop/enginelibrary/el_transaction_guard_impl.cpp',
    'djinterop/enginelibrary/encode_decode_utils.cpp',
    'djinterop/enginelibrary/performance_data_format.cpp',
    'djinterop/enginelibrary/schema_1_6_0.cpp',
    'djinterop/enginelibrary/schema_1_7_1.cpp',
    'djinterop/enginelibrary/schema.cpp',
    'djinterop/crate.cpp',
    'djinterop/database.cpp',
    'djinterop/enginelibrary.cpp',
    'djinterop/track.cpp',
    'djinterop/transaction_guard.cpp',
    'djinterop/impl/crate_impl.cpp',
    'djinterop/impl/database_impl.cpp',
    'djinterop/impl/track_impl.cpp',
    'djinterop/impl/transaction_guard_impl.cpp',
    'djinterop/impl/util.cpp',
]

# Dependencies required by the main library.
core_deps = [thread_dep, dl_dep, sqlite3_dep, zlib_dep, sqlite_modern_cpp_dep]

# Used by unit tests to reference internal classes.
internal_inc = include_directories('.')

# Set a compile-time definition that we are building the libdjinterop source.
# This information is needed in `config.hpp` to determine whether public symbols
# ought to be exported or imported.
building_library_args = ['-DDJINTEROP_SOURCE']

# Set hidden visibility arguments as required.
hidden_visibility_args = []
if get_option('default_library') != 'static'
    if cpp_compiler.has_argument('-fvisibility=hidden')
        hidden_visibility_args = ['-fvisibility=hidden']
    endif
endif

djinterop_lib = library(
    'djinterop',
    sources: sources,
    include_directories: inc,
    dependencies: core_deps,
    install: true,
    cpp_args: building_library_args + hidden_visibility_args)

