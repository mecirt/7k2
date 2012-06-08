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

// Filename   : OFLAME.CPP
// Description: class Flame
// Onwership  : Gilbert

// Frank Jan Sorensen Alias:Frank Patxi (fjs@lab.jt.dk) for the
// fireroutine.

#include <oflame.h>
#include <ovgabuf.h>
#include <all.h>
#include <osys.h>
#include <colcode.h>
#include <ocoltbl.h>

//-------------- Begin Function Flame::Flame ----------//
Flame::Flame()
{
	heat_map = 0;
	bitmap = 0;
}
//-------------- End Function Flame::Flame ----------//


//-------------- Begin Function Flame::Flame ----------//
Flame::Flame(short width, short height, short flameWidth, FlameType flameType, const char* fileName)
{
	heat_map = 0;
	bitmap = 0;
	init( width, height, flameWidth, flameType, fileName );
}
//-------------- End Function Flame::Flame ----------//


//-------------- Begin Function Flame::~Flame ----------//
Flame::~Flame()
{
	deinit();
}
//-------------- End Function Flame::~Flame ----------//


//-------------- Begin Function Flame::init ----------//
//
// <short> width             width of the bitmap
// <short> height            height of the bitmap
// <short> flameWdith        length of flame source at base line
// <FlameType> flameType     determine how flame source is generated at base line
//
void Flame::init(short width, short height, short flameWidth, FlameType flameType, const char* fileName)
{
	
	load_palette(fileName);
	map_width = width;
	map_height = height;

	seed = (width * height + flameWidth) | 1;
	// allocated more space for overflowing during rise()
	heat_map = (unsigned char *) mem_resize(heat_map, width * height + 4);

	memset(heat_map, 0, width*height);

	bitmap = (unsigned char *) mem_resize(bitmap, width*height + 2*sizeof(short) );

	memset(bitmap+2*sizeof(short), width*height, 255);
	*(short *)bitmap = width;
	*(((short *)bitmap)+1) = height;

	smooth = 16;
	shade_base = -1;
	decay = 800/ height;
	if( decay < 2)
		decay = 2;

	// ---------- put flame sources
	switch(flameType)
	{
	case FLAME_CENTRE_POINT:
		{
			short i = width/2;
			heat_map[i] = 250;

			//-------- smoothing around
			for( short j = 1 ; j < smooth; ++j)
			{
				unsigned char k = 250 * (smooth-j)/smooth;
				if( i-j >= 0)
					heat_map[i-j] = k;
				if( i+j < width)
					heat_map[i+j] = k;
			}
		}
		break;

	case FLAME_RANDOM_POINTS:
		{
			for(short n = flameWidth*4/smooth; n > 0; --n)
			{
				short i = (width-flameWidth)/2+random(flameWidth) ;
				heat_map[i] = 250;

				//-------- smoothing around
				for( short j = 1 ; j < smooth; ++j)
				{
					unsigned char k = 250 * (smooth-j)/smooth;
					if( i-j >= 0 && k>heat_map[i-j])
						heat_map[i-j] = k;
					if( i+j < width && k>heat_map[i+j])
						heat_map[i+j] = k;
				}
			}
		}
		break;

	case FLAME_WIDE:
		{
			short left = (width - flameWidth)/2;
			short right = left + flameWidth;
			memset(heat_map+left, 250, flameWidth);
			for( short j = 1; j < smooth; ++j)
			{
				unsigned char k = 250 * (smooth-j)/smooth;
				if( left-j >= 0)
					heat_map[left-j] = k;
				if( right+j <= width)
					heat_map[right+j-1] = k;
			}

		}
		break;
	default:
		err_now("undefined FlameType");
	}
}
//-------------- End Function Flame::init ----------//


//-------------- Begin Function Flame::deinit ----------//
void Flame::deinit()
{
	if(heat_map)
	{
		mem_del(heat_map);
		heat_map=NULL;
	}

	if(bitmap)
	{
		mem_del(bitmap);
		bitmap=NULL;
	}
}
//-------------- End Function Flame::deinit ----------//


//-------------- Begin Function Flame::operator= ----------//
Flame& Flame::operator= (Flame &f)
{
	if(heat_map && f.heat_map && map_width*map_height == f.map_width*f.map_height)
	{
		memcpy(heat_map, f.heat_map, f.map_width*f.map_height);
	}
	else
	{
		if(heat_map)
		{
			mem_del(heat_map);
			heat_map = NULL;
		}
		if(f.heat_map)
		{
			heat_map = (unsigned char *) mem_add(f.map_width*f.map_height+4);
			memcpy(heat_map, f.heat_map, f.map_width*f.map_height);
		}
	}

	if(bitmap && f.bitmap && map_width*map_height == f.map_width*f.map_height)
	{
		memcpy(bitmap, f.bitmap, f.map_width*f.map_height + 2*sizeof(short));
	}
	else
	{
		if(bitmap)
		{
			mem_del(bitmap);
			bitmap = NULL;
		}
		if(f.bitmap)
		{
			bitmap = (unsigned char *) mem_add(f.map_width*f.map_height+2*sizeof(short));
			memcpy(bitmap, f.bitmap, f.map_width*f.map_height + 2*sizeof(short));
		}
	}

	seed = f.seed;
	map_width = f.map_width;
	map_height = f.map_height;
	decay = f.decay;
	smooth = f.smooth;
	shade_base = f.shade_base;

	return(*this);
}
//-------------- End Function Flame::operator= ----------//

//-------------- Begin Function Flame::heat_up ----------//
void Flame::heat_up(short h)
{
	decay = h;
}
//-------------- End Function Flame::heat_up ----------//


//-------------- Begin Function Flame::rise ----------//
//
// change heat_map to next pattern
// <short> wind         -1 = blow left (west), 0 = center, +1 = blow right
//
void Flame::rise(short wind)
{
	// rise of each particle of frame
	unsigned char *p;			// pointer to heat_map

	short x,y;
/*
	// 1st version, left to right, top to bottom
	for( x=0; x < map_width; ++x)
	{
		y = map_height-2;
		p = heat_map + y*map_width + x;
		for( ; y >= 0; --y, p-=map_width )
		{
			unsigned char v = *p;
			if( v < decay || x <= 0 || x >= map_width-1)
			{
				p[map_width] = 0;					// the pixel just above
			}
			else
			{
				// draw a point on the buffer
				p[map_width -random(3)+1  + wind*random(2)] =
					v - random(decay);
			}
		}
	}
*/
	 
	// 2nd version top to bottom, left to center and right to center
	for( y = map_height-2; y >= 0; --y)
	{
		// left to center
		p = heat_map + y * map_width;
		for( x = 0; x <= map_width/2; ++x, ++p)
		{
			unsigned char v = *p;
			if( v < decay || x <= 0)
			{
				p[map_width] = 0;				// the pixel just above
			}
			else
			{
				// draw a point on the buffer
				p[map_width -random(3)+1 + wind*random(2)] =
					v - random(decay);
			}
		}

		// right to center
		p = heat_map + y * map_width + map_width-1;
		for( x = map_width; x > map_width/2; --x, --p)
		{
			unsigned char v = *p;
			if( v < decay || x >= map_width-1)
			{
				p[map_width] = 0;				// the pixel just above
			}
			else
			{
				// draw a point on the buffer
				p[map_width -random(3)+1 + wind*random(2)] =
					v - random(decay);
			}
		}

	}
}
//-------------- End Function Flame::rise ----------//


//-------------- Begin Function Flame::gen_bitmap ----------//
//
// convert heat_map to transparent bitmap
// to be blt to vgabuf by put_bitmap_trans or put_bitmap_area_trans
//
// [unsigned char] shadeColor       first (and brightest) color of the color scale
//                                   (default: 0x10 for yellow-to-red flame)
//
void Flame::gen_bitmap(unsigned char shadeColor)
{
	// generate color code
	static unsigned char lastShadeBase = 0;
	static unsigned char colorTable[256];
/*	static unsigned char colorTable[256] = 
	{
		TRANSPARENT_CODE, 0xd2, 0xd2, 0xd2, 0xd2, 0xd2, 0xd2, 0xd2,
		0xd2, 0xd2, 0xd2, 0xd2, 0xd2, 0xd2, 0xd2, 0xd2,
		0xd2, 0xd2, 0xd2, 0xd2, 0xd2, 0xd2, 0xd2, 0xd2,
		0xd2, 0xd2, 0xd2, 0xd2, 0xd2, 0xd2, 0xd2, 0xd2,
		0xd1, 0xd1, 0xd1, 0xd1, 0xd1, 0xd1, 0xd1, 0xd1, 
		0xd1, 0xd1, 0xd1, 0xd1, 0xd1, 0xd1, 0xd1, 0xd1, 
		0xd1, 0xd1, 0xd1, 0xd1, 0xd1, 0xd1, 0xd1, 0xd1, 
		0xd1, 0xd1, 0xd1, 0xd1, 0xd1, 0xd1, 0xd1, 0xd1, 
		0xb2, 0xb2, 0xb2, 0xb2, 0xb2, 0xb2, 0xb2, 0xb2, 
		0xb2, 0xb2, 0xb2, 0xb2, 0xb2, 0xb2, 0xb2, 0xb2, 
		0xb2, 0xb2, 0xb2, 0xb2, 0xb2, 0xb2, 0xb2, 0xb2, 
		0xb2, 0xb2, 0xb2, 0xb2, 0xb2, 0xb2, 0xb2, 0xb2, 
		0xb2, 0xb2, 0xb2, 0xb2, 0xb2, 0xb2, 0xb2, 0xb2, 
		0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 
		0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 
		0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 
		0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 
		0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 
		0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 
		0x7a, 0x7a, 0x7a, 0x7a, 0x7a, 0x7a, 0x7a, 0x7a, 
		0x7a, 0x7a, 0x7a, 0x7a, 0x7a, 0x7a, 0x7a, 0x7a, 
		0x7a, 0x7a, 0x7a, 0x7a, 0x7a, 0x7a, 0x7a, 0x7a, 
		0x7a, 0x7a, 0x7a, 0x7a, 0x7a, 0x7a, 0x7a, 0x7a, 
		0x7a, 0x7a, 0x7a, 0x7a, 0x7a, 0x7a, 0x7a, 0x7a, 
		0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 
		0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 
		0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 
		0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 
		0xeb, 0xeb, 0xeb, 0xeb, 0xeb, 0xeb, 0xeb, 0xeb, 
		0xeb, 0xeb, 0xeb, 0xeb, 0xeb, 0xeb, 0xeb, 0xeb, 
		0xeb, 0xeb, 0xeb, 0xeb, 0xeb, 0xeb, 0xeb, 0xeb, 
		0xeb, 0xeb, 0xeb, 0xeb, 0xeb, 0xeb, 0xeb, 0xeb, 
	};
	*/
	shade_base = shadeColor;
	if( shade_base != lastShadeBase )
	{
		lastShadeBase = (unsigned char)shade_base;
		unsigned char *ct = colorTable;
		*ct = TRANSPARENT_CODE;
		ct++;
		for(int b = 1; b <= 255; ++b, ct++)
		{
		//	*ct = shadeColor + (b >> 5);
			*ct = b;
		}
	} 

	unsigned char *b = bitmap+2*sizeof(short);
	unsigned char *p = heat_map+(map_height-1)*map_width;

	for(int i = 0; i < map_height; i++)
	{
		for(int j = 0; j < map_width; j++)
			*b++ = colorTable[(unsigned int)*p++];
		p -= 2 * map_width;
	}
}
//-------------- End Function Flame::gen_bitmap ----------//


//-------------- Begin Function Flame::mask_bottom ----------//
//
// call after gen_bitmap,
// to round the bottom of bitmap
//
void Flame::mask_bottom()
{
	for( short x = 0; x < map_width; ++x)
	{
		short dx = x - map_width/2;
		short y;
		// fit a parabola
		// h = map_height/4 * (dx / (map_width/2))^2
		int h = map_height * dx * dx / (map_width * map_width);

		// modify it randomly
		if( h > 0)
			h += random(3)-1;		// -1 to +1
		else
			h += random(16)/8;			// 0 or +1;

		unsigned char *p = bitmap+2*sizeof(short)+(map_height-1)*map_width+x;
		for(y = 0; y < h; ++y)
		{
			*p = TRANSPARENT_CODE;
			p -= map_width;
		}
	}
}
//-------------- End Function Flame::mask_bottom ----------//


//-------------- Begin Function Flame::mask_transparent ----------//
//
// call after gen_bitmap,
// mask bitmap out every one out of two points to tranparent
//
void Flame::mask_transparent()
{
	unsigned char *b = bitmap + 2*sizeof(short);
	if( map_width & 1)
	{
		// odd number
		for( short y = 0; y < map_height; ++y)
			for( short x = y & 1; x < map_width; x += 2)
		{
			*b = TRANSPARENT_CODE;
			b += 2;
		}
	}
	else
	{
		// even number
		for( short y = 0; y < map_height; ++y)
		{
			b+= y & 1;		// shift one byte on odd line
			for( short x = y & 1; x < map_width; x += 2)
			{
				*b = TRANSPARENT_CODE;
				b += 2;
			}
			b-= y & 1;		// shift back to even dot.
		}
	}
}
//-------------- End Function Flame::mask_transparent ----------//


//-------------- Begin Function Flame::draw_step ----------//
void Flame::draw_step(short left, short bottom, VgaBuf *vgabuf, short wind)
{
	// rise of each particle of frame
	unsigned char *p;			// pointer to heat_map
	unsigned char *d;			// pointer to vgabuf
	short x,y;
	for( x=0; x < map_width; ++x)
	{
		y = map_height-2;
		p = heat_map + y*map_width + x;
		d = (unsigned char *)vgabuf->buf_ptr() + 
			(bottom-y)*vgabuf->buf_pitch() + left + x;
		for( ; y >= 0; --y, p-=map_width, d+=vgabuf->buf_pitch() )
		{
			unsigned char v = *p;
			if( /* v == 0 || */ v < decay || x <= 0 || x >= map_width-1)
			{
				p[map_width] = 0;					// the pixel just above
			}
			else
			{
				unsigned char n = v - random(decay);
				p[map_width -random(3)+1  + wind*random(2)] = n;
				// draw a point on the buffer
				*d = shade_base + 0x0f - ( n >> 4);
			}
		}
	}
}
//-------------- End Function Flame::mask_transparent ----------//


//-------------- Begin Function Flame::flush_point ---------//
void Flame::flush_point(short x, short y)
{
	if( x < 0)
		x = random(map_width/2) + random(map_width/2);
	if( y < 1)
		y = 1 + random(map_height/4);

	unsigned char *center = heat_map + y*map_width + x;
	*center >>= 3;
	center[-1] >>= 2;			// left
	if( x-2 >= 0)
		center[-2] >>= 1;
	center[1] >>= 2;			// right
	if( x+2 < map_width)
		center[2] >>= 1;
	center[map_width] >>= 2;	// top
	center[map_width-1] >>= 1;
	center[map_width+1] >>= 1;
}

//-------------- End Function Flame::flush_point ---------//


//-------------- Begin Function Flame::random ---------//
unsigned Flame::random(unsigned bound)
{
   #define MULTIPLIER      0x015a4e35L
   #define INCREMENT       1
   seed = MULTIPLIER * seed + INCREMENT;
	return seed % bound;

}

//-------------- End Function Flame::random ---------//

static RGBColor identity_color_function(RGBColor col, int, int)
{
	return col;
}

//-------------- Begin Function Flame::load_palette -------------- //
void Flame::load_palette(const char* fileName)
{
	String str;
	File palFile;
	ColorTable colorTable;
	
	str  = DIR_IMAGE;
	str += fileName;
//	str += "ICE.COL";
	str += ".COL";

	if( palFile.file_open(str,0) )
	{ 
		BYTE palBuf[256][3];
		palFile.file_seek(8);     				// bypass the header info
		palFile.file_read(palBuf, sizeof(palBuf));
		palFile.file_close();

		// ------- palette description -------------//

		PalDesc palBufDesc( palBuf, 3, 256, 8 );

		//-------- create color remap table ---------//

		colorTable.generate_table_fast( 1, palBufDesc, identity_color_function );
		memcpy( color_remap_table, colorTable.get_table(0), sizeof(color_remap_table) );
	}
}
//-------------- End Function Flame::load_palette -------------- //
