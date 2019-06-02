public_header_files = [
    'enginelibrary.hpp',
]
el_public_header_files = [
    'enginelibrary/album_art.hpp',
    'enginelibrary/crate.hpp',
    'enginelibrary/database.hpp',
    'enginelibrary/musical_key.hpp',
    'enginelibrary/pad_colour.hpp',
    'enginelibrary/performance_data.hpp',
    'enginelibrary/schema_version.hpp',
    'enginelibrary/track.hpp'
]

# Meson appears to flatten header files into the subdir when installing, so we
# separate the files and perform this little hack instead.
install_headers(public_header_files, subdir: 'djinterop')
install_headers(el_public_header_files, subdir: 'djinterop/enginelibrary')

