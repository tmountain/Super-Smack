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

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "die.h"

#define MAX_STR_WIDTH 256

int num_rows = 0;
int row_num = 1;
const char* fmt = 0, *fmt_end;
FILE* out;

struct option long_options[] =
{
  {"help",     no_argument,       0, 'h'},
  {"version",  no_argument,       0, 'V'},
  {"num-rows", required_argument, 0, 'n'},
  {"format",   required_argument, 0, 'f'},
  {0,0,0,0}
};

void print_version()
{
  fprintf(stderr, "%s version %s \n", progname, VERSION);  
}

void usage()
{
  print_version();
  fprintf(stderr, "MySQL AB, by Sasha Pachev\n");
  fprintf(stderr, "Prints lines random data to stdout in given format\n");
  fprintf(stderr, "Usage: gen-data [options]\n");
  fprintf(stderr, "-?, --help - this message\n");
  fprintf(stderr, "-V, --version - show version\n");
  fprintf(stderr, "-n, --num-rows=num - number of rows\n");
  fprintf(stderr, "-f, --format=fmt_str - format string\n");
  fprintf(stderr, "Format can contain any character + %% followed by\n\
a format specifier. Valid format specifiers: \n\
 s - string \n\
 d - integer \n\
\n\
Additionally, you can prefix a format speficier with: \n\
 n - generate exactly n characters \n\
 m-n - generate between m and n  characters \n");
  

}

void parse_args(int argc, char** argv)
{
  int c, opt_ind = 0;
  while((c = getopt_long(argc, argv,"?Vf:n:", long_options,
			 &opt_ind)) != EOF)
    {
      switch(c)
	{
	case '?': usage(); exit(0);
	case 'V': print_version(); exit(0);
	case 'f': fmt = optarg; break;
	case 'n': num_rows = atoi(optarg); break;
	default: usage(); exit(1);
	}
    }

  if(!num_rows || !fmt)
    {
      die(0, "You must supply num-rows and fmt");
    }
  fmt_end = fmt + strlen(fmt);
}

void print_str(int width, bool dot)
{
  width = (dot) ;
}

int get_rand(int min_r, int max_r)
{
  return (min_r == max_r) ? min_r : min_r + rand() % (max_r - min_r); 
}

int get_width(int min_width, int max_width)
{
  if(!max_width)
    return min_width;
  else if(min_width != max_width)
    return get_rand(min_width, max_width);
  
  return min_width; 
}

void print_row_num(int min_width, int max_width)
{
  int width = get_width(min_width, max_width);
  fprintf(out, "%*d", width, row_num);
}

void print_num(int min_width, int max_width)
{
  int width = get_width(min_width, max_width);
  fprintf(out, "%*d", width, rand());
}

void print_str(int min_width, int max_width)
{
  int width = get_width(min_width, max_width);
  if(width > MAX_STR_WIDTH || !width)
    width = MAX_STR_WIDTH;
  int i;
  for(i = 0; i < width; ++i)
    fputc('a' + get_rand(0, 'z' - 'a'), out);
}


void print_row()
{
  const char* p;
  int width = 0, min_width = 0;
  enum {ST_FMT, ST_PLAIN} parse_state = ST_PLAIN;
  for(p = fmt; p < fmt_end; ++p)
    {
      char c = *p;
      switch(parse_state)
	{
	case ST_PLAIN:
	  if(c != '%')
	    {
	     fputc(c, out);
	    }
	  else
	    {
	     parse_state = ST_FMT;
             min_width = width = 0;
            }
	  break;

	case ST_FMT:
	  switch(c)
	    {
	    case '%':
	      fputc(c, out);
	      parse_state = ST_PLAIN;
	      break;
	    case '-':
	      min_width = width;
	      width = 0;
	      break;
	    case 's':
	      print_str(min_width, width);
	      parse_state = ST_PLAIN;
	      break;
	    case 'n':
	      print_row_num(min_width, width);
	      parse_state = ST_PLAIN;
	      break;
	    case 'd':
	      print_num(min_width, width);
	      parse_state = ST_PLAIN;
	      break;
	    default:
	      if(isdigit(c))
		width = width * 10 + c - '0';
	      else
		die(0, "'%c' - invalid format specifier", c);
	      break;
	    }
	}
    }
  fputc('\n', out);
  ++row_num;
}

int main(int argc, char** argv)
{
  progname = argv[0];
  out = stdout;
  parse_args(argc, argv);
  int i;
  for(i = 0; i < num_rows; ++i)
    print_row();
}
