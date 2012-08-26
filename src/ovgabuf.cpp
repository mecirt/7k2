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

//Filename    : OVGABUF.CPP
//Description : OVGABUF direct draw surface class

#include <ovgabuf.h>
#include <ovga.h>
#include <omodeid.h>
#include <all.h>
#include <omouse.h>
#include <imgfun.h>
#include <osys.h>
#include <oworld.h>
#include <obitmap.h>
#include <obitmapw.h>
#include <ocoltbl.h>
#include <stdio.h>

short *VgaBuf::default_remap_table;
short *VgaBuf::default_blend_table;

// ------- pass inline function ------//

int VgaBuf::buf_size() const { return BufferSize (this); }
int VgaBuf::buf_width()	const { return BufferWidth (this); }
int VgaBuf::buf_height() const { return BufferHeight (this); }


//-------- Begin of function VgaBuf::VgaBuf ----------//

VgaBuf::VgaBuf()
{
	memset( this, 0, sizeof(VgaBuf) );
}
//-------- End of function VgaBuf::VgaBuf ----------//


//-------- Begin of function VgaBuf::~VgaBuf ----------//

VgaBuf::~VgaBuf()
{
	deinit();
}
//-------- End of function VgaBuf::~VgaBuf ----------//


//
// Create a direct draw front buffer.
//
void VgaBuf::init_surface()
{
  if (!InitSurface (this)) return;

	lock_bit_stack = 0;
	lock_stack_count = 0;
	default_remap_table = vga.default_remap_table;	// new for 16-bit
	default_blend_table = vga.default_blend_table;	// new for 16-bit
}


//------ Begin of function VgaBuf::deinit --------//

void VgaBuf::deinit()
{
  DeinitSurface (this);
}
//-------- End of function VgaBuf::deinit ----------//


//-------- Begin of function VgaBuf::is_buf_lost ----------//
//
BOOL VgaBuf::is_buf_lost()
{
  return IsBufferLost (this);
}
//--------- End of function VgaBuf::is_buf_lost ----------//


//-------- Begin of function VgaBuf::restore_buf ----------//
//
// Restore buffers that have been lost.
//
BOOL VgaBuf::restore_buf()
{
  if (!RestoreBuffer (this)) 
  {
#ifdef DEBUG
    debug_msg("Error restoring direct draw buffer");
#endif
    return FALSE;
  }

  return TRUE;
}
//--------- End of function VgaBuf::restore_buf ----------//


//------------- Begin of function VgaBuf::lock_buf --------------//

void VgaBuf::lock_buf()
{
  err_if( buf_locked )
    err_now( "VgaBuf::lock_buf() error, buffer already locked." );

  if (LockBuffer (this))
    buf_locked = TRUE;
  else
  {
#ifdef DEBUG
    debug_msg( "Failed to lock the buffer." );
#endif
  }
}
//--------------- End of function VgaBuf::lock_buf --------------//


//------------- Begin of function VgaBuf::unlock_buf --------------//

void VgaBuf::unlock_buf()
{
  if( !vptr_dd_buf )
    return;
  err_when( !buf_locked );

  if (UnlockBuffer (this))
    buf_locked = FALSE;
  else
  {
#ifdef DEBUG
    debug_msg( "Failed to unlock the buffer." );
#endif
  }
}
//--------------- End of function VgaBuf::unlock_buf --------------//


//------------- Begin of function VgaBuf::temp_unlock --------------//
//
// Unlock the Vga buffer temporarily.
//
void VgaBuf::temp_unlock()
{
	// push buf_locked
	err_when(lock_stack_count >= sizeof(lock_bit_stack)*8);
	err_when(buf_locked != 0 && buf_locked != 1);	// 0 or 1
	lock_bit_stack = (lock_bit_stack << 1) | buf_locked;
	++lock_stack_count;

	if( buf_locked )
		unlock_buf();

	err_when( buf_locked );
}
//--------------- End of function VgaBuf::temp_unlock --------------//


//------------- Begin of function VgaBuf::temp_restore_lock --------------//
//
// Restore the previous lock stage.
// paired with temp_unlock()
//
void VgaBuf::temp_restore_lock()
{
	// pop buf_locked
	err_when(lock_stack_count==0);
	DWORD newBufLocked = lock_bit_stack & 1;
	lock_bit_stack >>= 1;
	lock_stack_count--;
	
	if( newBufLocked )
		lock_buf();
}
//--------------- End of function VgaBuf::temp_restore_lock --------------//



//------------- Begin of function VgaBuf::temp_lock --------------//
//
// Lock the Vga buffer temporarily.
//
void VgaBuf::temp_lock()
{
	// push buf_locked
	err_when(lock_stack_count >= sizeof(lock_bit_stack)*8);
	err_when(buf_locked != 0 && buf_locked != 1);	// 0 or 1
	lock_bit_stack = (lock_bit_stack << 1) | buf_locked;
	++lock_stack_count;

	if( !buf_locked )
		lock_buf();
	err_when( !buf_locked );
}
//------------- End of function VgaBuf::temp_lock --------------//


//------------- Begin of function VgaBuf::temp_restore_unlock --------------//
//
// Restore the previous unlock stage.
// paired with temp_lock()
//
void VgaBuf::temp_restore_unlock()
{
	// pop buf_locked
	err_when(lock_stack_count==0);
	DWORD newBufLocked = lock_bit_stack & 1;
	lock_bit_stack >>= 1;
	lock_stack_count--;
	
	if( !newBufLocked )
		unlock_buf();
}
//------------- End of function VgaBuf::temp_restore_unlock --------------//

void VgaBuf::put_bitmap(int x, int y, char *bitmapBuf, short *colorRemapTable, int transparency, bool hmirror)
{
  put_bitmap_area(x, y, bitmapBuf, -1, -1, -1, -1, colorRemapTable, transparency, hmirror);
}

#define TRANSCODE 0xff
#define MULTITRANS_CODE 0xf8
#define EFFECT_CODE 0xef

// transparency: 0=no, 1=simple, 2=RLE, 3=RLE with half alpha, 4=blend, no transparency, 5=weak blend, no transparency
void VgaBuf::put_bitmap_area(int x, int y, char *bitmapBuf, int srcX1, int srcY1, int srcX2, int srcY2, short *colorRemapTable, int transparency, bool hmirror)
{
  // TODO: codes 4 and 5 (blending)
  bool crop = true;
  if ((srcX1 < 0) && (srcY1 < 0) && (srcX2 < 0) && (srcY2 < 0)) crop = false;

  if (!colorRemapTable) colorRemapTable = default_remap_table;

  // first four bytes hold width/height, so pull them
  int width = *((short*)bitmapBuf);
  bitmapBuf += 2;
  int height = *((short*)bitmapBuf);
  bitmapBuf += 2;

  int skip = 0;

  for (int yy = 0; yy < height; ++yy) {
    for (int xx = 0; xx < width; ++xx) {
      if (skip) {
        skip--;
        continue;
      }
      unsigned char pixel = (unsigned char) *bitmapBuf++;
      if (transparency && (transparency <= 3) && (pixel == TRANSCODE)) continue;
      // skip multiple pixels at once (a sort-of RLE with transparency)
      if (((transparency == 2) || (transparency == 3)) && (pixel >= MULTITRANS_CODE)) {
        if (pixel == MULTITRANS_CODE)
          skip = (unsigned char) *bitmapBuf++;
        else
          skip = TRANSCODE - pixel + 1;
        skip--;  // this one is already skipped
        continue;
      }
      if (crop && ((yy < srcY1) || (yy > srcY2) || (xx < srcX1) || (xx > srcX2))) continue;

      short *bufptr = buf_ptr(x + (hmirror?width-xx:xx), y + yy);

      if (((transparency == 2) || (transparency == 3)) && (pixel >= EFFECT_CODE)) {
        doIMGeffect(pixel - EFFECT_CODE, bufptr);
        continue;
      }

      if (transparency == 3)  // alpha-adjust
      {
        short mask = getAlphaMask(1);
        *bufptr = ((colorRemapTable[pixel] >> 1) & mask) + ((((*bufptr) + 1) >> 1) & mask);
      }
      else
        *bufptr = colorRemapTable[pixel];
    }
  }
}


void VgaBuf::put_bitmapW(int x, int y, short *bitmapBuf, bool transparency)
{
  put_bitmapW_area(x, y, bitmapBuf, -1, -1, -1, -1, transparency);

}

void VgaBuf::put_bitmapW_area(int x, int y, short *bitmapBuf, int srcX1, int srcY1, int srcX2, int srcY2, bool transparency)
{
  bool crop = true;
  if ((srcX1 < 0) && (srcY1 < 0) && (srcX2 < 0) && (srcY2 < 0)) crop = false;

  // first four bytes hold width/height, so pull them
  int width = *bitmapBuf;
  bitmapBuf++;
  int height = *bitmapBuf;
  bitmapBuf++;

  for (int yy = 0; yy < height; ++yy) {
    for (int xx = 0; xx < width; ++xx) {
      short pixel = *bitmapBuf++;
      if (transparency && (pixel == transparent_code_w)) continue;
      if (crop && ((yy < srcY1) || (yy > srcY2) || (xx < srcX1) || (xx > srcX2))) continue;

      short *bufptr = buf_ptr(x + xx, y + yy);
      *bufptr = pixel;
    }
  }
}

void VgaBuf::read_bitmapW(int x1,int y1,int x2,int y2, short* bitmapPtr)
{
  short *imageBuf = cur_buf_ptr;
  long pitch = cur_pitch;
  long w = x2 - x1 + 1, h = y2 - y1 + 1;
  *bitmapPtr = w;
  ++bitmapPtr;
  *bitmapPtr = h;
  ++bitmapPtr;
  for (long y = y1; y <= y2; ++y) {
    memcpy (bitmapPtr, ((char *) imageBuf) + y * pitch + x1 * 2, 2 * w);
    bitmapPtr += w;
  }
}

//---------- Begin of function VgaBuf::save_area ---------//
//
// save_area() differs from save_area() as :
//
// save_area() save the screen to a user-defined buffer.
// save_area()  save the screen to the global screen saving buffer in Vga object
//
// <int>   x1,y1,x2,y2 = the area of the screen
// [char*] saveScr     = the pointer to the previous saved buffer
//                       (default : initialize a new buffer)
//
short* VgaBuf::save_area(int x1, int y1, int x2, int y2, short* saveScr )
{
	err_when( x1>x2 || y1>y2 || x1<0 || y1<0 || x2>=VGA_WIDTH || y2>=VGA_HEIGHT );

	long newSize = sizeof(short)*4 + BitmapW::size((x2-x1+1),(y2-y1+1));

	saveScr = (short *)mem_resize( saveScr, newSize );

	short* shortPtr = (short*) saveScr;

	*shortPtr++ = x1;
	*shortPtr++ = y1;
	*shortPtr++ = x2;
	*shortPtr++ = y2;

	read_bitmapW( x1,y1,x2,y2, shortPtr );

   return saveScr;
}
//------------ End of function VgaBuf::save_area ---------//


//----------- Begin of function VgaBuf::rest_area --------//
//
// Restore previously saved screen
//
// char* saveScr     = the previously saved screen
// [int] releaseFlag = whether release the buffer of the saved screen or not
//                     (default : 1)
//
void VgaBuf::rest_area(short* saveScr, int releaseFlag)
{
   int  x1,y1,x2,y2;

   if( saveScr == NULL )
      return;

   short* shortPtr = (short*) saveScr;

	x1 = *shortPtr++;
   y1 = *shortPtr++;
   x2 = *shortPtr++;
   y2 = *shortPtr++;

   err_when( x1>x2 || y1>y2 || x1<0 || y1<0 || x2>=VGA_WIDTH || y2>=VGA_HEIGHT );

	// ##### begin Gilbert 30/10 ########//
	put_bitmapW( x1, y1, shortPtr );
	// ##### end Gilbert 30/10 ########//

	if( releaseFlag )
		mem_del( saveScr );
}
//------------ End of function VgaBuf::rest_area ----------//


//------------ Begin of function VgaBuf::write_bmp_file --------------//
//
// Load a BMP file into the current VgaBuf DIB object.
//
// <char*> fileName - the name of the BMP file.
//
// return : <int> 1-succeeded, 0-failed.
//
int VgaBuf::write_bmp_file(char* fileName)
{
  return WriteBitmapFile(this, fileName);
}
//------------ End of function VgaBuf::write_bmp_file --------------//


//---------- Begin of function VgaBuf::put_large_bitmap ---------//
//
// Put a picture on the screen, when a picture's size is > 64K
// Pass a file pointer to put_large_bitmap() for continously reading the file
//
// Syntax
//
// <int>   x, y    = the location of the picture on the screen
// <FILE*> filePtr = pointer to the
//
//--------- Format of Picture buffer -------//
//
// <int> = picture width
// <int> = picture height
//
// char[...]    = non-compressed flat picture bitmap
//
//---------------------------------------------//

void VgaBuf::put_large_bitmap(int x1, int y1, File* filePtr, short *colorRemapTable)
{
	if( filePtr == NULL )
		return;

	if( !colorRemapTable )
		colorRemapTable = default_remap_table;

	int pictWidth = filePtr->file_get_short();

	//------ read in bitmap and display it --------//

	int pictHeight = filePtr->file_get_short();
	int x2 = x1 + pictWidth  - 1;
	int y2 = y1 + pictHeight - 1;

	long pictSize = (long) pictWidth * pictHeight;

	err_when( x1>x2 || y1>y2 || x1<0 || y1<0 || x2>=VGA_WIDTH || y2>=VGA_HEIGHT );

	//---- if pict size less than 64K, read in the picture in one step ----//

	if( Bitmap::size(pictWidth, pictHeight) <= COMMON_DATA_BUF_SIZE )
	{
		((Bitmap *)sys.common_data_buf)->init(pictWidth, pictHeight);

		filePtr->file_read( ((Bitmap *)sys.common_data_buf)->bitmap, pictSize );

		put_bitmap( x1, y1, (char *)sys.common_data_buf, colorRemapTable );
	}
	else //----- if the picture size > 64K, read in line by line -----//
	{
		int bufferLine = (COMMON_DATA_BUF_SIZE - 2*sizeof(short) )/ pictWidth;   // max. no. of lines can be in the buffer
		int ty=y1+bufferLine-1;

		if( ty> y2 )
			ty=y2;

		while( y1<=y2 )
		{
			((Bitmap *)sys.common_data_buf)->init( pictWidth, (ty-y1+1) );
			filePtr->file_read( ((Bitmap *)sys.common_data_buf)->bitmap, (unsigned)pictWidth * (ty-y1+1) );

			put_bitmap( x1, y1, sys.common_data_buf, colorRemapTable );

			y1 += bufferLine;

			if( (ty+=bufferLine) > y2 )
				ty=y2;
		}
	}
}
//----------- End of function VgaBuf::put_large_bitmap --------//


//---------- Begin of function VgaBuf::put_large_bitmap_trans ---------//
//
// Put a picture on the screen, when a picture's size is > 64K
// Pass a file pointer to put_large_bitmap_trans() for continously reading the file
//
// Syntax
//
// <int>   x, y    = the location of the picture on the screen
// <FILE*> filePtr = pointer to the
//
//--------- Format of Picture buffer -------//
//
// <int> = picture width
// <int> = picture height
//
// char[...]    = non-compressed flat picture bitmap
//
//---------------------------------------------//

void VgaBuf::put_large_bitmap_trans(int x1, int y1, File* filePtr, short *colorRemapTable)
{
	if( filePtr == NULL )
		return;

	if( !colorRemapTable )
		colorRemapTable = default_remap_table;

	int pictWidth = filePtr->file_get_short();

	//------ read in bitmap and display it --------//

	int pictHeight = filePtr->file_get_short();
	int x2 = x1 + pictWidth  - 1;
	int y2 = y1 + pictHeight - 1;

	long pictSize = (long) pictWidth * pictHeight;

	err_when( x1>x2 || y1>y2 || x1<0 || y1<0 || x2>=VGA_WIDTH || y2>=VGA_HEIGHT );

	//---- if pict size less than 64K, read in the picture in one step ----//

	if( Bitmap::size(pictWidth, pictHeight) <= COMMON_DATA_BUF_SIZE )
	{
		((Bitmap *)sys.common_data_buf)->init(pictWidth, pictHeight);

		filePtr->file_read( ((Bitmap *)sys.common_data_buf)->bitmap, pictSize );

		put_bitmap_trans_remap( x1, y1, (char *)sys.common_data_buf, colorRemapTable );
	}
	else //----- if the picture size > 64K, read in line by line -----//
	{
		int bufferLine = (COMMON_DATA_BUF_SIZE - 2*sizeof(short) )/ pictWidth;   // max. no. of lines can be in the buffer
		int ty=y1+bufferLine-1;

		if( ty> y2 )
			ty=y2;

		while( y1<=y2 )
		{
			((Bitmap *)sys.common_data_buf)->init( pictWidth, (ty-y1+1) );
			filePtr->file_read( ((Bitmap *)sys.common_data_buf)->bitmap, (unsigned)pictWidth * (ty-y1+1) );

			put_bitmap_trans_remap( x1, y1, sys.common_data_buf, colorRemapTable );

			y1 += bufferLine;

			if( (ty+=bufferLine) > y2 )
				ty=y2;
		}
	}
}
//----------- End of function VgaBuf::put_large_bitmap_trans --------//



