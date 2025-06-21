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
logging_sources = files(
  'lib/logging.cpp',
)

logging_headers = files(
  'include/logging.h'
)

dependencies = [
    quill_dep,
]

liblogging = library('logging',
    logging_sources, 
    include_directories: include_directories('include'),
    cpp_args: ['-fvisibility=default'],
    install : true,
    dependencies: dependencies
    )

logging_dep = declare_dependency(
    include_directories: include_directories('include'),
    link_with: liblogging,
    dependencies: dependencies
)

install_headers(logging_headers, subdir : 'fourdst/fourdst/logging')
