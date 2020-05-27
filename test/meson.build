boost_test_dep = dependency('boost', modules : ['system', 'filesystem'])

# Dependencies required by all unit test executables.
test_deps = [boost_test_dep, sqlite3_dep]

engine_library_test_names = [
    'crate_test',
    'database_test',
    'performance_data_test',
    'schema_test',
    'track_test'
]

# Don't run unit tests under MSVC for now, as there are linker errors
# which I don't have time to investigate properly.
# (Briefly, MSVC looks for djinterop.lib and doesn't use libdjinterop.a)
compiler = meson.get_compiler('cpp')
if compiler.get_id() != 'msvc'
    foreach test_name : engine_library_test_names
        exe = executable(
            'el_' + test_name,
            'enginelibrary/' + test_name + '.cpp',
            cpp_args : ['-DTESTDATA_DIR=' + testdata_dir],
            include_directories : [inc, internal_inc],
            dependencies : test_deps,
            link_with : djinterop_lib)
        test(test_name, exe)
    endforeach
endif
