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

#include <ctype.h>
#include <math.h>

#include "die.h"
#include "smack.h"
#include "engines.h"
#include "parse.h"

extern int yylineno;

map <string, Query*, less<string> > query_pool;
map <string, Dictionary*, less<string> > dict_pool;
map <string, Client*, less<string> > client_pool;  
map<string, Val*, less<string> > var_hash;
string current_client = "main";
vector<int> client_pids;

Query_def_line::Query_def_line(string& str):str(str)
{
}

void Query_name_def_line::set_q_param(Query* q)
{
  q->name = str;
}

void Query_query_def_line::set_q_param(Query* q)
{
  q->query = str;
}

void Query_type_def_line::set_q_param(Query* q)
{
  q->type_name = str;
}

void Query_has_result_set_def_line::set_q_param(Query* q)
{
  q->has_result_set = (tolower(str[0]) == 'y');
}

void Query_parsed_def_line::set_q_param(Query* q)
{
  q->parsed = (tolower(str[0]) == 'y');
}

Dictionary* Dict_def::make_dict()
{
  Dictionary* dict = NULL;
  
  if(dict_type == "rand")
    {
      dict = new Rand_dictionary;
    }
  else if(dict_type == "seq")
    {
      dict = new Seq_dictionary;
    }
  else if(dict_type == "unique")
    {
      dict = new Unique_dictionary;
    }
  else
    die(0, "unknown dictionary type '%s' on line %d", dict_type.c_str(),
        yylineno);

  if(file_size_equiv.length())
    dict->file_size_equiv = atoi(file_size_equiv.c_str());
  if(delim.length())
    dict->delim = delim[0];
  
  if(source_type == "file")
    {
      dict->load_file(source.c_str());
    }
  else if(source_type == "list")
    {
      dict->load_list(source.c_str());
    }
  else if(source_type == "template")
    {
      if(!(dict_type == "unique"))
	die(0, "line %d: template source type is supported only for \
dictionaries of type 'unique'", yylineno);
      ((Unique_dictionary*)dict)->set_template(source.c_str());
    }
  else
    die(0, "unknown dictionary source type: '%s' on line %d",
        source_type.c_str(), yylineno);
 
  dict->name = name;
  
  return dict;
}

void Dict_dict_type_def_line::update_dict_def(Dict_def& d)
{
  d.dict_type = str;
}

void Dict_source_type_def_line::update_dict_def(Dict_def& d)
{
  d.source_type = str;
}

void Dict_source_def_line::update_dict_def(Dict_def& d)
{
  d.source = str;
}

void Dict_delim_def_line::update_dict_def(Dict_def& d)
{
  d.delim = str;
}

void Dict_file_size_equiv_def_line::update_dict_def(Dict_def& d)
{
  d.file_size_equiv = str;
}

Table_def_line::Table_def_line(string& str):str(str)
{
}

void Table_def::validate_table(string& name)
{
  Client* c = client_pool[client];
  if(!c)
    die(0, "Missing or undefined client in the table section on line %d",
	yylineno);
  Table t;
  bool do_create = 0, do_drop = 0;
  int min_rows_lim = atoi(min_rows.c_str());
  c->connect();
  t.name = name;
  if(c->get_table_info(t))
    {
      do_create = 1;
    }
  else
    {
      if(t.num_rows < min_rows_lim)
	{
	  do_drop = 1;
	  do_create = 1;
	}
    }

  if(do_drop)
    {
      cout << "Table '" << name <<
	"' does not meet condtions, will be dropped" << endl;
      string query = "drop table ";
      query += name;
      c->safe_query(query.c_str());
    }
  
  if(do_create)
    {
      char path[MAX_PATH];
      const char* file = mk_data_path(path, data_file.c_str());
      struct stat f;
      cout << "Creating table '" << name << "'" << endl;
      c->safe_query(create_st.c_str());
      if(stat(file, &f) && populate_data_file(file, gen_data_file.c_str()))
	die(1, "Error generating data file");
      cout << "Loading data from file '" << file << "' into table '" << name <<
	   "'" << endl;
      c->load_table_data(name, file);
      cout << "Table " << name << " is now ready for the test" << endl;
    }

  c->disconnect();
}

void Table_client_def_line::update_table_def(Table_def* t)
{
  t->client = str;
}

void Table_create_def_line::update_table_def(Table_def* t)
{
  t->create_st = str;
}

void Table_min_rows_def_line::update_table_def(Table_def* t)
{
  t->min_rows = str;
}

void Table_data_file_def_line::update_table_def(Table_def* t)
{
  t->data_file = str;
}

void Table_gen_data_file_def_line::update_table_def(Table_def* t)
{
  t->gen_data_file = str;
}


Client* Client_def::make_client()
{
  Client* c;
  c = new_client(db_type);
  c->user = user;
  c->pass = pass;
  c->db = db;
  c->host = host;
  c->socket = this->socket;
  c->port = port;
  
  if(qb)
    c->set_query_barrel(qb);

  c->set_dictionary_pool(&dict_pool);
  
  return c;
}

void Client_query_barrel_def_line::update_client_def(Client_def& cd)
{
  int str_len = str.size();
  char buf[str_len + 1];
  int i,j = 0;
  int last_was_space = 1;
  int num_shots = 0;
  enum {NUM_SHOTS, QUERY_NAME,SLEEP_TIME} parse_state = NUM_SHOTS;
  cd.qb = new Query_barrel;
  
  for(i = 0; i < str_len; i++)
    {
     if(str[i] == ' ')
       if(last_was_space)
         continue;
       else
         {
           last_was_space = 1;
           buf[j] = 0;
           j = 0;
           
           switch(parse_state)
             {
             case NUM_SHOTS:
	       if(buf[0] == '-')
		 {
		   float sl = atof(buf+1);
		   int sec,usec;
		   sec = (int)floor(sl);
		   sl -= sec;
		   usec = (int)floor(sl*1000000);
		   cd.qb->add_sleep_action(sec,usec);
		   break;
		 }
               num_shots = atoi(buf);
               parse_state = QUERY_NAME;
               break;
               
             case QUERY_NAME:
               {
                 Query* q = query_pool[buf];
                 if(!q)
                   die(0, "query '%s' referenced on line %d has not \
been defined", buf, yylineno);
                 cd.qb->add_query_charge(*q, num_shots);
                 parse_state = NUM_SHOTS;
                 break;
               }
             }
         } // else
     else
       {
         last_was_space = 0;
         buf[j++] = str[i];
       }
       
    }

  if(!last_was_space)
    {
      buf[j] = 0;
      if(buf[0] == '-')
	{
	  float sl = atof(buf+1);
	  int sec,usec;
	  sec = (int)floor(sl);
	  sl -= sec;
	  usec = (int)floor(sl*1000000);
	  cd.qb->add_sleep_action(sec,usec);
	  return;
	}
      Query* q = query_pool[buf];
      if(!q)
        die(0, "query '%s' referenced on line %d has not been defined",
            buf, yylineno);
       cd.qb->add_query_charge(*q, num_shots);
    }
}

void Client_user_def_line::update_client_def(Client_def& cd)
{
  cd.user = str;
}

void Client_pass_def_line::update_client_def(Client_def& cd)
{
  cd.pass = str;
}

void Client_host_def_line::update_client_def(Client_def& cd)
{
  cd.host = str;
}

void Client_socket_def_line::update_client_def(Client_def& cd)
{
  cd.socket = str;
}

void Client_port_def_line::update_client_def(Client_def& cd)
{
  cd.port = str;
}


void Client_db_def_line::update_client_def(Client_def& cd)
{
  cd.db = str;
}

void Param_list::print()
{
  if(params.size() > 0)
    cout << params[0];
  
  for(int i = 1; i < params.size(); i++)
    {
      cout << "," << params[i];
    }
}

void Main_line::print()
{
  cout << "main_line: client: " << client << ",method=" << method ;
  if(param_list)
    {
      cout << ",param_list: " ;
      param_list->print();
    }

  cout << endl;
}

void Main_line::execute()
{
  if(!(client == current_client) && !(method == "init"))
    return;
  
  Client* c = client_pool[client];
  if(!c)
    die(0, "client '%s' referenced on line %d is not defined",
        client.c_str(), yylineno);
  if(method == "connect")
    {
      c->connect();
    }
  else if(method == "init")
    {
      if(!(current_client == "main")) return;
      pid_t pid;
      switch(pid = fork())
        {
        case 0:
          c->init();
          current_client = client;
          break;
        case -1:
          die(1, "could not fork()");
          break;
          
        default:
          client_pids.insert(client_pids.end(), pid);
          break;
        }
          
    }
  else if(method == "sleep")
    {
      if(param_list->size() != 2)
        die(0, "line %d: sleep takes two parameters (sec,ms), you have %d",
            yylineno, param_list->size());
      sleep(atoi(param_list->pchar_val(0)));
      usleep(atoi(param_list->pchar_val(1))/1000);
    }
  else if(method == "print")
    {
      if(param_list->size() != 1)
        die(0, "line %d: print takes one parameter, you have %d",
            yylineno, param_list->size());
      cout << param_list->pchar_val(0) << endl;
    }
  else if(method == "query")
    {
      if(param_list->size() != 1)
        die(0, "line %d: query takes one parameter, you have %d",
            yylineno, param_list->size());
      const char* q = param_list->pchar_val(0);
      c->safe_query(q);
    }
  else if(method == "run_save_result")
    {
      if(param_list->size() != 2)
        die(0, "line %d: run_save_result takes two parameters, you have %d",
            yylineno, param_list->size());
      const char* q_name = param_list->pchar_val(0);
      const char* fname = param_list->pchar_val(1);
      Query *q = query_pool[q_name];
      
      if(!q)
        die(0, "line %d: query '%s' has not been defined", yylineno,
            q_name);

      c->safe_query(*q);
      if(fname[0] == '-' && !fname[1])
	fname = NULL;
      if(q->has_result_set)
        c->dump_result(fname);
      
    }
  else if(method == "run_check_result")
    {
    }
  else if(method == "set_num_rounds")
    {
      if(param_list->size() != 1)
        die(0, "line %d: set_num_rounds takes one parameter, you have %d",
            yylineno, param_list->size());
      int num_rounds = param_list->int_val(0);
      c->set_num_rounds(num_rounds);
    }
  else if(method == "disconnect")
    {
      c->disconnect();
    }
  else if(method == "create_threads")
    {
      c->start_clock();
      if(param_list->size() != 1)
        die(0, "line %d: create_threads takes one parameter, you have %d",
            yylineno, param_list->size());
      int num_threads = param_list->int_val(0);
      c->create_threads(num_threads);
    }
  else if(method == "collect_threads")
    {
      c->thread_sync();
      c->stop_clock();
      cout << "Query Barrel Report for client " << client << endl;
      c->print_connect_times();
      c->print_barrel_report();
    }
  else if(method == "unload_query_barrel")
    {
      c->unload_query_barrel();
    }
  else
    {
      die(0, "unknown method '%s' called for client '%s' on line %d",
          method.c_str(), client.c_str(), yylineno);
    }
}






