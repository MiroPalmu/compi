project(
    'meson_template',
    'cpp',
    default_options : ['warning_level=3', 'werror=true', 'optimization=3', 'cpp_std=c++17']
)


####################################################################################################
# Variables tracking sources and libraries
####################################################################################################
# Directories where compiler is able to include from
project_include_directories = []
project_include_directories += include_directories('include')

# These might be used later eg. for formatting
# .hpp files
project_header_files = []
# .cpp files
project_sources = []


####################################################################################################
# Subprojects (using meson wraps)
####################################################################################################

catch2 = subproject('catch2')

####################################################################################################
# Dependencies
####################################################################################################

project_dependencies = []

project_dependencies += dependency('catch2-with-main')

# Conan
conan_pkgs= {
    # Names must match the package that cmake looks for
    #'fmt': 'fmt/9.0.0@',  # <- Must contain @, otherwise Conan will think it is a path

    # ... the dependency list goes on
}
# Adding new dependencies to this dict is error-free, but if you
# change an existing entry, e.g. the pakage version, you probably need
# `meson configure --clearcache` to let new setting takes effect.

conan_program = find_program(['conan'], required: true, native: true)

foreach pkg_name, conan_ref : conan_pkgs
    module_path = meson.current_build_dir() / 'conan-cmake' / pkg_name
    run_command(conan_program, 'install', conan_ref, '-if', module_path,
        '-g', 'cmake_find_package', check: true)
    project_dependencies += dependency(pkg_name, method: 'cmake', cmake_module_path: module_path)
endforeach

####################################################################################################
# Subdirectors
####################################################################################################

subdir('docs')
subdir('include')
subdir('src')
subdir('tests')

####################################################################################################
# Executable
####################################################################################################

executable(
    'temp_executable',
    project_sources,
    include_directories : project_include_directories,
    dependencies : project_dependencies
)
