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

//Filename    : OVGABUF2.CPP
//Description : OVGABUF direct draw surface class - part 2

#include <all.h>
#include <imgfun.h>
#include <omouse.h>
#include <opower.h>
#include <ocoltbl.h>
#include <ovga.h>
#include <ovgabuf.h>
#include <omodeid.h>
#include <oimgres.h>
#include <ofont.h>
#include <obitmap.h>
#include <obitmapw.h>
#include <olinear.h>

//------- Define static class member vars ---------//

//char VgaBuf::color_light =(char)0x9E;    // color of the light panel side
//char VgaBuf::color_dark  =(char)0x99;    // color of the dark panel side
char VgaBuf::color_light =(char)126;    // color of the light panel side
char VgaBuf::color_dark  =(char)94;    // color of the dark panel side
//char VgaBuf::color_up	 =(char)0x9D;    // color of up_panel surface
//char VgaBuf::color_down  =(char)0x9C;    // color of down_panel surface
char VgaBuf::color_up	 =(char)126;    // color of up_panel surface
char VgaBuf::color_down  =(char)92;    // color of down_panel surface
char VgaBuf::color_push  =(char)0x9B;    // color of pushed button surface
char VgaBuf::color_border=(char)0x98;    // color of the border

// -------- define constant for d3_panel ------//

#define D3_PANEL_L1_X1Y1_R 150
#define D3_PANEL_L1_X1Y1_G 149
#define D3_PANEL_L1_X1Y1_B 70
#define D3_PANEL_L1_X2Y1_R 88
#define D3_PANEL_L1_X2Y1_G 84
#define D3_PANEL_L1_X2Y1_B 33
#define D3_PANEL_L1_X1Y2_R 77
#define D3_PANEL_L1_X1Y2_G 72
#define D3_PANEL_L1_X1Y2_B 38
#define D3_PANEL_L1_X2Y2_R 39
#define D3_PANEL_L1_X2Y2_G 40
#define D3_PANEL_L1_X2Y2_B 16

#define D3_PANEL_L2_X1Y1_R 248
#define D3_PANEL_L2_X1Y1_G 225
#define D3_PANEL_L2_X1Y1_B 105
#define D3_PANEL_L2_X2Y1_R 130
#define D3_PANEL_L2_X2Y1_G 118
#define D3_PANEL_L2_X2Y1_B 54
#define D3_PANEL_L2_X1Y2_R 109
#define D3_PANEL_L2_X1Y2_G 98
#define D3_PANEL_L2_X1Y2_B 44
#define D3_PANEL_L2_X2Y2_R 100
#define D3_PANEL_L2_X2Y2_G 97
#define D3_PANEL_L2_X2Y2_B 40

#define D3_PANEL_L3_COLOR 99
#define D3_PANEL_L4_COLOR 108


//------------- Begin of function VgaBuf::rect --------------//
//
// Draw a rect on VGA screen
//
// Syntax : rect( x1, y1, x2, y2, lineWidth, color )
//
// int x1,y1       - the top left vertex of the rect
// int x2,y2       - the bottom right vertex of the rect
// int lineThick   - thickness of the lines of the rect
// int color       - the color of the rect
//
void VgaBuf::rect(int x1,int y1,int x2,int y2,int lt, int color)
{
	lt--;

	bar(x1,y1,x2,y1+lt,color);
	bar(x1,y1,x1+lt,y2,color);
	bar(x1,y2-lt,x2,y2,color);
	bar(x2-lt,y1,x2,y2,color);
}
//--------------- End of function VgaBuf::rect --------------//


//------------- Start of function VgaBuf::d3_rect --------------//
//
// Draw a d3 rect on VGA screen
//
// Syntax : d3_rect( x1, y1, x2, y2, lineWidth, color )
//
// int x1,y1       - the top left vertex of the d3_rect
// int x2,y2       - the bottom right vertex of the d3_rect

void VgaBuf::d3_rect(int x1,int y1,int x2,int y2)
{
	rect( x1+1, y1+1, x2, y2, 1, V_WHITE );
	rect( x1, y1, x2-1, y2-1, 1, VGA_GRAY+8 );
}
//--------------- End of function VgaBuf::d3_rect --------------//

//------------- Start of function VgaBuf::separator --------------//
//
// Draw a VGA separator line
//
// Syntax : separator( x1, y1, x2, y2 )
//
// int x1,y1       - the top left vertex of the separator
// int x2,y2       - the bottom right vertex of the separator
//
void VgaBuf::separator(int x1, int y1, int x2, int y2)
{
   if( y1+1==y2 )       // horizontal line
   {
      bar( x1, y1, x2, y1, V_WHITE        );
      bar( x1, y2, x2, y2, color_dark );
	}
   else
   {
      bar( x1, y1, x1, y2, V_WHITE        );
      bar( x2, y1, x2, y2, color_dark );
   }
}
//--------------- End of function VgaBuf::separator --------------//


//------------- Start of function VgaBuf::indicator --------------//
//
// <int>   x1, y1, x2, y2 = coordination of the indicator
// <float> curValue       = the value of the bar
// <float> maxValue       = max value, the bar width = maxBarWidth * curValue / maxValue
// <int>   indiColor      = color of the indicator
// [int]   backColor      = background color
//                          (default : vga.color_down)
//			    (-2 if don't paint background color)
//
void VgaBuf::indicator(int x1, int y1, int x2, int y2, float curValue,
		    float maxValue, int indiColor, int backColor)
{
   if( backColor == -1 )
      backColor = color_down;

	int cutPoint;
	if( curValue <= 0.0f )
		cutPoint = x1;
	else if( curValue >= maxValue )
		cutPoint = x2 + 1;
	else
		cutPoint = x1 + int(float(x2 - x1 + 1) * curValue / maxValue);

	if( cutPoint > x1 )
	{
		int cutHeight = (y1 * 5 + y2 * 3) / 8;		// cut at 3/8 of between y1 and y2
		err_when( y2 - y1 - 1 < 4 );

		LinearCounter brightness;
		brightness.init( y1, 0, cutHeight, MAX_BRIGHTNESS_ADJUST_DEGREE-2 );		// glowing
		int y;
		for( y=y1; y <= cutHeight; y++, brightness.inc() )
		{
			barW_fast( x1, y, cutPoint, y, vga.vga_color_table->get_table(brightness.y)[indiColor]);
		}

		brightness.init( cutHeight, MAX_BRIGHTNESS_ADJUST_DEGREE-2, y2, -MAX_BRIGHTNESS_ADJUST_DEGREE/2 );		//
		for( ; y<=y2; ++y, brightness.inc() )
		{
			barW_fast( x1, y, cutPoint, y, vga.vga_color_table->get_table(brightness.y)[indiColor]);
		}
	}

	if( cutPoint <= x2 )
	{
		bar( cutPoint, y1, x2, y2, backColor );
	}
}
//--------------- End of function VgaBuf::indicator --------------//


//---------- Begin of function VgaBuf::line -------------//
//
// Draw a line
//
// <int> x1,y1,x2,y2 = the coordination of the line
// <int> lineColor   = color of the line
//
void VgaBuf::line(int x1,int y1,int x2,int y2,int lineColor)
{
  int	dx = x2 - x1;
  int	dy = y2 - y1;
  short *bufPtr = buf_ptr(x1,y1);
  short color = translate_color(lineColor);
  int	d;
  int	inc_x = dx > 0 ? 1 : -1;
  int	inc_y = dy > 0 ? 1 : -1;
  int	lPitch = dy > 0 ? cur_pitch : -cur_pitch;
  if( dy == 0)
  {
    if( x1 <= x2)
    {
      // from left to right
      for(short x = x1; x <= x2; ++x, ++bufPtr)
        *bufPtr = color;
    }
    else
    {
      // from right to left
      for( short x = x1; x >= x2; --x, --bufPtr)
        *bufPtr = color;
    }
    return;
  }

  if( dx == 0)
  {
    if( y1 <= y2)
    {
      // from top to bottom
      for(short y = y1; y <= y2; ++y, bufPtr += lPitch)
        *bufPtr = color;
    }
    else
    {
      // from bottom to top
      for( short y = y1; y >= y2; --y, bufPtr -= lPitch)
        *bufPtr = color;
    }
    return;
  }

  if( abs(dy) <= abs(dx) )
  {
    // draw gentle line
    // use x as independent variable
    dx = abs(dx);
    dy = abs(dy);

    d = 2 * dy - dx;	
    int x = x1-inc_x;
    do
    {
      x += inc_x;
      *bufPtr = translate_color(lineColor);
      bufPtr += inc_x;
      if( d >= 0)
      {
        // y increase by 1
        bufPtr += lPitch;
        d += 2 * (dy - dx);
      }
      else
      {
        // y remain unchange;
        d += 2 * dy;
      }
    } while ( x != x2);
  }
  else
  {
    // draw steep line
    // use y as independent variable
    dx = abs(dx);
    dy = abs(dy);

    d = 2 * dx - dy;
    int y = y1 - inc_y;
    do
    {
      y += inc_y;
      *bufPtr = translate_color(lineColor);
      bufPtr += lPitch;
      if( d >= 0)
      {
        // x increase by 1
        bufPtr += inc_x;
        d += 2 * (dx - dy);
      }
      else
      {
        // x remain unchange;
        d += 2 * dx;
      }
    } while ( y != y2);
  }
}
//------------ End of function VgaBuf::line -------------//


//----------- Begin of function VgaBuf::d3_panel_up ------------//
//
// Draw a Ms windows style 3D panel
//
// thickness can be 1-4
//
// <int> x1,y1,x2,y2 = the four vertex of the panel
// [int] thick       = thickness of the border
//                     (default:2)
// [int] paintCentre = paint the center area of the plane or not
//                     pass 0 if the area has just been painted with bar()
//                     (default:1)
//
void VgaBuf::d3_panel_up(int x1,int y1,int x2,int y2,int t,int paintCentre)
{
	// int i,x,y;

	err_when( x1>x2 || y1>y2 || x1<0 || y1<0 || x2>=VGA_WIDTH || y2>=VGA_HEIGHT );

	//------------------------------------------------//

	// ####### begin Gilbert 17/10 ##########//
	/*
	if( paintCentre )
		bar_fast(x1+2, y1+2, x2-3, y2-3, color_up );  // center

	//--------- white border on top and left sides -----------//

	bar_fast(x1,y1,x2-3,y1+1,0x9a );
	draw_pixel(x2-2, y1, 0x9a);
	bar_fast(x1,y1+2,x1+1,y2-3, 0x9a );    // left side
	draw_pixel(x1, y2-2, 0x9a);

	//--------- black border on bottom and right sides -----------//

	bar_fast(x2-2,y1+2,x2-1,y2-1, 0x90 );     // bottom side
	draw_pixel(x2-1, y1+1, 0x90);
	bar_fast(x1+2,y2-2,x2-3,y2-1, 0x90 );		 // right side
	draw_pixel(x1+1, y2-1, 0x90);

	//--------- junction between white and black border --------//
	draw_pixel(x2-1, y1, 0x97);
	draw_pixel(x2-2, y1+1, 0x97);
	draw_pixel(x1, y2-1, 0x97);
	draw_pixel(x1+1, y2-2, 0x97);

	//--------- gray shadow on bottom and right sides -----------//
	bar_fast(x2, y1+1, x2, y2, 0x97);
	bar_fast(x1+1, y2, x2-1, y2, 0x97);

	//-------------------------------------------//
	*/

	LinearCounter redCount, greenCount, blueCount;

	if( paintCentre )
	{
		bar_fast(x1+4, y1+4, x2-4, y2-4, color_up );  // center
	}

	short *pixelPtr;
	int pitch = buf_pitch();
	// outer layer, top border

	redCount.init(   x1, D3_PANEL_L1_X1Y1_R, x2, D3_PANEL_L1_X2Y1_R );
	greenCount.init( x1, D3_PANEL_L1_X1Y1_G, x2, D3_PANEL_L1_X2Y1_G );
	blueCount.init(  x1, D3_PANEL_L1_X1Y1_B, x2, D3_PANEL_L1_X2Y1_B );
	pixelPtr = buf_ptr( x1, y1 );
	for( ; !redCount.is_end(); redCount.inc(), greenCount.inc(), blueCount.inc(), ++pixelPtr )
	{
		*pixelPtr = vga.make_pixel(redCount.y, greenCount.y, blueCount.y);
	}

	// outer layer, bottom border
	redCount.init(   x1, D3_PANEL_L1_X1Y2_R, x2, D3_PANEL_L1_X2Y2_R );
	greenCount.init( x1, D3_PANEL_L1_X1Y2_G, x2, D3_PANEL_L1_X2Y2_G );
	blueCount.init(  x1, D3_PANEL_L1_X1Y2_B, x2, D3_PANEL_L1_X2Y2_B );
	pixelPtr = buf_ptr( x1, y2 );
	for( ; !redCount.is_end(); redCount.inc(), greenCount.inc(), blueCount.inc(), ++pixelPtr )
	{
		*pixelPtr = vga.make_pixel(redCount.y, greenCount.y, blueCount.y);
	}

	//outer layer left border
	redCount.init(   y1, D3_PANEL_L1_X1Y1_R, y2, D3_PANEL_L1_X1Y2_R );
	greenCount.init( y1, D3_PANEL_L1_X1Y1_G, y2, D3_PANEL_L1_X1Y2_G );
	blueCount.init(  y1, D3_PANEL_L1_X1Y1_B, y2, D3_PANEL_L1_X1Y2_B );
	pixelPtr = buf_ptr( x1, y1 );
	for( ; !redCount.is_end(); redCount.inc(), greenCount.inc(), blueCount.inc(), pixelPtr += pitch )
	{
		*pixelPtr = vga.make_pixel(redCount.y, greenCount.y, blueCount.y);
	}

	//outer layer right border
	redCount.init(   y1, D3_PANEL_L1_X2Y1_R, y2, D3_PANEL_L1_X2Y2_R );
	greenCount.init( y1, D3_PANEL_L1_X2Y1_G, y2, D3_PANEL_L1_X2Y2_G );
	blueCount.init(  y1, D3_PANEL_L1_X2Y1_B, y2, D3_PANEL_L1_X2Y2_B );
	pixelPtr = buf_ptr( x2, y1 );
	for( ; !redCount.is_end(); redCount.inc(), greenCount.inc(), blueCount.inc(), pixelPtr += pitch )
	{
		*pixelPtr = vga.make_pixel(redCount.y, greenCount.y, blueCount.y);
	}

	// inner layer, top border

	redCount.init(   x1+1, D3_PANEL_L2_X1Y1_R, x2-1, D3_PANEL_L2_X2Y1_R );
	greenCount.init( x1+1, D3_PANEL_L2_X1Y1_G, x2-1, D3_PANEL_L2_X2Y1_G );
	blueCount.init(  x1+1, D3_PANEL_L2_X1Y1_B, x2-1, D3_PANEL_L2_X2Y1_B );
	pixelPtr = buf_ptr( x1+1, y1+1 );
	for( ; !redCount.is_end(); redCount.inc(), greenCount.inc(), blueCount.inc(), ++pixelPtr )
	{
		*pixelPtr = vga.make_pixel(redCount.y, greenCount.y, blueCount.y);
	}

	// inner layer, bottom border
	redCount.init(   x1+1, D3_PANEL_L2_X1Y2_R, x2-1, D3_PANEL_L2_X2Y2_R );
	greenCount.init( x1+1, D3_PANEL_L2_X1Y2_G, x2-1, D3_PANEL_L2_X2Y2_G );
	blueCount.init(  x1+1, D3_PANEL_L2_X1Y2_B, x2-1, D3_PANEL_L2_X2Y2_B );
	pixelPtr = buf_ptr( x1+1, y2-1 );
	for( ; !redCount.is_end(); redCount.inc(), greenCount.inc(), blueCount.inc(), ++pixelPtr )
	{
		*pixelPtr = vga.make_pixel(redCount.y, greenCount.y, blueCount.y);
	}

	//inner layer left border
	redCount.init(   y1+1, D3_PANEL_L2_X1Y1_R, y2-1, D3_PANEL_L2_X1Y2_R );
	greenCount.init( y1+1, D3_PANEL_L2_X1Y1_G, y2-1, D3_PANEL_L2_X1Y2_G );
	blueCount.init(  y1+1, D3_PANEL_L2_X1Y1_B, y2-1, D3_PANEL_L2_X1Y2_B );
	pixelPtr = buf_ptr( x1+1, y1+1 );
	for( ; !redCount.is_end(); redCount.inc(), greenCount.inc(), blueCount.inc(), pixelPtr += pitch )
	{
		*pixelPtr = vga.make_pixel(redCount.y, greenCount.y, blueCount.y);
	}

	//inner layer right border
	redCount.init(   y1+1, D3_PANEL_L2_X2Y1_R, y2-1, D3_PANEL_L2_X2Y2_R );
	greenCount.init( y1+1, D3_PANEL_L2_X2Y1_G, y2-1, D3_PANEL_L2_X2Y2_G );
	blueCount.init(  y1+1, D3_PANEL_L2_X2Y1_B, y2-1, D3_PANEL_L2_X2Y2_B );
	pixelPtr = buf_ptr( x2-1, y1+1 );
	for( ; !redCount.is_end(); redCount.inc(), greenCount.inc(), blueCount.inc(), pixelPtr += pitch )
	{
		*pixelPtr = vga.make_pixel(redCount.y, greenCount.y, blueCount.y);
	}

	// layer 3
	if( x2-x1 >= 4 && y2-y1 >= 4 )
	{
		bar_fast( x1+2, y1+2, x2-2, y1+2, D3_PANEL_L3_COLOR );	// top
		bar_fast( x1+2, y2-2, x2-2, y2-2, D3_PANEL_L3_COLOR );	// bottom
		bar_fast( x1+2, y1+2, x1+2, y2-2, D3_PANEL_L3_COLOR );	// left
		bar_fast( x2-2, y1+2, x2-2, y2-2, D3_PANEL_L3_COLOR );	// right
	}

	// layer 4
	if( x2-x1 >= 6 && y2-y1 >= 6 )
	{
		bar_fast( x1+3, y1+3, x2-3, y1+3, D3_PANEL_L4_COLOR );	// top
		bar_fast( x1+3, y2-3, x2-3, y2-3, D3_PANEL_L4_COLOR );	// bottom
		bar_fast( x1+3, y1+3, x1+3, y2-3, D3_PANEL_L4_COLOR );	// left
		bar_fast( x2-3, y1+3, x2-3, y2-3, D3_PANEL_L4_COLOR );	// right
	}

	// ####### begin Gilbert 17/10 ##########//
}
//------------- End of function VgaBuf::d3_panel_up ------------//


//----------- Begin of function VgaBuf::d3_panel_down ------------//
//
// Draw a Ms windows style 3D panel with panel pushed downwards
//
// <int> x1,y1,x2,y2 = the four vertex of the panel
// [int] thick       = thickness of the border
//                     (default:2)
// [int] paintCentre = paint the center area of the plane or not
//                     pass 0 if the area has just been painted with bar()
//                     (default:1)
//
void VgaBuf::d3_panel_down(int x1,int y1,int x2,int y2,int t,int paintCentre)
{
	err_when( x1>x2 || y1>y2 || x1<0 || y1<0 || x2>=VGA_WIDTH || y2>=VGA_HEIGHT );

	//---------- main center area -----------//

	// ####### begin Gilbert 17/10 ##########//
	/*
	if( paintCentre )
		bar_fast(x1+2, y1+2, x2-3, y2-3, color_down );  // center

	//--------- black border on top and left sides -----------//

	bar_fast(x1,y1,x2-3,y1+1,0x90 );
	draw_pixel(x2-2, y1, 0x90);
	bar_fast(x1,y1+2,x1+1,y2-3, 0x90 );    // left side
	draw_pixel(x1, y2-2, 0x90);

	//--------- while border on bottom and right sides -----------//

	bar_fast(x2-2,y1+2,x2-1,y2-1, 0x9a );     // bottom side
	draw_pixel(x2-1, y1+1, 0x9a);
	bar_fast(x1+2,y2-2,x2-3,y2-1, 0x9a );		 // right side
	draw_pixel(x1+1, y2-1, 0x9a);

	//--------- junction between white and black border --------//
	draw_pixel(x2-1, y1, 0x97);
	draw_pixel(x2-2, y1+1, 0x97);
	draw_pixel(x1, y2-1, 0x97);
	draw_pixel(x1+1, y2-2, 0x97);

	//--------- remove shadow, copy from back  -----------//
	bar_fast(x2, y1+1, x2, y2, 0x9c);
	bar_fast(x1+1, y2, x2-1, y2, 0x9c);
*/

	LinearCounter redCount, greenCount, blueCount;

	if( paintCentre )
	{
		bar_fast(x1+5, y1+5, x2-3, y2-3, color_down );  // center
	}

	short *pixelPtr;
	int pitch = buf_pitch();
	// outer layer, top border

	redCount.init(   x1, D3_PANEL_L1_X1Y1_R, x2, D3_PANEL_L1_X2Y1_R );
	greenCount.init( x1, D3_PANEL_L1_X1Y1_G, x2, D3_PANEL_L1_X2Y1_G );
	blueCount.init(  x1, D3_PANEL_L1_X1Y1_B, x2, D3_PANEL_L1_X2Y1_B );
	pixelPtr = buf_ptr( x1, y1 );
	for( ; !redCount.is_end(); redCount.inc(), greenCount.inc(), blueCount.inc(), ++pixelPtr )
	{
		*pixelPtr = vga.make_pixel(redCount.y, greenCount.y, blueCount.y);
	}

	// outer layer, bottom border
	redCount.init(   x1, D3_PANEL_L1_X1Y2_R, x2, D3_PANEL_L1_X2Y2_R );
	greenCount.init( x1, D3_PANEL_L1_X1Y2_G, x2, D3_PANEL_L1_X2Y2_G );
	blueCount.init(  x1, D3_PANEL_L1_X1Y2_B, x2, D3_PANEL_L1_X2Y2_B );
	pixelPtr = buf_ptr( x1, y2 );
	for( ; !redCount.is_end(); redCount.inc(), greenCount.inc(), blueCount.inc(), ++pixelPtr )
	{
		*pixelPtr = vga.make_pixel(redCount.y, greenCount.y, blueCount.y);
	}

	//outer layer left border
	redCount.init(   y1, D3_PANEL_L1_X1Y1_R, y2, D3_PANEL_L1_X1Y2_R );
	greenCount.init( y1, D3_PANEL_L1_X1Y1_G, y2, D3_PANEL_L1_X1Y2_G );
	blueCount.init(  y1, D3_PANEL_L1_X1Y1_B, y2, D3_PANEL_L1_X1Y2_B );
	pixelPtr = buf_ptr( x1, y1 );
	for( ; !redCount.is_end(); redCount.inc(), greenCount.inc(), blueCount.inc(), pixelPtr += pitch )
	{
		*pixelPtr = vga.make_pixel(redCount.y, greenCount.y, blueCount.y);
	}

	//outer layer right border
	redCount.init(   y1, D3_PANEL_L1_X2Y1_R, y2, D3_PANEL_L1_X2Y2_R );
	greenCount.init( y1, D3_PANEL_L1_X2Y1_G, y2, D3_PANEL_L1_X2Y2_G );
	blueCount.init(  y1, D3_PANEL_L1_X2Y1_B, y2, D3_PANEL_L1_X2Y2_B );
	pixelPtr = buf_ptr( x2, y1 );
	for( ; !redCount.is_end(); redCount.inc(), greenCount.inc(), blueCount.inc(), pixelPtr += pitch )
	{
		*pixelPtr = vga.make_pixel(redCount.y, greenCount.y, blueCount.y);
	}

	// inner layer, top border

	redCount.init(   x1+1, D3_PANEL_L2_X1Y1_R, x2-1, D3_PANEL_L2_X2Y1_R );
	greenCount.init( x1+1, D3_PANEL_L2_X1Y1_G, x2-1, D3_PANEL_L2_X2Y1_G );
	blueCount.init(  x1+1, D3_PANEL_L2_X1Y1_B, x2-1, D3_PANEL_L2_X2Y1_B );
	pixelPtr = buf_ptr( x1+1, y1+1 );
	for( ; !redCount.is_end(); redCount.inc(), greenCount.inc(), blueCount.inc(), ++pixelPtr )
	{
		*pixelPtr = vga.make_pixel(redCount.y, greenCount.y, blueCount.y);
	}

	// inner layer, bottom border
	redCount.init(   x1+1, D3_PANEL_L2_X1Y2_R, x2-1, D3_PANEL_L2_X2Y2_R );
	greenCount.init( x1+1, D3_PANEL_L2_X1Y2_G, x2-1, D3_PANEL_L2_X2Y2_G );
	blueCount.init(  x1+1, D3_PANEL_L2_X1Y2_B, x2-1, D3_PANEL_L2_X2Y2_B );
	pixelPtr = buf_ptr( x1+1, y2-1 );
	for( ; !redCount.is_end(); redCount.inc(), greenCount.inc(), blueCount.inc(), ++pixelPtr )
	{
		*pixelPtr = vga.make_pixel(redCount.y, greenCount.y, blueCount.y);
	}

	//inner layer left border
	redCount.init(   y1+1, D3_PANEL_L2_X1Y1_R, y2-1, D3_PANEL_L2_X1Y2_R );
	greenCount.init( y1+1, D3_PANEL_L2_X1Y1_G, y2-1, D3_PANEL_L2_X1Y2_G );
	blueCount.init(  y1+1, D3_PANEL_L2_X1Y1_B, y2-1, D3_PANEL_L2_X1Y2_B );
	pixelPtr = buf_ptr( x1+1, y1+1 );
	for( ; !redCount.is_end(); redCount.inc(), greenCount.inc(), blueCount.inc(), pixelPtr += pitch )
	{
		*pixelPtr = vga.make_pixel(redCount.y, greenCount.y, blueCount.y);
	}

	//inner layer right border
	redCount.init(   y1+1, D3_PANEL_L2_X2Y1_R, y2-1, D3_PANEL_L2_X2Y2_R );
	greenCount.init( y1+1, D3_PANEL_L2_X2Y1_G, y2-1, D3_PANEL_L2_X2Y2_G );
	blueCount.init(  y1+1, D3_PANEL_L2_X2Y1_B, y2-1, D3_PANEL_L2_X2Y2_B );
	pixelPtr = buf_ptr( x2-1, y1+1 );
	for( ; !redCount.is_end(); redCount.inc(), greenCount.inc(), blueCount.inc(), pixelPtr += pitch )
	{
		*pixelPtr = vga.make_pixel(redCount.y, greenCount.y, blueCount.y);
	}

	// layer 3
	if( x2-x1 >= 4 && y2-y1 >= 4 )
	{
		bar_fast( x1+2, y1+2, x2-2, y1+3, D3_PANEL_L3_COLOR );	// top, width 2
		bar_fast( x1+2, y1+2, x1+3, y2-2, D3_PANEL_L3_COLOR );	// left, width 2
	}

	// layer 4
	if( x2-x1 >= 6 && y2-y1 >= 6 )
	{
		bar_fast( x1+4, y1+4, x2-2, y1+4, D3_PANEL_L4_COLOR );	// top
		bar_fast( x1+4, y2-2, x2-2, y2-2, D3_PANEL_L4_COLOR );	// bottom
		bar_fast( x1+4, y1+4, x1+4, y2-2, D3_PANEL_L4_COLOR );	// left
		bar_fast( x2-2, y1+4, x2-2, y2-2, D3_PANEL_L4_COLOR );	// right
	}

	// ####### end Gilbert 17/10 ##########//

	//----------- show mouse ----------//
}
//------------- End of function VgaBuf::d3_panel_down ------------//


//----------- Begin of function VgaBuf::d3_panel_up_clear ------------//
//
// clear the center of the either up or down panel
//
// <int> x1,y1,x2,y2 = the four vertex of the panel
// [int] thick       = thickness of the border
//                     (default:2)
//
void VgaBuf::d3_panel_up_clear(int x1,int y1,int x2,int y2,int t)
{
	// ####### begin Gilbert 19/10 #########//
	bar(x1+4, y1+4, x2-4, y2-4, color_up );
	// ####### end Gilbert 19/10 #########//
}
//------------- End of function VgaBuf::d3_panel_up_clear ------------//


//----------- Begin of function VgaBuf::d3_panel_down_clear ------------//
//
// clear the center of the either up or down panel
//
// <int> x1,y1,x2,y2 = the four vertex of the panel
// [int] thick       = thickness of the border
//                     (default:2)
//
void VgaBuf::d3_panel_down_clear(int x1,int y1,int x2,int y2,int t)
{
	// ####### begin Gilbert 19/10 #########//
	bar(x1+5, y1+5, x2-3, y2-3, color_down );
	// ####### end Gilbert 19/10 #########//
}
//------------- End of function VgaBuf::d3_panel_down_clear ------------//


//----------- Begin of function VgaBuf::draw_d3_up_border ------------//
//
// Draw interface border.
//
void VgaBuf::draw_d3_up_border(int x1,int y1,int x2,int y2)
{
	d3_panel_up( x1, y1, x2, y2, 4, 0 );
}
//------------- End of function VgaBuf::draw_d3_up_border ------------//


//----------- Begin of function VgaBuf::draw_d3_down_border ------------//
//
// Draw interface border.
//
void VgaBuf::draw_d3_down_border(int x1,int y1,int x2,int y2)
{
	d3_panel_down( x1, y1, x2, y2, 4, 0 );
}
//------------- End of function VgaBuf::draw_d3_down_border ------------//


