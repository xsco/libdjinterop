project(
    'djinterop',
    'cpp', 'c',
    version: '0.12.0',
    license: 'LGPL-3.0',
    default_options: ['cpp_std=c++17', 'default_library=both'])

cpp_compiler = meson.get_compiler('cpp')
if cpp_compiler.get_id() == 'msvc'
    # Ask MSVC to populate the __cplusplus macro properly.
    add_global_arguments('/Zc:__cplusplus', language: 'cpp')
endif

# libdl is "special", not like other dependencies
dl_dep = cpp_compiler.find_library('dl', required: false)

thread_dep = dependency('threads')
sqlite3_dep = dependency('sqlite3', version: '>=3.11.0', fallback: ['sqlite', 'sqlite_dep'])
zlib_dep = dependency('zlib', version: '>=1.2.8', fallback: ['zlib', 'zlib_dep'])

sqlite_modern_cpp_dep = declare_dependency(
    dependencies: [sqlite3_dep],
    include_directories: 'ext/sqlite_modern_cpp')

inc = include_directories('include')

subdir('include')
subdir('src')
subdir('testdata')
subdir('test')
subdir('example')

# TODO - add Doxygen support

pkg_mod = import('pkgconfig')
pkg_mod.generate(libraries : djinterop_lib,
                 version : meson.project_version(),
                 name : 'lib' + meson.project_name(),
                 filebase : meson.project_name(),
                 description : 'Provides access to database formats used ' +
                    'to store information about DJ record libraries')

