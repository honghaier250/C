#
# Copyright (C) 1997, 1998, 1999, 2000, 2001, 2002, 2003, 2004, 2005,
# 2006, 2007, 2008, 2009 Free Software Foundation, Inc.
#
# This file is part of GNU Inetutils.
#
# GNU Inetutils is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or (at
# your option) any later version.
#
# GNU Inetutils is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see `http://www.gnu.org/licenses/'.

bin_PROGRAMS = @talk_BUILD@

EXTRA_PROGRAMS = talk

talk_SOURCES = ctl.c ctl_transact.c display.c get_addrs.c get_names.c init_disp.c invite.c io.c look_up.c msgs.c talk.c

noinst_HEADERS = talk.h talk_ctl.h

INCLUDES = -I$(top_srcdir)/lib -I../lib -I$(top_srcdir)/libinetutils $(NCURSES_INCLUDE)

LDADD = $(LIBCURSES) -L../libinetutils -linetutils ../lib/libgnu.a
