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

// Filename    : OBLOB2.H
// Description : header file of 2D binary block, four side extensible

#ifndef __OBLOB2W_H
#define __OBLOB2W_H

#include <obitmapw.h>

// ------- define class Blob2DW ----------//

class Blob2DW
{
public:
	BitmapW *ptr;
	unsigned int alloc_size;
	short	left_edge;
	short	top_edge;
	short	width;
	short	height;

public:
	Blob2DW();
	Blob2DW( Blob2DW & );
	~Blob2DW();

	Blob2DW& operator=(Blob2DW &);

	void	resize(short left, short top, short w, short h);
	int	extend_to(short x, short y);
	int	extend_to(short x1, short y1, short x2, short y2);

	short *bitmap_ptr()	{ return (short *)ptr; }
	short *buf_ptr()		{ return ptr->bitmap; }
	int	buf_pitch()		{ return ptr->get_pitch(); }
	int	buf_true_pitch() { return ptr->get_true_pitch(); }
	short *p(short x, short y, int autoExtend=1);
	void	fill_area(short x1, short y1, short x2, short y2, short color,
		int autoExtend=1);
	void	auto_clip(short *pLeft, short *pTop, short *pWidth, short *pHeight, int autoResize=1);

	void	clear();
	int	is_clear()	{ return width == 0 && height == 0; }
};

#endif
