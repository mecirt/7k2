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

//Filename    : OVGA.H
//Description : Header file for class OVGA (Direct Draw version)

#ifndef __OVGA_H
#define __OVGA_H

#include <asmfun.h>

#ifndef __OVGABUF_H
#include <ovgabuf.h>
#endif

#ifndef __COLOR_H
#include <color.h>
#endif

//----------- define constants ----------//

//#define VGA_WIDTH             800
//#define VGA_HEIGHT            600
//#define VGA_BPP                16

#define MAX_BRIGHTNESS_ADJUST_DEGREE	32

//----------- Define constant -------------//

#define IF_LIGHT_BORDER_COLOR     V_WHITE
#define IF_DARK_BORDER_COLOR		 V_BLACK
#define IF_UP_BRIGHTNESS_ADJUST	 5
#define IF_DOWN_BRIGHTNESS_ADJUST 6

//-------- Define macro functions ---------//

#define get_bitmap_width(bitmapPtr)  (*(short*)bitmapPtr)
#define get_bitmap_height(bitmapPtr) (*((short*)bitmapPtr+1))

//-------- Define class Vga ----------------//


struct RGBColor;
class ColorTable;
class VgaCustomPalette;

class Vga
{
public:

			union
			{
#ifdef __DDRAW_INCLUDED__
				LPDIRECTDRAW4		dd_obj;
#endif
				LPVOID				vptr_dd_obj;
			};

			union
			{
#ifdef __DDRAW_INCLUDED__
				LPDIRECTDRAWPALETTE	dd_pal;
#endif
				LPVOID					vptr_dd_pal;
			};

			union
			{
#ifdef __DDRAW_INCLUDED__
				PALETTEENTRY			pal_entry_buf[256];
#endif
				DWORD						dw_pal_entry_buf[256];
			};

		  ColorTable*				vga_color_table;
		  ColorTable*				vga_blend_table;
		  unsigned char			gray_remap_table[256];
		  int							pixel_format_flag;				// see IMGFUN.H	// new for 16-bit
		  short *					default_remap_table;				// new for 16-bit
		  short *					default_blend_table;				// new for 16-bit

		  static VgaBuf*			active_buf;
		  static char				use_back_buf;
		  static char				opaque_flag;

public:
		  Vga();
		  ~Vga();

		  BOOL   init();
		  BOOL   init_dd();
		  BOOL   set_mode(int width, int height);
		  void   deinit();

		  BOOL	is_inited();

		  BOOL   load_pal(const char* fileName);
		  void	init_gray_remap_table();

		  void   activate_pal(VgaBuf*);
		  void   release_pal();

		  void 	d3_panel_up(int x1,int y1,int x2,int y2,int vgaFrontOnly=0,int drawBorderOnly=0);
		  void 	d3_panel_down(int x1,int y1,int x2,int y2,int vgaFrontOnly=0,int drawBorderOnly=0);
		  void	d3_panel2_up(int x1,int y1,int x2,int y2,int vgaFrontOnly=0,int drawBorderOnly=0);
		  void	d3_panel2_down(int x1,int y1,int x2,int y2,int vgaFrontOnly=0,int drawBorderOnly=0);
		  void 	separator(int x1, int y1, int x2, int y2);

		  void	adjust_brightness(int changeValue);

		  void 	use_front()	{ use_back_buf=0; active_buf = &vga_front; }
		  void 	use_back()	{ use_back_buf=1; active_buf = &vga_back;  }

		  BOOL   blt_buf(int x1, int y1, int x2, int y2, int putMouseCursor=1);
		  void	flip();

		  void 	disp_image_file(const char* fileName,int x1=0, int y1=0);
		  void 	finish_disp_image_file();

		  int		translate_color(unsigned char c)			// calc 8-bit color to 16-bit color // new for 16 bit
					{ return default_remap_table[c]; }

		  int		make_pixel(BYTE red, BYTE green, BYTE blue);
		  int		make_pixel(RGBColor *);
		  void	decode_pixel(int, RGBColor *);

private:
		  void	init_color_table();
};

extern Vga vga;
extern "C"
{
	extern short transparent_code_w __asmsym__("_transparent_code_w");
}

//--------------------------------------------//

#endif
