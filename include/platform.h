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


// Platform-specific functionality

#ifndef __PLATFORM_H
#define __PLATFORM_H

struct bitmap_file;

void ShowMessageBox (const char *text);
void ShowMouseCursor (bool show);

int ProcessNextEvent();
void WaitNextEvent();

bitmap_file *load_bitmap_file(const char *filename);
int unload_bitmap_file(bitmap_file *bitmap);
void read_bitmap_palette(bitmap_file *bitmap, int idx, int *red, int *green, int *blue);

#endif
