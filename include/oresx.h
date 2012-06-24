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

//Filename    : ORESX.H
//Description : Header file of Object ResourceIdx

#ifndef __ORESX_H
#define __ORESX_H

#include <ofile.h>

//--------- Define structure ResIndex ----------//

struct ResIndex
{
   char name[9];
   long pointer;
} __attribute__((packed));

//--------- Define class ResourceIdx ----------//

class ResourceIdx : public File
{
public:
	enum     { DEF_BUF_SIZE = 5120 };   // default buffer size : 5K

	ResIndex *index_buf;         // index buffer pointer
	char     *data_buf;          // data buffer pointer
	unsigned data_buf_size;      // size of the data buffer

	// ##### begin Gilbert 15/9 #######//
	ResIndex *sorted_index_buf;
	// ##### end Gilbert 15/9 #######//

	char	   init_flag;
	char     read_all;           // read all data from resource file to memory
	char     use_common_buf;     // use vga's buffer as data buffer or not
	int      rec_count;          // total no. of records
	int      cur_rec_no;         // current record no

	char*		user_data_buf;
	unsigned user_data_buf_size;
	int		user_start_read_pos;	// the starting position of the data to be read into the buffer

public:
	ResourceIdx() 	{ init_flag=0; }
	~ResourceIdx()	{ deinit(); }

   ResourceIdx(const char* resFile, int readAll, int useCommonBuf=0)
       { init_flag=0; init(resFile, readAll, useCommonBuf); }

   void  init(const char* resFile, int readAll, int useCommonBuf=0);
	void  deinit();

	int   is_inited() 	{ return init_flag; }

	void	set_user_buf(char* userDataBuf, int bufSize, int userStartReadPos=0);
	void  reset_user_buf();

   char* read(const char*);
	int	read_into_user_buf(char* dataName, char* userDataBuf, int bufSize, int userStartReadPos=0);
	int 	data_size(char* dataName);

   int   get_index(const char*);
   char* get_data(int);
	char	*data_name(int);

   File* get_file(const char*, int&);
   File* get_file(int, int&);

	// ##### begin Gilbert 15/9 #######//
protected:
	void	build_sorted_index();
	// ##### end Gilbert 15/9 #######//
};

//--------------------------------------------//

#endif
