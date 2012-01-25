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

using namespace std;

#include "../config.h"

#include "client.h"
#include "die.h"
#include <fstream>
#include <iostream>
#include <sys/time.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <sys/file.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>


#define MAX_VAR_BUF 512

#if !defined(__FreeBSD__) && !defined(__OpenBSD__) && !defined(__NetBSD__) && !(defined(__MACH__) && defined(__APPLE__))
#if defined(__GNU_LIBRARY__) && !defined(_SEM_SEMUN_UNDEFINED)
       /* union semun is defined by including <sys/sem.h> */
#else
       /* according to X/OPEN we have to define it ourselves */
       union semun {
               int val;                    /* value for SETVAL */
               struct semid_ds *buf;       /* buffer for IPC_STAT, IPC_SET */
               unsigned short int *array;  /* array for GETALL, SETALL */
               struct seminfo *__buf;      /* buffer for IPC_INFO */
       };
#endif  
#endif
#ifdef __NetBSD__
	  /* NetBSD needs this as well, it's not defined in sys/sem.h */
	   union semun {
               int val;                    /* value for SETVAL */
               struct semid_ds *buf;       /* buffer for IPC_STAT, IPC_SET */
               unsigned short int *array;  /* array for GETALL, SETALL */
               struct seminfo *__buf;      /* buffer for IPC_INFO */
       };

#endif
#if !defined(__FreeBSD__) && !defined(__OpenBSD__) && !defined(__NetBSD__) && !(defined(__MACH__) && defined(__APPLE__))
#define SUPER_SMACK_RESTART ERESTART
#else
#define SUPER_SMACK_RESTART EINTR
#endif

void Client::connect()
{
  struct timeval start,end;
  gettimeofday(&start, NULL);
  do_connect();
  gettimeofday(&end, NULL);
  connect_time = (end.tv_sec - start.tv_sec) * 1000 +
    (end.tv_usec - start.tv_usec)/1000;
  if(parent_thread)
    {
      num_reported_clients++;
      total_connect_time += connect_time;
    }
  connected = 1;
}

Val* Client::get_param_val(string& s)
{
  if(s == "max_connect_time")
    return new Val(max_connect_time);
  else if(s == "min_connect_time")
    return new Val(min_connect_time);
  else if(s == "avg_connect_time")
    {
     int avg_connect_time = (num_reported_clients) ?
          total_connect_time/num_reported_clients : 0;
       return new Val(avg_connect_time);
    }
  else
   die(0, " '%s' - unknown client param", s.c_str());

  return NULL; //impossible
}

Val* Client::get_param_val(string& s1, string& s2)
{
  Query_type_report* qr = qb->get_report_by_name(s1);
  if(!qr)
    die(0, " '%s' - no such query type", s1.c_str());

  if(s2 == "num_queries")
    return new Val(qr->num_queries);
  else if(s2 == "max_time")
    return new Val(qr->max_time);
  else if(s2 == "min_time")
    return new Val(qr->min_time);
  else if(s2 == "q_per_s")
    {
      double run_time = qb->get_run_time();
      if(run_time == 0.0)
	return new Val(0);
      double q_per_s = (double)qr->num_queries/run_time;
      return new Val(q_per_s);
    }
  else
   die(0, "'%s' - unknown query report param", s2.c_str());
  
  return NULL;
}

int Client::escape_string(char* buf, const char* str, int str_len)
{
  const char* str_end = str + str_len;
  char* buf_start = buf;
  
  for(; str < str_end; ++str)
    {
      char c = *str;
      if(c == '\'' || c == '"' || c == '\\')
	{
	  *buf++ = '\\';
	}
      *buf++ = c;
    }
  return (int)(buf - buf_start);
}


void Client::connect(const char* host, const char* db,
                     const char* user, const char* pass)
{
  this->host = host;
  this->db = db;
  this->user = user;
  this->pass = pass;
  connect();
}

int Client::safe_parsed_query(const char* query, int max_query,
                              int abort_on_error )
{
  char parsed_query[max_query];
  char var_buf[MAX_VAR_BUF];
  int var_buf_offset = 0;
  int offset = 0;
  int i;
  int in_var = 0;
  int escaped = 0;

  for(i = 0; int c = query[i]; i++)
    {
      if(!in_var)
        if(!escaped)
          {
            switch(c)
              {
              case '\\': escaped = 1; break;
              case '$': in_var = 1; var_buf_offset = 0; break;
              default:
                if(offset < max_query - 1)
                  parsed_query[offset++] = c;
                break;
              }
          }
        // if (escaped) and !in_var
        else
          {
            if(offset < max_query - 1)
              parsed_query[offset++] = c;
            
          }
      // if in_var and don't care about escaped, nothing escaped in_var
      else
        {
          if(isalnum(c) || c == '_')
            {
              if(var_buf_offset < sizeof(var_buf) - 1)
                var_buf[var_buf_offset++] = c;
            }
          else
            {
              var_buf[var_buf_offset] = 0;
              Dictionary* d = (*dictionaries)[var_buf];
              if(!d)
                die(0, "The dictionary %s does not exist", var_buf);
              const char* word = d->next_word();
              int word_len = strlen(word);
              
              if(2*word_len + offset + 2 < max_query) 
                offset += escape_string(parsed_query + offset, word,
                                              word_len);
              if(offset < max_query - 1)
                parsed_query[offset++] = c;

              var_buf_offset = 0;
              in_var = 0;
              
            }
        }
        
    }
  
  parsed_query[offset] = 0;
  return safe_query(parsed_query, abort_on_error);
}


void Client::set_dictionary(const char* name, Dictionary* dict)
{
  if(!dictionaries) dictionaries = new map<string, Dictionary*,
                      less<string> >;
  
  (*dictionaries)[name] = dict;
}


void Client::print()
{
  cout << "client: " << name << ":" << endl <<
    "user=" << user << endl <<
    "pass=" << pass << endl <<
    "host=" << host << endl <<
    "db=" << db << endl ;
}

void Client:: unload_query_barrel(int abort_on_error)
{
  register int i,j;
  
  for(i = 0; i < qb->get_num_rounds(); i++)
    {
      for(j = 0; j < qb->actions.size(); j++)
	{
	  qb->actions[j]->fire(this);
	}
    }
          
}

int Client::safe_query(Query& q, int abort_on_error)
{
  if(q.parsed)
    return safe_parsed_query(q.query.c_str(), q.query.size() * 2,
                             abort_on_error);
  else
    return safe_query(q.query.c_str(), abort_on_error);
}
                      

void Client::create_threads(int num_threads)
{
  int i;
  
  for(i = 0; i < num_threads - 1; i++)
    {
      if(parent_thread)
        switch(fork())
          {
          case 0:
            close(report_pipe[0]);
            parent_thread = 0;
            running_threads = 0;
            srand(time(NULL) * getpid());
            // a child does not care how many threads there
            // are
            return;
            
          case -1:
            cerr << "failed on fork():" << strerror(errno) <<  endl;
            break;
            
          default:
            parent_thread = 1;
            running_threads++;
            break;
          }
    }
  
  close(report_pipe[1]);
}

void Client::clean_up()
{
  disconnect();
  if(pipe_sem_id != -1)
    semctl(pipe_sem_id, 0, IPC_RMID,0);
}

Client::~Client()
{
}

void Client::setup_pipe()
{
  if(pipe(report_pipe) == -1)
    die(1, "could not open pipe");
  key_t sem_key = ftok(".", 's');
  if((pipe_sem_id = semget(sem_key, 0, 0666)) == -1)
    {
      if((pipe_sem_id = semget(sem_key, 1, IPC_CREAT|IPC_EXCL|0666 )) == -1)
      {
	die(1, "Error creating semaphore");
      }
    }

  union semun semopts;
  semopts.val = 1;
  semctl(pipe_sem_id, 0, SETVAL, semopts);
  kill_sem_id = pipe_sem_id;
}

void Client::lock_pipe_sem()
{
  struct sembuf sem_lock = { 0, -1, 0};
  
  if(semop(pipe_sem_id, &sem_lock, 1) == -1)
    {
      die(1, "Could not lock the pipe semaphore %d", pipe_sem_id);
    }
}

void Client::unlock_pipe_sem()
{
  struct sembuf sem_lock = { 0, 1, 0};
  if(semop(pipe_sem_id, &sem_lock, 1) == -1)
    {
      die(1, "Could not unlock the pipe semaphore %d", pipe_sem_id);
    }
}

void  Client::thread_sync()
{
  if(parent_thread)
    while(1)
      {
        pid_t pid = wait4(0, 0, 0, NULL);
        if(pid > 0)
          transfer_report_data();
        else
          {
            if (errno == SUPER_SMACK_RESTART)
              continue;
            else
              break;
          }
      }
  else
    {
      transfer_report_data();
      disconnect();
      close(report_pipe[1]);
      exit(0);
    }
        
}

void Client::transfer_report_data()
{
  if(parent_thread)
    {
      fd_set rfds;
      int fd = report_pipe[0];
      for(;;)
	{
          FD_ZERO(&rfds);
	  FD_SET(fd, &rfds);
	  int err = select(fd+1, &rfds, NULL, NULL, NULL);
          if(err == -1)
	    if(errno != EINTR)
             die(1, "error in select()");
            else
	      continue;
	  else
	    break;
	}  

      int temp_connect_time,data_len;
     
      if((data_len = read(fd,&temp_connect_time, sizeof(connect_time))) < 0)
	die(1, "Error in read() while collecting threads");
      if(data_len == 0) return;
      
      total_connect_time += temp_connect_time;
      num_reported_clients++;
      
      if(max_connect_time < temp_connect_time)
	max_connect_time = temp_connect_time;
      if(min_connect_time > temp_connect_time || min_connect_time == -1)
	min_connect_time = temp_connect_time;
      
      qb->fd_update_report(report_pipe[0]);
    }
  else
    {
      fd_set wfds;
      int fd = report_pipe[1];
      for(;;)
	{
          FD_ZERO(&wfds);
	  FD_SET(fd, &wfds);
	  int err = select(fd+1, NULL, &wfds, NULL, NULL);
          if(err == -1)
	    if(errno != EINTR)
             die(1, "error in select()");
            else
	      continue;
	  else
	    break;
	}
      lock_pipe_sem();
      if(write(fd, &connect_time, sizeof(connect_time)) < 0)
	die(1, "error on write()");
      qb->fd_send_report(fd);
      unlock_pipe_sem();
    }
}











