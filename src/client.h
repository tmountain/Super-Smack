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

#ifndef CLIENT_H
#define CLIENT_H

using namespace std;

#include "../config.h"
#include <string>
#include <vector>
#include <iostream>

#include  <map>

#include "dictionary.h"
#include "query.h"
#include "val.h"

extern int db_type;

class Table
{
public:
  string name;
  int num_rows;
};

class Client
{
 protected:

  map < string, Dictionary*, less <string> > *dictionaries;
  
  int  connected;
  int  parent_thread;
  int  running_threads;
  int  report_pipe[2];
  int  pipe_sem_id;
  int connect_time, min_connect_time,
    max_connect_time, total_connect_time; // in ms
  int num_reported_clients;
  

 public:
  
  string name;
  string host;
  string user;
  string pass;
  string db;
  string port;
  string socket;
  
  Query_barrel* qb;

  Client()
    {
      host = "localhost";
      user = "test";
      pass = "";
      db = "test";
      port = "";
      socket = "";
      connected = 0;
      connect_time = min_connect_time = -1;
      total_connect_time = max_connect_time = 0;
      num_reported_clients = 0;
      parent_thread = 1;
      running_threads = 1;
      pipe_sem_id = -1;
      qb = NULL;
      dictionaries = NULL;
    }

  virtual ~Client();

  void init()
    {
      setup_pipe();
      srand(time(NULL) * getpid());
    }

  void connect();
  virtual void do_connect() = 0;
  void connect(const char* host, const char* db, const char* user,
               const char* pass);

  Val* get_param_val(string& s1, string& s2);
  Val* get_param_val(string& s);
  
  void print_connect_times()
  {
    int avg_connect_time = (num_reported_clients) ?
      total_connect_time/num_reported_clients : 0;
    
        cout << "connect: max=" << max_connect_time << "ms " <<
	" min=" << min_connect_time << "ms" <<
	" avg= " << avg_connect_time << "ms" <<
	" from " << num_reported_clients << " clients " <<
	  endl;
  }

  int safe_query(Query& q, int abort_on_error = 1) ;
  virtual int safe_query(const char* query, int abort_on_error = 1) = 0;
  int safe_parsed_query(const char* query, int max_query,
			int abort_on_error = 1);
  virtual int escape_string(char* buf, const char* str, int str_len);
  virtual void lose_result() = 0;
  virtual void disconnect() = 0;
  virtual int get_table_info(Table& t) = 0;
  virtual int load_table_data(string& table_name, const char* fname) = 0;

  void lock_pipe_sem();
  void unlock_pipe_sem();
  void clean_up();

  // returns 0 on success, -1 on error if abort_on_error is false
  virtual int dump_result(const char* dump_file, int abort_on_error = 1) = 0;

  // returns 0 if the result mathes the data in test_file, 0 if the result
  // is retrieved successfully but does not match, and -1 if
  //there was an error
  // and abort_on_error is false
  virtual int compare_result(const char* cmp_file, int abort_on_error = 1) = 0;

  void set_dictionary(const char* name, Dictionary* dict);
  void set_dictionary_pool(map<string, Dictionary* , less<string> >
                           *dict_pool)
    {
      dictionaries = dict_pool;
    }
  
  void set_query_barrel(Query_barrel* qb)
    {
      this->qb = qb;
    }

  void unload_query_barrel(int abort_on_error = 1);
  void print_barrel_report()
    {
      if(qb)
        qb->print_report();
    }

  void create_threads(int num_threads);
  void setup_pipe();
  void thread_sync();
  void transfer_report_data();
  void set_num_rounds(int n)
    {
      if(qb)
        qb->set_num_rounds(n);
    }
  
  void start_clock()
    {
      qb->start_clock();
    }

  void stop_clock()
    {
      qb->stop_clock();
    }

  void print();
  
};

#endif






