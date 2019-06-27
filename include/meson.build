public_header_files = [
    'djinterop/enginelibrary.hpp',
]
el_public_header_files = [
    'djinterop/enginelibrary/album_art.hpp',
    'djinterop/enginelibrary/crate.hpp',
    'djinterop/enginelibrary/database.hpp',
    'djinterop/enginelibrary/musical_key.hpp',
    'djinterop/enginelibrary/pad_color.hpp',
    'djinterop/enginelibrary/performance_data.hpp',
    'djinterop/enginelibrary/schema_version.hpp',
    'djinterop/enginelibrary/track.hpp'
]

# Meson appears to flatten header files into the subdir when installing, so we
# separate the files and perform this little hack instead.
install_headers(public_header_files, subdir: 'djinterop')
install_headers(el_public_header_files, subdir: 'djinterop/enginelibrary')

