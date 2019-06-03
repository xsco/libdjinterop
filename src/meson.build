sources = [
    'enginelibrary/crate.cpp',
    'enginelibrary/database.cpp',
    'enginelibrary/encode_decode_utils.cpp',
    'enginelibrary/performance_data_format.cpp',
    'enginelibrary/performance_data.cpp',
    'enginelibrary/schema_1_6_0.cpp',
    'enginelibrary/schema_1_7_1.cpp',
    'enginelibrary/schema.cpp',
    'enginelibrary/track.cpp',
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

