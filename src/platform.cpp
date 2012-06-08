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

// Platform-specific functionality for 7k2. All the directx/windows requiring code goes here (as wrappers usually).

#define NEED_WINDOWS

#include <platform.h>
#include <all.h>
#include <win32_compat.h>
#include <osys.h>
#include <ovgalock.h>
#include <ovgabuf.h>

#include <unistd.h>

void ShowMessageBox (const char *text) {
  OutputDebugString(text);
  if( vga_front.vptr_dd_buf )
  {
    VgaFrontLock vgaLock;

    ShowMouseCursor(true);
    MessageBox(sys.main_hwnd, text, WIN_TITLE, MB_OK | MB_ICONERROR);
    ShowMouseCursor(false);
  }
  else
  {
    ShowMouseCursor(true);
    MessageBox(sys.main_hwnd, text, WIN_TITLE, MB_OK | MB_ICONERROR);
    ShowMouseCursor(false);
  }
}

void ShowMouseCursor (bool show)
{
  ShowCursor (show);
}

// Get next Windows event. Return 0 if the process is to end, 1 if a message was processed, 2 if there was no message
int ProcessNextEvent()
{
    MSG msg;
    if (PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE))
    {
      if (!GetMessage( &msg, NULL, 0, 0)) return 0;
      TranslateMessage(&msg);
      DispatchMessage(&msg);
      return 1;
    }
    return 2;
}

void WaitNextEvent()
{
  WaitMessage();
}

// the bitmap file structure /////////////////////////////////////////////////
struct bitmap_file
        {
        BITMAPFILEHEADER bitmapfileheader;  // this contains the bitmapfile header
        BITMAPINFOHEADER bitmapinfoheader;  // this is all the info is
        PALETTEENTRY     palette[256];      // we will store the palette here
        UCHAR            *buffer;           // this is a pointer to the data
        };


bitmap_file *load_bitmap_file(const char *filename)
{
// this function opens a bitmap file and loads the data into bitmap
// this function will only work with non-compressed 8 bit palettized images
// it uses file handles instead of streams just for a change, no reason

#define BITMAP_ID        0x4D42       // this is the universal id for a bitmap
int      file_handle,                 // the file handle
         index;                       // looping index
OFSTRUCT file_data;                   // the file data information

String	str(DIR_IMAGE);
			str += filename;
//			str += ".COL";

// open the file if it exists
	if ((file_handle = OpenFile(str, &file_data,OF_READ))==-1)
		return(0);
  bitmap_file *bitmap = new bitmap_file;

// now load the bitmap file header
	_lread(file_handle, &bitmap->bitmapfileheader,sizeof(BITMAPFILEHEADER));

// test if this is a bitmap file
	if (bitmap->bitmapfileheader.bfType!=BITMAP_ID)
   {
		// close the file
	   _lclose(file_handle);
           delete bitmap;
 
	   // return error
		return(0);
   } // end if

// we know this is a bitmap, so read in all the sections

// load the bitmap file header
	_lread(file_handle, &bitmap->bitmapinfoheader,sizeof(BITMAPINFOHEADER));

// now the palette
	_lread(file_handle, &bitmap->palette,256*sizeof(PALETTEENTRY));

// now set all the flags in the palette correctly and fix the reverse BGR
	for (index=0; index<256; index++)
    {
		 int temp_color = bitmap->palette[index].peRed;
	    bitmap->palette[index].peRed  = bitmap->palette[index].peBlue;
		 bitmap->palette[index].peBlue = temp_color;
		bitmap->palette[index].peFlags = PC_NOCOLLAPSE;
    } // end for index

// finally the image data itself
	lseek(file_handle,-(int)(bitmap->bitmapinfoheader.biSizeImage),SEEK_END);

// allocate the memory for the image
	if (!(bitmap->buffer = new UCHAR [bitmap->bitmapinfoheader.biSizeImage]))
   {
   // close the file
		_lclose(file_handle);
                delete bitmap;
   // return error
		return(0);

   } // end if

// now read it in
	_lread(file_handle,bitmap->buffer,bitmap->bitmapinfoheader.biSizeImage);

// bitmaps are usually upside down, so flip the image
int biWidth  = bitmap->bitmapinfoheader.biWidth,
    biHeight = bitmap->bitmapinfoheader.biHeight;

// allocate the temporary buffer
UCHAR *flip_buffer = new UCHAR[biWidth*biHeight];

// copy image to work area
memcpy(flip_buffer,bitmap->buffer,biWidth*biHeight);

// flip vertically
for (index=0; index<biHeight; index++)
    memcpy(&bitmap->buffer[((biHeight-1) - index)*biWidth],&flip_buffer[index * biWidth], biWidth);

// release the working memory
delete [] flip_buffer;

// close the file
_lclose(file_handle);

// return success
return bitmap;

} // end Load_Bitmap_File

///////////////////////////////////////////////////////////////////////////////

int unload_bitmap_file(bitmap_file *bitmap)
{
// this function releases all memory associated with "bitmap"
if (bitmap->buffer)
   {
   // release memory
   delete [] bitmap->buffer;

   // reset pointer
   bitmap->buffer = NULL;

   } // end if

  delete bitmap;

// return success
return(1);

} // end Unload_Bitmap_File

///////////////////////////////////////////////////////////////////////////////

void read_bitmap_palette(bitmap_file *bitmap, int idx, int *red, int *green, int *blue)
{
  *red = bitmap->palette[idx].peRed;
  *green = bitmap->palette[idx].peGreen;
  *blue = bitmap->palette[idx].peBlue;
}


