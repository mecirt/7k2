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

//Filename    : OFILE.CPP
//Description : Object File

#define NO_WINDOWS

#include <all.h>
#include <ofile.h>
#include <obox.h>
#include <stdio.h>

//--------- Define static variables -----------//

static const char *path_array[] = { "" };	// multiple search path

//-------- Begin of function File::file_open ----------//
//
// Open an existing file for reading
// If the file is not found, call err.run()
//
// <char*> fileName      = name of the file
// [int]   handleError   = handle error
//                         ( if 1, when error happen, err.run() will be called
//                           immediately )
//                         ( default : 1 )
// [int]   allowVarySize = allow the writing size and the read size to be different
//                         ( default : 0 )
//
// return : 1-success, 0-fail
//
int File::file_open(const char* fileName, int handleError, int allowVarySize)
{
	if( strlen(fileName) > MAX_PATH )
      err.run( "File : file name is too long." );

   if (file_handle != NULL)
      file_close();

   strcpy( file_name, fileName );

   handle_error    = handleError;
   allow_vary_size = allowVarySize;

   //--------- search all path to open the file ------//

   for( int i=0 ; i<sizeof(path_array)/sizeof(path_array[0]) ; i++ )
   {
      char    filePath[MAX_PATH];
      strcpy( filePath, path_array[i] );
      strcat( filePath, file_name );

      file_handle = fopen(filePath, "r");

      if (file_handle) return true;
   }

   // No luck. Try to convert the ext to lowercase
   int len = strlen(file_name);
   if (len > 3) {
     file_name[len-1] = tolower(file_name[len-1]);
     file_name[len-2] = tolower(file_name[len-2]);
     file_name[len-3] = tolower(file_name[len-3]);
   }

   for( int i=0 ; i<sizeof(path_array)/sizeof(path_array[0]) ; i++ )
   {
      char    filePath[MAX_PATH];
      strcpy( filePath, path_array[i] );
      strcat( filePath, file_name );

      file_handle = fopen(filePath, "r");

      if (file_handle) return true;
   }

   // still nothing. convert it all to lowercase
   if (len > 3) {
     for (int i = len - 1; i >= 0; i--) {
       if (file_name[i] == '/') break;
       file_name[i] = tolower(file_name[i]);
     }
   }

   for( int i=0 ; i<sizeof(path_array)/sizeof(path_array[0]) ; i++ )
   {
      char    filePath[MAX_PATH];
      strcpy( filePath, path_array[i] );
      strcat( filePath, file_name );

      file_handle = fopen(filePath, "r");

      if (file_handle) return true;
   }

   // try with the first letter in uppercase
   if (len > 3) {
     for (int i = len - 1; i >= 0; i--) {
       if (file_name[i-1] == '/') {
         file_name[i] = toupper(file_name[i]);
         break;
       }
     }
   }

   for( int i=0 ; i<sizeof(path_array)/sizeof(path_array[0]) ; i++ )
   {
      char    filePath[MAX_PATH];
      strcpy( filePath, path_array[i] );
      strcat( filePath, file_name );

      file_handle = fopen(filePath, "r");

      if (file_handle) return true;
   }


   err.run( "Error opening file %s.", file_name );

   return false;
}
//---------- End of function File::file_open ----------//


//-------- Begin of function File::file_create ----------//
//
// Create a new file for writing (reading is also permitted)
//
// <char*> fileName      = name of the file
// [int]   handleError   = handle error
//                         ( if 1, when error happen, err.run() will be called
//                           immediately )
//                         ( default : 1 )
// [int]   allowVarySize = allow the writing size and the read size to be different
//                         ( default : 0 )
//
//
// return : 1-success, 0-fail
//
int File::file_create(const char* fileName, int handleError, int allowVarySize)
{
   if( strlen(fileName) > MAX_PATH )
      err.run( "File : file name is too long." );

   strcpy( file_name, fileName );

   handle_error    = handleError;
   allow_vary_size = allowVarySize;

   // cannot use creat() because it can't specify Binary file type

   file_handle = fopen(fileName, "w");

   if( file_handle == NULL)
     err.run( "Error creating file %s", file_name );

   return true;
}
//---------- End of function File::file_create ----------//

//### begin alex 24/7 ###//
//-------- Begin of function File::file_append ----------//
//
// Open an existing file for reading
// If the file is not found, call err.run()
//
// <char*> fileName      = name of the file
// [int]   handleError   = handle error
//                         ( if 1, when error happen, err.run() will be called
//                           immediately )
//                         ( default : 1 )
// [int]   allowVarySize = allow the writing size and the read size to be different
//                         ( default : 0 )
//
// return : 1-success, 0-fail
//
int File::file_append(const char* fileName, int handleError, int allowVarySize)
{
	if( strlen(fileName) > MAX_PATH )
      err.run( "File : file name is too long." );

   if (file_handle != NULL)
      file_close();

   strcpy( file_name, fileName );

   handle_error    = handleError;
   allow_vary_size = allowVarySize;

   //--------- search all path to open the file ------//

   for( int i=0 ; i<sizeof(path_array)/sizeof(path_array[0]) ; i++ )
   {
      char    filePath[MAX_PATH];
      strcpy( filePath, path_array[i] );
      strcat( filePath, fileName );

      file_handle = fopen(filePath, "a");

      if (file_handle != NULL)
			return true;
   }

   err.run( "Error opening file %s.", file_name );

   return false;
}
//---------- End of function File::file_append ----------//
//#### end alex 24/7 ####//

//-------- Begin of function File::file_close ----------//
//
void File::file_close()
{
   if (file_handle != NULL)
   {
      fclose(file_handle);
      file_handle = NULL;
   }
}
//---------- End of function File::file_close ----------//


//-------- Begin of function File::~File ----------//
//
File::~File()
{
   file_close();
}
//---------- End of function File::~File ----------//


//-------- Begin of function File::file_write ----------//
//
// Write a block of data to the file
//
// <void*>    dataBuf = pointer to data buffer to be written to the file
// <unsigned> dataSize = length of the data (must < 64K)
//
// return : 1-success, 0-fail
//
int File::file_write(void* dataBuf, unsigned dataSize)
{
   err_when(file_handle == NULL);       // not initialized

   if( allow_vary_size )        // allow the writing size and the read size to be different
   {
      if( dataSize > 0xFFFF )
         file_put_unsigned_short(0);    // if exceed the unsigned short limit, don't write it
      else
         file_put_unsigned_short( dataSize );
   }

   int actualWritten = fwrite (dataBuf, dataSize, 1, file_handle);
		
   if( !actualWritten && handle_error )
      err.run( "Error writing file %s", file_name );

   return actualWritten;
}
//---------- End of function File::file_write ----------//

//-------- Begin of function File::file_read ----------//
//
// Read a block of data from the file
//
// <void*>    dataBuf = pointer to data buffer to be written to the file
// <unsigned> dataSize = length of the data (must < 64K)
//
// return : 1-success, 0-fail
//

int File::file_read(void* dataBuf, unsigned dataSize)
{
  if (dataSize == 0) return 1;

	#define MAX_READ_SIZE 0xFFF0

	err_when (file_handle == NULL);       // not initialized

	int curPos=file_pos();

  start_read:

   unsigned readSize=dataSize, writtenSize=dataSize;

   if( allow_vary_size )        // allow the writing size and the read size to be different
   {
      writtenSize = file_get_unsigned_short();

      if( writtenSize )    // writtenSize==0, if the size > 0xFFFF
         readSize = min(dataSize, writtenSize);  // the read size is the minimum of the written size and the supposed read size
   }

   int actualRead = fread(dataBuf, 1, dataSize, file_handle);

   if( readSize < writtenSize )
      file_seek( writtenSize-readSize, false );

   //---- if the data size has been increased, reset the unread area ---//

   if( readSize < dataSize )
      memset( (char*)dataBuf+readSize, 0, dataSize-readSize );

   //----- if reading error, popup box and ask for retry -----//

	if( !actualRead && handle_error )
	{
		char msgStr[100];

		sprintf( msgStr, "Error reading file %s, Retry ?", file_name );
puts(msgStr);

		if( box.ask(msgStr) )
		{
			file_seek( curPos );
			goto start_read;
		}
	}

	return actualRead;
}
//---------- End of function File::file_read ----------//


//-------- Begin of function File::file_put_short ----------//
//
// Put a short integer to the file
//
// <short int> = the short integer
//
// return : 1-success, 0-fail
//
int File::file_put_short(short value)
{
   err_when( file_handle == NULL );       // not initialized

	unsigned int actualWritten;	

	if( fwrite (&value, sizeof(short), 1, file_handle) )
		return true;
	if( handle_error )
		err.run( "Error writing file %s", file_name );

	return false;
}
//---------- End of function File::file_put_short ----------//


//-------- Begin of function File::file_get_short ----------//
//
// Get a short integer from the file
//
// return : the short integer
//
short File::file_get_short()
{
   err_when( file_handle == NULL );       // not initialized

	unsigned int actualRead;	
	short value;

	if( fread (&value, sizeof(short), 1, file_handle) )
		return value;
	if( handle_error )
		err.run( "Error reading file %s", file_name );

	return false;
}
//---------- End of function File::file_get_short ----------//


//-------- Begin of function File::file_put_unsigned_short ----------//
//
// Put a unsigned short integer to the file
//
// <unsigned short> = the short integer
//
// return : 1-success, 0-fail
//
int File::file_put_unsigned_short(unsigned short value)
{
   err_when( file_handle == NULL );       // not initialized

	if( fwrite (&value, sizeof(unsigned short), 1, file_handle) )
		return true;
	if( handle_error )
		err.run( "Error writing file %s", file_name );

	return false;
}
//---------- End of function File::file_put_unsigned_short ----------//


//-------- Begin of function File::file_get_unsigned_short ----------//
//
// Get a short integer from the file
//
// return : the short integer
//
unsigned short File::file_get_unsigned_short()
{
   err_when( file_handle == NULL );       // not initialized

	unsigned short value;

	if( fread (&value, sizeof(unsigned short), 1, file_handle) )
		return value;
	if( handle_error )
		err.run( "Error reading file %s", file_name );

	return 0;
}
//---------- End of function File::file_get_unsigned_short ----------//


//-------- Begin of function File::file_put_long ----------//
//
// Put a long integer to the file
//
// <long int> = the long integer
//
// return : 1-success, 0-fail
//
int File::file_put_long(long value)
{
   err_when( file_handle == NULL );       // not initialized

	unsigned int actualWritten;	

	if( fwrite (&value, sizeof(long), 1, file_handle) )
		return true;
	if( handle_error )
		err.run( "Error writing file %s", file_name );

	return false;
}
//---------- End of function File::file_put_long ----------//


//-------- Begin of function File::file_get_long ----------//
//
// Get a long integer from the file
//
// return : the long integer
//
long File::file_get_long()
{
   err_when( file_handle == NULL );       // not initialized

	unsigned int actualRead;	
	long  value;

	if( fread (&value, sizeof(long), 1, file_handle) )
		return value;
	if( handle_error )
		err.run( "Error reading file %s", file_name );

	return false;
}
//---------- End of function File::file_get_long ----------//


//---------- Start of function File::file_seek ---------//
//
// <long> offset = seek offset
// [int]  fromStart - whether to seek from start (default yes)
//
// return : the offset of the pointer's new position, measured in
//          bytes from the file beginning.
//
long File::file_seek(long offset, bool fromStart)
{
  return fseek(file_handle, offset, fromStart ? SEEK_SET : SEEK_CUR);
}
//------------ End of function File::file_seek ----------//


//---------- Start of function File::file_pos ---------//
//
// Get the position of current file pointer
//
// return : the position of current file pointer
//
long File::file_pos()
{
   return ftell(file_handle);
}

//------------ End of function File::file_pos ----------//


//---------- Start of function File::file_size ---------//

long File::file_size()
{
  long pos = ftell(file_handle);
  fseek(file_handle, 0, SEEK_END);
  long sz = ftell(file_handle);
  fseek(file_handle, pos, SEEK_SET);
  return sz;
}
//------------ End of function File::file_size ----------//
