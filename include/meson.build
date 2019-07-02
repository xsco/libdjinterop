public_header_files = [
    # TODO
]
el_public_header_files = [
    'djinterop/album_art.hpp',
    'djinterop/crate.hpp',
    'djinterop/database.hpp',
    'djinterop/musical_key.hpp',
    'djinterop/pad_color.hpp',
    'djinterop/performance_data.hpp',
    'djinterop/semantic_version.hpp',
    'djinterop/track.hpp'
]

# Meson appears to flatten header files into the subdir when installing, so we
# separate the files and perform this little hack instead.
install_headers(public_header_files, subdir: 'djinterop')
install_headers(el_public_header_files, subdir: 'djinterop/enginelibrary')

