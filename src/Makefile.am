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

INCLUDES = @MYSQL_INCLUDE@ @PGSQL_INCLUDE@ @ORACLE_INCLUDE@

noinst_PROGRAMS=test-dictionary test-client test_tcp_client
bin_PROGRAMS=super-smack gen-data
test_dictionary_SOURCES=test-dictionary.cc \
	dictionary.cc dictionary.h \
	die.cc die.h \
	smack.h libsmack.cc
test_tcp_client_SOURCES=test_tcp_client.cc tcp_client.cc tcp_client.h
test_client_SOURCES=\
	test-client.cc \
	client.cc client.h \
	engines.cc engines.h \
	die.cc die.h \
	dictionary.cc dictionary.h \
	query.cc query.h \
	smack.h libsmack.cc \
	mysql-client.cc mysql-client.h \
	pg-client.cc pg-client.h \
	ora-client.cc ora-client.h
test_client_LDADD = @MYSQL_LIB@ @PGSQL_LIB@ @ORACLE_LIB@


super_smack_SOURCES=\
	super-smack.cc super-smack.h\
	client.cc client.h \
	engines.cc engines.h \
	die.cc die.h \
        dictionary.cc dictionary.h \
	super-smack-yacc.yy \
        super-smack-yacc.h \
	super-smack-lex.ll \
	query.cc query.h \
        parse.cc parse.h \
	val.h smack.h libsmack.cc \
	mysql-client.cc mysql-client.h \
	pg-client.cc pg-client.h \
	ora-client.cc ora-client.h \
	tcp_client.h tcp_client.cc
super_smack_LDADD = @MYSQL_LIB@ @PGSQL_LIB@ @ORACLE_LIB@

gen_data_SOURCES=gen-data.cc die.cc die.h

# yacc needs to generate a y.tab.h
YFLAGS = -d

# extra files to delete with ``make clean''
CLEANFILES = super-smack-yacc.cc super-smack-yacc.h super-smack-lex.cc

# Don't update the files from bitkeeper
%::SCCS/s.%
