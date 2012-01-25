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

#ifdef HAVE_MYSQL

#include <mysql.h>
#include <vector>
#include <fstream>
#include <iostream>
#include <sys/time.h>

#include "client.h"
#include "engines.h"
#include "mysql-client.h"

#include "die.h"


void Mysql_client::do_connect()
{
  mysql_init(&con);
  mysql_real_connect(&con, host.c_str(), user.c_str(), pass.c_str(),
		     db.c_str(), atoi(port.c_str()),
		     socket.length() ? socket.c_str():0,0);
  check_error();
}

int  Mysql_client::check_error(int fatal)
{
  int  error = 0;
  
  if((error = mysql_errno(&con)))
    if(fatal)
      die(0, "aborting because of a database error: %s", mysql_error(&con));
    else
      cerr << "non-fatal database error: " << mysql_error(&con) << endl;
  
  return error;
}


int Mysql_client::safe_query(const char* query, int abort_on_error)
{
 if( mysql_query(&con, query))
   {
     cerr << "Error running query " << query << ":" << mysql_error(&con)
          << endl;
     if(abort_on_error) die(0, "aborting on failed query");
     return -1;
   }
 
 return 0;
}

void Mysql_client::lose_result()
{
  MYSQL_RES* res = mysql_store_result(&con);
  if(res)
    {
      while(mysql_fetch_row(res));
      mysql_free_result(res);
    }
}

int Mysql_client::compare_result(const char* cmp_file, int abort_on_error)
{
  ofstream os(cmp_file);
  if(!os)
    if(abort_on_error)
      die(1, "could not open cmp file %s", cmp_file);
    else
      {
        cerr << "could not open dump file " << cmp_file << endl;
        return -1;
      }

  MYSQL_RES* res = mysql_store_result(&con);
  if(!res)
    return check_error(abort_on_error);

  MYSQL_FIELD* fields = mysql_fetch_fields(res);
  int num_fields = mysql_num_fields(res);
  int i;
  
  for( i = 0; i < num_fields; i++)
    {
      os << fields[i].name << "\t";
    }
  os << endl;
  
  MYSQL_ROW row;

  
  while((row = mysql_fetch_row(res)))
    {
      int i;
      unsigned long* lengths = mysql_fetch_lengths(res);
      for(i = 0; i < num_fields; i++)
        {
          if(row[i])
            os.write(row[i], lengths[i]);
          else
            os << "NULL";
          os << "\t";
        }

      os << endl;
    }

  mysql_free_result(res);
  return 0;
}

int Mysql_client::get_table_info(Table& t)
{
  MYSQL_RES* res;
  string query = "select count(*) from ";
  query += t.name;
  if(safe_query(query.c_str(), 0))
    return 1;
  if(!(res = mysql_store_result(&con)))
    die(0, "Error in mysql_store_result()");
  MYSQL_ROW row = mysql_fetch_row(res);
  if(!row)
  {
    mysql_free_result(res);
    die(0, "No results from select count(*) query ");
  }
  t.num_rows = atoi(row[0]);
  mysql_free_result(res);
  return 0;
}

int Mysql_client::load_table_data(string& table_name,
				  const char* data_file_name)
{
  string query = "load data infile '";
  query += data_file_name;
  query += "' into table ";
  query += table_name;
  query += " fields terminated by ','";
  safe_query(query.c_str());
  return 0;
}

int Mysql_client::dump_result(const char* dump_file, int abort_on_error)
{
  ostream* os;
  int alloced_os = 0;
  
  if(dump_file)
  {
    os = new ofstream(dump_file);
    alloced_os = 1;
  }
  else
    os = &cout;
  
  if(!*os)
    if(abort_on_error)
      die(1, "could not open dump file %s",
	  dump_file ? dump_file : "STDOUT");
    else
      {
        cerr << "could not open dump file " << dump_file << endl;
	if (alloced_os)
	  delete os;
        return -1;
      }

  MYSQL_RES* res = mysql_store_result(&con);
  if(!res)
    return check_error(abort_on_error);

  MYSQL_FIELD* fields = mysql_fetch_fields(res);
  int num_fields = mysql_num_fields(res);
  int i;
  
  for( i = 0; i < num_fields; i++)
    {
      *os << fields[i].name << "\t";
    }
  *os << endl;
  
  MYSQL_ROW row;

  
  while((row = mysql_fetch_row(res)))
    {
      int i;
      unsigned long* lengths = mysql_fetch_lengths(res);
      for(i = 0; i < num_fields; i++)
        {
          if(row[i])
            os->write(row[i], lengths[i]);
          else
            *os << "NULL";
          *os << "\t";
        }

      *os << endl;
    }

  mysql_free_result(res);
  if (alloced_os)
    delete os;
  return 0;
}


void  Mysql_client::disconnect()
{
  if(connected)
    {
      mysql_close(&con);
      connected = 0;
    }
}

int Mysql_client::escape_string(char* buf, const char* str, int str_len)
{
  return mysql_escape_string(buf, str, str_len);
}

Client *mysql_new()
{
  return new Mysql_client;
};

class Engine mysql_engine("MySQL", "mysql", &mysql_new, "1.0", "Sasha Pachev");

#endif
