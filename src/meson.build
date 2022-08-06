sources = [
    'djinterop/engine/engine.cpp',
    'djinterop/engine/encode_decode_utils.cpp',
    'djinterop/engine/schema/schema_1_6_0.cpp',
    'djinterop/engine/schema/schema_1_7_1.cpp',
    'djinterop/engine/schema/schema_1_9_1.cpp',
    'djinterop/engine/schema/schema_1_11_1.cpp',
    'djinterop/engine/schema/schema_1_13_0.cpp',
    'djinterop/engine/schema/schema_1_13_1.cpp',
    'djinterop/engine/schema/schema_1_13_2.cpp',
    'djinterop/engine/schema/schema_1_15_0.cpp',
    'djinterop/engine/schema/schema_1_17_0.cpp',
    'djinterop/engine/schema/schema_1_18_0_desktop.cpp',
    'djinterop/engine/schema/schema_1_18_0_os.cpp',
    'djinterop/engine/schema/schema_2_18_0.cpp',
    'djinterop/engine/schema/schema.cpp',
    'djinterop/engine/v1/engine_crate_impl.cpp',
    'djinterop/engine/v1/engine_database_impl.cpp',
    'djinterop/engine/v1/engine_storage.cpp',
    'djinterop/engine/v1/engine_track_impl.cpp',
    'djinterop/engine/v1/engine_transaction_guard_impl.cpp',
    'djinterop/engine/v1/performance_data_format.cpp',
    'djinterop/engine/v2/beat_data_blob.cpp',
    'djinterop/engine/v2/change_log_table.cpp',
    'djinterop/engine/v2/database_impl.cpp',
    'djinterop/engine/v2/engine_library.cpp',
    'djinterop/engine/v2/information_table.cpp',
    'djinterop/engine/v2/loops_blob.cpp',
    'djinterop/engine/v2/overview_waveform_data_blob.cpp',
    'djinterop/engine/v2/playlist_table.cpp',
    'djinterop/engine/v2/playlist_entity_table.cpp',
    'djinterop/engine/v2/quick_cues_blob.cpp',
    'djinterop/engine/v2/track_data_blob.cpp',
    'djinterop/engine/v2/track_table.cpp',
    'djinterop/engine/v2/track_impl.cpp',
    'djinterop/crate.cpp',
    'djinterop/database.cpp',
    'djinterop/track.cpp',
    'djinterop/transaction_guard.cpp',
    'djinterop/util.cpp',
    'djinterop/impl/crate_impl.cpp',
    'djinterop/impl/database_impl.cpp',
    'djinterop/impl/track_impl.cpp',
    'djinterop/impl/transaction_guard_impl.cpp',
]

# Dependencies required by the main library.
core_deps = [
    thread_dep,
    dl_dep,
    zlib_dep,
    sqlite3_dep,
    sqlite_modern_cpp_dep,
    date_dep
]

# Set a compile-time definition that we are building the libdjinterop source.
# This information is needed in `config.hpp` to determine whether public symbols
# ought to be exported or imported.
building_library_args = ['-DDJINTEROP_SOURCE']

djinterop_lib = library(
    'djinterop',
    sources: sources,
    include_directories: inc,
    dependencies: core_deps,
    install: true,
    version: meson.project_version(),
    soversion: meson.project_version().split('.')[0],
    cpp_args: building_library_args)

