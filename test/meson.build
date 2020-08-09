boost_test_dep = dependency('boost', modules : ['system', 'filesystem'])

# Dependencies required by all unit test executables.
test_deps = [boost_test_dep, sqlite3_dep, sqlite_modern_cpp_dep]

engine_library_test_names = [
    'crate_test',
    'database_test',
    'enginelibrary_test',
    'performance_data_test',
    'track_test'
]

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
