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

#include <stdio.h>
#include <iostream>
#include <string>
#include <sys/wait.h>
#include <unistd.h>

#ifdef __linux__
// TODO: get rid of getopt_long, and use my own option parsing code instead
#define HAVE_LONG_OPT
#endif

#ifdef HAVE_LONG_OPT
#include <getopt.h>
#endif

#include "die.h"
#include "query.h"
#include "client.h"
#include "engines.h"
#include "parse.h"

#include "super-smack.h"

int glob_argc = 0;
char **glob_argv = 0;


#ifdef HAVE_LONG_OPT
struct option long_options[] =
{
  {"help",    no_argument,       0, 'h'},
  {"version", no_argument,       0, 'V'},
  {"db-type", required_argument, 0, 'd'},
  {"datadir", required_argument, 0, 'D'},
  {0,0,0,0}     
};
#endif

int db_type=1;
FILE *input = stdin;

void set_parse_file(char* fname)
{
  FILE *fp = fopen(fname, "r");
  if(!fp) die(1, "Could not open input file %s", fname);
  input = fp;
}

void set_db_type(const char* db_str)
{
  if(!(db_type = engine_id(db_str)))
    die(0, "Unknown database type '%s'", db_str);
}

#ifndef HAVE_LONG_OPT
#define getopt_long(argc,argv,args,not_used1,not_used2) getopt(argc,argv,args)
#endif

void get_options()
{
  int c, opt_ind = 0;
  while((c = getopt_long(glob_argc, glob_argv, "hVd:D:", long_options,
			 &opt_ind)) != EOF)
    {
      switch(c)
	{
	case 'h': usage(); exit(0);
	case 'V': print_version(); exit(0);
	case 'd': set_db_type(optarg); break;
	case 'D': datadir = optarg; break;
	default: usage(); exit(1);
	}
    }

  glob_argc -= optind;
  glob_argv += optind;
  
  if(glob_argc > 0)
    {
      set_parse_file(glob_argv[0]);
    }
}


void wait_for_client_threads()
{
  int i;
  for(i = 0; i < client_pids.size(); i++)
    {
      waitpid(client_pids[i], 0, 0);
    }
}

void print_version()
{
  printf("%s version %s \n", progname, VERSION);  
}

void print_engines()
{
  printf("Id\tHandle\t\tName\t\tVersion\t\tAuthor\n");
  printf("--\t------\t\t----\t\t-------\t\t------\n");
  for(int i=0; engines[i].id > 0; i++)
    printf("%i\t%s%s\t%s%s\t%s%s\t%s\n", 
           engines[i].id,
           engines[i].handle, strlen(engines[i].handle)>7?"":"\t",
           engines[i].name, strlen(engines[i].name)>7?"":"\t",
           engines[i].version, strlen(engines[i].version)>7?"":"\t",
           engines[i].author);
}

void usage()
{
  print_version();
  fprintf(stderr, "MySQL AB, by Sasha Pachev and Jeremy Cole\n");
  fprintf(stderr, "Runs multi-threaded benchmarks on database engines.\n\n");
  fprintf(stderr, "The following engines are supported:\n\n");
  print_engines();
  fprintf(stderr, "\n\
Usage: super-smack [options] [smack_source]\n\
\
Valid options are:\
  -h, --help               Display this message\n\
  -V, --version            Show version\n\
  -d, --db-type=handle     Select database type\n\
  -D, --datadir=path       Path to super-smack datadir\n\
\n\
");
}

int main(int argc, char** argv)
{
  glob_argc = argc;
  glob_argv = argv;
  get_options();
  do_parse(input);

  if(current_client == "main")
      wait_for_client_threads();

  return(0);
}




