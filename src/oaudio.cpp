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

//Filename    : OAUDIO.CPP
//Description : Object Midi Audio and Digitized Sound
//Ownership   : Gilbert

#include <all.h>
#include <oaudio.h>
#include <osys.h>

// no point deferring this to platform.cpp as we need this everywhere
#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>

#define CHANNEL_COUNT 16

struct ChannelData {
  Mix_Chunk *data;
};

struct Audio::Private {
  ChannelData channel[CHANNEL_COUNT];
  int wav_volume;
  char	run_yield;						// 0 = skip Audio::yield()

  void set_volume(int channel, DsVolume vol)
  {
    int v = vol.ds_vol * MIX_MAX_VOLUME / 100 * wav_volume / 100;
    Mix_Volume(channel, v);
  }
};

//--------- Begin of function Audio::Audio ----------//

Audio::Audio()
{
  d = new Private;
  init_flag = 0;
}
//--------- Begin of function Audio::Audio ----------//


//--------- Begin of function Audio::~Audio ----------//

Audio::~Audio()
{
  deinit();
  delete d;
}
//--------- Begin of function Audio::~Audio ----------//


//--------- Begin of function Audio::init ----------//
//
// Initialize the mid driver
//
// return : <int> 1 - initialized successfully
//                0 - init fail
//
int Audio::init()
{
  //-------- init vars -----------//

  d->run_yield = 0;
  wav_init_flag = 0;

  wav_flag = 1;

  wav_buf = NULL;

  wav_buf_size = 0;
  for (int ch = 0; ch < CHANNEL_COUNT; ++ch)
    d->channel[ch].data = 0;

  d->wav_volume = 100;		// 0(slient) - 100(loudest)

  //--------- init devices ----------//

  if( init_wav() )
  {
    wav_res.init( DIR_RES"A_WAVE2.RES", 0, 0 );      // 2nd 0-don't read all, 3rd 0-don't use vga buffer
    wav_buf = (unsigned char *) mem_add(DEFAULT_WAV_BUF_SIZE);
    wav_buf_size = DEFAULT_WAV_BUF_SIZE;
  }

  //----------------------------------//

  init_flag = wav_init_flag;

  return 1;
}
//--------- End of function Audio::init ----------//


//--------- Begin of function Audio::deinit ----------//

void Audio::deinit()
{
  if( init_flag )
  {
    //------- deinit vars --------//

    d->run_yield = 0;
    init_flag = 0;

    //------- deinit devices -------//

    deinit_wav();
  }
}
//--------- End of function Audio::deinit ----------//


//--------- Begin of function Audio::init_wav ----------//
//
// Initialize digitized wav driver
//
// return : <int> 1 - initialized successfully
//                0 - init fail
//
int Audio::init_wav()
{
  if( wav_init_flag )
    return 1;

  Mix_Init(0);
  if (Mix_OpenAudio(11025, MIX_DEFAULT_FORMAT, 1, 1024) >= 0)
  {
    Mix_AllocateChannels(CHANNEL_COUNT);
    wav_init_flag = 1;
  }

  return wav_init_flag;
}
//--------- End of function Audio::init_wav ----------//

//--------- Begin of function Audio::deinit_wav ----------//

void Audio::deinit_wav()
{
  stop_wav();

  if( wav_buf )
  {
    mem_del(wav_buf);
    wav_buf = NULL;
  }

  if(wav_init_flag)
  {
    Mix_CloseAudio();
    Mix_Quit();
    wav_init_flag = 0;
  }
}
//--------- End of function Audio::deinit_wav ----------//


//------- Begin of function Audio::play_wav -------//
//
// Play digitized wav from the wav resource file
//
// short resIdx = index of wave file in A_WAVE2.RES
// long vol = volume (0 to 100)
// long pan = pan (-10000 to 10000)
//
// return : <int> 1 - wav loaded and is playing
//                0 - wav not played
//
int Audio::play_wav(short resIdx, DsVolume dsVolume)
{
  if( !wav_init_flag || !wav_flag )   // a initialized and workable midi device can be disabled by user setting
    return 0;

  //-------- Load wav file header-------//
  int   dataSize;

  File* filePtr = wav_res.get_file(resIdx, dataSize);

  if( !filePtr )
    return 0;

  if( dataSize > wav_buf_size )
  {
    wav_buf_size = dataSize;
    wav_buf = (unsigned char *) mem_resize( wav_buf, wav_buf_size );
  }
  if( !filePtr->file_read( wav_buf, dataSize))
    return 0;

  return play_resided_wav(wav_buf, dsVolume);
}
//------- End of function Audio::play_wav -------//


//------- Begin of function Audio::play_resided_wav -------//
//
// Play digitized wav from the wav file in memory
//
// <char*>        wavBuf = point to the wav in memory
// long vol = volume (0 to 100)
// long pan = pan (-10000 to 10000)
//
// return : <int> 1 - wav loaded and is playing
//                0 - wav not played
//
int Audio::play_resided_wav(unsigned char* wavBuf, DsVolume dsVolume)
{
  if( !wav_init_flag || !wav_flag )   // a initialized and workable midi device can be disabled by user setting
    return -1;

  Mix_Chunk *chunk = Mix_QuickLoad_WAV(wavBuf);
  int channel = Mix_PlayChannel(-1, chunk, 0);
  if (channel < 0) return -1;
  d->channel[channel].data = chunk;
  d->set_volume(channel, dsVolume);
  return channel;
}
//------- End of function Audio::play_resided_wav -------//

//------- Begin of function Audio::get_free_wav_ch --------//
int Audio::get_free_wav_ch()
{
  int count = 0;
  for (int ch = 0; ch < CHANNEL_COUNT; ++ch)
    if (!Mix_Playing(ch)) count++;
  return count;
}
//------- End of function Audio::get_free_wav_ch --------//

//------- Begin of function Audio::play_long_wav --------//
int Audio::play_long_wav(const char *wavName, DsVolume dsVolume)
{
  if( !wav_init_flag || !wav_flag )   // a initialized and workable midi device can be disabled by user setting
    return -1;

  Mix_Chunk *chunk = Mix_LoadWAV(wavName);
  int channel = Mix_PlayChannel(-1, chunk, 0);
  if (channel < 0) return -1;
  d->channel[channel].data = chunk;
  d->set_volume(channel, dsVolume);
  return channel;
}
//------- End of function Audio::play_long_wav ----------//

//------- Begin of function Audio::stop_long_wav ------------//
int Audio::stop_long_wav(int ch)
{
  if (!Mix_Playing(ch)) return 0;
  Mix_HaltChannel(ch);
  return 1;
}
//------- End of function Audio::stop_long_wav ------------//

//------- Begin of function Audio::is_long_wav_playing ------------//
//
// return wheather a short sound effect is stopped
//
// <int>        the serial no returned by play_wav or play_resided_wav
//
int Audio::is_long_wav_playing(int ch)
{
  return !Mix_Playing(ch);
}
//------- End of function Audio::is_long_wav_playing ------------//

//------- Begin of function Audio::play_loop_wav -------//
int	Audio::play_loop_wav(const char *wavName, DsVolume dsVolume)
{
  if( !wav_init_flag || !wav_flag )   // a initialized and workable midi device can be disabled by user setting
    return 0;

  Mix_Chunk *chunk = Mix_LoadWAV(wavName);
  int channel = Mix_PlayChannel(-1, chunk, -1);
  if (channel < 0) return -1;
  d->channel[channel].data = chunk;
  d->set_volume(channel, dsVolume);
  return channel;
}
//------- End of function Audio::play_loop_wav ---------//


//------- Begin of function Audio::volume_loop_wav -------//
void	Audio::volume_loop_wav(int ch, DsVolume dsVolume)
{
  d->set_volume(ch, dsVolume);
}
//------- End of function Audio::volume_loop_wav -------//


//------- Begin of function Audio::fade_out_loop_wav -------//
//
// <int> fadeRate, time for volume 100 wave drop to slience
//
void Audio::fade_out_loop_wav(int ch, int fadeRate)
{
  Mix_FadeOutChannel (ch, 100 * fadeRate);
}
//------- End of function Audio::fade_out_loop_wav -------//


//------- Begin of function Audio::get_loop_wav_volume -------//
DsVolume Audio::get_loop_wav_volume(int ch)
{
  return DsVolume(Mix_Volume(ch, -1), 0);
}
//------- End of function Audio::get_loop_wav_volume -------//


//------- Begin of function Audio::is_loop_wav_fading -------//
int Audio::is_loop_wav_fading(int ch)
{
  return (Mix_FadingChannel(ch) == MIX_FADING_OUT);
}
//------- End of function Audio::is_loop_wav_fading -------//


//------- Begin of function Audio::yield ---------------//
void	Audio::yield()
{
  if( !d->run_yield)
    return;

  d->run_yield = 0;			// suspend recursive Audio::yield();

  // clear terminated chunks
  for (int ch = 0; ch < CHANNEL_COUNT; ++ch) {
    if (d->channel[ch].data && (!Mix_Playing(ch))) {
      Mix_FreeChunk(d->channel[ch].data);
      d->channel[ch].data = 0;
    }
  }

  d->run_yield = 1;
}

//------- End of function Audio::yield ---------------//


//------- Begin of function Audio::stop_wav -------//
//
void Audio::stop_wav()
{
  if( !wav_init_flag || !wav_flag )
    return;

  Mix_HaltChannel(-1);
}
//------- End of function Audio::stop_wav -------//

//------- Begin of function Audio::stop_loop_wav -------//
void	Audio::stop_loop_wav(int ch)
{
  Mix_HaltChannel(ch);
}
//------- End of function Audio::stop_loop_wav ---------//

//----------------- Begin of Audio::toggle_wav -----------------//
//
void Audio::toggle_wav(int wavFlag)
{
  if( !wavFlag )
    stop_wav();

  wav_flag = wavFlag;
}
//------------------- End of Audio::toggle_wav ------------------//


//-------------- Begin of Audio::set_wav_volume -------------//
//
// Set wav volume
//
// <int> wavVolume = wav volume, 0-100
//
void Audio::set_wav_volume(int wavVolume)
{
  if( !wav_init_flag )
    return;

  d->wav_volume = wavVolume;
  d->set_volume(-1, DsVolume(100, 0));   // set_volume multiplies by wav_volume, hence that 100
}
//--------------- End of Audio::set_wav_volume --------------//


// ------------ Begin of function Audio::volume_long_wav -------//
void Audio::volume_long_wav(int ch, DsVolume dsVolume)
{
  d->set_volume(ch, dsVolume);
}
// ------------ End of function Audio::volume_long_wav -------//

