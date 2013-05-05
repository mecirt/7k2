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

// Filename    : OMODEID.H
// Description : DisplayModeInfo


#include <omodeid.h>
#include <all.h>

#define MAX_DISPLAY_MODE_ID 2

static DisplayModeInfo *display_info_array = NULL;

/*
static DisplayModeInfo display_info_array[MAX_DISPLAY_MODE_ID] = 
{
	// 800x600x16
	{
		MODE_ID_800x600x16,						// mode id
		800, 600, 16,							// screen_width, height, bbp
		0, 68-12, 575, 598, 576, 531+12,				// zoom_matrix_x1,y1,x2,y2,width,height
		592, 24, 791, 223, 200, 200,			// map_matrix_x1,y1,x2,y2,width,height
		332, 0, 574, 67-12,						// top_menu_x1,y1,x2,y2
		576, 258, 799, 583,						// info_x1,y1,x2,y2
		716-3, 3, 24, 24,							// map_mode_button_x1, y1, width, height
		588, 6,									// menu_button_x1, y1
		486-30, 24,								// repu_button_x1, y1
		480, 12-2, 105,							// date_x1, y1, length
		378-20, 9+4, 67+100,								// food_x1, y1, length
		376-20, 36+1, 69+100,							// cash_x1, y1, length
		0, 0, 797, 598						// mouse_x1,y1,x2,y2
	},

	// 1024x768x16
	{
		MODE_ID_1024x768x16,					// mode id
		1024,768, 16,							// screen_width, height, bbp
		32, 2, 799, 762, 768, 761,				// zoom_matrix_x1,y1,x2,y2,width,height  {y1 must never 0, 64 must be a factor of y1 - y2}
		816, 193, 1015, 392, 200, 200,			// map_matrix_x1,y1,x2,y2,width,height
	//	810, 120, 1018, 167,					// top_menu_x1,y1,x2,y2
		804-5, 2, 1023, 74,						// top_menu_x1,y1,x2,y2
		800, 426, 1023, 751,					// info_x1,y1,x2,y2
		937, 171, 24, 24,						// map_mode_button_x1, y1, width, height
		901, 20+60,								// menu_button_x1, y1
		814+155, 165-165,								// repu_button_x1, y1
		875, 150-90, 150,							// date_x1, y1, length
		947-120, 122-95, 72,							// food_x1, y1, length
		848+50, 122-95, 56,							// cash_x1, y1, length

		32, 2, 1018, 762							// mouse_x1,y1,x2,y2
	},
};
*/

static void init_display_mode(int id, int width, int height, int bpp)
{
  DisplayModeInfo a;
  a.mode_id = id;
  a.screen_width = width;
  a.screen_height = height;
  a.screen_bpp = bpp;

  // atrix_width must be a multiplier of 64
  a.zoom_matrix_x1 = 0;
  a.zoom_matrix_y1 = 56;
  a.zoom_matrix_x2 = width - 220;
  a.zoom_matrix_x2 -= a.zoom_matrix_x2 % 64;
  a.zoom_matrix_y2 = height - 2;
  a.zoom_matrix_width = a.zoom_matrix_x2 - a.zoom_matrix_x1 + 1;
  a.zoom_matrix_height = a.zoom_matrix_y2 - a.zoom_matrix_y1 + 1;

  a.map_matrix_x1 = width - 208;
  a.map_matrix_y1 = 24;
  a.map_matrix_x2 = width - 9;
  a.map_matrix_y2 = 223;
  a.map_matrix_width = a.map_matrix_x2 - a.map_matrix_x1 + 1;
  a.map_matrix_height = a.map_matrix_y2 - a.map_matrix_y1 + 1;

  a.top_menu_x1 = 332;
  a.top_menu_y1 = 0;
  a.top_menu_x2 = a.top_menu_x1 + 242;
  a.top_menu_y2 = 55;

  a.info_x1 = width - 224;
  a.info_y1 = 258;
  a.info_x2 = width - 1;
  a.info_y2 = height - 17;

  a.map_mode_button_x1 = width - 88;
  a.map_mode_button_y1 = 3;
  a.map_mode_button_width = 24;
  a.map_mode_button_height = 24;

  a.menu_button_x1 = width - 212;
  a.menu_button_y1 = 6;

  a.repu_button_x1 = 456;
  a.repu_button_y1 = 24;
  a.date_x1 = 480;
  a.date_y1 = 10;
  a.date_length = 105;
  a.food_x1 = 358;
  a.food_y1 = 13;
  a.food_length = 167;
  a.cash_x1 = 356;
  a.cash_y1 = 37;
  a.cash_length = 169;

  a.mouse_x1 = 0;
  a.mouse_y1 = 0;
  a.mouse_x2 = width - 3;
  a.mouse_y2 = height - 2;

  display_info_array[id] = a;
}

static void init_display_modes()
{
  if (display_info_array) return;
  display_info_array = new DisplayModeInfo[MAX_DISPLAY_MODE_ID];
  init_display_mode(0, 800, 600, 16);
  init_display_mode(1, 1024, 768, 16);
}

void DisplayModeInfo::init(int modeId)
{
  init_display_modes();

	if( modeId >= 0 && modeId < MAX_DISPLAY_MODE_ID )
	{
		 *this = display_info_array[modeId];
	}
	else
	{
		err_here();
	}	
}

// return NULL for non-existing display mode
DisplayModeInfo *DisplayModeInfo::get_display_info(int modeId)
{
  init_display_modes();

	if( modeId >= 0 && modeId < MAX_DISPLAY_MODE_ID )
		return display_info_array + modeId;
	else
		return NULL;
}


void DisplayModeInfo::set_current_display_mode(int modeId)
{
  init_display_modes();

	if( modeId >= 0 && modeId < MAX_DISPLAY_MODE_ID )
	{
		current_display_mode = display_info_array[modeId];
	}
	else
	{
		err_here();
	}	
}



