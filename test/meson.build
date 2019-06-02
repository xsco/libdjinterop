boost_test_dep = dependency('boost', modules : [
    'system', 'filesystem', 'unit_test_framework'])

engine_library_test_names = [
    'crate_test',
    'database_test',
    'performance_data_test',
    'schema_test',
    'track_test'
]

foreach test_name : engine_library_test_names
    exe = executable(
        'el_' + test_name,
        'enginelibrary/' + test_name + '.cpp',
        cpp_args : ['-DTESTDATA_DIR=' + testdata_dir],
        include_directories : [inc, internal_inc],
        dependencies : [boost_test_dep],
        link_with : djinterop_lib)
    test(test_name, exe)
endforeach

