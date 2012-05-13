/*
 * Seven Kingdoms 2: The Fryhtan War
 *
 * Copyright 1999 Enlight Software Ltd.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

//Filename    : OFILE.H
//Description : Header file for file object

#ifndef __OFILE_H
#define __OFILE_H

#include <stdio.h>

//--------------------------------------//

class File
{
public:
   char file_name[MAX_PATH+1];

   FILE *file_handle;
   int    handle_error;

   char   allow_vary_size;    // allow the writing size and the read size to be different

public:
   File()     { file_handle = NULL; }
   ~File();

   int   file_open(const char*, int=1, int=0);
   int   file_create(const char*, int=1, int=0);
   int   file_append(const char*, int=1, int=0);
   void  file_close();

   long  file_size();
   long  file_seek(long, bool fromStart = true);
   long  file_pos();

   int   file_read(void*, unsigned);
   int   file_write(void*, unsigned);

   short file_get_short();
   int   file_put_short(short);

   unsigned short file_get_unsigned_short();
   int   file_put_unsigned_short(unsigned short);

   long  file_get_long();
   int   file_put_long(long);
};

#endif


