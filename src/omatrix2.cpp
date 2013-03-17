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

// Filename    : OMATRIX2.CPP
// Description : render a location

#include <omatrix.h>
#include <oblob2w.h>
#include <oworldmt.h>
#include <ovga.h>
#include <ocoltbl.h>
#include <oterrain.h>
#include <oconfig.h>
#include <osys.h>
#include <oexpmask.h>


// ------- define constant table ---------//
// 

static short last_draw_state = -1;

// slope_brightness_table must be array, cannot be pointer
static long slope_brightness_table[4 * MAX_BRIGHTNESS_ADJUST_DEGREE + 1];

static long full_slope_brightness_table[4 * MAX_BRIGHTNESS_ADJUST_DEGREE + 1] = 
{
	 0x0500, 0x0500, 0x0500, 0x0500, 0x0500, 0x0500, 0x0500, 0x0500,
	 0x0400, 0x0400, 0x0400, 0x0400, 0x0400, 0x0400, 0x0400, 0x0400,
	 0x0300, 0x0300, 0x0300, 0x0300, 0x0300, 0x0300, 0x0300, 0x0300,
	 0x0200, 0x0200, 0x0200, 0x0200, 0x0200, 0x0200, 0x0200, 0x0200,
	 0x0100, 0x0100, 0x0100, 0x0100, 0x0100, 0x0100, 0x0000, 0x0000,
	 0x0000, 0x0000, 0x0000,-0x0100,-0x0100,-0x0100,-0x0100,-0x0200,
	-0x0200,-0x0200,-0x0300,-0x0300,-0x0400,-0x0400,-0x0500,-0x0500,
	-0x0600,-0x0700,-0x0700,-0x0800,-0x0900,-0x0a00,-0x0a00,-0x0b00,
	-0x0c00,
	-0x0d00,-0x0e00,-0x0f00,-0x1000,-0x1100,-0x1100,-0x1200,-0x1300,
	-0x1400,-0x1500,-0x1600,-0x1700,-0x1800,-0x1900,-0x1a00,-0x1a00,
	-0x1b00,-0x1c00,-0x1d00,-0x1e00,-0x1e00,-0x1f00,-0x1f00,-0x2000,
	-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000
	-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000
	-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000
	-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000
	-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000
};


static long fog_slope_brightness_table[4 * MAX_BRIGHTNESS_ADJUST_DEGREE + 1] = 
{	// minus 600 from slope_brightness_table
	-0x0100,-0x0100,-0x0100,-0x0100,-0x0100,-0x0100,-0x0100,-0x0100,
	-0x0200,-0x0200,-0x0200,-0x0200,-0x0200,-0x0200,-0x0200,-0x0200,
	-0x0300,-0x0300,-0x0300,-0x0300,-0x0300,-0x0300,-0x0300,-0x0300,
	-0x0400,-0x0400,-0x0400,-0x0400,-0x0400,-0x0400,-0x0400,-0x0400,
	-0x0500,-0x0500,-0x0500,-0x0500,-0x0500,-0x0500,-0x0600,-0x0600,
	-0x0600,-0x0600,-0x0600,-0x0700,-0x0700,-0x0700,-0x0700,-0x0800,
	-0x0800,-0x0800,-0x0900,-0x0900,-0x0a00,-0x0a00,-0x0b00,-0x0b00,
	-0x0c00,-0x0d00,-0x0d00,-0x0e00,-0x0f00,-0x1000,-0x1000,-0x1100,
	-0x1200,
	-0x1300,-0x1400,-0x1500,-0x1600,-0x1700,-0x1700,-0x1800,-0x1900,
	-0x1a00,-0x1b00,-0x1c00,-0x1d00,-0x1e00,-0x1f00,-0x2000,-0x2000,
	-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,
	-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,
	-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,
	-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,
	-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,
	-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,
};

static long dark_slope_brightness_table[4 * MAX_BRIGHTNESS_ADJUST_DEGREE + 1] = 
{	// all dark
	-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,
	-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,
	-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,
	-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,
	-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,
	-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,
	-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,
	-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,
	-0x2000,
	-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,
	-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,
	-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,
	-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,
	-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,
	-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,
	-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,
	-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,-0x2000,
};

static long *slope_tables[3] = { dark_slope_brightness_table, fog_slope_brightness_table, full_slope_brightness_table };

#define SEA_COLOR 0xf0


// ---------- define static variable used in rendering --------- //

static long	c00, c10, c01, c20, c11, c02, c30, c21, c12, c03;

// row a
static long rowaZ, rowaDuZM2, rowaDuuZM2, DuuuZM4D3;
static long rowaDvZM2, rowaDvvZM2, DvvvZM4D3, 		// for increase rowaZ
	rowaDuvZM4, DuvvZM4,			// for increase rowaDuZM2
	DuuvZM4;	// for increase rowaDuuZM2

// row b
static long rowbZ, rowbDuZM2, rowbDuuZM2;
static long rowbDvZM2, rowbDvvZM2,					// for increase rowbZ
	rowbDuvZM4;						// for increase rowbDuZM2

// variable for inner loop
// row a
static long aZ, aDuZM2, aDuuZM2;
// row b
static long bZ, bDuZM2, bDuuZM2;

// loop counters
static long vCount, uCount;

// remap table
static unsigned short *remapTables;

// for sea square
static short seaAltitude;
static long seaColor;


// Source of rotl and rotr functions:
//   http://en.wikipedia.org/wiki/Circular_shift
unsigned int rotl(const unsigned int value, int shift)
{
	if( (shift &= sizeof(value) * 8 - 1) == 0 )
		return value;
	return (value << shift) | (value >> (sizeof(value) * 8 - shift));
}

unsigned int rotr(const unsigned int value, int shift)
{
	if( (shift &= sizeof(value) * 8 - 1) == 0 )
		return value;
	return (value >> shift) | (value << (sizeof(value) * 8 - shift));
}

long Location::evaluate_z(short x, short y)
{
	long z = ((c30 * x + c20) * x + c10) * x;
	z += ((c03 * y + c02) * y + c01) * y;
	z += ((c21 * x + c12 * y) + c11) * x * y;
	z = (z + c00) >> C_MULTIPLIER_SHIFT;

	return z;
}


// render special
void LocationCorners::render_special(Blob2DW *blob, int color1, int color2)
{
	blob->resize(ZOOM_LOC_Y_WIDTH, loc_ptr->min_y, ZOOM_LOC_X_WIDTH - ZOOM_LOC_Y_WIDTH, loc_ptr->max_y-loc_ptr->min_y+1 );

	// try to freeze the blob, don't allow it to resize

	c00 = loc_ptr->c00;
	c10 = loc_ptr->c10;
	c01 = loc_ptr->c01;
	c20 = loc_ptr->c20;
	c11 = loc_ptr->c11;
	c02 = loc_ptr->c02;
	c30 = loc_ptr->c30;
	c21 = loc_ptr->c21;
	c12 = loc_ptr->c12;
	c03 = loc_ptr->c03;

	// plot four pixel (2x2) at one time
	// top row A start from (u=0, v=0)
	// bottom row B start from (u=1, v=1)

	// row a
	rowaZ = c00;
	rowaDuZM2 = c10 * 2;
	rowaDuuZM2 = c20 * 4;
	DuuuZM4D3 = c30 * 8;		// common for row a and b
	// for increase rowaZ
	rowaDvZM2 = c01 * 2;
	rowaDvvZM2 = c02 * 4;
	DvvvZM4D3 = c03 * 8;		// common for row a and b
	// for increase rowaDuZM2
	rowaDuvZM4 = c11 * 4;
	DuvvZM4 = c12 * 8;
	// for increase rowaDuuZM2
	DuuvZM4 = c21 * 8;

	// row b
	rowbZ = c00 + c01 + c10 + c20 + c11 + c02 + c30 + c21 + c12 + c03;
	rowbDuZM2 = (c30*3 + c21*2 + c12 + c20*2 + c11 + c10) * 2;
	rowbDuuZM2 = (c30*3 + c21 + c20) * 4;
	// long rowxDuuuZM4D3 = c30 * 8;		// common for row a and b
	// for increase rowbZ
	rowbDvZM2 = (c21 + c12*2 + c03*3 + c11 + c02*2 + c01) * 2;
	rowbDvvZM2 = (c12 + c03*3 + c02) * 4;
	// rowxDvvvZM4D3 = c03 * 8;		// common for row a and b
	// for increase rowbDuZM2
	rowbDuvZM4 = (c21*2 + c12*2 + c11) * 4;
	// rowxDuvvZM4 = c12 * 8;
	// for increase rowbDuuZM2
	// rowxDuuvZM4 = c21 * 8;

	err_when(ZOOM_Z_WIDTH != 0 || ZOOM_Z_HEIGHT != -1);

	remapTables = vga.vga_color_table->get_table();

	short *bufPtr = blob->ptr->bitmap
		+ (-ZOOM_Z_HEIGHT*top_left->altitude - blob->top_edge) * blob->ptr->get_pitch()
		- blob->left_edge;
		// do not use (unsigned char *)blob->p(0, -ZOOM_Z_HEIGHT*top_left->altitude , 0);
		// because it returns NULL when outside boundary

#define bufTruePitch (2*(ZOOM_LOC_X_WIDTH - ZOOM_LOC_Y_WIDTH))
#define bufTruePitchShift 7
	err_when( bufTruePitch != blob->buf_true_pitch() );
	err_when( (1 << bufTruePitchShift) != bufTruePitch );

#define MAX_BRIGHTNESS_SHIFT 5
#if(1 << MAX_BRIGHTNESS_SHIFT != MAX_BRIGHTNESS_ADJUST_DEGREE)
	#error
#endif

	seaAltitude = 0;
	if( loc_ptr->loc_flag & LOCATE_SEA_COAST )
	{
		if( top_left->is_sea() )
			seaAltitude = top_left->sea_altitude;
		if( top_right->is_sea() )
			seaAltitude = top_right->sea_altitude;
		if( bottom_left->is_sea() )
			seaAltitude = bottom_left->sea_altitude;
		if( bottom_right->is_sea() )
			seaAltitude = bottom_right->sea_altitude;
	}

        unsigned short usColor;
        usColor = vga.translate_color(color1);
        color1 = ((unsigned long)usColor << 16) | usColor;
        usColor = vga.translate_color(color2);
        color2 = ((unsigned long)usColor << 16) | usColor;

        short *buffer = bufPtr - 1;

        for (int v = 0; v < LOCATE_HEIGHT / 2; ++v) {

          // init inner loop a
          short *buf = buffer;

          int edx;
          aZ = rowaZ;
          aDuZM2 = rowaDuZM2;
          aDuuZM2 = rowaDuuZM2;

          for (int u = 0; u < LOCATE_WIDTH / 2; ++u) {
            // draw row a dot
            edx = aZ >> C_MULTIPLIER_SHIFT;
            if (edx <= seaAltitude)
              edx = seaAltitude;

            edx = edx << bufTruePitchShift;
            edx = -edx;
            buf[edx/2] = color1;
            buf[edx/2+bufTruePitch/2] = color1;

            aZ += aDuZM2 + aDuuZM2 + c30*8;
            aDuZM2 += 2*aDuuZM2 + 3*c30*8;
            aDuuZM2 += 3*c30*8;

            buf += bufTruePitch + 4;
          }  // end U loop

          // init inner loop b
          buffer += bufTruePitch/2;
          uCount = LOCATE_WIDTH / 2;
          buf = buffer;

          bZ = rowbZ;
          bDuZM2 = rowbDuZM2;
          bDuuZM2 = rowbDuuZM2;

          for (int u = 0; u < LOCATE_WIDTH / 2; ++u) {
            // draw row b dot
            edx = bZ >> C_MULTIPLIER_SHIFT;
            if (edx <= seaAltitude)
              edx = seaAltitude;

            edx = edx << bufTruePitchShift;
            edx = -edx;
            buf[edx/2] = color2;
            buf[edx/2+bufTruePitch/2] = color2;

            bZ += aDuZM2 + aDuuZM2 + c30*8;
            bDuZM2 += 2 * bDuuZM2 + 3 * c30*8;
            bDuuZM2 += 3 * c30*8;

            buf += bufTruePitch/2/* + 2*/;
          }  // end U loop

          // inc and compare outer loop
          rowaZ += rowaDvZM2 + rowaDvvZM2 + c03*8;
          rowaDvZM2 += 2 * rowaDvvZM2 + 3 * c03*8;
          rowaDvvZM2 += 3 * c03*8;

          rowaDuZM2 += rowaDuvZM4 + c12*8;
          rowaDuvZM4 += 2 * c12*8;
          rowaDuuZM2 += c21*8;

          rowbZ += rowbDvZM2 + rowbDvvZM2 + c03*8;

          rowbDvZM2 += 2 * rowbDvvZM2 + 3 * c03*8;
          rowbDvvZM2 += 3 * c03*8;
          rowbDuZM2 += rowbDuvZM4 + c12*8;
          rowbDuvZM4 += 2 * c12*8;
          rowbDuuZM2 += c21*8;

//          buffer -= 2;
        } // end V loop

#undef bufTruePitch
#undef bufTruePitchShift
#undef MAX_BRIGHTNESS_SHIFT
}






void LocationCorners::render(BitmapW *bitmapPtr, char *texturePtr, int offsetX, int offsetY, BYTE drawState )
{
	// try to freeze the blob, don't allow it to resize

	c00 = loc_ptr->c00;
	c10 = loc_ptr->c10;
	c01 = loc_ptr->c01;
	c20 = loc_ptr->c20;
	c11 = loc_ptr->c11;
	c02 = loc_ptr->c02;
	c30 = loc_ptr->c30;
	c21 = loc_ptr->c21;
	c12 = loc_ptr->c12;
	c03 = loc_ptr->c03;

	// plot four pixel (2x2) at one time
	// top row A start from (u=0, v=0)
	// bottom row B start from (u=1, v=1)

	// row a
	rowaZ = c00;
	rowaDuZM2 = c10 * 2;
	rowaDuuZM2 = c20 * 4;
	DuuuZM4D3 = c30 * 8;		// common for row a and b
	// for increase rowaZ
	rowaDvZM2 = c01 * 2;
	rowaDvvZM2 = c02 * 4;
	DvvvZM4D3 = c03 * 8;		// common for row a and b
	// for increase rowaDuZM2
	rowaDuvZM4 = c11 * 4;
	DuvvZM4 = c12 * 8;
	// for increase rowaDuuZM2
	DuuvZM4 = c21 * 8;

	// row b
	rowbZ = c00 + c01 + c10 + c20 + c11 + c02 + c30 + c21 + c12 + c03;
	rowbDuZM2 = (c30*3 + c21*2 + c12 + c20*2 + c11 + c10) * 2;
	rowbDuuZM2 = (c30*3 + c21 + c20) * 4;
	// rowxDuuuZM4D3 = c30 * 8;		// common for row a and b
	// for increase rowbZ
	rowbDvZM2 = (c21 + c12*2 + c03*3 + c11 + c02*2 + c01) * 2;
	rowbDvvZM2 = (c12 + c03*3 + c02) * 4;
	// rowxDvvvZM4D3 = c03 * 8;		// common for row a and b
	// for increase rowbDuZM2
	rowbDuvZM4 = (c21*2 + c12*2 + c11) * 4;
	// rowxDuvvZM4 = c12 * 8;
	// for increase rowbDuuZM2
	// rowxDuuvZM4 = c21 * 8;

	err_when(ZOOM_Z_WIDTH != 0 || ZOOM_Z_HEIGHT != -1);

	// set slope_brightness_table
	// set slope_brightness_table
	if( last_draw_state != drawState )
	{
		switch( drawState )
		{
		case FULL_DARK_MASK_ID:
			memcpy( slope_brightness_table, dark_slope_brightness_table, sizeof(slope_brightness_table) );
			break;
		case HALF_BRIGHT_MASK_ID:
			memcpy( slope_brightness_table, fog_slope_brightness_table, sizeof(slope_brightness_table) );
			break;
		case FULL_BRIGHT_MASK_ID:
			memcpy( slope_brightness_table, full_slope_brightness_table, sizeof(slope_brightness_table) );
			break;
		default:
			err_here();	// call render_fog instead of render
		}
		last_draw_state = drawState;
	}

	remapTables = terrain_res.vga_color_table->get_table();

	short *bufPtr = bitmapPtr->get_ptr( offsetX, offsetY );

	const int bufTruePitch = bitmapPtr->get_true_pitch();
	const int negBufTruePitch = -bufTruePitch;
	const int bufTruePitchPlusTwoPixels = bufTruePitch + 2*sizeof(short);

#define MAX_BRIGHTNESS_SHIFT 5
#if(1 << MAX_BRIGHTNESS_SHIFT != MAX_BRIGHTNESS_ADJUST_DEGREE)
	#error
#endif

	if( !(loc_ptr->loc_flag & LOCATE_SEA_COAST) )
	{
		short *tmpBufPtr = bufPtr - 1;
		texturePtr += 4;

		for( vCount = 0; vCount < LOCATE_HEIGHT / 2; ++vCount )
		{
			aZ = rowaZ;
			aDuZM2 = rowaDuZM2;
			aDuuZM2 = rowaDuuZM2;
			int eax = rowaZ;

			for( uCount = 0; uCount < LOCATE_WIDTH / 2; ++uCount )
			{
				int ebx = aDuZM2 >> (C_MULTIPLIER_SHIFT - MAX_BRIGHTNESS_SHIFT + 1);
				ebx = slope_brightness_table[MAX_BRIGHTNESS_ADJUST_DEGREE * 2  + ebx];

				eax = (eax & 0xFFFF0000) | *(reinterpret_cast<unsigned short *>(texturePtr));
				ebx = (ebx & 0xFFFFFF00) | *(reinterpret_cast<unsigned char *>(texturePtr) + 1);
				eax = rotr(eax, 16);
				eax = (eax & 0xFFFF0000) | *(remapTables + ebx);
				eax = rotl(eax, 16);
				ebx = (ebx & 0xFFFFFF00) | *(reinterpret_cast<unsigned char *>(texturePtr));

				int edx = aZ >> C_MULTIPLIER_SHIFT;
				edx *= negBufTruePitch / 2;
				eax = (eax & 0xFFFF0000) | *(remapTables + ebx);
				*(reinterpret_cast<int *>(tmpBufPtr + edx)) = eax;
				edx -= negBufTruePitch / 2;
				*(reinterpret_cast<int *>(tmpBufPtr + edx)) = eax;

				texturePtr += 2;

				aZ += aDuZM2 + aDuuZM2 + DuuuZM4D3;
				eax = aZ;
				aDuZM2 += 2 * aDuuZM2 + 3 * DuuuZM4D3;
				aDuuZM2 += 3 * DuuuZM4D3;

				tmpBufPtr += bufTruePitchPlusTwoPixels / 2;
			}

			bufPtr += bufTruePitch / 2;
			tmpBufPtr = bufPtr - 1;

			bZ = rowbZ;
			bDuZM2 = rowbDuZM2;
			bDuuZM2 = rowbDuuZM2;
			eax = rowbZ;

			for( uCount = 0; uCount < LOCATE_WIDTH / 2; ++uCount )
			{
				int ebx = bDuZM2 >> (C_MULTIPLIER_SHIFT - MAX_BRIGHTNESS_SHIFT + 1);
				ebx = slope_brightness_table[MAX_BRIGHTNESS_ADJUST_DEGREE * 2  + ebx];

				eax = (eax & 0xFFFF0000) | *(reinterpret_cast<unsigned short *>(texturePtr));
				ebx = (ebx & 0xFFFFFF00) | *(reinterpret_cast<unsigned char *>(texturePtr) + 1);
				eax = rotr(eax, 16);
				eax = (eax & 0xFFFF0000) | *(remapTables + ebx);
				eax = rotl(eax, 16);
				ebx = (ebx & 0xFFFFFF00) | *(reinterpret_cast<unsigned char *>(texturePtr));

				int edx = bZ >> C_MULTIPLIER_SHIFT;
				edx *= negBufTruePitch / 2;
				eax = (eax & 0xFFFF0000) | *(remapTables + ebx);
				*(reinterpret_cast<int *>(tmpBufPtr + edx)) = eax;
				edx -= negBufTruePitch / 2;
				*(reinterpret_cast<int *>(tmpBufPtr + edx)) = eax;

				texturePtr += 2;

				bZ += bDuZM2 + bDuuZM2 + DuuuZM4D3;
				eax = bZ;
				bDuZM2 += 2 * bDuuZM2 + 3 * DuuuZM4D3;
				bDuuZM2 += 3 * DuuuZM4D3;

				tmpBufPtr += bufTruePitchPlusTwoPixels / 2;
			}

			rowaZ += rowaDvZM2 + rowaDvvZM2 + DvvvZM4D3;
			rowaDvZM2 += 2 * rowaDvvZM2 + 3 * DvvvZM4D3;
			rowaDvvZM2 += 3 * DvvvZM4D3;
			rowaDuZM2 += rowaDuvZM4 + DuvvZM4;
			rowaDuvZM4 += 2 * DuvvZM4;
			rowaDuuZM2 += DuuvZM4;

			rowbZ += rowbDvZM2 + rowbDvvZM2 + DvvvZM4D3;
			rowbDvZM2 += 2 * rowbDvvZM2 + 3 * DvvvZM4D3;
			rowbDvvZM2 += 3 * DvvvZM4D3;
			rowbDuZM2 += rowbDuvZM4 + DuvvZM4;
			rowbDuvZM4 += 2 * DuvvZM4;
			rowbDuuZM2 += DuuvZM4;

			bufPtr -= 2;
			tmpBufPtr = bufPtr - 1;
		}
	}

#undef MAX_BRIGHTNESS_SHIFT
}



void LocationCorners::render_special(BitmapW *bitmapPtr, int color1, int color2, int offsetX, int offsetY)
{
	// try to freeze the blob, don't allow it to resize

	c00 = loc_ptr->c00;
	c10 = loc_ptr->c10;
	c01 = loc_ptr->c01;
	c20 = loc_ptr->c20;
	c11 = loc_ptr->c11;
	c02 = loc_ptr->c02;
	c30 = loc_ptr->c30;
	c21 = loc_ptr->c21;
	c12 = loc_ptr->c12;
	c03 = loc_ptr->c03;

	// plot four pixel (2x2) at one time
	// top row A start from (u=0, v=0)
	// bottom row B start from (u=1, v=1)

	// row a
	rowaZ = c00;
	rowaDuZM2 = c10 * 2;
	rowaDuuZM2 = c20 * 4;
	DuuuZM4D3 = c30 * 8;		// common for row a and b
	// for increase rowaZ
	rowaDvZM2 = c01 * 2;
	rowaDvvZM2 = c02 * 4;
	DvvvZM4D3 = c03 * 8;		// common for row a and b
	// for increase rowaDuZM2
	rowaDuvZM4 = c11 * 4;
	DuvvZM4 = c12 * 8;
	// for increase rowaDuuZM2
	DuuvZM4 = c21 * 8;

	// row b
	rowbZ = c00 + c01 + c10 + c20 + c11 + c02 + c30 + c21 + c12 + c03;
	rowbDuZM2 = (c30*3 + c21*2 + c12 + c20*2 + c11 + c10) * 2;
	rowbDuuZM2 = (c30*3 + c21 + c20) * 4;
	// long rowxDuuuZM4D3 = c30 * 8;		// common for row a and b
	// for increase rowbZ
	rowbDvZM2 = (c21 + c12*2 + c03*3 + c11 + c02*2 + c01) * 2;
	rowbDvvZM2 = (c12 + c03*3 + c02) * 4;
	// rowxDvvvZM4D3 = c03 * 8;		// common for row a and b
	// for increase rowbDuZM2
	rowbDuvZM4 = (c21*2 + c12*2 + c11) * 4;
	// rowxDuvvZM4 = c12 * 8;
	// for increase rowbDuuZM2
	// rowxDuuvZM4 = c21 * 8;

	err_when(ZOOM_Z_WIDTH != 0 || ZOOM_Z_HEIGHT != -1);

	remapTables = vga.vga_color_table->get_table();

	short *bufPtr = bitmapPtr->get_ptr( offsetX, offsetY );

	const int bufTruePitch = bitmapPtr->get_true_pitch();
	const int negBufTruePitch = -bufTruePitch;
	const int bufTruePitchPlusTwoPixels = bufTruePitch + 2*sizeof(short);

#define MAX_BRIGHTNESS_SHIFT 5
#if(1 << MAX_BRIGHTNESS_SHIFT != MAX_BRIGHTNESS_ADJUST_DEGREE)
	#error
#endif

	seaAltitude = 0;
	if( loc_ptr->loc_flag & LOCATE_SEA_COAST )
	{
		if( top_left->is_sea() )
			seaAltitude = top_left->sea_altitude;
		if( top_right->is_sea() )
			seaAltitude = top_right->sea_altitude;
		if( bottom_left->is_sea() )
			seaAltitude = bottom_left->sea_altitude;
		if( bottom_right->is_sea() )
			seaAltitude = bottom_right->sea_altitude;
	}
// color1 even pixel color, color2 odd pixel color
		unsigned short usColor;
		usColor = vga.translate_color(color1);
		color1 = ((unsigned long)usColor << 16) | usColor;
		usColor = vga.translate_color(color2);
		color2 = ((unsigned long)usColor << 16) | usColor;

        short *buffer = bufPtr/* - 1*/;

        for (int v = 0; v < LOCATE_HEIGHT / 2; ++v) {

          // init inner loop a
          short *buf = buffer;

          int edx;
          aZ = rowaZ;
          aDuZM2 = rowaDuZM2;
          aDuuZM2 = rowaDuuZM2;

          for (int u = 0; u < LOCATE_WIDTH / 2; ++u) {
            // draw row a dot
            edx = aZ >> C_MULTIPLIER_SHIFT;
            if (edx <= seaAltitude)
              edx = seaAltitude;

            edx *= negBufTruePitch;
            buf[edx/2] = color1;
            buf[edx/2 + bufTruePitch/2] = color1;

            aZ += aDuZM2 + aDuuZM2 + c30*8;
            aDuZM2 += 2*aDuuZM2 + 3*c30*8;
            aDuuZM2 += 3*c30*8;

            buf += bufTruePitch/2 /*+ 2*/;
          }  // end U loop

          // init inner loop b
          buffer += bufTruePitch/2;
          uCount = LOCATE_WIDTH / 2;
          buf = buffer;

          bZ = rowbZ;
          bDuZM2 = rowbDuZM2;
          bDuuZM2 = rowbDuuZM2;

          for (int u = 0; u < LOCATE_WIDTH / 2; ++u) {
            // draw row b dot
            edx = bZ >> C_MULTIPLIER_SHIFT;
            if (edx <= seaAltitude)
              edx = seaAltitude;

            edx *= negBufTruePitch;
            buf[edx/2] = color2;
            buf[edx/2 + bufTruePitch/2] = color2;

            bZ += aDuZM2 + aDuuZM2 + c30*8;
            bDuZM2 += 2 * bDuuZM2 + 3 * c30*8;
            bDuuZM2 += 3 * c30*8;

            buf += bufTruePitch/2 /*+ 2*/;
          }  // end U loop

          // inc and compare outer loop
          rowaZ += rowaDvZM2 + rowaDvvZM2 + c03*8;
          rowaDvZM2 += 2 * rowaDvvZM2 + 3 * c03*8;
          rowaDvvZM2 += 3 * c03*8;

          rowaDuZM2 += rowaDuvZM4 + c12*8;
          rowaDuvZM4 += 2 * c12*8;
          rowaDuuZM2 += c21*8;

          rowbZ += rowbDvZM2 + rowbDvvZM2 + c03*8;

          rowbDvZM2 += 2 * rowbDvvZM2 + 3 * c03*8;
          rowbDvvZM2 += 3 * c03*8;
          rowbDuZM2 += rowbDuvZM4 + c12*8;
          rowbDuvZM4 += 2 * c12*8;
          rowbDuuZM2 += c21*8;

          /*buffer -= 2;*/
        } // end V loop
}


