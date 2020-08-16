# Generate config file based on build-time feature detection.
# Note that most build configuration for public headers is handled in the
# parent build file; this file exists solely because the `output` parameter
# of configure_file() does not, at the time of writing (meson 0.55.0), support
# writing the output file to a different directory.
conf_data = configuration_data()
if cpp_compiler.has_header('optional')
    conf_data.set10('DJINTEROP_STD_OPTIONAL', true)
endif
if cpp_compiler.has_header('experimental/optional')
    conf_data.set10('DJINTEROP_STD_EXPERIMENTAL_OPTIONAL', true)
endif
configure_file(
    input: 'config.hpp.in',
    output: 'config.hpp',
    configuration: conf_data,
    format: 'cmake',
    install_dir: get_option('includedir') + '/djinterop'
)

