# ***********************************************************************
#
#   Copyright (C) 2025 -- The 4D-STAR Collaboration
#   File Author: Emily Boudreaux
#   Last Modified: March 19, 2025
#
#   liblogging is free software; you can use it and/or modify
#   it under the terms and restrictions the GNU General Library Public
#   License version 3 (GPLv3) as published by the Free Software Foundation.
#
#   liblogging is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
#   See the GNU Library General Public License for more details.
#
#   You should have received a copy of the GNU Library General Public License
#   along with this software; if not, write to the Free Software
#   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
#
# *********************************************************************** #
project('liblogging', 'cpp', version: 'v1.1.6', default_options: ['cpp_std=c++23'], meson_version: '>=1.5.0')

# Add default visibility for all C++ targets
add_project_arguments('-fvisibility=default', language: 'cpp')

if get_option('build_python')
    local_py_install = import('python').find_installation('python3', pure: false)
    fourdst_pkg = local_py_install.get_install_dir() / 'fourdst'
    logging_libdir             = fourdst_pkg / 'lib'
    logging_header_install_dir = fourdst_pkg / 'include' / 'fourdst' / 'logging'

    vendor_lib_dir             = fourdst_pkg / 'lib' / 'vendor'
    vendor_header_dir          = fourdst_pkg / 'include' / 'fourdst' / 'vendor'

    message('Installing liblogging in python mode. Install locations are...')
else
    logging_libdir             = get_option('libdir')
    logging_header_install_dir = get_option('includedir') / 'fourdst' / 'logging'

    vendor_lib_dir            = get_option('libdir') / 'fourdst' / 'vendor'
    vendor_header_dir         = get_option('includedir') / 'fourdst' / 'vendor'

    message('Installing liblogging in non python (normal) mode. Install locations are...')
endif
message('    Package Libraries: ' + logging_libdir)
message('    Package Headers:   ' + logging_header_install_dir)
message('    Vendor Libraries:  ' + vendor_lib_dir)
message('    Vendor Headers:    ' + vendor_header_dir)

subdir('build-config')
subdir('src')

if get_option('build_tests')
  subdir('tests')
endif

if get_option('build_examples')
  subdir('examples')
endif

if get_option('pkg_config')
    message('Generating pkg-config file for liblogging...')
    vendor_inc_flags = '-I' + get_option('prefix') / vendor_header_dir
    pkg = import('pkgconfig')
    pkg.generate(
        name: 'liblogging',
        description: 'Logging manager for SERiF and related projects',
        version: meson.project_version(),
        libraries: [liblogging,'-Wl,-rpath,${libdir}'],
        filebase: 'fourdst_logging',
        install_dir: join_paths(get_option('libdir'), 'pkgconfig'),
        extra_cflags: vendor_inc_flags
    )
endif
