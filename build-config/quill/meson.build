cmake = import('cmake')

quill_cmake_options = cmake.subproject_options()
quill_cmake_options.add_cmake_defines({
    'BUILD_STATIC_LIBS': 'ON',
    'BUILD_SHARED_LIBS': 'OFF',
    'CMAKE_SKIP_INSTALL_RULES': 'ON',
    'CMAKE_POSITION_INDEPENDENT_CODE': 'ON' 
})

quill_sp = cmake.subproject(
    'quill',
    options: quill_cmake_options,
)

quill_dep = quill_sp.dependency('quill')

if get_option('default_library') != 'static'
    message('Registering quill headers for installation...')
    quill_headers = meson.global_source_root() + '/subprojects/quill/include/quill'
    install_subdir(quill_headers, install_dir: get_option('includedir'))
endif
