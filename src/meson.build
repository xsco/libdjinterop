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
    'sqlite3.c'
]

# Used by unit tests to reference internal classes.
internal_inc = include_directories('.')

djinterop_lib = library(
    'djinterop',
    sources: sources,
    include_directories: inc,
    dependencies: core_deps,
    install: true)

