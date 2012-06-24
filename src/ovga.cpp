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

//Filename    : OVGA.CPP
//Description : VGA manipulation functions (Direct Draw version)

#define DEBUG_LOG_LOCAL 1

#include <all.h>
#include <imgfun.h>
#include <colcode.h>
#include <omouse.h>
#include <omousecr.h>
#include <ocoltbl.h>
#include <ofile.h>
#include <osys.h>
#include <ovga.h>
#include <olog.h>
#include <ovgalock.h>
#include <omodeid.h>

//-------- Define constant --------//

#define UP_OPAQUE_COLOR       (VGA_GRAY+10)
#define DOWN_OPAQUE_COLOR     (VGA_GRAY+13)

//------ Define static class member vars ---------//

char    Vga::opaque_flag  = 0;
VgaBuf* Vga::active_buf   = &vga_buffer;

extern "C"
{
	short transparent_code_w;
}

// ------ declare static function ----------//

RGBColor log_alpha_func(RGBColor, int, int);

//-------- Begin of function Vga::Vga ----------//

Vga::Vga()
{
	memset( this, 0, sizeof(Vga) );

   vga_color_table = new ColorTable;
	vga_blend_table = new ColorTable;
}
//-------- End of function Vga::Vga ----------//


//-------- Begin of function Vga::~Vga ----------//

Vga::~Vga()
{
   deinit();      // 1-is final

	delete vga_blend_table;
   delete vga_color_table;
}
//-------- End of function Vga::~Vga ----------//


//-------- Begin of function Vga::init ----------//

BOOL Vga::init()
{
   //--------- Initialize DirectDraw object --------//

   if( !init_dd() )
      return FALSE;

   if( !set_mode(VGA_WIDTH, VGA_HEIGHT) )
      return FALSE;

   File palFile;
   palFile.file_open(DIR_RES"pal_std.res");

   BYTE palBuf[0x100][3];
   palFile.file_seek(8);     				// bypass the header info
   palFile.file_read(palBuf, sizeof(palBuf));
   palFile.file_close();

   // ------- palette description -------------//

   PalDesc palBufDesc( palBuf, 3, 0x100, 8 );

   //-------- create color remap table ---------//

   vga_color_table->generate_table_fast( 0, palBufDesc, ColorTable::bright_func );
   default_remap_table = (short *) malloc(0x100 * 2);
   memcpy( default_remap_table, vga_color_table->get_table(0), 0x100 * 2 );

   default_blend_table = 0;
  
   return TRUE;
}
//-------- End of function Vga::init ----------//


//-------- Begin of function Vga::init_dd ----------//

BOOL Vga::init_dd()
{
  return InitGraphics();
}
//-------- End of function Vga::init_dd ----------//


//-------- Begin of function Vga::set_mode ----------//

BOOL Vga::set_mode(int w, int h)
{
  if (!SetDisplayMode (w, h)) return false;

  // assembly functions to initalize effect processing
  INITeffect(pixel_format_flag);
  INITbright(pixel_format_flag);

  return true;
}
//-------- End of function Vga::set_mode ----------//


//-------- Begin of function Vga::deinit ----------//

void Vga::deinit()
{
  DeinitGraphics();
}
//-------- End of function Vga::deinit ----------//

//----------- Begin of function Vga::flip ----------//
void Vga::flip()
{
	mouse_cursor.before_flip();

	vga_buffer.temp_unlock();

  FlipBuffer (&vga_buffer);

	vga_buffer.temp_restore_lock();

	mouse_cursor.after_flip();

  vga_buffer.bar_alpha( 0, 0, VGA_WIDTH-1, VGA_HEIGHT-1, 5, V_BLACK );
}
//----------- End of function Vga::flip ----------//



//----------- Begin of function Vga::d3_panel_up ------------//
//
// <int> x1,y1,x2,y2  = the four vertex of the panel
// [int] vgaFrontOnly = do all the bitmap processing on the front buffer only
//                      (default: 0)
// [int] drawBorderOnly = draw border only, do not brighten the center area
//                        (default: 0)
//
void Vga::d3_panel_up(int x1,int y1,int x2,int y2,int vgaFrontOnly,int drawBorderOnly)
{
   err_when( x1>x2 || y1>y2 || x1<0 || y1<0 || x2>=VGA_WIDTH || y2>=VGA_HEIGHT );

   if( !drawBorderOnly )
   {
		// ##### begin Gilbert 19/10 #####//
      if( Vga::opaque_flag )
         vga_back.bar(x1+1, y1+1, x2-1, y2-1, VgaBuf::color_light);
      else
			vga_back.bar_alpha( x1+1, y1+1, x2-1, y2-1, IF_UP_BRIGHTNESS_ADJUST/2-1, VgaBuf::color_light );
		// ##### end Gilbert 19/10 #####//
   }

	vga_back.draw_d3_up_border( x1, y1, x2, y2 );

}
//------------- End of function Vga::d3_panel_up ------------//


//----------- Begin of function Vga::d3_panel_down ------------//
//
// <int> x1,y1,x2,y2  = the four vertex of the panel
// [int] vgaFrontOnly = do all the bitmap processing on the front buffer only
//                      (default: 0)
// [int] drawBorderOnly = draw border only, do not brighten the center area
//                        (default: 0)
//
void Vga::d3_panel_down(int x1,int y1,int x2,int y2,int vgaFrontOnly,int drawBorderOnly)
{
   err_when( x1>x2 || y1>y2 || x1<0 || y1<0 || x2>=VGA_WIDTH || y2>=VGA_HEIGHT );

   if( !drawBorderOnly )
   {
      if( Vga::opaque_flag )
         vga_back.bar(x1+1, y1+1, x2-1, y2-1, VgaBuf::color_dark );
      else
         vga_back.bar_alpha(x1+1, y1+1, x2-1, y2-1, IF_DOWN_BRIGHTNESS_ADJUST/2-1, VgaBuf::color_dark );
   }

	vga_back.draw_d3_down_border( x1, y1, x2, y2 );
}
//------------- End of function Vga::d3_panel_down ------------//


//----------- Begin of function Vga::d3_panel2_up ------------//
//
// <int> x1,y1,x2,y2  = the four vertex of the panel
// [int] vgaFrontOnly = do all the bitmap processing on the front buffer only
//                      (default: 0)
// [int] drawBorderOnly = draw border only, do not brighten the center area
//                        (default: 0)
//
void Vga::d3_panel2_up(int x1,int y1,int x2,int y2,int vgaFrontOnly,int drawBorderOnly)
{
   err_when( x1>x2 || y1>y2 || x1<0 || y1<0 || x2>=VGA_WIDTH || y2>=VGA_HEIGHT );

   VgaBuf *vgaBuf = &vga_back;

   if( !drawBorderOnly )
      vgaBuf->bar_alpha(x1+2, y1+2, x2-3, y2-3, IF_UP_BRIGHTNESS_ADJUST/2-1, V_WHITE);

   //--------- white border on top and left sides -----------//

   vgaBuf->bar_fast( x1,y1,x2-3,y1+1,0x9a );
   vgaBuf->draw_pixel(x2-2, y1, 0x9a);
   vgaBuf->bar_fast( x1,y1+2,x1+1,y2-3, 0x9a );    // left side
   vgaBuf->draw_pixel(x1, y2-2, 0x9a);

   //--------- black border on bottom and right sides -----------//

   vgaBuf->bar_fast( x2-2,y1+2,x2-1,y2-1, 0x90 );     // bottom side
   vgaBuf->draw_pixel(x2-1, y1+1, 0x90);
   vgaBuf->bar_fast( x1+2,y2-2,x2-3,y2-1, 0x90 );      // right side
   vgaBuf->draw_pixel(x1+1, y2-1, 0x90);

   //--------- junction between white and black border --------//
   vgaBuf->draw_pixel(x2-1, y1, 0x97);
   vgaBuf->draw_pixel(x2-2, y1+1, 0x97);
   vgaBuf->draw_pixel(x1, y2-1, 0x97);
   vgaBuf->draw_pixel(x1+1, y2-2, 0x97);

   //--------- gray shadow on bottom and right sides -----------//
   vgaBuf->bar_fast( x2, y1+1, x2, y2, 0x97);
   vgaBuf->bar_fast( x1+1, y2, x2-1, y2, 0x97);

   //-------------------------------------------//

   mouse.show_area();
}
//------------- End of function Vga::d3_panel_up ------------//


//----------- Begin of function Vga::d3_panel2_down ------------//
//
// <int> x1,y1,x2,y2  = the four vertex of the panel
// [int] vgaFrontOnly = do all the bitmap processing on the front buffer only
//                      (default: 0)
// [int] drawBorderOnly = draw border only, do not brighten the center area
//                        (default: 0)
//
void Vga::d3_panel2_down(int x1,int y1,int x2,int y2,int vgaFrontOnly,int drawBorderOnly)
{
   err_when( x1>x2 || y1>y2 || x1<0 || y1<0 || x2>=VGA_WIDTH || y2>=VGA_HEIGHT );

   VgaBuf* vgaBuf;
   vgaBuf = &vga_buffer;

   if( !drawBorderOnly )
      vgaBuf->bar_alpha(x1+2, y1+2, x2-3, y2-3, IF_DOWN_BRIGHTNESS_ADJUST/2-1, V_WHITE);

   mouse.hide_area( x1,y1,x2,y2 );

   //--------- black border on top and left sides -----------//

   vgaBuf->bar_fast( x1,y1,x2-3,y1+1,0x90 );
   vgaBuf->draw_pixel(x2-2, y1, 0x90);
   vgaBuf->bar_fast( x1,y1+2,x1+1,y2-3, 0x90 );    // left side
   vgaBuf->draw_pixel(x1, y2-2, 0x90);

   //--------- while border on bottom and right sides -----------//

   vgaBuf->bar_fast( x2-2,y1+2,x2-1,y2-1, 0x9a );     // bottom side
   vgaBuf->draw_pixel(x2-1, y1+1, 0x9a);
   vgaBuf->bar_fast( x1+2,y2-2,x2-3,y2-1, 0x9a );      // right side
   vgaBuf->draw_pixel(x1+1, y2-1, 0x9a);

   //--------- junction between white and black border --------//
   vgaBuf->draw_pixel(x2-1, y1, 0x97);
   vgaBuf->draw_pixel(x2-2, y1+1, 0x97);
   vgaBuf->draw_pixel(x1, y2-1, 0x97);
   vgaBuf->draw_pixel(x1+1, y2-2, 0x97);

   //--------- remove shadow, copy from back  -----------//
   vgaBuf->bar_fast( x2, y1+1, x2, y2, 0x9c);
   vgaBuf->bar_fast( x1+1, y2, x2-1, y2, 0x9c);

   mouse.show_area();
}
//------------- End of function Vga::d3_panel2_down ------------//


//------------- Start of function Vga::separator --------------//
//
// Draw a VGA separator line
//
// Syntax : separator( x1, y1, x2, y2 )
//
// int x1,y1       - the top left vertex of the separator
// int x2,y2       - the bottom right vertex of the separator
//
void Vga::separator(int x1, int y1, int x2, int y2)
{
   err_when( x1>x2 || y1>y2 || x1<0 || y1<0 || x2>=VGA_WIDTH || y2>=VGA_HEIGHT );

   if( y1+1==y2 )       // horizontal line
   {
      vga_front.bar_alpha(x1, y1, x2, y1, IF_UP_BRIGHTNESS_ADJUST/2-1, V_WHITE);
      vga_front.bar_alpha(x1, y2, x2, y2, IF_DOWN_BRIGHTNESS_ADJUST/2-1, V_WHITE);
   }
   else
   {
      vga_front.bar_alpha(x1, y1, x1, y2, IF_UP_BRIGHTNESS_ADJUST/2-1, V_WHITE);
      vga_front.bar_alpha(x2, y1, x2, y2, IF_DOWN_BRIGHTNESS_ADJUST/2-1, V_WHITE);
   }
}
//--------------- End of function Vga::separator --------------//


int Vga::make_pixel(BYTE red, BYTE green, BYTE blue)
{
	// ##### begin Gilbert 19/10 #######//
	return IMGmakePixel( (blue << 16) + (green << 8) + red);
	// ##### end Gilbert 19/10 #######//
}

int Vga::make_pixel(RGBColor *rgb)
{
	int u;
	memcpy(&u, rgb, sizeof(RGBColor));
	return IMGmakePixel(u);
}

void Vga::decode_pixel(int p, RGBColor *rgb)
{
	int u = IMGdecodePixel(p);
	memcpy(rgb, &u, sizeof(RGBColor));
}



// --------- begin of static function log_alpha_func -------//
//
// function for calculating table for IMGbltBlend
//
RGBColor log_alpha_func(RGBColor i, int scale, int absScale)
{
	RGBColor r;
	if( scale >= 0 )
	{
		r.red   = i.red   - (i.red   >> scale);
		r.green = i.green - (i.green >> scale);
		r.blue  = i.blue  - (i.blue  >> scale);
	}
	else
	{
		r.red   = i.red   >> -scale;
		r.green = i.green >> -scale;
		r.blue  = i.blue  >> -scale;
	}

	return r;
}


