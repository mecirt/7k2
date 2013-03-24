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

void VgaBuf::put_bitmap(int x, int y, char *bitmapBuf, short *colorRemapTable, int transparency, bool hmirror, short *custom_buffer, int custom_pitch)
{
  put_bitmap_area(x, y, bitmapBuf, -1, -1, -1, -1, colorRemapTable, transparency, hmirror, custom_buffer, custom_pitch);
}

#define TRANSCODE 0xff
#define MULTITRANS_CODE 0xf8
#define EFFECT_CODE 0xef
#define BUFFER_INDEX(x,y) ((short *) ((char *)buffer + pitch * (y)) + (x))

short doIMGeffect(unsigned char id, short color)
{
  RGBColor c;

  vga.decode_pixel(id, &c);

  c.red >>= 1;
  c.green >>= 1;
  c.blue >>= 1;
  switch (id) {
    case 0:    // shadow filter, nothing more to do here
      break;
    case 1:    // blue filter
      c.blue |= 0x80;
      break;
    case 2:    // green filter
      c.green |= 0x80;
      break;
    case 3:    // cyan filter
      c.green |= 0x80;
      c.blue |= 0x80;
      break;
    case 4:    // red filter
      c.red |= 0x80;
      break;
    case 5:    // purple filter
      c.red |= 0x80;
      c.blue |= 0x80;
      break;
    case 6:    // yellow filter
      c.red |= 0x80;
      c.green |= 0x80;
      break;
    case 7:    // grey scale filter
      c.red |= 0x80;
      c.blue |= 0x80;
      c.green |= 0x80;
      break;
  }

  return vga.make_pixel(&c);
}

// transparency: 0=no, 1=simple, 2=RLE, 3=RLE with half alpha, 4=blend, no transparency, 5=weak blend, no transparency
void VgaBuf::put_bitmap_area(int x, int y, char *bitmapBuf, int srcX1, int srcY1, int srcX2, int srcY2, short *colorRemapTable, int transparency, bool hmirror, short *custom_buffer, int custom_pitch)
{
  short *buffer = custom_buffer ? custom_buffer : cur_buf_ptr;
  int pitch = custom_buffer ? custom_pitch : cur_pitch;

  // TODO: incorporate codes 4 and 5 (blending)
  // is it even used at all? can't find any usage ...
/*
  if (transparency == 4) {
    puts("transparency 4");
    if (hmirror)
      IMGbltBlendAreaRemap(buffer, pitch, x, y, bitmapBuf, srcX1, srcY1, srcX2, srcY2, default_blend_table);
    else
      IMGbltBlendAreaRemapHMirror(buffer, pitch, x, y, bitmapBuf, srcX1, srcY1, srcX2, srcY2, default_blend_table );
    return;
  }
  if (transparency == 5) {
    puts("transparency 5");
    if (hmirror)
      IMGbltWeakblendAreaRemap(buffer, pitch, x, y, bitmapBuf, srcX1, srcY1, srcX2, srcY2, default_blend_table);
    else
      IMGbltWeakblendAreaRemapHMirror(buffer, pitch, x, y, bitmapBuf, srcX1, srcY1, srcX2, srcY2, default_blend_table );
    return;
  }
*/

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
      if ((transparency >= 2) && (pixel >= MULTITRANS_CODE)) {
        if (pixel == MULTITRANS_CODE)
          skip = (unsigned char) *bitmapBuf++;
        else
          skip = TRANSCODE - pixel + 1;
        skip--;  // this one is already skipped
        continue;
      }
      if (crop && ((yy < srcY1) || (yy > srcY2) || (xx < srcX1) || (xx > srcX2))) continue;

      short *bufptr = BUFFER_INDEX(x + (hmirror?width-xx:xx), y + yy);

      if ((transparency >= 2) && (pixel >= EFFECT_CODE)) {
        *bufptr = doIMGeffect(pixel - EFFECT_CODE, *bufptr);
        continue;
      }

      if (transparency >= 3)  // alpha-adjust
      {
        RGBColor c, c2;
        vga.decode_pixel(colorRemapTable[pixel], &c);
        vga.decode_pixel(*bufptr, &c2);
        *bufptr = vga.make_pixel(c.red / 2 + c2.red / 2, c.green / 2 + c2.green / 2, c.blue / 2 + c2.blue / 2);
      }
      else
        *bufptr = colorRemapTable[pixel];
    }
  }
}


void VgaBuf::put_bitmapW(int x, int y, short *bitmapBuf, bool transparency, short *custom_buffer, int custom_pitch)
{
  put_bitmapW_area(x, y, bitmapBuf, -1, -1, -1, -1, transparency);

}

void VgaBuf::put_bitmapW_area(int x, int y, short *bitmapBuf, int srcX1, int srcY1, int srcX2, int srcY2, bool transparency, short *custom_buffer, int custom_pitch)
{
  bool crop = true;
  if ((srcX1 < 0) && (srcY1 < 0) && (srcX2 < 0) && (srcY2 < 0)) crop = false;

  short *buffer = custom_buffer ? custom_buffer : cur_buf_ptr;
  int pitch = custom_buffer ? custom_pitch : cur_pitch;

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

      short *bufptr = BUFFER_INDEX(x + xx, y + yy);
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

// transparency: 0=transparent, 5+ = solid
void VgaBuf::put_bar(int X1, int Y1, int X2, int Y2, short color, char transparency, short *custom_buffer, int custom_pitch)
{
  if (transparency <= 0) return;

  short *buffer = custom_buffer ? custom_buffer : cur_buf_ptr;
  int pitch = custom_buffer ? custom_pitch : cur_pitch;

  RGBColor c;
  int alpha;
  if (transparency < 5) {
    vga.decode_pixel(color, &c);
    alpha = 128 + 128 * transparency / 5;  // reduce transparency to make things readable
    c.red = c.red * alpha / 256;
    c.green = c.green * alpha / 256;
    c.blue = c.blue * alpha / 256;
  }

  for (int yy = Y1; yy <= Y2; ++yy) {
    for (int xx = X1; xx <= X2; ++xx) {
      short *bufptr = BUFFER_INDEX(xx, yy);
      if (transparency >= 5)
        *bufptr = color;
      else
      {
        RGBColor c2;
        vga.decode_pixel(*bufptr, &c2);
        c2.red = c2.red * (256 - alpha) / 256 + c.red;
        c2.green = c2.green * (256 - alpha) / 256 + c.green;
        c2.blue = c2.blue * (256 - alpha) / 256 + c.blue;
        *bufptr = vga.make_pixel(&c2);
      }
    }
  }
}

//----------- Begin of function VgaBuf::adjust_brightness ------------//
//
// clear the center of the either up or down panel
//
// <int> x1,y1,x2,y2  = the four vertex of the panel
// <int> adjustDegree = the degree of brightness to adjust
//
void VgaBuf::adjust_brightness(int x1,int y1,int x2,int y2,int adjustDegree)
{
#if( MAX_BRIGHTNESS_ADJUST_DEGREE > 10 )
  adjustDegree *= MAX_BRIGHTNESS_ADJUST_DEGREE / 10;
#endif

  err_when( adjustDegree < -MAX_BRIGHTNESS_ADJUST_DEGREE ||
      adjustDegree >  MAX_BRIGHTNESS_ADJUST_DEGREE );

  // these are for the BUFFER_INDEX macro
  short *buffer = cur_buf_ptr;
  int pitch = cur_pitch;

   for (int yy = y1; yy <= y2; ++yy) {
    for (int xx = x1; xx <= x2; ++xx) {
      short *bufptr = BUFFER_INDEX(xx, yy);
      RGBColor c;
      vga.decode_pixel(*bufptr, &c);
      c = ColorTable::bright_func(c, adjustDegree, MAX_BRIGHTNESS_ADJUST_DEGREE);
      *bufptr = vga.make_pixel(&c);
    }
  }
}
//------------- End of function VgaBuf::adjust_brightness ------------//

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



