/* provide a replacement openat function
   Copyright (C) 2004-2006, 2008-2009 Free Software Foundation, Inc.

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

/* written by Jim Meyering */

#ifndef _GL_HEADER_OPENAT
#define _GL_HEADER_OPENAT

#include <fcntl.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <stdbool.h>

#ifndef __attribute__
# if __GNUC__ < 2 || (__GNUC__ == 2 && __GNUC_MINOR__ < 8)
#  define __attribute__(x) /* empty */
# endif
#endif

#ifndef ATTRIBUTE_NORETURN
# define ATTRIBUTE_NORETURN __attribute__ ((__noreturn__))
#endif

#if !HAVE_OPENAT

int openat_permissive (int fd, char const *file, int flags, mode_t mode,
                       int *cwd_errno);
bool openat_needs_fchdir (void);

#else

# define openat_permissive(Fd, File, Flags, Mode, Cwd_errno) \
    openat (Fd, File, Flags, Mode)
# define openat_needs_fchdir() false

#endif

void openat_restore_fail (int) ATTRIBUTE_NORETURN;
void openat_save_fail (int) ATTRIBUTE_NORETURN;

/* Using these function names makes application code
   slightly more readable than it would be with
   fchownat (..., 0) or fchownat (..., AT_SYMLINK_NOFOLLOW).  */
static inline int
chownat (int fd, char const *file, uid_t owner, gid_t group)
{
  return fchownat (fd, file, owner, group, 0);
}

static inline int
lchownat (int fd, char const *file, uid_t owner, gid_t group)
{
  return fchownat (fd, file, owner, group, AT_SYMLINK_NOFOLLOW);
}

static inline int
chmodat (int fd, char const *file, mode_t mode)
{
  return fchmodat (fd, file, mode, 0);
}

static inline int
lchmodat (int fd, char const *file, mode_t mode)
{
  return fchmodat (fd, file, mode, AT_SYMLINK_NOFOLLOW);
}

static inline int
statat (int fd, char const *name, struct stat *st)
{
  return fstatat (fd, name, st, 0);
}

static inline int
lstatat (int fd, char const *name, struct stat *st)
{
  return fstatat (fd, name, st, AT_SYMLINK_NOFOLLOW);
}

#if GNULIB_FACCESSAT
/* For now, there are no wrappers named laccessat or leuidaccessat,
   since gnulib doesn't support faccessat(,AT_SYMLINK_NOFOLLOW) and
   since access rights on symlinks are of limited utility.  */

static inline int
accessat (int fd, char const *file, int mode)
{
  return faccessat (fd, file, mode, 0);
}

static inline int
euidaccessat (int fd, char const *file, int mode)
{
  return faccessat (fd, file, mode, AT_EACCESS);
}
#endif

#endif /* _GL_HEADER_OPENAT */
