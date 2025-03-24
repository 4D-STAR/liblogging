# Define the library
probe_sources = files(
  'private/probe.cpp',
)

probe_headers = files(
    'public/probe.h'
)

dependencies = [
    config_dep,
    mfem_dep,
    quill_dep
]

# Define the liblogger library so it can be linked against by other parts of the build system
libprobe = static_library('probe',
    probe_sources, 
    include_directories: include_directories('public'),
    cpp_args: ['-fvisibility=default'],
    install : true,
    dependencies: dependencies
    )

probe_dep = declare_dependency(
    include_directories: include_directories('public'),
    link_with: libprobe,
    dependencies: dependencies
)