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

// Platform-specific functionality for 7k2. All the directx/windows requiring code goes here (as wrappers usually).

#define NEED_WINDOWS

#include <platform.h>
#include <all.h>
#include <win32_compat.h>
#include <osys.h>
#include <ovgalock.h>
#include <ovgabuf.h>

void ShowMessageBox (const char *text) {
  OutputDebugString(text);
  if( vga_front.vptr_dd_buf )
  {
    VgaFrontLock vgaLock;

    ShowCursor(TRUE);
    MessageBox(sys.main_hwnd, text, WIN_TITLE, MB_OK | MB_ICONERROR);
    ShowCursor(FALSE);
  }
  else
  {
    ShowCursor(TRUE);
    MessageBox(sys.main_hwnd, text, WIN_TITLE, MB_OK | MB_ICONERROR);
    ShowCursor(FALSE);
  }
}

