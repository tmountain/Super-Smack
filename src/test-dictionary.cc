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

#include <iostream>
#include <fstream>
#include "dictionary.h"  

int  main(int argc, char** argv )
{
 char *file = "/usr/dict/words";
 Rand_dictionary rd,rd1;
 rd.load_file(file);
 int i;
 cout << "Random from file: " << endl;
 for(i = 0; i < 10; i++)
   {
     cout << rd.next_word() << endl;
   }

 rd1.load_list("sasha,sarah,matt,monty,david");
 cout << "Random from list: " << endl;
 for(i = 0; i < 10; i++)
   {
     cout << rd1.next_word() << endl;
   }

 Seq_dictionary sd, sd1;
 sd.load_file(file);
 sd1.load_list("sasha,sarah,benjamin,dallin,runner,word");
 cout << "Sequential from file: " << endl;

 for(i = 0; i < 10; i++)
   {
     cout << sd.next_word() << endl;
   }

 cout << "Sequential from list: " << endl;

 for(i = 0; i < 10; i++)
   {
     cout << sd1.next_word() << endl;
   }

 cout << "Unique: " << endl;
 Unique_dictionary ud;
 ud.set_template("sasha%04d");
 
 for(i = 0; i < 10; i++)
   {
     cout << ud.next_word() << endl;
   }
 
}

