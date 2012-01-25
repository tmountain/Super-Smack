#ifndef SMACK_H
#define SMACK_H

#ifndef MAX_PATH
#define MAX_PATH 128
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h> 
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

extern const char* datadir;

char* mk_data_path(char* buf, const char* file);
int populate_data_file(const char* file, const char* cmd);

#endif
