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
#include <SDL/SDL.h>

static int maxDisplayMode = 0;
int defaultMode = 0;

static DisplayModeInfo *display_info_array = NULL;

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
  SDL_Rect **modes = SDL_ListModes(NULL, SDL_FULLSCREEN|SDL_HWSURFACE);
  if (modes == (SDL_Rect**)0) return;
  maxDisplayMode = 0;
  for (int i = 0; modes[i]; ++i) {
    if ((modes[i]->w < 800) || (modes[i]->h < 600)) continue;
    maxDisplayMode++;
  }
  display_info_array = new DisplayModeInfo[maxDisplayMode];
  int idx = 0;
  for (int i = 0; modes[i]; ++i)
  {
    if ((modes[i]->w < 800) || (modes[i]->h < 600)) continue;
    init_display_mode(idx++, modes[i]->w, modes[i]->h, 16);
    if ((modes[i]->w == 800) && (modes[i]->h == 600)) defaultMode = idx;
  }
}

void DisplayModeInfo::init(int modeId)
{
  init_display_modes();

	if( modeId >= 0 && modeId < maxDisplayMode )
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

	if( modeId >= 0 && modeId < maxDisplayMode )
		return display_info_array + modeId;
	else
		return NULL;
}


void DisplayModeInfo::set_current_display_mode(int modeId)
{
  init_display_modes();

	if( modeId >= 0 && modeId < maxDisplayMode )
	{
		current_display_mode = display_info_array[modeId];
	}
	else
	{
		err_here();
	}	
}

void DisplayModeInfo::set_current_display_mode(int w, int h)
{
  init_display_modes();

  for (int i = 0; i < maxDisplayMode; ++i)
    if ((display_info_array[i].screen_width == w) && (display_info_array[i].screen_height == h)) {
      set_current_display_mode(i);
      return;
    }
}



