project(
    'djinterop',
    'cpp', 'c',
    version: '0.9.0',
    license: 'LGPL-3.0',
    default_options: ['cpp_std=c++17', 'c_std=c11'])

add_global_arguments('-D_MODERN_SQLITE_BOOST_OPTIONAL_SUPPORT', language: 'cpp')

# libdl is "special", not like other dependencies
cpp_compiler = meson.get_compiler('cpp')
dl_dep = cpp_compiler.find_library('dl', required: false)

thread_dep = dependency('threads')
zlib_dep = dependency('zlib', version: '>=1.2.8', fallback: ['zlib', 'zlib_dep'])
boost_dep = dependency('boost', version: '>=1.65') # Header-only libraries

core_deps = [thread_dep, dl_dep, zlib_dep, boost_dep]

inc = include_directories('include')

subdir('include')
subdir('src')
subdir('testdata')
subdir('test')

# TODO - add Doxygen support

pkg_mod = import('pkgconfig')
pkg_mod.generate(libraries : djinterop_lib,
                 version : meson.project_version(),
                 name : 'lib' + meson.project_name(),
                 filebase : meson.project_name(),
                 description : 'Provides access to database formats used ' +
                    'to store information about DJ record libraries')

