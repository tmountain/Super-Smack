# Copyright (C) 2000 MySQL AB & MySQL Finland AB & TCX DataKonsult AB
# 
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

# Process this file with automake to create Makefile.in

AUTOMAKE_OPTIONS=foreign
SUBDIRS=src
EXTRA_DIST=README INSTALL TUTORIAL MANUAL
SMACKS_DIR= @SMACKS_DIR@
DATADIR= @DATADIR@

install-data-local:
	$(mkinstalldirs) $(SMACKS_DIR) $(DATADIR)
	cp -rp $(srcdir)/smacks/* $(SMACKS_DIR)


dist-hook:
	mkdir -p $(distdir)/smacks
	cp -p $(srcdir)/smacks/*.smack $(distdir)/smacks

# Don't update the files from bitkeeper
%::SCCS/s.%
