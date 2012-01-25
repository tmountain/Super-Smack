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

#ifndef QUERY_H
#define QUERY_H

using namespace std;

#include "../config.h"

#include <sys/time.h>
#include <unistd.h>
#include <map>
#include <string>
#include <vector>

class Query
{
public:
  string query;
  string name;
  string type_name;
  int has_result_set;
  int parsed;
  
  Query(): query(""),name(""),type_name(""),has_result_set(0),parsed(0)
    {
    }

  Query(const Query& other)
    {
      *this = other;
    }
  
  Query& operator = (const Query& other)
    {
      if(this != &other)
        {
          query = other.query;
          name = other.name;
          type_name = other.type_name;
          has_result_set = other.has_result_set;
          parsed = other.parsed;
        }
      return *this;
    }

  void print();
};

enum Barrel_action_type {QUERY_ACTION=1,SLEEP_ACTION=2};

class Client;

class Query_barrel_action
{
public:
  virtual Barrel_action_type get_type() = 0;
  virtual void fire(Client* cli) = 0;
  virtual ~Query_barrel_action() {}
};

class Query_charge: public Query_barrel_action
{
public:
  Query query;
  int num_shots;

  Query_charge():num_shots(0) {}
  Query_charge(Query& query, int num_shots)
    {
      this->query = query;
      this->num_shots = num_shots;
    }

  ~Query_charge() {}

  Barrel_action_type get_type() { return QUERY_ACTION; }
  void fire(Client* cli);
  
};

class Sleep_action: public Query_barrel_action
{
public:
  int sec,usec;
  Sleep_action(): sec(0),usec(0) {}
  Sleep_action(int sec,int usec): sec(sec),usec(usec) {}
  ~Sleep_action(){}
  Barrel_action_type get_type() { return SLEEP_ACTION;}
  void fire(Client* cli)
  {
    sleep(sec);
    usleep(usec);
  }
};


class Query_type_report
{
public:
  int num_queries;
  int  min_time; // in ms
  int max_time; // in ms

  Query_type_report(): num_queries(0),min_time(0),max_time(0) {}
  void update(int q_time);
  void update(int num_queries, int max_q_time, int min_q_time); 
  void print(int t_s = 0, int t_us = 0);
};

class Query_report
{
public:
  map<string, Query_type_report*, less<string> > type_reports;
  struct timeval start, end;

  Query_report()
    {
      start.tv_sec = start.tv_usec = end.tv_sec =
                     end.tv_usec = 0;
    }
  
  
  void update(const char* type_name, int q_time);
  void print();
  void fd_send(int fd); // write serialized to a file descriptor
  void update_from_fd(int fd); // read and update (+=) from a file descriptor
  void start_clock();
  void stop_clock();
  double get_run_time()
  {
    return (double)(end.tv_sec - start.tv_sec) +
		    (double)(end.tv_usec - start.tv_usec)/1000000.0;
  }
  
};

class Query_barrel
{
protected:
  vector<Query_charge> qc;
  vector<Sleep_action> sa;
  // the above will store the actual data
  
  
  int num_rounds;
  Query_report qr;
public:
  vector<Query_barrel_action*> actions;
  // this is to have a sequence of actions
  Query_barrel() : num_rounds(0) {}
  ~Query_barrel();
  
  int set_num_rounds(int num_rounds)
    {
      return this->num_rounds = num_rounds;
    }
  int get_num_rounds() { return num_rounds;}
  int num_query_charges() { return qc.size();}
  Query* get_query(int i) { return &(qc[i].query);}
  int num_charge_shots(int i) { return qc[i].num_shots;}
  
  
  void add_query_charge(Query& q, int num_shots)
    {
     actions.insert(actions.end(), new Query_charge(q, num_shots));
     if(!qr.type_reports[q.type_name])
       qr.type_reports[q.type_name] = new Query_type_report;
    }

  void add_sleep_action(int sec, int usec)
    {
     actions.insert(actions.end(), new Sleep_action(sec, usec));
    }

  void update_report(string& type_name, int q_time )
    {
      qr.update(type_name.c_str(), q_time);
    }

  void print_report()
    {
      qr.print();
    }

  void fd_send_report(int fd)
    {
      qr.fd_send(fd);
    }

  void fd_update_report(int fd)
    {
      qr.update_from_fd(fd);
    }

  void start_clock()
    {
      qr.start_clock();
    }
  void stop_clock()
    {
      qr.stop_clock();
    }

  Query_type_report* get_report_by_name(string& name)
  {
    return qr.type_reports[name];
  }

  double get_run_time() { return qr.get_run_time(); }
};

#endif



