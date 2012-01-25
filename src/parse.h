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

#ifndef PARSE_H
#define PARSE_H

#include "../config.h"

#include <stdio.h>
#include <string>
#include <vector>
#include <map>

#include "query.h"
#include "client.h"
#include "dictionary.h"
#include "val.h"

extern int glob_argc;
extern char** glob_argv;

class Table_def
{
public:
  Table_def(){}

  string create_st;
  string min_rows;
  string client;
  string data_file;
  string gen_data_file;

  void validate_table(string& name);
};

class Table_def_line
{
public:  
  string str;
  Table_def_line(string& str);
  virtual ~Table_def_line() {}
  virtual void update_table_def(Table_def* t) = 0;
};

class Table_client_def_line: public Table_def_line
{
public:
  Table_client_def_line(string& str): Table_def_line(str){}
  void update_table_def(Table_def* t);
};

class Table_create_def_line: public Table_def_line
{
public:
  Table_create_def_line(string& str): Table_def_line(str){}
  void update_table_def(Table_def* t);
};

class Table_min_rows_def_line: public Table_def_line
{
public:
  Table_min_rows_def_line(string& str): Table_def_line(str){}
  void update_table_def(Table_def* t);
};

class Table_data_file_def_line: public Table_def_line
{
public:
  Table_data_file_def_line(string& str): Table_def_line(str){}
  void update_table_def(Table_def* t);
};

class Table_gen_data_file_def_line: public Table_def_line
{
public:
  Table_gen_data_file_def_line(string& str): Table_def_line(str){}
  void update_table_def(Table_def* t);
};

class Query_def_line
{
public:
  string str;

  Query_def_line(string& str);
  virtual ~Query_def_line() {}
  virtual void set_q_param(Query* q) = 0;
};

class Query_name_def_line: public Query_def_line
{
public:
  Query_name_def_line(string& str):Query_def_line(str) {}
  void set_q_param(Query* q);   
};

class Query_query_def_line: public Query_def_line
{
public:
  Query_query_def_line(string& str):Query_def_line(str) {}
  void set_q_param(Query* q);   
};


class Query_parsed_def_line: public Query_def_line
{
public:
  Query_parsed_def_line(string& str):Query_def_line(str) {}
  void set_q_param(Query* q);   
};

class Query_type_def_line: public Query_def_line
{
public:
  Query_type_def_line(string& str):Query_def_line(str) {}
  void set_q_param(Query* q);   
};


class Query_has_result_set_def_line: public Query_def_line
{
public:
  Query_has_result_set_def_line(string& str):Query_def_line(str) {}
  void set_q_param(Query* q);   
};

class Dict_def
{
public:
  Dict_def(){}

  string source;
  string source_type;
  string dict_type;
  string name;
  string file_size_equiv;
  string delim;
  
  Dictionary* make_dict();
  
};

class Dict_def_line
{
public:
  string str;
  
  Dict_def_line(string& str): str(str) {}
  virtual ~Dict_def_line() {}
  virtual void update_dict_def(Dict_def& d) = 0;
};

class Dict_source_def_line: public Dict_def_line
{
public:
  Dict_source_def_line(string& str): Dict_def_line(str) {}
  void update_dict_def(Dict_def& d);
};

class Dict_source_type_def_line: public Dict_def_line
{
public:
  Dict_source_type_def_line(string& str): Dict_def_line(str) {}
  void update_dict_def(Dict_def& d);
};

class Dict_delim_def_line: public Dict_def_line
{
public:
  Dict_delim_def_line(string& str): Dict_def_line(str) {}
  void update_dict_def(Dict_def& d);
};

class Dict_file_size_equiv_def_line: public Dict_def_line
{
public:
  Dict_file_size_equiv_def_line(string& str): Dict_def_line(str) {}
  void update_dict_def(Dict_def& d);
};


class Dict_dict_type_def_line: public Dict_def_line
{
public:
  Dict_dict_type_def_line(string& str): Dict_def_line(str) {}
  void update_dict_def(Dict_def& d);
};

class Client_def
{
public:
  string name;
  string user;
  string host;
  string db;
  string pass;
  string socket;
  string port;
  
  Query_barrel* qb;

  Client_def(): qb(NULL) {}
  Client* make_client();
};

class Client_def_line
{
public:
  string str;
  
  Client_def_line(string& str): str(str) {}
  virtual ~Client_def_line() {}
  virtual void update_client_def(Client_def& cd) = 0;
};


class Client_query_barrel_def_line: public Client_def_line
{
  
public:
  Client_query_barrel_def_line(string& str): Client_def_line(str)
    {
    }
  void update_client_def(Client_def& cd);
};

class Client_user_def_line: public Client_def_line
{
  
public:
  Client_user_def_line(string& str): Client_def_line(str)
    {
    }
  void update_client_def(Client_def& cd);
};

class Client_pass_def_line: public Client_def_line
{
  
public:
  Client_pass_def_line(string& str): Client_def_line(str)
    {
    }
  void update_client_def(Client_def& cd);
};

class Client_host_def_line: public Client_def_line
{
  
public:
  Client_host_def_line(string& str): Client_def_line(str)
    {
    }
  void update_client_def(Client_def& cd);
};

class Client_db_def_line: public Client_def_line
{
  
public:
  Client_db_def_line(string& str): Client_def_line(str)
    {
    }
  void update_client_def(Client_def& cd);
};

class Client_port_def_line: public Client_def_line
{
  
public:
  Client_port_def_line(string& str): Client_def_line(str)
    {
    }
  void update_client_def(Client_def& cd);
};

class Client_socket_def_line: public Client_def_line
{
  
public:
  Client_socket_def_line(string& str): Client_def_line(str)
    {
    }
  void update_client_def(Client_def& cd);
};

class Param_list
{
public:
  vector<string> params;
  void add(string& a) { params.insert(params.end(), a);}
  string& operator [] ( int n)
    {
      if(n >= 0 && n <= params.size())
        return params[n];
      return *new string("");
    }

  void print();
  unsigned int size() { return params.size();}
  int int_val(int n) { return atoi(params[n].c_str()); }
  const char* pchar_val(int n) { return params[n].c_str();}
};

class Main_line
{
 public:
  string client;
  string method;
  Param_list* param_list;
  
  Main_line(): param_list(NULL) {}
  ~Main_line() { delete param_list; }
  
  void print();
  void execute();
};

extern map<string, Query*, less<string> > query_pool;  
extern map<string, Dictionary*, less<string> > dict_pool;  
extern map<string, Client*, less<string> > client_pool;  
extern string current_client;
extern vector<int> client_pids;

#endif










