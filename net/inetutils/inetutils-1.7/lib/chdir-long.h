/* provide a chdir function that tries not to fail due to ENAMETOOLONG
   Copyright (C) 2004, 2005 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* Written by Jim Meyering.  */

#include <unistd.h>
#include <limits.h>

#ifndef PATH_MAX
# ifdef MAXPATHLEN
#  define PATH_MAX MAXPATHLEN
# endif
#endif

/* On systems without PATH_MAX, presume that chdir accepts
   arbitrarily long directory names.  */
#ifndef PATH_MAX
# define chdir_long(Dir) chdir (Dir)
#else
int chdir_long (char *dir);
#endif
