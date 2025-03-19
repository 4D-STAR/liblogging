# ***********************************************************************
#
#   Copyright (C) 2025 -- The 4D-STAR Collaboration
#   File Author: Emily Boudreaux
#   Last Modified: February 28, 2025
#
#   4DSSE is free software; you can use it and/or modify
#   it under the terms and restrictions the GNU General Library Public
#   License version 3 (GPLv3) as published by the Free Software Foundation.
#
#   4DSSE is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
#   See the GNU Library General Public License for more details.
#
#   You should have received a copy of the GNU Library General Public License
#   along with this software; if not, write to the Free Software
#   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
#
# *********************************************************************** #
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
    dependencies: [config_dep, mfem_dep, quill_dep, macros_dep]
    )

probe_dep = declare_dependency(
    include_directories: include_directories('public'),
    link_with: libprobe,
)