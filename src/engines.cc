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
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <strings.h>

#include "engines.h"

class Engine engines[MAX_ENGINES];

static int initialized = 0;

Engine::Engine(char *name, char *handle, Client *(*create)(), char *version, char *author)
{
  int i=0;

  if(!initialized) {
    bzero(engines, sizeof(engines));
    initialized = 1;
  };

  for(i=0; i < MAX_ENGINES; i++) {
    if(engines[i].id == 0) {
      engines[i].id = i+1;
      engines[i].name = strdup(name);
      engines[i].handle = strdup(handle);
      engines[i].create = create;
      engines[i].version = strdup(version);
      engines[i].author = strdup(author);
      i=MAX_ENGINES;
    };
  };
};

Engine::~Engine()
{
  if(this->id) {
    this->id = 0;
    free(this->name);
    free(this->version);
    free(this->author);
  };
};

Client *new_client(const char *handle) {
  if(handle)
    for(int i=0; i < MAX_ENGINES; i++)
      if(engines[i].id && !strcmp(engines[i].handle, handle))
        return engines[i].create();
  return NULL;
};

Client *new_client(int id) {
  if(id > 0 && id < MAX_ENGINES) 
    for(int i=0; i < MAX_ENGINES; i++)
      if(engines[i].id == id)
        return engines[i].create();
  return NULL;
};

int engine_id(const char *handle) {
  if(handle)
    for(int i=0; i < MAX_ENGINES; i++)
      if(engines[i].id && !strcmp(engines[i].handle, handle))
        return engines[i].id;
  return 0;
};
