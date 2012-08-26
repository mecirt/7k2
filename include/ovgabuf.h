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

//Filename    : VGABUF.H
//Description : Header file for class VgaBuffer, Direct Draw Buffer.

#ifndef __VGABUF_H
#define __VGABUF_H

#include <imgfun.h>
#include <win32_compat.h>

typedef void *LPVOID;

//-------- Define class VgaBuf ----------------//

class File;
class Bitmap;
class BitmapW;

class VgaBuf
{
public:
	union
	{
#ifdef __DDRAW_INCLUDED__
		LPDIRECTDRAWSURFACE4  dd_buf;
#endif
		LPVOID					vptr_dd_buf;
	};
	union
	{
#ifdef __DDRAW_INCLUDED__
		LPDIRECTDRAWSURFACE4  dd_front_buf;
#endif
		LPVOID					vptr_dd_front_buf;
	};

	union
	{
#ifdef __DDRAW_INCLUDED__
		DDSURFACEDESC2			buf_des;
#endif
		char					c_buf_des[0x100];
	};

	BOOL						buf_locked;			// whether the and back buffers have been locked or not.
   short*					cur_buf_ptr;
	long						cur_pitch;			// buf_des.lPitch

	// ------- temp_unlock --------- //
	UCHAR						lock_stack_count;
	WORD						lock_bit_stack;

	//--------- back buffer ----------//

	short* buf_ptr() const  { return cur_buf_ptr; }
	short* buf_ptr(int x, int y) const { return (short *)((char *)cur_buf_ptr + cur_pitch*y) + x; }

	// pitch in pixel
	int 	buf_pitch()					{ return cur_pitch >> 1; }		// in no. of pixel

	// pitch in byte
	int 	buf_true_pitch()			{ return cur_pitch; }

	int	buf_size() const;
	int   buf_width() const;
	int   buf_height() const;

	//---- GUI colors -----//

	static char color_light;      // color of the light panel side
	static char color_dark;       // color of the dark panel side
	static char color_up;         // color of up_panel surface
	static char color_down;       // color of down_panel surface
	static char color_push;       // color of pushed button surface
	static char color_border;     // color of color

	static short *default_remap_table;				// new for 16-bit
	static short *default_blend_table;				// new for 16-bit

public:
	VgaBuf();
	~VgaBuf();

	//---------- system functions ----------//

	void 		init_surface();
	void		deinit();

	BOOL		is_buf_lost();
	BOOL		restore_buf();

	void		lock_buf();
	void		unlock_buf();

	void		temp_unlock();
	void		temp_restore_lock();
	void		temp_lock();
	void		temp_restore_unlock();

	int 		write_bmp_file(char* fileName);

	//---------- painting functions ----------//

	static int	translate_color(unsigned char c)		// calc 8-bit color to 16-bit color // new for 16 bit
				{ return default_remap_table[c]; }

	void		bar(int x1,int y1,int x2,int y2,int colorCode);
	void		bar_fast(int x1,int y1,int x2,int y2,int colorCode)
				{ IMGbar(cur_buf_ptr, cur_pitch, x1, y1, x2, y2, translate_color(colorCode)); } 
	void		barW_fast(int x1,int y1,int x2,int y2,int colorCode)
				{ IMGbar(cur_buf_ptr, cur_pitch, x1, y1, x2, y2, colorCode); } 
	void		bar_up(int x1,int y1,int x2,int y2)		{ bar( x1, y1, x2, y2, color_up); }
	void		bar_down(int x1,int y1,int x2,int y2)		{ bar( x1, y1, x2, y2, color_down); }
	void		bar_alpha(int x1, int y1, int x2, int y2, int logAlpha, int colorColor );

	void		draw_pixel(int x1,int y1,int colorCode)
				{ *buf_ptr(x1, y1) = translate_color(colorCode); }

	void  	separator(int,int,int,int);
	void		line(int x1,int y1,int x2,int y2,int colorCode);
	void  	thick_line(int x1,int y1,int x2,int y2,int colorCode);

	void  	indicator(int,int,int,int,float,float,int,int= -1);

	void  	rect(int,int,int,int,int,int);
	void  	d3_rect(int,int,int,int);

	void  	d3_panel_up(int,int,int,int,int=2,int=1);
	void  	d3_panel_down(int,int,int,int,int=2,int=1);
	void  	d3_panel_up_clear(int,int,int,int,int=2);
	void  	d3_panel_down_clear(int,int,int,int,int=2);

	void 		adjust_brightness(int x1,int y1,int x2,int y2,int adjustDegree);
	void 		draw_d3_up_border(int x1,int y1,int x2,int y2);
	void 		draw_d3_down_border(int x1,int y1,int x2,int y2);

	//-------- buffer saving functions --------//

	short* 	save_area(int,int,int,int,short* =0L);
	void  	rest_area(short*,int=1 );

	//------- large bitmap displaying functions -------//

	void 		put_large_bitmap(int x1, int y1, File* filePtr, short *colorRemapTable=0);
	void 		put_large_bitmap_trans(int x1, int y1, File* filePtr, short *colorRemapTable=0);

	//---------- bitmap manipulation functions ----------//

	// 8->16 blt
        // transparency: 0=no, 1=simple, 2=RLE, 3=RLE with half alpha
        void put_bitmap(int x, int y, char *bitmapBuf, short *colorRemapTable = 0, int transparency = 0, bool hmirror = false);
        void put_bitmap_area(int x, int y, char *bitmapBuf, int srcX1, int srcY1, int srcX2, int srcY2, short *colorRemapTable = 0, int transparency = 0, bool hmirror = false);

        // Wrappers - these just call the above two functions. For compatibility with code

	// functions with transparent color keying

	void		put_bitmap_trans(int x,int y,char* bitmapBuf)
				{ put_bitmap(x, y, bitmapBuf, 0, 1); }

	void		put_bitmap_area_trans( int x, int y, char *bitmapBuf, int srcX1, int srcY1, int srcX2, int srcY2 )
				{ put_bitmap_area(x, y, bitmapBuf, srcX1, srcY1, srcX2, srcY2, 0, 1); }

	void		put_bitmap_trans_remap( int x, int y, char *bitmapBuf, short *colorRemapTable )
				{ put_bitmap(x, y, bitmapBuf, colorRemapTable, 1); }

	void		put_bitmap_trans_decompress( int x, int y, char *bitmapBuf )
				{ put_bitmap(x, y, bitmapBuf, 0, 2); }

	void		put_bitmap_trans_remap_decompress( int x, int y, char *bitmapBuf, short *colorRemapTable)
				{ put_bitmap(x, y, bitmapBuf, colorRemapTable, 2); }

	void		put_bitmap_area_trans_remap_decompress( int x, int y, char *bitmapBuf, int srcX1, int srcY1, int srcX2, int srcY2, short *colorRemapTable)
				{ put_bitmap_area(x, y, bitmapBuf, srcX1, srcY1, srcX2, srcY2, colorRemapTable, 2); }

	// functions with blend bitmap

	void		put_bitmap_blend( int x, int y, char *bitmapBuf )
	{ IMGbltBlendRemap( cur_buf_ptr, cur_pitch, x, y, bitmapBuf, default_blend_table ); }

	void		put_bitmap_blend_hmirror( int x, int y, char *bitmapBuf )
	{ IMGbltBlendRemapHMirror( cur_buf_ptr, cur_pitch, x, y, bitmapBuf, default_blend_table ); }

	void		put_bitmap_blend_area( int x, int y, char *bitmapBuf, int srcX1, int srcY1, int srcX2, int srcY2 )
	{ IMGbltBlendAreaRemap( cur_buf_ptr, cur_pitch, x, y, bitmapBuf, srcX1, srcY1, srcX2, srcY2, default_blend_table ); }

	void		put_bitmap_blend_area_hmirror( int x, int y, char *bitmapBuf, int srcX1, int srcY1, int srcX2, int srcY2 )
	{ IMGbltBlendAreaRemapHMirror( cur_buf_ptr, cur_pitch, x, y, bitmapBuf, srcX1, srcY1, srcX2, srcY2, default_blend_table ); }

	// functions with weak blend bitmap

	void		put_bitmap_weak_blend( int x, int y, char *bitmapBuf )
	{ IMGbltWeakblendRemap( cur_buf_ptr, cur_pitch, x, y, bitmapBuf, default_blend_table ); }

	void		put_bitmap_weak_blend_hmirror( int x, int y, char *bitmapBuf )
	{ IMGbltWeakblendRemapHMirror( cur_buf_ptr, cur_pitch, x, y, bitmapBuf, default_blend_table ); }

	void		put_bitmap_weak_blend_area( int x, int y, char *bitmapBuf, int srcX1, int srcY1, int srcX2, int srcY2 )
	{ IMGbltWeakblendAreaRemap( cur_buf_ptr, cur_pitch, x, y, bitmapBuf, srcX1, srcY1, srcX2, srcY2, default_blend_table ); }

	void		put_bitmap_weak_blend_area_hmirror( int x, int y, char *bitmapBuf, int srcX1, int srcY1, int srcX2, int srcY2 )
	{ IMGbltWeakblendAreaRemapHMirror( cur_buf_ptr, cur_pitch, x, y, bitmapBuf, srcX1, srcY1, srcX2, srcY2, default_blend_table ); }

	// 16->16 blt

        void put_bitmapW(int x, int y, short *bitmapBuf, bool transparency = false);
        void put_bitmapW_area(int x, int y, short *bitmapBuf, int srcX1, int srcY1, int srcX2, int srcY2, bool transparency = false);
	void put_bitmapW(int x, int y, BitmapW *bitmapWBuf, bool transparency = false)
		{ put_bitmapW(x, y, (short *)bitmapWBuf, transparency); }
	void put_bitmapW_area(int x, int y, BitmapW *bitmapWBuf, int srcX1, int srcY1, int srcX2, int srcY2, bool transparency = false)
		{ put_bitmapW_area(x, y, (short *)bitmapWBuf, srcX1, srcY1, srcX2, srcY2, transparency); }

	void		read_bitmapW(int x1,int y1,int x2,int y2, short* bitmapWBuf);
	void		read_bitmapW(int x1,int y1,int x2,int y2, BitmapW* bitmapWBuf)
				{ read_bitmapW(x1, y1, x2, y2, (short *)bitmapWBuf); }
};

extern VgaBuf vga_buffer;
// for compatibility with existing code:
#define vga_back vga_buffer
#define vga_front vga_buffer

//--------------------------------------------//

#endif
