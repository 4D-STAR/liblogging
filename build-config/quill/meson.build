quill_cmake_options = cmake.subproject_options()
quill_cmake_options.add_cmake_defines({
                                     'BUILD_SHARED_LIBS': 'ON',
                                     'CMAKE_SKIP_INSTALL_RULES': 'ON'
})
quill_sp = cmake.subproject(
    'quill',
    options: quill_cmake_options,
)
quill_dep = quill_sp.dependency('quill')

message('Registering quill headers (' + meson.global_source_root() + '/subprojects/quill/include/quill) for installation...')
quill_headers = meson.global_source_root() + '/subprojects/quill/include/quill'
install_subdir(quill_headers, install_dir: get_option('includedir'))
message('Done registering quill headers for installation!')
