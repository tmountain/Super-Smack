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

#ifndef VAL_H
#define VAL_H

#include "../config.h"

#include <stdio.h>

struct Val
{
  string strval;
  double dval;
  bool delete_me_not;
  Val() : strval(""), dval(0.0),delete_me_not(0) {}
  Val(string& s) { set(s);}
  Val(const char* s) { set(s);}
  Val(double d) { set(d);}
  Val(int n) { set(n);}

  void set(string& s) { strval = s; dval=atof(s.c_str());}
  void set(const char* s) { strval = s; dval=atof(s);}
  void set(double d)
  {
    dval = d;
    char buf[32];
    snprintf(buf, sizeof(buf), "%f", d );
    strval = buf;
  }
  void set(int n) { set((double)n);}
  void set(Val& other)
  {
    if(this != &other)
      {
	strval = other.strval;
	dval = other.dval;
      }
  }
  void concat(Val& other)
  {
    string tmp = strval + other.strval; 
    set(tmp);
  }
};

extern map<string, Val*, less<string> > var_hash;

#endif
