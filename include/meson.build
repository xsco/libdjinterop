# Used only to generate config.hpp in the nested include dir.
subdir('djinterop')

djinterop_header_files = [
    'djinterop/album_art.hpp',
    'djinterop/crate.hpp',
    'djinterop/database.hpp',
    'djinterop/djinterop.hpp',
    'djinterop/exceptions.hpp',
    'djinterop/musical_key.hpp',
    'djinterop/optional.hpp',
    'djinterop/pad_color.hpp',
    'djinterop/performance_data.hpp',
    'djinterop/semantic_version.hpp',
    'djinterop/track.hpp',
    'djinterop/track_snapshot.hpp',
    'djinterop/transaction_guard.hpp'
]
djinterop_engine_header_files = [
    'djinterop/engine/engine.hpp',
    'djinterop/engine/engine_version.hpp',
]
djinterop_engine_v2_header_files = [
    'djinterop/engine/v2/beat_data_blob.hpp',
    'djinterop/engine/v2/change_log_table.hpp',
    'djinterop/engine/v2/engine_library.hpp',
    'djinterop/engine/v2/information_table.hpp',
    'djinterop/engine/v2/loops_blob.hpp',
    'djinterop/engine/v2/overview_waveform_data_blob.hpp',
    'djinterop/engine/v2/playlist_entity_table.hpp',
    'djinterop/engine/v2/playlist_table.hpp',
    'djinterop/engine/v2/quick_cues_blob.hpp',
    'djinterop/engine/v2/track_data_blob.hpp',
    'djinterop/engine/v2/track_table.hpp',
]

install_headers(djinterop_header_files, subdir: 'djinterop')
install_headers(djinterop_engine_header_files, subdir: 'djinterop/engine')
install_headers(djinterop_engine_v2_header_files, subdir: 'djinterop/engine/v2')
