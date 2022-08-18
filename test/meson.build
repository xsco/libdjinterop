# Note: Boost Test is used in header-only mode.
boost_test_dep = dependency('boost', modules : ['system', 'filesystem'])

# Dependencies required by all unit test executables.
test_deps = [boost_test_dep, thread_dep]

# Core tests.
core_test_names = [
    'semantic_version_test'
]
foreach test_name : core_test_names
	exe = executable(
		test_name,
		'djinterop/' + test_name + '.cpp',
		cpp_args : ['-DTESTDATA_DIR=' + testdata_dir],
		include_directories : [inc],
		dependencies : test_deps,
		link_with : djinterop_lib)
	test(test_name, exe)
endforeach

# The high-level API covers the same concepts across different library types.
# Hence they should be tested equivalently.
high_level_test_names = [
    'crate_test',
    'database_test',
    'engine_test',
    'track_test',
    'track_snapshot_test'
]

# Engine high-level tests.
foreach test_name : high_level_test_names
	exe = executable(
		'engine_' + test_name,
		'djinterop/engine/' + test_name + '.cpp',
		cpp_args : ['-DTESTDATA_DIR=' + testdata_dir],
		include_directories : [inc],
		dependencies : test_deps,
		link_with : djinterop_lib)
	test('engine_' + test_name, exe)
endforeach

# Engine v2 low-level tests.
engine_v2_test_names = [
    'track_table_test',
]
foreach test_name : engine_v2_test_names
	exe = executable(
		'engine_v2_' + test_name,
		'djinterop/engine/v2/' + test_name + '.cpp',
		cpp_args : ['-DTESTDATA_DIR=' + testdata_dir],
		include_directories : [inc],
		dependencies : test_deps,
		link_with : djinterop_lib)
	test('engine_v2_' + test_name, exe)
endforeach
