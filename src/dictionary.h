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

#ifndef DICTIONARY_H
#define DICTIONARY_H

using namespace std;

#include "../config.h"

#include <string>
#include <vector>
#include <time.h>
#include <stdlib.h>

extern const char* datadir;


class Dictionary
{
protected:
  vector<string> words;
public:
  string name;
  int max_field_size;
  int min_field_size;
  char delim;
  int file_size_equiv;
  
  Dictionary();
  virtual ~Dictionary(){}
  int load_file(const char* file);
  void load_list(const char* list_str);
  virtual const char* next_word() = 0;
  virtual void print() = 0;
};

class Rand_dictionary: public Dictionary
{
public:
  Rand_dictionary(): Dictionary()
   {
    srand(time(NULL));
   }
  
  const char* next_word();
  void print();
};

class Seq_dictionary: public Dictionary
{
protected:
 int cur_word_ind;
public:
 Seq_dictionary(): Dictionary(),cur_word_ind(0)
  {
  }
  const char* next_word();
  void print();
};

class Unique_dictionary: public Dictionary
{
protected:
 int id;
 string templ;
 int buf_size;

 void fix_template();
  
public:
 Unique_dictionary(): Dictionary(),id(0),templ("%d")
  {
    buf_size = 512;
  }
  void set_template(const char * templ)
   {
    this->templ = templ;
    fix_template();
    buf_size = strlen(templ) + 512;
   }
  const char* next_word();
  void print();
};

#endif

