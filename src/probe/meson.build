# Define the library
probe_sources = files(
  'private/probe.cpp',
)

probe_headers = files(
    'public/probe.h'
)

# Define the liblogger library so it can be linked against by other parts of the build system
libprobe = static_library('probe',
    probe_sources, 
    include_directories: include_directories('public'),
    cpp_args: ['-fvisibility=default'],
    install : true,
    dependencies: [config_dep, mfem_dep, quill_dep, warning_control_dep]
    )

probe_dep = declare_dependency(
    include_directories: include_directories('public'),
    link_with: libprobe,
)