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

#define INITGUID

// define DIRECTINPUT_VERSION before include dinput.h for directx 3 emulation
#define DIRECTINPUT_VERSION 0x0300
#include <dinput.h>
// these three are here because we can't define INITGUID in more files
#include <dplay.h>
#include <dplobby.h>


#include <platform.h>
#include <all.h>
#include <osys.h>
#include <ovga.h>
#include <ovgalock.h>
#include <ovgabuf.h>
#include <resource.h>
#include <ocoltbl.h>
#include <olog.h>
#include <omodeid.h>
#include <omouse.h>
#include <omousecr.h>
#include <opower.h>

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

int WriteBitmapFile (const VgaBuf *buf, const char *fileName)
{

	 File				bmpFile;
	 BITMAPINFO*	bmpInfoPtr = NULL;
	 char*			bitmapPtr = NULL;

	 int				hasPaletteFlag = 0;

	 bmpFile.file_create(fileName, 1, 0);		// 1-handle error, 0-disable variable file size

	 //------------ Write the file header ------------//

	 BITMAPFILEHEADER bmpFileHdr;

	 bmpFileHdr.bfType 		= 0x4D42;			// set the type to "BM"
	 bmpFileHdr.bfSize 		= buf->buf_size();
	 bmpFileHdr.bfReserved1 = 0;
	 bmpFileHdr.bfReserved2 = 0;
	 bmpFileHdr.bfOffBits   = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	 if( hasPaletteFlag )
		bmpFileHdr.bfOffBits += sizeof(RGBQUAD)*256;

	 bmpFile.file_write(&bmpFileHdr, sizeof(bmpFileHdr));

	 //------------ Write in the info header -----------//

	 BITMAPINFOHEADER bmpInfoHdr;

	 bmpInfoHdr.biSize			 = sizeof(BITMAPINFOHEADER);
	 bmpInfoHdr.biWidth			 = buf->buf_des.dwWidth;
	 bmpInfoHdr.biHeight			 = buf->buf_des.dwHeight;
	 bmpInfoHdr.biPlanes			 = 1; 
	 bmpInfoHdr.biBitCount		 = 24;
    bmpInfoHdr.biCompression	 = BI_RGB; 
	 bmpInfoHdr.biSizeImage	    = bmpInfoHdr.biWidth * bmpInfoHdr.biHeight * bmpInfoHdr.biBitCount / 8;
	 bmpInfoHdr.biXPelsPerMeter = 0;
    bmpInfoHdr.biYPelsPerMeter = 0; 
	 bmpInfoHdr.biClrUsed		 = 0; 
    bmpInfoHdr.biClrImportant  = 0; 

	 bmpFile.file_write(&bmpInfoHdr, sizeof(bmpInfoHdr));

	 //------------ write the color table -----------//

	 if( hasPaletteFlag )
	 {
		 LPDIRECTDRAWPALETTE ddPalettePtr;				// get the direct draw surface's palette
		 buf->dd_buf->GetPalette(&ddPalettePtr);

		 PALETTEENTRY *palEntries = (PALETTEENTRY*) mem_add( sizeof(PALETTEENTRY)*256 );
		 ddPalettePtr->GetEntries(0, 0, 256, palEntries);

		 RGBQUAD *colorTable = (RGBQUAD*) mem_add( sizeof(RGBQUAD)*256 );		// allocate a color table with 256 entries 
			
		 for( int i=0 ; i<256 ; i++ )
		 {
			 colorTable[i].rgbBlue  = palEntries[i].peBlue;
			 colorTable[i].rgbGreen = palEntries[i].peGreen;
			 colorTable[i].rgbRed   = palEntries[i].peRed; 
			 colorTable[i].rgbReserved = 0;
		 }
			 
		 bmpFile.file_write(colorTable, sizeof(RGBQUAD)*256);

		 mem_del(palEntries);
		 mem_del(colorTable);
	 }

	 //----------- write the bitmap ----------//

	 if( bmpInfoHdr.biBitCount == 8 )
	 {
		 for( int y=buf->buf_height()-1 ; y>=0 ; y-- )					// write in reversed order as DIB's vertical order is reversed
			 bmpFile.file_write(buf->buf_ptr(0,y), buf->buf_width());
	 }
	 else if( bmpInfoHdr.biBitCount == 24 )
	 {
		 int lineBufferSize = sizeof(RGBColor) * bmpInfoHdr.biWidth;
		 RGBColor *lineBuffer = (RGBColor *)mem_add( lineBufferSize );
		 for( int y = buf->buf_height()-1; y>=0 ; --y )
		 {
			 register short *pixelPtr = buf->buf_ptr( 0, y );
			 register RGBColor *lineBufPtr = lineBuffer;
			 for( int x = buf->buf_width()-1; x >= 0; --x, ++pixelPtr, ++lineBufPtr)
			 {
				 vga.decode_pixel( *pixelPtr, lineBufPtr );
				 // exchange Red and blue
				 BYTE r = lineBufPtr->red;
				 lineBufPtr->red = lineBufPtr->blue;
				 lineBufPtr->blue = r;
			 }
			 bmpFile.file_write(lineBuffer, lineBufferSize );
		 }
		 mem_del(lineBuffer);
	 }
	 else
	 {
		 err_here();
	 }

	 //------------ close the file -----------//

	 bmpFile.file_close();

	 return 1;
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

const char *dd_err_str( const char *s, HRESULT rc)
{
  static char retStr[200];
  sprintf( retStr, "%s (%x)", s, rc );
  return retStr;
}

LPDIRECTDRAW4 dd_obj = NULL;

bool InitGraphics ()
{
  if (dd_obj) return true;

  err_when( sizeof(vga.dd_pal) > sizeof(vga.vptr_dd_pal) );
  err_when( sizeof(vga.pal_entry_buf) > sizeof(vga.dw_pal_entry_buf) );

  //--------- Create direct draw object --------//

  DEBUG_LOG("Attempt DirectDrawCreate");
  LPDIRECTDRAW dd1Obj;
  int rc = DirectDrawCreate( NULL, &dd1Obj, NULL );
  DEBUG_LOG("DirectDrawCreate finish");

  if( rc != DD_OK )
  {
#ifdef DEBUG
    debug_msg("DirectDrawCreate failed err=%d", rc);
#endif
    err.run( dd_err_str("DirectDrawCreate failed!", rc) );
    return FALSE;
  }

  //-------- Query DirectDraw4 interface --------//

  DEBUG_LOG("Attempt Query DirectDraw4");
  rc = dd1Obj->QueryInterface(IID_IDirectDraw4, (void **)&dd_obj);
  DEBUG_LOG("Query DirectDraw2 finish");
  if( rc != DD_OK )
  {
#ifdef DEBUG
    debug_msg("Query DirectDraw4 failed err=%d", rc);
#endif
    err.run( dd_err_str("Query DirectDraw4(DirectX6) failed", rc) );
    dd1Obj->Release();
    return false;
  }

  dd1Obj->Release();

  //-----------------------------------------------------------//
  // grab exclusive mode if we are going to run as fullscreen
  // otherwise grab normal mode.
  //-----------------------------------------------------------//

  DEBUG_LOG("Attempt DirectDraw SetCooperativeLevel");
  rc = dd_obj->SetCooperativeLevel(main_hwnd,
      DDSCL_EXCLUSIVE |
      DDSCL_FULLSCREEN );
  DEBUG_LOG("DirectDraw SetCooperativeLevel finish");

  if( rc != DD_OK )
  {
#ifdef DEBUG
    debug_msg("SetCooperativeLevel failed err=%d", rc);
#endif
    err.run( dd_err_str("SetCooperativeLevel failed", rc) );
    return false;
  }

  return true;
}

bool SetDisplayMode (int w, int h)
{
  HRESULT rc;

  //-------------- set Direct Draw mode ---------------//

  DEBUG_LOG("Attempt DirectDraw SetDisplayMode");
  // IDirectDraw2::SetDisplayMode requires 5 parameters
  rc = dd_obj->SetDisplayMode( w, h, VGA_BPP, 0, 0);
  DEBUG_LOG("DirectDraw SetDisplayMode finish");

  if( rc != DD_OK )
  {
#ifdef DEBUG
    debug_msg("SetMode failed err=%d", rc);
#endif
    // ######## begin Gilbert 2/6 ########//
    // err.run( dd_err_str("SetMode failed ", rc) );
    err.msg( dd_err_str("SetMode failed ", rc) );
    // ######## end Gilbert 2/6 ########//
    return FALSE;
  }

  //----------- get the pixel format flag -----------//

  DDSURFACEDESC2 ddsd;
  memset(&ddsd, 0, sizeof(ddsd) );
  ddsd.dwSize = sizeof(ddsd);

  vga.pixel_format_flag = -1;

  if( dd_obj->GetDisplayMode(&ddsd) == DD_OK && ddsd.dwFlags & DDSD_PIXELFORMAT )
  {
    if( ddsd.ddpfPixelFormat.dwFlags & DDPF_RGB 
        && ddsd.ddpfPixelFormat.dwRGBBitCount == (DWORD)VGA_BPP )
    {
      if( ddsd.ddpfPixelFormat.dwRBitMask == 0x001fL
          && ddsd.ddpfPixelFormat.dwGBitMask == 0x001fL << 5
          && ddsd.ddpfPixelFormat.dwBBitMask == 0x001fL << 10 )
      {
        vga.pixel_format_flag = PIXFORM_RGB_555;
      }
      else if( ddsd.ddpfPixelFormat.dwRBitMask == 0x001fL
          && ddsd.ddpfPixelFormat.dwGBitMask == 0x003fL << 5
          && ddsd.ddpfPixelFormat.dwBBitMask == 0x001fL << 11 )
      {
        vga.pixel_format_flag = PIXFORM_RGB_565;
      }
      else if( ddsd.ddpfPixelFormat.dwBBitMask == 0x001fL
          && ddsd.ddpfPixelFormat.dwGBitMask == 0x001fL << 5
          && ddsd.ddpfPixelFormat.dwRBitMask == 0x001fL << 10 )
      {
        vga.pixel_format_flag = PIXFORM_BGR_555;
      }
      else if( ddsd.ddpfPixelFormat.dwBBitMask == 0x001fL
          && ddsd.ddpfPixelFormat.dwGBitMask == 0x003fL << 5
          && ddsd.ddpfPixelFormat.dwRBitMask == 0x001fL << 11 )
      {
        vga.pixel_format_flag = PIXFORM_BGR_565;
      }
    }
  }

  // allow forcing display mode

  if( m.is_file_exist( "PIXMODE.SYS" ) )
  {
    File pixModeFile;
    pixModeFile.file_open( "PIXMODE.SYS" );

    char readBuffer[8];
    memset( readBuffer, 0, sizeof(readBuffer) );
    long readLen = 6;

    pixModeFile.file_read( readBuffer, readLen );

    if( strncmp( readBuffer, "RGB555", readLen ) == 0 )
    {
      vga.pixel_format_flag = PIXFORM_RGB_555;
    }
    else if( strncmp( readBuffer, "RGB565", readLen ) == 0 )
    {
      vga.pixel_format_flag = PIXFORM_RGB_565;
    }
    else if( strncmp( readBuffer, "BGR555", readLen ) == 0 )
    {
      vga.pixel_format_flag = PIXFORM_BGR_555;
    }
    else if( strncmp( readBuffer, "BGR565", readLen ) == 0 )
    {
      vga.pixel_format_flag = PIXFORM_BGR_565;
    }

    pixModeFile.file_close();
  }

  if( vga.pixel_format_flag == -1 )
  {
    ShowMessageBox("Cannot determine the pixel format of this display mode.");

    vga.pixel_format_flag = PIXFORM_BGR_565;
  }

  //----------- display the system cursor -------------//
  SetCursor(NULL);

  return TRUE;
}

void DeinitGraphics ()
{
  if (!dd_obj) return;
  dd_obj->Release();
  dd_obj = NULL;
}

// ****** End of Window and DirectX initialisation ******


// Surfaces

bool InitSurface (VgaBuf *buf, bool back, DWORD w, DWORD h, int videoMemoryFlag)
{
  LPDIRECTDRAW4 ddPtr = (LPDIRECTDRAW4) dd_obj;

  DDSURFACEDESC2       ddsd;
  HRESULT             rc;

  // check size of union structure
  err_when( sizeof(buf->dd_buf) > sizeof(buf->vptr_dd_buf) );
  err_when( sizeof(buf->buf_des) > sizeof(buf->c_buf_des) );

  memset( &ddsd, 0, sizeof(ddsd) );
  ddsd.dwSize = sizeof( ddsd );

  if (back) {
    ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT |DDSD_WIDTH;

    // create back buffer
    ddsd.ddsCaps.dwCaps = (videoMemoryFlag & 1) ? 0 : DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;

    ddsd.dwWidth  = w ? w : VGA_WIDTH;
    ddsd.dwHeight = h ? h : VGA_HEIGHT;

  } else {
    ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
  }
	
  rc = ddPtr->CreateSurface (&ddsd, &buf->dd_buf, NULL);
  if( rc != DD_OK )
  {
    err.run( dd_err_str("Error creating Direct Draw surface!" ,rc) );
    return false;
  }

  return true;
}

void DeinitSurface (VgaBuf *buf)
{
  if (buf->dd_buf)
    buf->dd_buf->Release();
  buf->dd_buf = NULL;
}

bool AttachSurface (VgaBuf *to, VgaBuf *surface)
{
  HRESULT rc;
  if (to->dd_buf && surface->dd_buf)
  {
    rc = to->dd_buf->AddAttachedSurface(surface->dd_buf);
    if( rc != DD_OK ) {
      err.run( dd_err_str("Cannot attach flipping surface", rc) );
      return false;
    }
    return true;
  }
  return false;
}

bool DetachSurface (VgaBuf *to, VgaBuf *surface)
{
  HRESULT rc;
  if (to->dd_buf && surface->dd_buf)
  {
    rc = to->dd_buf->DeleteAttachedSurface(0, surface->dd_buf);
    if( rc != DD_OK ) {
      err.run( dd_err_str("Cannot detach surface", rc) );
      return false;
    }
    return true;
  }
  return false;
}

bool LockBuffer (VgaBuf *buf)
{
  memset(&buf->buf_des, 0, sizeof(buf->buf_des));
  buf->buf_des.dwSize = sizeof(buf->buf_des);

  int rc = buf->dd_buf->Lock(NULL, &buf->buf_des, DDLOCK_WAIT | DDLOCK_NOSYSLOCK , NULL);

  RestoreBufferPointers (buf);

  if (rc != DD_OK)
    err_now( dd_err_str("Locking buffer failed.", rc) );
  return (rc == DD_OK);
}

bool UnlockBuffer (VgaBuf *buf)
{
  int rc = buf->dd_buf->Unlock(NULL);
  if (rc != DD_OK)
    err_now( dd_err_str("Unlocking buffer failed.", rc) );
  return (rc == DD_OK);
}

void FlipBuffer (VgaBuf *buf)
{
  buf->dd_buf->Flip(NULL, DDFLIP_WAIT );
}

bool IsBufferLost (VgaBuf *buf)
{
  return buf->dd_buf && buf->dd_buf->IsLost() == DDERR_SURFACELOST;
}

bool RestoreBuffer (VgaBuf *buf)
{
  if (buf->dd_buf && buf->dd_buf->Restore() == DD_OK) return true;
  return false;
}

void RestoreBufferPointers (VgaBuf *buf)
{
  buf->cur_buf_ptr = (short *) buf->buf_des.lpSurface;
  buf->cur_pitch = buf->buf_des.lPitch;
}

int BufferSize (const VgaBuf *buf)
{
  return buf->buf_des.dwWidth * buf->buf_des.dwHeight * sizeof(short);
}

int BufferWidth (const VgaBuf *buf)
{
  return buf->buf_des.dwWidth;
}

int BufferHeight (const VgaBuf *buf)
{
  return buf->buf_des.dwHeight;
}


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


// mouse

LPDIRECTINPUT direct_input;
LPDIRECTINPUTDEVICE di_mouse_device, di_keyb_device;

#define MOUSE_BUFFER_SIZE 200
#define KEYB_BUFFER_SIZE 16

static DIDEVICEOBJECTDATA mouse_data[MOUSE_BUFFER_SIZE];
static DIDEVICEOBJECTDATA keyb_data[KEYB_BUFFER_SIZE];


void InitInputDevices (void *hinstance)
{
  HINSTANCE hinst = (HINSTANCE) hinstance;
  HRESULT hr;
  hr = DirectInputCreate(hinst, DIRECTINPUT_VERSION, &direct_input, NULL);
  if(hr)
    err.run( "Failed creating DirectInput");

  {
    VgaFrontLock vgaLock;
    hr = direct_input->CreateDevice(GUID_SysMouse,&di_mouse_device,NULL);
  }

  if(hr)
    err.run( "Failed creating mouse interface from DirectInput");

  // ------- set cooperative level --------//
  hr = di_mouse_device->SetCooperativeLevel((HWND)get_main_hwnd(), DISCL_FOREGROUND | DISCL_EXCLUSIVE);

  // ------- set data format ---------//
  if(!hr)
    hr = di_mouse_device->SetDataFormat(&c_dfDIMouse);

  // ------- set relative coordinate mode --------//
  // DirectInput default is relative
  if(!hr)
  {
    DIPROPDWORD propDword;
    propDword.diph.dwSize = sizeof(DIPROPDWORD);
    propDword.diph.dwHeaderSize = sizeof(DIPROPHEADER);
    propDword.diph.dwHow = DIPH_DEVICE;			// Entire device
    propDword.diph.dwObj =  0;						// Entire device, so zero
    propDword.dwData = DIPROPAXISMODE_REL;
    hr = di_mouse_device->SetProperty(DIPROP_AXISMODE, &propDword.diph);
  }

  // ------- set buffer size --------//
  if(!hr)
  {
    DIPROPDWORD propDword;
    propDword.diph.dwSize = sizeof(DIPROPDWORD);
    propDword.diph.dwHeaderSize = sizeof(DIPROPHEADER);
    propDword.diph.dwHow = DIPH_DEVICE;			// Entire device
    propDword.diph.dwObj =  0;						// Entire device, so zero
    propDword.dwData = MOUSE_BUFFER_SIZE; // * sizeof(DIDEVICEOBJECTDATA);
    hr = di_mouse_device->SetProperty(DIPROP_BUFFERSIZE, &propDword.diph);
  }

  if(hr)
    err.run( "Failed initializing mouse interface");


  // ------- create direct input keyboard interface --------- //
  {
    VgaFrontLock vgaLock;
    hr = direct_input->CreateDevice(GUID_SysKeyboard,&di_keyb_device,NULL);
  }

  if(hr)
    err.run( "Failed creating keyboard interface from DirectInput");

  // ------- set cooperative level --------//
  hr = di_keyb_device->SetCooperativeLevel((HWND)get_main_hwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);

  // ------- set data format ---------//
  if(!hr)
    hr = di_keyb_device->SetDataFormat(&c_dfDIKeyboard);

  // ------- set buffer size --------//
  if(!hr)
  {
    DIPROPDWORD propDword;
    propDword.diph.dwSize = sizeof(DIPROPDWORD);
    propDword.diph.dwHeaderSize = sizeof(DIPROPHEADER);
    propDword.diph.dwHow = DIPH_DEVICE;			// Entire device
    propDword.diph.dwObj =  0;						// Entire device, so zero
    propDword.dwData = KEYB_BUFFER_SIZE; // * sizeof(DIDEVICEOBJECTDATA);
    hr = di_keyb_device->SetProperty(DIPROP_BUFFERSIZE, &propDword.diph);
  }

  if(hr)
    err.run( "Failed initializing keyboard interface");
}

void UninitInputDevices ()
{
  if( di_keyb_device )
  {
    di_keyb_device->Unacquire();
    di_keyb_device->Release();
    di_keyb_device = NULL;
  }
  if( di_mouse_device )
  {
    di_mouse_device->Unacquire();
    di_mouse_device->Release();
    di_mouse_device = NULL;
  }
  if( direct_input )
  {
    direct_input->Release();
    direct_input = NULL;
  }
}

int PollInputDevices ()
{
	// ---- acquire mouse device and count the message queued ----//
	HRESULT hr;
	bool mouseAcquired = true;
	bool keybAcquired = true;

	if( (hr = di_mouse_device->Acquire()) != DI_OK && hr != S_FALSE)
	{
		mouseAcquired = false;
	}

	if( (hr = di_keyb_device->Acquire()) != DI_OK && hr != S_FALSE)
	{
		keybAcquired = false;
	}

	DWORD mouseLen, keybLen;

	int moveFlag = 0;
	int rc = 0;

	int maxGetDataTry = 2;
	while( --maxGetDataTry >= 0 )
	{
		mouseLen = MOUSE_BUFFER_SIZE;
		keybLen = KEYB_BUFFER_SIZE;

		if( !mouseAcquired
			||	(hr = di_mouse_device->GetDeviceData( sizeof(DIDEVICEOBJECTDATA),
				mouse_data, &mouseLen, 0)) != DI_OK && hr != S_FALSE )
			mouseLen = 0;
		if( !keybAcquired
			|| (hr = di_keyb_device->GetDeviceData( sizeof(DIDEVICEOBJECTDATA),
				keyb_data, &keybLen, 0)) != DI_OK && hr != S_FALSE)
			keybLen = 0;

		if( !mouseLen && !keybLen )
			break;

		DIDEVICEOBJECTDATA *mouseMsg = mouse_data, *keybMsg = keyb_data;

		while( mouseLen > 0 || keybLen > 0)
		{
			// merge mouse event and keyboard event
			MouseEvent ev;
			int earlyDevice = 0;			// 1 = mouse, 2 = keyboard
			if( mouseLen > 0 && keybLen > 0 )
			{
				if( DISEQUENCE_COMPARE(mouseMsg->dwSequence, <=, keybMsg->dwSequence) )
					earlyDevice = 1;
				else
					earlyDevice = 2;
			}
			else if( mouseLen > 0)
			{
				earlyDevice = 1;
			}
			else if( keybLen > 0)
			{
				earlyDevice = 2;
			}

			if( earlyDevice == 1 )
			{
				if (mouseMsg->dwOfs == DIMOFS_BUTTON0)
				{
					if( mouseMsg->dwData & 0x80)
					{
						// mouse button pressed
						mouse.left_press = LEFT_BUTTON_MASK;
						ev.event_type = LEFT_BUTTON;
						ev.x = mouse.cur_x;
						ev.y = mouse.cur_y;
						ev.time = mouseMsg->dwTimeStamp;
						ev.scan_code = 0;
						ev.skey_state = mouse.skey_state;
						mouse.add_event(&ev);
						rc = 1;
					}
					else
					{
						// mouse button released
						mouse.left_press = 0;
						ev.event_type = LEFT_BUTTON_RELEASE;
						ev.x = mouse.cur_x;
						ev.y = mouse.cur_y;
						ev.time = mouseMsg->dwTimeStamp;
						ev.scan_code = 0;
						ev.skey_state = mouse.skey_state;
						mouse.add_event(&ev);
						mouse.reset_boundary();			// reset_boundary whenever left button is released
						rc = 1;
					}
                                }
                                else if (mouseMsg->dwOfs == DIMOFS_BUTTON1)
                                {
					if( mouseMsg->dwData & 0x80)
					{
						// mouse button pressed
						mouse.right_press = RIGHT_BUTTON_MASK;
                        			ev.event_type = RIGHT_BUTTON;
						ev.x = mouse.cur_x;
						ev.y = mouse.cur_y;
						ev.time = mouseMsg->dwTimeStamp;
						ev.scan_code = 0;
						ev.skey_state = mouse.skey_state;
						mouse.add_event(&ev);
						rc = 1;
					}
					else
					{
						// mouse button released
						mouse.right_press = 0;
						ev.event_type = RIGHT_BUTTON_RELEASE;
						ev.x = mouse.cur_x;
						ev.y = mouse.cur_y;
						ev.time = mouseMsg->dwTimeStamp;
						ev.scan_code = 0;
						ev.skey_state = mouse.skey_state;
						mouse.add_event(&ev);
						rc = 1;
					}
                                }
                                else if (mouseMsg->dwOfs == DIMOFS_BUTTON2)
                                {
					// not interested
                                }
                                else if (mouseMsg->dwOfs == DIMOFS_BUTTON3)
                                {
					// not interested
                                }
                                else if (mouseMsg->dwOfs == DIMOFS_X)
                                {
					mouse.cur_x += mouse.micky_to_displacement(mouseMsg->dwData);
					// ##### begin Gilbert 9/10 ######//
					switch( mouse.bound_type )
					{
					case 0:		// rectangular boundary
						if(mouse.cur_x < mouse.bound_x1)
							mouse.cur_x = mouse.bound_x1;
						if(mouse.cur_x > mouse.bound_x2)
							mouse.cur_x = mouse.bound_x2;
						break;
					case 1:		// rhombus boundary
						{
							int boundMargin = abs(mouse.cur_y - (mouse.bound_y1+mouse.bound_y2)/2);
							if( mouse.cur_x < mouse.bound_x1+boundMargin )
								mouse.cur_x = mouse.bound_x1+boundMargin;
							if( mouse.cur_x > mouse.bound_x2-boundMargin )
								mouse.cur_x = mouse.bound_x2-boundMargin;
						}
						break;
					default:
						err_here();
					}
					// ##### end Gilbert 9/10 ######//
					moveFlag = 1;
                                }
                                else if (mouseMsg->dwOfs == DIMOFS_Y)
                                {
					mouse.cur_y += mouse.micky_to_displacement(mouseMsg->dwData);
					// ##### begin Gilbert 9/10 ######//
					switch( mouse.bound_type )
					{
					case 0:		// rectangular boundary
						if(mouse.cur_y < mouse.bound_y1)
							mouse.cur_y = mouse.bound_y1;
						if(mouse.cur_y > mouse.bound_y2)
							mouse.cur_y = mouse.bound_y2;
						break;
					case 1:		// rhombus boundary
						{
							int boundMargin = abs(mouse.cur_x - (mouse.bound_x1+mouse.bound_x2)/2);
							if( mouse.cur_y < mouse.bound_y1+boundMargin )
								mouse.cur_y = mouse.bound_y1+boundMargin;
							if( mouse.cur_y > mouse.bound_y2-boundMargin )
								mouse.cur_y = mouse.bound_y2-boundMargin;
						}
						break;
					default:
						err_here();
					}
					// ##### end Gilbert 9/10 ######//
					moveFlag = 1;
                                }
                                else if (mouseMsg->dwOfs == DIMOFS_Z)
                                {
					// not interested
				}
				--mouseLen;
				++mouseMsg;
			}
			else if( earlyDevice == 2 )
			{
				if( mouse.process_key_message( keybMsg->dwOfs, keybMsg->dwData, keybMsg->dwTimeStamp) )
					rc = 1;
				--keybLen;
				++keybMsg;
			}
		}
	}

	if(moveFlag)
	{
		mouse_cursor.process(mouse.cur_x, mouse.cur_y);     // repaint mouse cursor
		power.mouse_handler();
	}

	return rc;
}

void GetMousePos (int *x, int *y)
{
  POINT pt;
  GetCursorPos(&pt);
  *x = pt.x;
  *y = pt.y;
}

void SetMousePos (int x, int y)
{
  SetCursorPos (x, y);
}



