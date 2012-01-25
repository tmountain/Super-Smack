/* Copyright (C) 2000 MySQL AB & MySQL Finland AB & TCX DataKonsult AB

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA */

#ifndef SUPER_SMACK_H
#define SUPER_SMACK_H

#include "../config.h"

void set_parse_file(char *);
void usage();
void print_version();
void set_db_type(const char *);
void get_options();
void wait_for_client_threads();
int main(int, char **);

extern void do_parse(FILE *);  // from yacc

#endif /* SUPER_SMACK_H */
