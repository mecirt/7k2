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

#include <win32_compat.h>

#include <windowsx.h>
#include <ddraw.h>

#include <platform.h>
#include <all.h>
#include <osys.h>
#include <ovgalock.h>
#include <ovgabuf.h>
#include <resource.h>


#include <unistd.h>

HWND main_hwnd = NULL;

void ShowMessageBox (const char *text) {
  OutputDebugString(text);
  if( vga_front.vptr_dd_buf )
  {
    VgaFrontLock vgaLock;

    ShowMouseCursor(true);
    MessageBox(main_hwnd, text, WIN_TITLE, MB_OK | MB_ICONERROR);
    ShowMouseCursor(false);
  }
  else
  {
    ShowMouseCursor(true);
    MessageBox(main_hwnd, text, WIN_TITLE, MB_OK | MB_ICONERROR);
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
    delete [] bitmap->buffer;
    bitmap->buffer = NULL;
  }
  delete bitmap;

  return 1;

} // end Unload_Bitmap_File

///////////////////////////////////////////////////////////////////////////////

void read_bitmap_palette(bitmap_file *bitmap, int idx, int *red, int *green, int *blue)
{
  *red = bitmap->palette[idx].peRed;
  *green = bitmap->palette[idx].peGreen;
  *blue = bitmap->palette[idx].peBlue;
}


// ****** Window and DirectX initialisation ******

static long FAR PASCAL static_main_win_proc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
   switch( message )
   {
      case WM_CREATE:
         main_hwnd = hWnd;
         break;

      case WM_ACTIVATEAPP:
			sys.active_flag = (BOOL)wParam && !IsIconic(hWnd);

         //--------------------------------------------------------------//
         // while we were not-active something bad happened that caused us
         // to pause, like a surface restore failing or we got a palette
         // changed, now that we are active try to fix things
         //--------------------------------------------------------------//

         if( sys.active_flag )
         {
            sys.unpause();
            sys.need_redraw_flag = 1;      // for Sys::disp_frame to redraw the screen
         }
         else
            sys.pause();
         break;

			// ##### begin Gilbert 31/10 #####//
		case WM_PAINT:
			sys.need_redraw_flag = 1;
			break;
			// ##### end Gilbert 31/10 #####//

       case WM_DESTROY:
          main_hwnd = NULL;
			 sys.deinit_directx();
			 PostQuitMessage( 0 );
			 break;

		 default:
			 break;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}

bool CreateMainWindow()
{
   //--------- register window class --------//

   WNDCLASS    wc;
   BOOL        rc;

   wc.style          = CS_DBLCLKS;
   wc.lpfnWndProc    = static_main_win_proc;
   wc.cbClsExtra     = 0;
   wc.cbWndExtra     = 0;
   wc.hInstance      = (HINSTANCE__ *) sys.app_hinstance;
   wc.hIcon          = LoadIcon( (HINSTANCE__ *) sys.app_hinstance, MAKEINTATOM(IDI_ICON1));
   wc.hbrBackground  = (HBRUSH__ *) GetStockObject(BLACK_BRUSH);
   wc.hCursor        = LoadCursor( NULL, IDC_ARROW );
   wc.lpszMenuName   = NULL;
   wc.lpszClassName  = WIN_CLASS_NAME;

   rc = RegisterClass( &wc );

   if( !rc )
      return false;

   //--------- create window -----------//

   main_hwnd = CreateWindowEx(
       WS_EX_APPWINDOW | WS_EX_TOPMOST,
       WIN_CLASS_NAME,
       WIN_TITLE,
       WS_VISIBLE |    // so we dont have to call ShowWindow
       WS_POPUP,
       0,
       0,
       GetSystemMetrics(SM_CXSCREEN),
       GetSystemMetrics(SM_CYSCREEN),
       NULL,
       NULL,
       (HINSTANCE__ *) sys.app_hinstance,
       NULL );

   if( !main_hwnd )
      return false;

   UpdateWindow( main_hwnd );
   SetFocus( main_hwnd );

   // Convert it to a plain window
   DWORD   dwStyle;
   dwStyle = GetWindowStyle(main_hwnd);
   dwStyle |= WS_POPUP;
   dwStyle &= ~(WS_OVERLAPPED | WS_CAPTION | WS_THICKFRAME | WS_MINIMIZEBOX);
   SetWindowLong(main_hwnd, GWL_STYLE, dwStyle);


   return true;
}

void CloseMainWindow()
{
  if( main_hwnd )
  {
    PostMessage(main_hwnd, WM_CLOSE, 0, 0);
    while (ProcessNextEvent() == 1) {};
  }
}

void FocusMainWindow()
{
  if (main_hwnd)
    SetFocus(main_hwnd);
}

void ShowMainWindow()
{
  if (main_hwnd)
    ShowWindow (main_hwnd, SW_RESTORE);
}

void InvalidateMainWindow()
{
  if (main_hwnd)
    InvalidateRect(main_hwnd, NULL, true);
}

void *get_main_hwnd() {
  return (void *) main_hwnd;
}

// ****** End of Window and DirectX initialisation ******


void BltFast (VgaBuf *targetBuffer, VgaBuf *sourceBuffer, int x1, int y1, int x2, int y2, int mode)
{
  RECT bltRect;

  bltRect.left   = x1;
  bltRect.top    = y1;
  bltRect.right  = x2+1;
  bltRect.bottom = y2+1;

  targetBuffer->dd_buf->BltFast(
      x1, y1,
      sourceBuffer->dd_buf,        // src surface
      &bltRect,               // src rect (all of it)
      (mode == 1) ? DDBLTFAST_WAIT : DDBLTFAST_NOCOLORKEY);

}



