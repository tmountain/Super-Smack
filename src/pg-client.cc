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

#ifdef HAVE_PGSQL

#include <libpq-fe.h>
#include <fstream>
#include <iostream>

#include "client.h"
#include "engines.h"
#include "pg-client.h"

#include "die.h"

int Pg_client::get_table_info(Table& t)
{
  string query = "select count(*) from ";
  query += t.name;
  if(safe_query(query.c_str(), 0))
    return 1;
  t.num_rows = atoi(PQgetvalue(res,0,0));
  lose_result();
  return 0;
}

int Pg_client::load_table_data(string& table_name,
				  const char* data_file_name)
{
  string query = "copy ";
  query += table_name;
  query += " from '";
  query += data_file_name;
  query += "' delimiters ',' ";
  safe_query(query.c_str());
  query = "grant all on ";
  query += table_name;
  query += " to public";
  safe_query(query.c_str());
  return 0;
}


void Pg_client::do_connect()
{
  con = PQsetdbLogin(host.c_str(), port.c_str(), 0, 0, db.c_str(),
		       user.c_str(), pass.c_str());
  if(PQstatus(con) == CONNECTION_BAD)
    {
      cerr <<  "Connection to Posgres failed: " << PQerrorMessage(con) <<
	endl;
      disconnect();
      exit(1);
    }
}

int Pg_client::safe_query(const char* query, int abort_on_error)
//int Pg_client::safe_query(const char* query, int abort_on_error = 1)
{
  if(res)
    PQclear(res);
  
  if(!(res = PQexec(con, query)) ||
     (PQresultStatus(res) != PGRES_COMMAND_OK &&
      PQresultStatus(res) != PGRES_TUPLES_OK))
    {
      cerr << "Error running query " << query << ":" <<
	PQerrorMessage(con) << endl;
      lose_result();
      if(abort_on_error) die(0, "aborting on failed query");
      return -1;
    }
  
  return 0;
}

void Pg_client::lose_result()
{
  if(res)
    {
      PQclear(res);
      res = 0;
    }
}

void Pg_client::disconnect()
{
  if(con)
    {
      PQfinish(con);
      con = 0;
    }
}

int Pg_client::compare_result(const char* cmp_file, int abort_on_error)
//int Pg_client::compare_result(const char* cmp_file, int abort_on_error = 1)
{
  die(0, "compare_result() not supported for Postgres yet");
  return 0;
}

//int Pg_client::dump_result(const char* dump_file, int abort_on_error = 1)
int Pg_client::dump_result(const char* dump_file, int abort_on_error)
{
  ofstream os;
  if(dump_file)
    os.open(dump_file);
  else
    printf("I have no idea what to do here\n");//    os.attach(1);
 
 
  if(!os)
    if(abort_on_error)
      die(1, "could not open dump file %s",
	  dump_file ? dump_file : "STDOUT");
    else
      {
        cerr << "could not open dump file " << dump_file << endl;
        return -1;
      }


  int num_fields = PQnfields(res);
  int i;
  
  for( i = 0; i < num_fields; i++)
    {
      os << PQfname(res, i) << "\t";
    }
  os << endl;
  
  int j;
  int num_rows = PQntuples(res);
  for(j = 0; j < num_rows; ++j)
    {
      int i;
      for(i = 0; i < num_fields; ++i)
        {
          if(!PQgetisnull(res, j, i))
            os.write(PQgetvalue(res, j,i), PQgetlength(res, j, i));
          else
            os << "NULL";
          os << "\t";
        }

      os << endl;
    }

  PQclear(res);
  return 0;
}

Client *pg_new() {
  return new Pg_client;
};

class Engine pg_engine("PostgreSQL", "pg", &pg_new, "1.0", "Sasha Pachev");

#endif
