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

//Filename    : ODIR.H
//Description : Object Directory Header

#ifndef __ODIR_H
#define __ODIR_H

#include <odynarr.h>
#include <time.h>

//---------- Define struct FileInfo ----------//

struct FileInfo
{
    char				name[MAX_PATH+1];
    unsigned long size;
    time_t time;
};

//---------- Define class Directory ----------//

class Directory : public DynArray
{
public:
   Directory();

   int       read(const char*, int=0);
   FileInfo* operator[](int recNo)  { return (FileInfo*) get(recNo); }
};

//--------------------------------------------//

#endif
