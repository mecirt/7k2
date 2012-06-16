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

#include <win32_compat.h>
class DsVolume;
class VgaBuf;

struct bitmap_file;

void ShowMessageBox (const char *text);
void ShowMouseCursor (bool show);

int ProcessNextEvent();
void WaitNextEvent();

bitmap_file *load_bitmap_file(const char *filename);
int unload_bitmap_file(bitmap_file *bitmap);
void read_bitmap_palette(bitmap_file *bitmap, int idx, int *red, int *green, int *blue);

bool CreateMainWindow();
void CloseMainWindow();
void FocusMainWindow();
void InvalidateMainWindow();
void ShowMainWindow();

// temporary
void *get_main_hwnd();

bool InitGraphics ();
bool SetDisplayMode (int w, int h);
void DeinitGraphics ();

// Init the surface. w/h/flag only valid for the back one.
bool InitSurface (VgaBuf *buf, bool back, DWORD w = 0, DWORD h = 0, int videoMemoryFlag = 0);
void DeinitSurface (VgaBuf *buf);
bool AttachSurface (VgaBuf *to, VgaBuf *surface);
bool DetachSurface (VgaBuf *to, VgaBuf *surface);
bool LockBuffer (VgaBuf *buf);
bool UnlockBuffer (VgaBuf *buf);
void FlipBuffer (VgaBuf *buf);
bool IsBufferLost (VgaBuf *buf);
bool RestoreBuffer (VgaBuf *buf);
void RestoreBufferPointers (VgaBuf *buf);
int BufferSize (const VgaBuf *buf);
int BufferWidth (const VgaBuf *buf);
int BufferHeight (const VgaBuf *buf);

void BltFast (VgaBuf *targetBuffer, VgaBuf *sourceBuffer, int x1, int y1, int x2, int y2, int mode);

int WriteBitmapFile (const VgaBuf *buf, const char *name);

// mouse
void InitInputDevices (void *hinstance);
void UninitInputDevices ();
int PollInputDevices ();
void UpdateSkeyState ();
int IsKey(unsigned scanCode, unsigned short skeyState, unsigned short charValue, unsigned flags);

void GetMousePos (int *x, int *y);
void SetMousePos (int x, int y);

// sound

struct IDirectSoundBuffer;
typedef struct IDirectSoundBuffer *LPDIRECTSOUNDBUFFER;

bool init_sound();
void deinit_sound();
LPDIRECTSOUNDBUFFER create_sound_buffer(const char *wav_buf, unsigned int wavDataLength);
void release_buffer (LPDIRECTSOUNDBUFFER *buffer);
bool set_volume (LPDIRECTSOUNDBUFFER buffer, int volume);
bool set_volume (LPDIRECTSOUNDBUFFER buffer, DsVolume volume);
int get_volume (LPDIRECTSOUNDBUFFER buffer);
int get_pan (LPDIRECTSOUNDBUFFER buffer);
bool fill_buffer (LPDIRECTSOUNDBUFFER buffer, const char *wav_buf, int wav_len, int start = 0);
bool play_buffer (LPDIRECTSOUNDBUFFER buffer, bool loop = false);

bool buffer_playing (LPDIRECTSOUNDBUFFER buffer);

#endif
