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

#ifndef ENGINES_H
#define ENGINES_H

#include "../config.h"

#include "client.h"

#define MAX_ENGINES 64

class Engine
{
 public:
  Engine() {};
  Engine(char *name, char *handle, Client *(*create)(), char *version, char *author);
  ~Engine();

  int id;
  char *name;
  char *handle;

  Client *(*create)();

  char *version;
  char *author;
};

extern class Engine engines[MAX_ENGINES];

Client *new_client(const char *handle);
Client *new_client(int id);

int engine_id(const char *handle);

#endif /* ENGINES_H */
