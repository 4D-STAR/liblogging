# Define the library
logger_sources = files(
  'private/logger.cpp',
)

logger_headers = files(
    'public/logger.h'
)

# Define the liblogger library so it can be linked against by other parts of the build system
liblogger = library('logger',
    logger_sources, 
    include_directories: include_directories('public'),
    cpp_args: ['-fvisibility=default'],
    install : true)

logger_dep = declare_dependency(
    include_directories: include_directories('public'),
    link_with: liblogger,
)
# Make headers accessible
install_headers(logger_headers, subdir : '4DSSE/logger')