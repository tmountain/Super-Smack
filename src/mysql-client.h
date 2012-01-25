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

#include "../config.h"

#ifndef MYSQL_CLIENT_H
#define MYSQL_CLIENT_H

#ifdef HAVE_MYSQL

#include <mysql.h>

class Mysql_client: public Client
{
protected:  
  MYSQL con;
public:
  Mysql_client():Client(){}
  ~Mysql_client() { clean_up(); }
  void do_connect();
  int safe_query(const char* query, int abort_on_error = 1);
  int escape_string(char* buf, const char* str, int str_len);

  void lose_result();
  void disconnect();
  int check_error(int fatal = 1);
  int compare_result(const char* cmp_file, int abort_on_error = 1) ;
  int dump_result(const char* dump_file, int abort_on_error = 1);
  int get_table_info(Table& t);
  int load_table_data(string& table_name, const char* data_file_name);
};

Client *mysql_new();

#endif /* HAVE_MYSQL */

#endif /* MYSQL_CLIENT_H */
