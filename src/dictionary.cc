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

#include "dictionary.h"
#include <iostream>
#include <fstream>
#include <stdio.h>

#include "smack.h"

#define MAX_FILE_LINE 512
#define MAX_UNIQUE    512
#define DICT_HUNK     8192

Dictionary::Dictionary():max_field_size(0),min_field_size(0),
			 delim('\n'),file_size_equiv(0)
{
  words.reserve(DICT_HUNK);
}

int Dictionary::load_file(const char* file)
{
  char path[MAX_PATH];
  file = mk_data_path(path, file);
  struct stat f;
  if(stat(file, &f))
    {
      cerr << "file " << file << " failed to stat" << endl;
      return -1;
    }
  int step = 1;
  if(file_size_equiv)
   step = f.st_size/file_size_equiv;
  if(!step) ++step;
  
  ifstream is(file);
  if(!is)
    {
      cerr << "could not open " << file << endl;
      return -1;
    }
  int line = 1;
  for(;!is.eof();++line)
    {
      char buf[MAX_FILE_LINE];
      is.getline(buf, sizeof(buf));
      if(is.eof()) return 0;
      if((line % step)) continue;
      
      char* p = strchr(buf, delim);
      if(p) *p = 0;
      words.insert(words.end(),buf);
    }
  
  return 0;
}

void Dictionary::load_list(const char *list_str)
{
  char word_buf[strlen(list_str) + 1];
  int i,c,j = 0;
  int escaped = 0;
  
  for(i = 0; (c = list_str[i]); i++)
    {
      switch(c)
        {
        case ',':
          if(escaped)
           {
            word_buf[j++] = c;
            escaped = 0;
           } 
          else
            {
              word_buf[j] = 0;
              words.insert(words.end(), word_buf);
              j = 0;
            }
         
          break;
        case '\\':
          if(escaped)
            word_buf[j++] = c;
          else
            escaped = 1;
          
        default:
          escaped = 0;
          word_buf[j++] =c;
          break;
        }
    }
}

const char*  Seq_dictionary::next_word()
{
  const char* res = words[cur_word_ind++].c_str();
  if(cur_word_ind == words.size())
    cur_word_ind = 0;
  return res;
}

const char*  Rand_dictionary::next_word()
{
  const char* res = words[rand() % words.size()].c_str();
  return res;
}

void Unique_dictionary::fix_template()
{
  string tmp = templ;
  templ = "";
  int saw_field = 0, last_percent = 0;
  int i = 0;
  for(i = 0; i < tmp.length(); i++)
    {
      int c = tmp[i];
      if(c == '%')
	{
	  last_percent = 1;
	  continue;
	}
      if(last_percent && c != '%')
	{
	  if(saw_field)
	    break;
	  saw_field = 1;
	  templ += '%';
	}
     
      
	 templ += (char)c;
	 last_percent = 0;
    }
}

const char*  Unique_dictionary::next_word()
{
  static char buf[MAX_UNIQUE];
  snprintf(buf, sizeof(buf), templ.c_str(), id++); 
  return buf;
}

void Seq_dictionary::print()
{
  cout << "seq_dictionary " << name << " " << words.size() <<
    " words" << endl;
}

void Unique_dictionary::print()
{
  cout << "unique_dictionary " << name << " " << templ <<
    " template" << endl;
}

void Rand_dictionary::print()
{
  cout << "rand_dictionary " << name << " " << words.size() <<
    " words" << endl;
}






