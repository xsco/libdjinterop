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

install_headers(djinterop_header_files, subdir: 'djinterop')
install_headers(djinterop_engine_header_files, subdir: 'djinterop/engine')
