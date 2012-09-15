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

// Platform-specific functionality for 7k2. This is the SDL version.

#include <win32_compat.h>

#include <SDL/SDL.h>

#include <platform.h>
#include <all.h>
#include <osys.h>
#include <ovga.h>
#include <ovgalock.h>
#include <ovgabuf.h>
#include <resource.h>
#include <ocoltbl.h>
#include <olog.h>
#include <omodeid.h>
#include <omouse.h>
#include <omousecr.h>
#include <opower.h>
#include <ovolume.h>
#include <key.h>

#include <unistd.h>
#include <fcntl.h>

void ShowMessageBox (const char *text) {
  puts(text);
}

void ShowMouseCursor (bool show)
{
  SDL_ShowCursor (show);
//  SDL_WM_GrabInput(show ? SDL_GRAB_OFF : SDL_GRAB_ON);
}

// Bitmap files. We only need this once, and use the palette from here, nothing else.
// So everything else is silently discarded.

struct PALETTEENTRY {
  unsigned char peRed, peGreen, peBlue, peFlags;
};

struct bitmap_file {
  PALETTEENTRY palette[256];
};


bitmap_file *load_bitmap_file(const char *filename)
{
  // Alright, so. First there are 54 bytes of header. We skip that. Then there's 256 4-byte palette entries.
  String str(DIR_IMAGE);
  str += filename;
  int f = open(str, O_RDONLY);
  if (!f) return 0;

  bitmap_file *bitmap = new bitmap_file;
  lseek (f, 54, SEEK_SET);
  read (f, &bitmap->palette, 256*sizeof(PALETTEENTRY));

  // now set all the flags in the palette correctly and fix the reverse BGR
  for (int index=0; index<256; index++)
  {
    int temp_color = bitmap->palette[index].peRed;
    bitmap->palette[index].peRed  = bitmap->palette[index].peBlue;
    bitmap->palette[index].peBlue = temp_color;
  }

  close(f);
  return bitmap;
}

///////////////////////////////////////////////////////////////////////////////

int unload_bitmap_file(bitmap_file *bitmap)
{
  delete bitmap;
  return 1;

} // end Unload_Bitmap_File

///////////////////////////////////////////////////////////////////////////////

void read_bitmap_palette(bitmap_file *bitmap, int idx, int *red, int *green, int *blue)
{
  *red = bitmap->palette[idx].peRed;
  *green = bitmap->palette[idx].peGreen;
  *blue = bitmap->palette[idx].peBlue;
}

int WriteBitmapFile (const VgaBuf *buf, const char *fileName)
{
  // TODO: implement this. Only used for screenshots
  return 1;
}

// ****** Window and SDL initialisation ******

bool CreateMainWindow()
{
  return true;
}

void CloseMainWindow()
{
}

void FocusMainWindow()
{
}

void ShowMainWindow()
{
}

void InvalidateMainWindow()
{
}

SDL_Surface *screen;
SDL_Event event;
static bool waitingEvent;

bool InitGraphics ()
{
  SDL_Init (SDL_INIT_EVERYTHING);
  screen = NULL;
  waitingEvent = false;
  return true;
}

bool SetDisplayMode (int w, int h)
{
  screen = SDL_SetVideoMode (w, h, 16, SDL_HWSURFACE | SDL_DOUBLEBUF /* | SDL_FULLSCREEN */);
  if (!screen) return false;
  SDL_WM_SetCaption ("Seven Kingdoms 2", NULL);
  sys.active_flag = 1;

  SDL_PixelFormat *pf = screen->format;

  vga.pixel_format_flag = -1;

      if(pf->Rmask == 0x001fL
          && pf->Gmask == 0x001fL << 5
          && pf->Bmask == 0x001fL << 10 )
      {
        vga.pixel_format_flag = PIXFORM_RGB_555;
      }
      else if( pf->Rmask == 0x001fL
          && pf->Gmask == 0x003fL << 5
          && pf->Bmask == 0x001fL << 11 )
      {
        vga.pixel_format_flag = PIXFORM_RGB_565;
      }
      else if( pf->Bmask == 0x001fL
          && pf->Gmask == 0x001fL << 5
          && pf->Rmask == 0x001fL << 10 )
      {
        vga.pixel_format_flag = PIXFORM_BGR_555;
      }
      else if( pf->Bmask == 0x001fL
          && pf->Gmask == 0x003fL << 5
          && pf->Rmask == 0x001fL << 11 )
      {
        vga.pixel_format_flag = PIXFORM_BGR_565;
      }

  if( vga.pixel_format_flag == -1 )
  {
    puts("Cannot determine the pixel format of this display mode.");
    vga.pixel_format_flag = PIXFORM_BGR_565;
  }

  return TRUE;
}

void DeinitGraphics ()
{
  SDL_Quit();
}

// ****** End of Window and DirectX initialisation ******


// Surfaces

bool InitSurface (VgaBuf *buf, DWORD w, DWORD h)
{
  buf->vptr_dd_buf = screen;
  return true;
}

void DeinitSurface (VgaBuf *buf)
{
  if (buf->vptr_dd_buf)
    SDL_FreeSurface ((SDL_Surface *) buf->vptr_dd_buf);
  buf->vptr_dd_buf = NULL;
}

bool LockBuffer (VgaBuf *buf)
{
  SDL_Surface *s = (SDL_Surface *) buf->vptr_dd_buf;
  if (SDL_LockSurface (s) != 0) return false;

  RestoreBufferPointers (buf);

  return true;
}

bool UnlockBuffer (VgaBuf *buf)
{
  SDL_UnlockSurface ((SDL_Surface *) buf->vptr_dd_buf);
  return true;
}

void FlipBuffer (VgaBuf *buf)
{
  SDL_Flip (screen);
  usleep(20 * 1000);
}

bool IsBufferLost (VgaBuf *buf)
{
  return false;
}

bool RestoreBuffer (VgaBuf *buf)
{
  return true;
}

void RestoreBufferPointers (VgaBuf *buf)
{
  SDL_Surface *s = (SDL_Surface *) buf->vptr_dd_buf;
  buf->cur_buf_ptr = (short *) s->pixels;
  buf->cur_pitch = s->pitch;
}

int BufferSize (const VgaBuf *buf)
{
  SDL_Surface *s = (SDL_Surface *) buf->vptr_dd_buf;
  return s->w * s->h * sizeof(short);
}

int BufferWidth (const VgaBuf *buf)
{
  SDL_Surface *s = (SDL_Surface *) buf->vptr_dd_buf;
  return s->w;
}

int BufferHeight (const VgaBuf *buf)
{
  SDL_Surface *s = (SDL_Surface *) buf->vptr_dd_buf;
  return s->h;
}

int MakePixel (unsigned char r, unsigned char g, unsigned char b)
{
  return SDL_MapRGB(screen->format, r, g, b);
}

void DecodePixel (int pixel, unsigned char *r, unsigned char *g, unsigned char *b)
{
  SDL_GetRGB(pixel, screen->format, r, g, b);
}

// mouse

void InitInputDevices (void *hinstance)
{
}

void UninitInputDevices ()
{
}

void UpdateSkeyState ()
{
  SDLMod mod = SDL_GetModState();

  mouse.skey_state = 0;
  if (mod & KMOD_LSHIFT) mouse.skey_state |= LEFT_SHIFT_KEY_MASK;
  if (mod & KMOD_RSHIFT) mouse.skey_state |= RIGHT_SHIFT_KEY_MASK;
  if (mod & KMOD_LCTRL) mouse.skey_state |= LEFT_CONTROL_KEY_MASK;
  if (mod & KMOD_RCTRL) mouse.skey_state |= RIGHT_CONTROL_KEY_MASK;
  if (mod & KMOD_LALT) mouse.skey_state |= LEFT_ALT_KEY_MASK;
  if (mod & KMOD_RALT) mouse.skey_state |= RIGHT_ALT_KEY_MASK;
  if (mod & KMOD_NUM) mouse.skey_state |= NUM_LOCK_STATE_MASK;
  if (mod & KMOD_CAPS) mouse.skey_state |= CAP_LOCK_STATE_MASK;
}

void process_key_message(SDL_KeyboardEvent ev)
{
  // check modifiers - we can not use ev.mod here, as the changes have not yet been added to there
  UpdateSkeyState ();

  bool pressed = (ev.state == SDL_PRESSED);

  if (pressed)
  {
    mouse.add_key_event(ev.keysym.sym, m.get_time());

    // capture screen
    if (ev.keysym.sym == SDLK_F11 && (mouse.skey_state & CONTROL_KEY_MASK))
      sys.capture_screen();
  }
  else
    mouse.add_key_release_event(ev.keysym.sym, m.get_time());

  // -------- update mouse.arrow_key_state ---------//
  int mask = 0;
  if (ev.keysym.sym == SDLK_LEFT) mask = ARROW_LEFT_KEY_MASK;
  if (ev.keysym.sym == SDLK_RIGHT) mask = ARROW_RIGHT_KEY_MASK;
  if (ev.keysym.sym == SDLK_UP) mask = ARROW_UP_KEY_MASK;
  if (ev.keysym.sym == SDLK_DOWN) mask = ARROW_DOWN_KEY_MASK;
  if (mask) {
    if (pressed) mouse.arrow_key_state |= mask;
    else mouse.arrow_key_state &= ~mask;
  }
}

// see Mouse::is_key
int IsKey(unsigned scanCode, unsigned short skeyState, unsigned short charValue, unsigned flags)
{
  unsigned short priChar = 0, shiftChar = 0, capitalChar = 0;
  unsigned onNumPad = 0;

  switch(scanCode)
  {
    case SDLK_ESCAPE: priChar = shiftChar = capitalChar = KEY_ESC; break;
    case SDLK_1: priChar = capitalChar = '1'; shiftChar = '!'; break;
    case SDLK_2: priChar = capitalChar = '2'; shiftChar = '@'; break;
    case SDLK_3: priChar = capitalChar = '3'; shiftChar = '#'; break;
    case SDLK_4: priChar = capitalChar = '4'; shiftChar = '$'; break;
    case SDLK_5: priChar = capitalChar = '5'; shiftChar = '%'; break;
    case SDLK_6: priChar = capitalChar = '6'; shiftChar = '^'; break;
    case SDLK_7: priChar = capitalChar = '7'; shiftChar = '&'; break;
    case SDLK_8: priChar = capitalChar = '8'; shiftChar = '*'; break;
    case SDLK_9: priChar = capitalChar = '9'; shiftChar = '('; break;
    case SDLK_0: priChar = capitalChar = '0'; shiftChar = ')'; break;
    case SDLK_MINUS: priChar = capitalChar = '-'; shiftChar = '_'; break;
    case SDLK_EQUALS: priChar = capitalChar = '='; shiftChar = '+'; break;
    case SDLK_BACKSPACE: priChar = capitalChar = shiftChar = KEY_BACK_SPACE; break;
    case SDLK_TAB: priChar = capitalChar = shiftChar = KEY_TAB; break;
    case SDLK_q: priChar = 'q'; capitalChar = shiftChar = 'Q'; break;
    case SDLK_w: priChar = 'w'; capitalChar = shiftChar = 'W'; break;
    case SDLK_e: priChar = 'e'; capitalChar = shiftChar = 'E'; break;
    case SDLK_r: priChar = 'r'; capitalChar = shiftChar = 'R'; break;
    case SDLK_t: priChar = 't'; capitalChar = shiftChar = 'T'; break;
    case SDLK_y: priChar = 'y'; capitalChar = shiftChar = 'Y'; break;
    case SDLK_u: priChar = 'u'; capitalChar = shiftChar = 'U'; break;
    case SDLK_i: priChar = 'i'; capitalChar = shiftChar = 'I'; break;
    case SDLK_o: priChar = 'o'; capitalChar = shiftChar = 'O'; break;
    case SDLK_p: priChar = 'p'; capitalChar = shiftChar = 'P'; break;
    case SDLK_LEFTBRACKET: priChar = capitalChar = '['; shiftChar = '{'; break;
    case SDLK_RIGHTBRACKET: priChar = capitalChar = ']'; shiftChar = '}'; break;
    case SDLK_KP_ENTER:		// Enter on numeric keypad
                       onNumPad = 1;			// fall through
    case SDLK_RETURN: priChar = capitalChar = shiftChar = KEY_RETURN;	break;
    case SDLK_a: priChar = 'a'; capitalChar = shiftChar = 'A'; break;
    case SDLK_s: priChar = 's'; capitalChar = shiftChar = 'S'; break;
    case SDLK_d: priChar = 'd'; capitalChar = shiftChar = 'D'; break;
    case SDLK_f: priChar = 'f'; capitalChar = shiftChar = 'F'; break;
    case SDLK_g: priChar = 'g'; capitalChar = shiftChar = 'G'; break;
    case SDLK_h: priChar = 'h'; capitalChar = shiftChar = 'H'; break;
    case SDLK_j: priChar = 'j'; capitalChar = shiftChar = 'J'; break;
    case SDLK_k: priChar = 'k'; capitalChar = shiftChar = 'K'; break;
    case SDLK_l: priChar = 'l'; capitalChar = shiftChar = 'L'; break;
    case SDLK_SEMICOLON: priChar = capitalChar = ';'; shiftChar = ':'; break;
    case SDLK_QUOTE: priChar = capitalChar = '\''; shiftChar = '\"'; break;
    case SDLK_BACKQUOTE: priChar = capitalChar = '`'; shiftChar = '~'; break;
    case SDLK_BACKSLASH: priChar = capitalChar = '\\'; shiftChar = '|'; break;
    case SDLK_z: priChar = 'z'; capitalChar = shiftChar = 'Z'; break;
    case SDLK_x: priChar = 'x'; capitalChar = shiftChar = 'X'; break;
    case SDLK_c: priChar = 'c'; capitalChar = shiftChar = 'C'; break;
    case SDLK_v: priChar = 'v'; capitalChar = shiftChar = 'V'; break;
    case SDLK_b: priChar = 'b'; capitalChar = shiftChar = 'B'; break;
    case SDLK_n: priChar = 'n'; capitalChar = shiftChar = 'N'; break;
    case SDLK_m: priChar = 'm'; capitalChar = shiftChar = 'M'; break;
    case SDLK_COMMA: priChar = capitalChar = ','; shiftChar = '<'; break;
    case SDLK_PERIOD: priChar = capitalChar = '.'; shiftChar = '>'; break;
    case SDLK_SLASH: priChar = capitalChar = '/'; shiftChar = '\?'; break;
    case SDLK_KP_MULTIPLY: priChar = capitalChar = shiftChar = '*'; onNumPad = 1; break; // * on numeric keypad
    case SDLK_SPACE: priChar = capitalChar = shiftChar = ' '; break;
    case SDLK_KP_PLUS: priChar = capitalChar = shiftChar = '+'; onNumPad = 1; break; // + on numeric keypad
    case SDLK_KP_DIVIDE: priChar = capitalChar = shiftChar = '/'; onNumPad = 1; break;		// / on numeric keypad
    case SDLK_KP_MINUS: priChar = capitalChar = shiftChar = '-'; onNumPad = 1; break;	// - on numeric keypad

    case SDLK_KP7: priChar = shiftChar = capitalChar = '7'; onNumPad = 1; break;
    case SDLK_KP8: priChar = shiftChar = capitalChar = '8'; onNumPad = 1; break;
    case SDLK_KP9: priChar = shiftChar = capitalChar = '9'; onNumPad = 1; break;
    case SDLK_KP4: priChar = shiftChar = capitalChar = '4'; onNumPad = 1; break;
    case SDLK_KP5: priChar = shiftChar = capitalChar = '5'; onNumPad = 1; break;
    case SDLK_KP6: priChar = shiftChar = capitalChar = '6'; onNumPad = 1; break;
    case SDLK_KP1: priChar = shiftChar = capitalChar = '1'; onNumPad = 1; break;
    case SDLK_KP2: priChar = shiftChar = capitalChar = '2'; onNumPad = 1; break;
    case SDLK_KP3: priChar = shiftChar = capitalChar = '3'; onNumPad = 1; break;
    case SDLK_KP0: priChar = shiftChar = capitalChar = '0'; onNumPad = 1; break;
    case SDLK_KP_PERIOD: priChar = shiftChar = capitalChar = '.'; onNumPad = 1; break;

                      // function keys
    case SDLK_F1: priChar = shiftChar = capitalChar = KEY_F1; break;
    case SDLK_F2: priChar = shiftChar = capitalChar = KEY_F2; break;
    case SDLK_F3: priChar = shiftChar = capitalChar = KEY_F3; break;
    case SDLK_F4: priChar = shiftChar = capitalChar = KEY_F4; break;
    case SDLK_F5: priChar = shiftChar = capitalChar = KEY_F5; break;
    case SDLK_F6: priChar = shiftChar = capitalChar = KEY_F6; break;
    case SDLK_F7: priChar = shiftChar = capitalChar = KEY_F7; break;
    case SDLK_F8: priChar = shiftChar = capitalChar = KEY_F8; break;
    case SDLK_F9: priChar = shiftChar = capitalChar = KEY_F9; break;
    case SDLK_F10: priChar = shiftChar = capitalChar = KEY_F10; break;
    case SDLK_F11: priChar = shiftChar = capitalChar = KEY_F11; break;
    case SDLK_F12: priChar = shiftChar = capitalChar = KEY_F12; break;

                  // arrow keys
    case SDLK_HOME: priChar = shiftChar = capitalChar = KEY_HOME; break;
    case SDLK_UP: priChar = shiftChar = capitalChar = KEY_UP; break;
    case SDLK_PAGEUP: priChar = shiftChar = capitalChar = KEY_PGUP; break;
    case SDLK_LEFT: priChar = shiftChar = capitalChar = KEY_LEFT; break;
    case SDLK_RIGHT: priChar = shiftChar = capitalChar = KEY_RIGHT; break;
    case SDLK_END: priChar = shiftChar = capitalChar = KEY_END; break;
    case SDLK_DOWN: priChar = shiftChar = capitalChar = KEY_DOWN; break;
    case SDLK_PAGEDOWN: priChar = shiftChar = capitalChar = KEY_PGDN; break;
    case SDLK_INSERT: priChar = shiftChar = capitalChar = KEY_INS; break;
    case SDLK_DELETE: priChar = shiftChar = capitalChar = KEY_DEL; break;

    case SDLK_AT: priChar = shiftChar = capitalChar = '@'; break;
    case SDLK_COLON: priChar = shiftChar = capitalChar = ':'; break;
    case SDLK_UNDERSCORE: priChar = shiftChar = capitalChar = '_'; break;
  }

  // check flags
  int retFlag = 1;

  // check shift key state
  if( !(flags & K_IGNORE_SHIFT) )
  {
    if( flags & K_IS_SHIFT )
    {
      if( !(skeyState & SHIFT_KEY_MASK) ) retFlag = 0;
    }
    else
    {
      if( skeyState & SHIFT_KEY_MASK ) retFlag = 0;
    }
  }

  // check contrl key state
  if( !(flags & K_IGNORE_CTRL) )
  {
    if( flags & K_IS_CTRL )
    {
      if( !(skeyState & CONTROL_KEY_MASK) ) retFlag = 0;
    }
    else
    {
      if( skeyState & CONTROL_KEY_MASK ) retFlag = 0;
    }
  }

  // check alt key state
  if( !(flags & K_IGNORE_ALT) )
  {
    if( flags & K_IS_ALT )
    {
      if( !(skeyState & ALT_KEY_MASK) ) retFlag = 0;
    }
    else
    {
      if( skeyState & ALT_KEY_MASK ) retFlag = 0;
    }
  }

  // check numpad state
  if( !(flags & K_IGNORE_NUMPAD) )
  {
    if( flags & K_ON_NUMPAD )
    {
      if( !onNumPad ) retFlag = 0;
    }
    else
    {
      if( onNumPad ) retFlag = 0;
    }
  }

  unsigned outChar = priChar;
  if( flags & K_TRANSLATE_KEY ) 
  {
    if( priChar != capitalChar )
    {
      // letter
      outChar = skeyState & CAP_LOCK_STATE_MASK ? 
        (skeyState & SHIFT_KEY_MASK ? priChar : capitalChar) :
        (skeyState & SHIFT_KEY_MASK ? shiftChar : priChar) ;
    }
    else
    {
      // symbol key
      outChar = skeyState & SHIFT_KEY_MASK ? shiftChar : priChar;
    }
  }

  if(!retFlag) return 0;

  int retFlag2 = (charValue == 0) || outChar == charValue
    || ((flags & K_IGNORE_SHIFT) && shiftChar == charValue)
    || ((flags & K_IGNORE_CAP_LOCK) && capitalChar == charValue)
    || ((flags & K_CASE_INSENSITIVE) && outChar == (unsigned short) tolower(charValue));

  return retFlag2 ? outChar : 0;
}


int PollInputDevices ()
{
  // nothing here - message processing already handled this
  return 0;
}

void GetMousePos (int *x, int *y)
{
  SDL_GetMouseState (x, y);
}

void SetMousePos (int x, int y)
{
  SDL_WarpMouse (x, y);
}


// Get next event. Return 0 if the process is to end, 1 if a message was processed, 2 if there was no message
int ProcessNextEvent()
{
  if (!waitingEvent) {  // if an event is waiting, we process that, otherwise grab the next one
    if (!SDL_PollEvent (&event)) return 2;
  }
  waitingEvent = false;

  // Alright, we have an event now.
  if (event.type == SDL_QUIT) return 0;

  if ((event.type == SDL_KEYDOWN) || (event.type == SDL_KEYUP)) {
    // key pressed or released
    process_key_message (event.key);
  }

  // mouse events
  if (event.type == SDL_MOUSEMOTION) {
    mouse.cur_x = event.motion.x;
    mouse.cur_y = event.motion.y;

    switch (mouse.bound_type)
    {
      case 0:		// rectangular boundary
        if (mouse.cur_x < mouse.bound_x1) mouse.cur_x = mouse.bound_x1;
        if (mouse.cur_x > mouse.bound_x2) mouse.cur_x = mouse.bound_x2;
        if (mouse.cur_y < mouse.bound_y1) mouse.cur_y = mouse.bound_y1;
        if (mouse.cur_y > mouse.bound_y2) mouse.cur_y = mouse.bound_y2;
        break;
      case 1:		// rhombus boundary
        {
          int boundMargin = abs(mouse.cur_x - (mouse.bound_x1+mouse.bound_x2)/2);
          if (mouse.cur_y < mouse.bound_y1+boundMargin) mouse.cur_y = mouse.bound_y1+boundMargin;
          if (mouse.cur_y > mouse.bound_y2-boundMargin) mouse.cur_y = mouse.bound_y2-boundMargin;

          boundMargin = abs(mouse.cur_y - (mouse.bound_y1 + mouse.bound_y2)/2);
          if (mouse.cur_x < mouse.bound_x1+boundMargin) mouse.cur_x = mouse.bound_x1+boundMargin;
          if (mouse.cur_x > mouse.bound_x2-boundMargin) mouse.cur_x = mouse.bound_x2-boundMargin;
        }
        break;
    }
    mouse_cursor.process(mouse.cur_x, mouse.cur_y);
    power.mouse_handler();
  }

  if ((event.type == SDL_MOUSEBUTTONUP) || (event.type == SDL_MOUSEBUTTONDOWN)) {
    bool press = (event.type == SDL_MOUSEBUTTONDOWN);
    MouseEvent ev;
    // left/right buttons only
    if ((event.button.button == SDL_BUTTON_LEFT) || (event.button.button == SDL_BUTTON_RIGHT)) {
      if (event.button.button == SDL_BUTTON_LEFT) {
        mouse.left_press = press ? LEFT_BUTTON_MASK : 0;
        ev.event_type = press ? LEFT_BUTTON : LEFT_BUTTON_RELEASE;
      }
      else {
        mouse.right_press = press ? RIGHT_BUTTON_MASK : 0;
        ev.event_type = press ? RIGHT_BUTTON : RIGHT_BUTTON_RELEASE;
      }

      ev.x = event.button.x;
      ev.y = event.button.y;
      ev.time = m.get_time();
      ev.scan_code = 0;
      ev.skey_state = mouse.skey_state;
      mouse.add_event(&ev);
      // reset_boundary whenever left button is released
      if ((!press) && (event.button.button == SDL_BUTTON_LEFT)) mouse.reset_boundary();
    }
  }

  // activation events
  if (event.type == SDL_ACTIVEEVENT) {
    if (event.active.state & SDL_APPACTIVE) {
      sys.active_flag = event.active.gain;
      if( sys.active_flag )
      {
        sys.unpause();
        sys.need_redraw_flag = 1;
      }
      else
        sys.pause();
    }
  }

  return 1;
}

void WaitNextEvent()
{
  if (SDL_WaitEvent (&event))
    waitingEvent = true;
}


// audio

bool init_sound ()
{
  // TODO
  return false;
}

void deinit_sound ()
{
  // TODO
}

LPDIRECTSOUNDBUFFER create_sound_buffer(const char *wav_buf, unsigned int wavDataLength)
{
  // TODO
  return NULL;
}

void release_buffer (LPDIRECTSOUNDBUFFER *buffer)
{
  // TODO
}

bool set_volume (LPDIRECTSOUNDBUFFER buffer, int volume)
{
  // TODO
  return true;
}

bool set_volume (LPDIRECTSOUNDBUFFER buffer, DsVolume volume)
{
  // TODO
  return true;
}

int get_volume (LPDIRECTSOUNDBUFFER buffer)
{
  // TODO
  return 0;
}

int get_pan (LPDIRECTSOUNDBUFFER buffer)
{
  // TODO
  return 0;
}

bool fill_buffer (LPDIRECTSOUNDBUFFER buffer, const char *wav_buf, int wav_len, int start)
{
  // TODO
  return false;
}

bool play_buffer (LPDIRECTSOUNDBUFFER buffer, bool loop)
{
  // TODO
  return false;
}

bool buffer_playing (LPDIRECTSOUNDBUFFER buffer)
{
  // TODO
  return false;
}

