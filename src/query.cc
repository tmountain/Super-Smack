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

#include <stdio.h>
#include <unistd.h>
#include <sys/file.h>
#include <errno.h>
#include <iostream>
#include <fcntl.h>

#include "query.h"
#include "die.h"
#include "client.h"

#define MAX_REPORT_LEN 1024
#define MAX_QUERY   8192


void Query::print()
{
  cout << "Query:" << "query=" << query << ",name=" << name <<
    ",has_result_set=" << has_result_set << ",type_name=" <<
    type_name << ",parsed=" << parsed << endl;
}

void Query_type_report::update(int num_queries, int max_q_time, int min_q_time)
{
  this->num_queries += num_queries;
  if(max_time < max_q_time) max_time = max_q_time;
  if(min_time > min_q_time) min_time = min_q_time;
}

void Query_type_report::update(int q_time)
{
  if(num_queries)
    min_time = max_time = q_time;
  else
    {
      if(min_time > q_time) min_time = q_time;
      if(max_time < q_time) max_time = q_time;
    }

  num_queries++;
}

void Query_type_report::print(int t_s, int t_us)
{
  char q_per_s[128];
  if(t_s == 0 && t_us == 0)
    strcpy(q_per_s, "n/a");
  else
    snprintf(q_per_s, sizeof(q_per_s), "%.2f", (float)num_queries/
             (float)(t_s + (float)t_us/1000000.0));
  
  cout << num_queries << "\t" << max_time << "\t" << min_time <<
    "\t" << q_per_s << endl;
}

void Query_report::update(const char* type_name, int q_time)
{
  Query_type_report *qtr = type_reports[type_name];
  if(qtr)
    {
      qtr->update(q_time);
    }
}

void Query_charge::fire(Client* cli)
{
  int i;
  for(i = 0; i < num_shots; i++)
    {
      struct timeval start, end;
      gettimeofday(&start, NULL);
            
      if(query.parsed)
	cli->safe_parsed_query(query.query.c_str(), MAX_QUERY, 1);
      else
	cli->safe_query(query.query.c_str(), 1);

      if(query.has_result_set)
	cli->lose_result();
            
      gettimeofday(&end, NULL);
      int q_time = (end.tv_sec - start.tv_sec)*1000 +
	(end.tv_usec - start.tv_usec)/1000;
      cli->qb->update_report(query.type_name, q_time);
    }
}

void Query_report::print()
{
  //flock(1, LOCK_EX); // lock stdout so that we won't have a conflict from
 fcntl(1, F_SETLK, F_WRLCK); 
  // another client
  cout << "Query_type\tnum_queries\tmax_time\tmin_time\tq_per_s" << endl;
  
  map<string, Query_type_report*, less<string> >::iterator i =
    type_reports.begin();

  if(type_reports.size())
   while(i != type_reports.end())
    {
      Query_type_report *qtr = (*i).second;
      if(qtr)
	{
         cout << (*i).first << "\t" ;
         qtr->print(end.tv_sec - start.tv_sec,
                         end.tv_usec - start.tv_usec );
	}
      else
	die(0, "Internal error: NULL query type report, %d report types",
	    type_reports.size());
      i++;
    }
  fcntl(1, F_UNLCK, F_WRLCK);
  //flock(1, LOCK_UN);
}

void Query_report::start_clock()
{
  gettimeofday(&start, NULL);
}

void Query_report::stop_clock()
{
  gettimeofday(&end, NULL);
}


void Query_report::update_from_fd(int fd)
{
  char buf[MAX_REPORT_LEN];
  int q_type_len, data_len, bytes_to_read, num_recs;

  if((data_len = read(fd, buf, 1)) < 0)
    die(1, "error on read() in update_from_fd()");
  if(data_len == 0) return;

  num_recs = (int)(*(unsigned char*)buf);
  int i;

  for(i = 0; i < num_recs; i++)
    {
      if((data_len = read(fd, buf, 1)) < 0)
	die(1, "error on read() in update_from_fd()");
      if(data_len == 0) return;
      q_type_len = (int) (*(unsigned char*)buf);
      bytes_to_read = q_type_len + 3 * sizeof(int);
      if((data_len = read(fd, buf, bytes_to_read)) < 0)
	die(1, "error on read() in update_from_fd():");
      
      if(data_len < bytes_to_read)
	die(1, "incomplete read in update_from_fd(), fix it some day");
      
      char* read_q_type = buf;
      // watch this cool hack, moving them, bytes around!
      int *p_read_num_queries = (int*)(buf + q_type_len),
	*read_max_q_time, *read_min_q_time;
      read_max_q_time = p_read_num_queries + 1;
      read_min_q_time = read_max_q_time + 1;
      int read_num_queries = *p_read_num_queries;
      // now that we've save num_queries, it is safe to slap  \0
      *p_read_num_queries = 0; // 4 bytes at once is OK, even faster
      Query_type_report *q = type_reports[read_q_type];
      if(q)
	q->update(read_num_queries, *read_max_q_time, *read_min_q_time);
      else
	die(0, "Internal error, invalid q_type '%s'", read_q_type);
    }
}

void  Query_report::fd_send(int fd)
{
  map<string, Query_type_report*, less<string> >::iterator i =
    type_reports.begin();
  char buf[MAX_REPORT_LEN];
  long len = 0; int num_recs = 0;
  char* p = (char*)buf + 1, *p_end = (char*)buf+sizeof(buf);
  
  while(i != type_reports.end())
    {
      string s((*i).first);
      long str_len = (*i).first.length();
      if((long)p + str_len + 3 *sizeof(int)  < (long)p_end )
      {
        *p++ = (char) str_len;
	const char* q_type_name = s.c_str();
	memcpy(p,q_type_name , str_len);
	p += str_len;
	memcpy(p, &((*i).second->num_queries), sizeof(int));
	p += sizeof(int);
	memcpy(p, &((*i).second->max_time), sizeof(int));
	p += sizeof(int);
	memcpy(p, &((*i).second->min_time), sizeof(int));
	p += sizeof(int);
        i++;
	num_recs++;
      }
      else
	die(0, "report buffer overflow -- too many query types");
    }
  
  len = (long)p - (long)buf;
  *(buf) = num_recs;

  if(write(fd, buf, len) != len)
     die(1, "write error in fd_send:");
}

Query_barrel::~Query_barrel()
{
  map<string, Query_type_report*, less<string> >::iterator i =
    qr.type_reports.begin();

  while(i != qr.type_reports.end())
    {
      delete (*i).second;
      i++;
    }

  int j;
  for(j = 0; j < actions.size(); j++)
    delete actions[j];
}










