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

//Filename    : ODIR.CPP
//Description : Object Directory

#include <string.h>
#include <win32_compat.h>
#include <odir.h>
#include <glob.h>
#include <unistd.h>
#include <sys/stat.h>


//----------- Define static function ------------//

static int sort_file_function( const void *a, const void *b );


//------- Begin of function Directory::Directory -------//

Directory::Directory() : DynArray( sizeof(FileInfo), 20 )
{
}

//-------- End of function Directory::Directory -------//


//------- Begin of function Directory::read -------//
//
// Read in the file list of the specified file spec.
//
// <char*> fileSpec = the file spec of the directory
// [int]   sortName = sort the file list by file name
//                    (default : 0)
//
// return : <int> the no. of files matched the file spec.
//
int Directory::read(const char *fileSpec, int sortName)
{
  printf ("Searching for %s\n", fileSpec);

  glob_t globs;
  if (glob (fileSpec, GLOB_NOSORT, NULL, &globs) != 0) return 0;
  int count = globs.gl_pathc;

  FileInfo fileInfo;
  for (int i = 0; i < count; ++i) {
    m.extract_file_name(fileInfo.name, globs.gl_pathv[i]);
    struct stat info;
    stat(fileInfo.name, &info);
    fileInfo.size = info.st_size;
    fileInfo.time = info.st_mtime;

    linkin(&fileInfo);
  }

  globfree (&globs);

  printf ("Success, returning %d\n", count);
  return count;
}
//-------- End of function Directory::read -------//


//------ Begin of function sort_file_function ------//
//
static int sort_file_function( const void *a, const void *b )
{
	return strcasecmp( ((FileInfo*)a)->name, ((FileInfo*)b)->name );
}
//------- End of function sort_file_function ------//
