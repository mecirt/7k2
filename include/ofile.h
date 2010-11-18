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

// #include <windows.h>

// -------- define constant and types assemble to Windows' -------//
// if definition is different from Windows, it will be reflected in compiling OFILE.CPP

typedef void *HANDLE;

#define MAX_PATH 260
#define INVALID_HANDLE_VALUE (HANDLE)-1

#define FILE_BEGIN   0
#define FILE_CURRENT 1
#define FILE_END     2

//--------------------------------------//

class File
{
public:
   char file_name[MAX_PATH+1];

   HANDLE file_handle;
   int    handle_error;

   char   allow_vary_size;    // allow the writing size and the read size to be different

public:
   File()     { file_handle=INVALID_HANDLE_VALUE; }
   ~File();

   int   file_open(char*, int=1, int=0);
   int   file_create(char*, int=1, int=0);
	int	file_append(char*, int=1, int=0);
   void  file_close();

   long  file_size();
   long  file_seek(long, int= -1);
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


