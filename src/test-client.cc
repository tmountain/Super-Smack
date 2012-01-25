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

#include "client.h"
#include "engines.h"
#include "die.h"
#include "dictionary.h"

int main(int argc, char** argv)
{
#ifdef HAVE_MYSQL  
  Client *c;
  c = new_client("mysql");
  c->Client::connect("localhost", "test", "root", "");
  Rand_dictionary d;
  if(d.load_file("/usr/dict/words") == -1)
    die(1, "Could not open the dictionary file");
  
  c->set_dictionary("words", &d);
  
  c->safe_parsed_query("select * from mysql_auth where username = '$words'",
                      1024);
  c->lose_result();

  c->safe_parsed_query("select * from mysql_auth where \
username in('$words','$words')",
                      1024);
  c->dump_result("/tmp/mysql-auth-test.dump");

  Query_barrel qb,qb1;
  qb.set_num_rounds(5);
  Query q1,q2;

  q1.query = "select * from mysql_auth";
  q1.parsed = 0;
  q1.name = "get_all";
  q1.type_name = "full_scan";
  q1.has_result_set = 1;
  
  q2.query = "select passwd from mysql_auth where username = '$words'";
  q2.parsed = 1;
  q2.name = "get_by_username";
  q2.type_name = "index_lookup";
  q2.has_result_set = 1;
  qb.add_query_charge(q1, 2);
  qb.add_query_charge(q2, 20);
  
  
  cout << "running a one threaded test:" << endl;
  c->set_query_barrel(&qb);
  //c->unload_query_barrel();
  c->print_barrel_report();

  qb1.set_num_rounds(5);
  qb1.add_query_charge(q2, 20);
  
  cout << "running a multi-threaded test: " << endl;
  c->disconnect();
  int num_threads = 50;
  if(argc > 1) num_threads = atoi(argv[1]);
  cout << "num_threads = " << num_threads << endl;

  c->set_query_barrel(&qb1);
  
  c->start_clock();
  c->create_threads(num_threads);
  c->connect();
  c->unload_query_barrel();
  c->thread_sync();
  c->stop_clock();
  c->print_barrel_report();

  delete c;
#endif  
  return 0;
}


