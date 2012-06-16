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
#include <obox.h>
#include <ovgalock.h>

#include <win32_compat.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>

//---------------- Define constant ------------------//
//
// DirectSoundBuffer size = LWAV_STREAM_BUFSIZ * LWAV_BANKS
// if it is going to play a high transfer rate wave
// (e.g. 16-bit 44.1kHz Stereo), increase LWAV_STREAM_BUFSIZ
//
//---------------------------------------------------//

#define LWAV_STREAM_BUFSIZ	0x2000
#define LWAV_BANKS			4
#define LOOPWAV_STREAM_BUFSIZ 0x1000
#define LOOPWAV_BANKS		4

#ifndef DSBCAPS_CTRLDEFAULT
	#define DSBCAPS_CTRLDEFAULT (DSBCAPS_CTRLFREQUENCY|DSBCAPS_CTRLPAN|DSBCAPS_CTRLVOLUME)
#endif


//--------- Begin of function wavefile_offset -------//
//
// find the "data" tag in a wave file
//
static const char * wavefile_data(const char *wavfile_buf)
{
	//----- position at WAVEfmt tag size ------//

	const char *p = wavfile_buf+0x10;
	DWORD tagSize=*(DWORD *)p;

	//-------- go to next tag field -----------//

	for( p += sizeof(DWORD)+tagSize; strncmp(p, "data", 4) != 0;
		  p += sizeof(DWORD)+tagSize)
	{
		p += 4;						// pointing at size of tag field
		tagSize = *(DWORD *)p;	// get the size of this tage field

		if(p - wavfile_buf > 128)
			return NULL;
	}

	//----- p pointing at the start of "data" tag ------//

	return p;
}
//--------- End of function wavefile_offset------------//

struct Audio::Private {
  LPDIRECTSOUNDBUFFER lp_wav_ch_dsb[MAX_WAV_CHANNEL];
  // DirectSoundBuffer of each channel
  int	wav_serial_no[MAX_WAV_CHANNEL];
  int	max_wav_serial_no;
  LPDIRECTSOUNDBUFFER lp_lwav_ch_dsb[MAX_LONG_WAV_CH];
  int	lwav_serial_no[MAX_LONG_WAV_CH];
  int	max_lwav_serial_no;
  // DirectSoundBuffer of each long wave
  File* lwav_fileptr[MAX_LONG_WAV_CH];
  // file point of each long wave
  short	lwav_bank[MAX_LONG_WAV_CH];	// which bank to be filled next
  short	lwav_bufsiz[MAX_LONG_WAV_CH];
  // buffer size of each channel = lwav_bufsiz[c]*LWAV_BANKS
  long	wav_volume;						// -10000 to 0
  char	run_yield;						// 0 = skip Audio::yield()

  LPDIRECTSOUNDBUFFER lp_loop_ch_dsb[MAX_LOOP_WAV_CH];
  File* loopwav_fileptr[MAX_LOOP_WAV_CH];
  int	repeat_offset[MAX_LOOP_WAV_CH];
  short	loopwav_bank[MAX_LOOP_WAV_CH];
  DWORD	loopwav_fade_time[MAX_LOOP_WAV_CH];
  int	loopwav_fade_rate[MAX_LOOP_WAV_CH];

  int assign_serial( int &);

  int get_free_wav_channel();

};

//-------------- Begin of function Audio::assign_serial ----------//
int Audio::Private::assign_serial(int &s)
{
	if( s == INT_MAX)
		return s = 1;
	return ++s;
}
//-------------- End of function Audio::assign_serial ----------//

int Audio::Private::get_free_wav_channel()
{
	for( int chanNum = 0; chanNum < MAX_WAV_CHANNEL; ++chanNum)
	{
		if( !buffer_playing(lp_wav_ch_dsb[chanNum]))
		{
			release_buffer(&lp_wav_ch_dsb[chanNum]);
		}

		if( !lp_wav_ch_dsb[chanNum] ) return chanNum;
	}
	
	return -1;
}


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

	int i;
	for(i = 0; i < MAX_WAV_CHANNEL; ++i)
	{
		d->lp_wav_ch_dsb[i] = NULL;
		d->wav_serial_no[i] = 0;
	}
	d->max_lwav_serial_no = 0;

	for(i=0; i < MAX_LONG_WAV_CH; ++i)
	{
		d->lp_lwav_ch_dsb[i] = NULL;
		d->lwav_serial_no[i] = 0;
		d->lwav_fileptr[i] = NULL;
	}
	d->max_lwav_serial_no = 0;

	for(i=0; i < MAX_LOOP_WAV_CH; ++i)
	{
		d->lp_loop_ch_dsb[i] = NULL;
		d->loopwav_fileptr[i] = NULL;
	}

	d->wav_volume = 100;		// 0(slient) - 100(loudest)

	//--------- init devices ----------//

	if( init_wav() )
	{
		wav_res.init( DIR_RES"A_WAVE2.RES", 0, 0 );      // 2nd 0-don't read all, 3rd 0-don't use vga buffer
		wav_buf 		 = mem_add(DEFAULT_WAV_BUF_SIZE);
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
  if (init_sound())
    wav_init_flag = 1;

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
		deinit_sound();
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
		wav_buf = mem_resize( wav_buf, wav_buf_size );
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
int Audio::play_resided_wav(const char* wavBuf, DsVolume dsVolume)
{
	if( !wav_init_flag || !wav_flag )   // a initialized and workable midi device can be disabled by user setting
		return 0;

	//-------- Load wav file header-------//
	DWORD wavDataOffset, wavDataLength;

	// determine the wave data offset and length
	const char * dataTag = wavefile_data(wavBuf);
	if (!dataTag)
	{
		err_now("Invalid wave file format");
		return 0;		// invalid RIFF WAVE format
	}

	wavDataOffset = (dataTag - wavBuf) + 4 + sizeof(DWORD);
	wavDataLength = *(DWORD *)(dataTag+4);

	//------- Create DirectSoundBuffer to store a wave ------//
	LPDIRECTSOUNDBUFFER lpDsb;

	// ------- assign buffer to a channel number ----------//
	lpDsb = NULL;
	int chanNum = d->get_free_wav_channel();
	if( chanNum == -1) return 0;
	// found an idle channel, create DirectSoundBuffer
	lpDsb = create_sound_buffer(wavBuf, wavDataLength);
	if (!lpDsb) return 0;
	d->lp_wav_ch_dsb[chanNum] = lpDsb;

	// Note : if not found, just play the sound, don't play the sound
	// increase MAX_WAV_CHANNEL
	
	//------- copy sound data to DirectSoundBuffer--------//
	// unlock vga_front
	VgaFrontLock vgaLock;

	if (!fill_buffer(lpDsb, wavBuf+wavDataOffset, wavDataLength)) return 0;

	//------- Set volume -----------//
	set_volume(lpDsb, dsVolume);

	//------- Play wav file --------//
	if (!play_buffer(lpDsb)) return 0;

	return d->wav_serial_no[chanNum] = d->assign_serial(d->max_wav_serial_no);
}
//------- End of function Audio::play_resided_wav -------//

// ###### begin Gilbert 6/12 ########//
//------- Begin of function Audio::get_free_wav_ch --------//
int Audio::get_free_wav_ch()
{
	int count = 0;
	for( int chanNum = 0; chanNum < MAX_WAV_CHANNEL; ++chanNum)
	{
		if (!buffer_playing(d->lp_wav_ch_dsb[chanNum]))
			release_buffer(&d->lp_wav_ch_dsb[chanNum]);

		if( !d->lp_wav_ch_dsb[chanNum] )
			count++;
	}
	
	return count;
}
//------- End of function Audio::get_free_wav_ch --------//
// ###### end Gilbert 6/12 ########//

//------- Begin of function Audio::stop_wav ------------//
//
// stop a short sound effect started by play_wav or play_resided_wav
//
// <int>        the serial no returned by play_wav or play_resided_wav
//
// return 1 - channel is found and stopped / channel not found
// return 0 - cannot stop the channel
//
int Audio::stop_wav(int serial)
{
	for( int chanNum = 0; chanNum < MAX_WAV_CHANNEL; ++chanNum)
	{
		if(d->lp_wav_ch_dsb[chanNum] != NULL && d->wav_serial_no[chanNum] == serial)
		{
			release_buffer(&d->lp_wav_ch_dsb[chanNum]);
			d->wav_serial_no[chanNum] = 0;
			return 1;
		}
	}
	return 0;
}
//------- End of function Audio::stop_wav ------------//

//------- Begin of function Audio::is_wav_playing ------------//
//
// return wheather a short sound effect is stopped
//
// <int>        the serial no returned by play_wav or play_resided_wav
//
int Audio::is_wav_playing(int serial)
{
	for( int chanNum = 0; chanNum < MAX_WAV_CHANNEL; ++chanNum)
	{
		if (d->lp_wav_ch_dsb[chanNum] && d->wav_serial_no[chanNum] == serial)
			return buffer_playing (d->lp_wav_ch_dsb[chanNum]);
	}
	return 0;
}
//------- End of function Audio::is_wav_playing ------------//

//------- Begin of function Audio::play_long_wav --------//
//
// Play digitized wav from the wav file
// suitable for very large wave file
//
// <char*>        wavName = name of the wave file
//
// return : <int> 1 - wav loaded and is playing
//                0 - wav not played
// note : it uses streaming DirectSoundBuffer
// Audio::yield() keeps on feeding data to it

// Create a DirectSoundBuffer of size lwav_buf_size[c]*LWAV_BANKS
// divide into LWAV_BANKS parts. Each time Audio::yield() is called,
// load wave file into one part. d->lwav_bank[c] record which part to be
// filled next for channel c.

int Audio::play_long_wav(const char *wavName, DsVolume dsVolume)
{
	if( !wav_init_flag || !wav_flag )   // a initialized and workable midi device can be disabled by user setting
		return 0;

	//-------- Load wav file header-------//
	DWORD wavDataOffset,wavDataLength;

	if( LWAV_STREAM_BUFSIZ*LWAV_BANKS > wav_buf_size )
	{
		wav_buf_size = LWAV_STREAM_BUFSIZ*LWAV_BANKS;
		wav_buf = mem_resize( wav_buf, wav_buf_size );
	}

	// File* filePtr = (File *)mem_add(sizeof(File));		// new File;
	File* filePtr = new File;		// new File;
	if(!filePtr->file_open(wavName,0,0))
	{
		char errmsg[60];
		sprintf(errmsg, "Cannot open %s", wavName);
		box.msg(errmsg);
		delete filePtr;
		return 0;
	}

	// load small part of the wave file (first 128 bytes) enough to hold
	// the hold header
	if( !filePtr->file_read( wav_buf, 128 ) )
	{
		delete filePtr;
		return 0;
	}

	// determine the wave data offset
	const char * dataTag = wavefile_data(wav_buf);
	if (!dataTag)
	{
		err_now("Invalid wave file format");
		delete filePtr;
		return 0;		// invalid RIFF WAVE format
	}
	wavDataOffset = (dataTag - wav_buf) + 4 + sizeof(DWORD);
	wavDataLength = *(DWORD *)(dataTag+4);

	// seek to the start of wave data
	filePtr->file_seek(wavDataOffset);

	WORD OptBufferSize=LWAV_STREAM_BUFSIZ,
		MinRemainder =(WORD)(wavDataLength % (OptBufferSize * LWAV_BANKS));
	//------- find out the best buffer size -------//
	// store it in OptBufferSize
	// criteria : below or equal to LWAV_STREAM_BUFSIZ and
	// minimize wavDataLength % (OptBufferSize * LWAV_BANKS)
	// i.e. minimize the truncation to the wave file
	for(WORD TryBufSiz=LWAV_STREAM_BUFSIZ-0x200; TryBufSiz <= LWAV_STREAM_BUFSIZ;
	TryBufSiz+=0x20)
	{
		WORD TryRemainder = (WORD)(wavDataLength % (TryBufSiz * LWAV_BANKS));
		if(TryRemainder < MinRemainder)
		{
			MinRemainder = TryRemainder;
			OptBufferSize = TryBufSiz;
		}
	}

	//------- Create DirectSoundBuffer to store a wave ------//
	LPDIRECTSOUNDBUFFER lpDsb;

	// ------- assign buffer to a channel number ----------//
	lpDsb = NULL;
	int chanNum;
	for( chanNum = 0; chanNum < MAX_LONG_WAV_CH; ++chanNum)
		if (!buffer_playing (d->lp_lwav_ch_dsb[chanNum]))
		{
			if(d->lp_lwav_ch_dsb[chanNum])
			{
				release_buffer(&d->lp_lwav_ch_dsb[chanNum]);
				delete d->lwav_fileptr[chanNum];
				d->lwav_fileptr[chanNum] = NULL;
			}

			// found an idle channel, create DirectSoundBuffer
			lpDsb = create_sound_buffer(wav_buf, OptBufferSize * LWAV_BANKS);
			if (!lpDsb) { delete filePtr; return 0; }
			d->lp_lwav_ch_dsb[chanNum] = lpDsb;
			d->lwav_fileptr[chanNum] = filePtr;			// no need to delete filePtr any more
			d->lwav_bank[chanNum] = 0;
			d->lwav_bufsiz[chanNum] = OptBufferSize;
			break;
		}
	if( chanNum >= MAX_LONG_WAV_CH)
	{
		delete filePtr;
		return 0;
	}
	// Note : if not found, just play the sound, don't play the sound
	// increase MAX_LONG_WAV_CH
	
	//------- copy sound data to DirectSoundBuffer--------//
	// unlock vga_front
	VgaFrontLock vgaLock;

	// load wave data into buffer
	// load data before lock DirectSoundBuffer in case the wave file
	// is very short
	if( !filePtr->file_read(wav_buf, OptBufferSize*LWAV_BANKS))
	{
		// file error
		err_now("Missing wave file");
		return 0;
	}
	if (!fill_buffer(lpDsb, wav_buf, OptBufferSize*LWAV_BANKS)) return 0;

  bool loop = true;
        if (filePtr->file_pos() >= filePtr->file_size() - 1) loop = false;

	set_volume(lpDsb, dsVolume);

	//------- Play wav file --------//
	if (!play_buffer(lpDsb, loop)) return 0;
	d->run_yield = 1;
	return d->lwav_serial_no[chanNum] = d->assign_serial(d->max_lwav_serial_no);
}
//------- End of function Audio::play_long_wav ----------//

//------- Begin of function Audio::stop_long_wav ------------//
//
// stop a short sound effect started by play_long_wav
//
// <int>        the serial no returned by play_long_wav
//
// return 1 - channel is found and stopped / channel not found
// return 0 - cannot stop the channel
//
int Audio::stop_long_wav(int serial)
{
	for( int chanNum = 0; chanNum < MAX_LONG_WAV_CH; ++chanNum)
	{
		if(d->lp_lwav_ch_dsb[chanNum] != NULL && d->lwav_serial_no[chanNum] == serial)
		{
			release_buffer(&d->lp_lwav_ch_dsb[chanNum]);
			delete d->lwav_fileptr[chanNum];
			d->lwav_fileptr[chanNum] = NULL;
			d->lwav_serial_no[chanNum] = 0;
			return 1;
		}
	}
	return 0;
}
//------- End of function Audio::stop_long_wav ------------//

//------- Begin of function Audio::is_long_wav_playing ------------//
//
// return wheather a short sound effect is stopped
//
// <int>        the serial no returned by play_wav or play_resided_wav
//
int Audio::is_long_wav_playing(int serial)
{
	for( int chanNum = 0; chanNum < MAX_LONG_WAV_CH; ++chanNum)
	{
		if(d->lp_lwav_ch_dsb[chanNum] && d->lwav_serial_no[chanNum] == serial)
			return buffer_playing(d->lp_lwav_ch_dsb[chanNum]);
	}
	return 0;
}
//------- End of function Audio::is_long_wav_playing ------------//

//--------------- Begin of Audio::vol_multiply --------------//
long Audio::vol_multiply(int relVolume)
{
	long dsVolume = (d->wav_volume * relVolume) - 10000;
	if( dsVolume > 0 )
		dsVolume = 0;
	else if( dsVolume < -10000 )
		dsVolume = -10000;
	return dsVolume;
}
//--------------- End of Audio::vol_multiply --------------//

//------- Begin of function Audio::play_loop_wav -------//
//
// Play digitized wav from the wav resource file
//
// <char*>        wavName = name of the wav in the resource file
// int				repeatOffset = offset of wave data to play on repeat
//											i.e. 0 to start of wave data
//
// return : <int> channel number (1 - MAX_LOOP_WAV_CH)
//          0     not played
//
int	Audio::play_loop_wav(const char *wavName, int repeatOffset, DsVolume dsVolume)
{
	if( !wav_init_flag || !wav_flag )   // a initialized and workable midi device can be disabled by user setting
		return 0;

	//-------- Load wav file header-------//
	DWORD wavDataOffset;

	// File* filePtr = (File *)mem_add(sizeof(File));		// new File;
	File* filePtr = new File;


	if(!filePtr->file_open(wavName,0,0))
	{
		char errmsg[60];
		sprintf(errmsg, "Cannot open %s", wavName);
		box.msg(errmsg);
		delete filePtr;
		return 0;
	}

	// load small part of the wave file (first 128 bytes) enough to hold
	// the hold header
	if( !filePtr->file_read( wav_buf, 128 ) )
	{
		delete filePtr;
		return 0;
	}

	// determine the wave data offset
	const char * dataTag = wavefile_data(wav_buf);
	if (!dataTag)
	{
		err_now("Invalid wave file format");
		delete filePtr;
		return 0;		// invalid RIFF WAVE format
	}
	wavDataOffset = (dataTag - wav_buf) + 4 + sizeof(DWORD);

	// seek to the start of wave data
	filePtr->file_seek(wavDataOffset);

	WORD OptBufferSize=LOOPWAV_STREAM_BUFSIZ;

	//------- Create DirectSoundBuffer to store a wave ------//
	LPDIRECTSOUNDBUFFER lpDsb;

	// ------- assign buffer to a channel number ----------//
	lpDsb = NULL;
	int chanNum;
	for( chanNum = 0; chanNum < MAX_LOOP_WAV_CH; ++chanNum)
		if (!buffer_playing (d->lp_loop_ch_dsb[chanNum]))
		{
			if(d->lp_loop_ch_dsb[chanNum])
			{
				release_buffer(&d->lp_loop_ch_dsb[chanNum]);
				// mem_del(d->loopwav_fileptr[chanNum]);	// delete lwav_fileptr[chanNum];
				delete d->loopwav_fileptr[chanNum];
				d->loopwav_fileptr[chanNum] = NULL;
			}
			// found an idle channel, create DirectSoundBuffer
			lpDsb = create_sound_buffer(wav_buf, OptBufferSize * LWAV_BANKS);
			if (!lpDsb) return 0;
			d->lp_loop_ch_dsb[chanNum] = lpDsb;
			d->loopwav_fileptr[chanNum] = filePtr;			// no need to delete filePtr any more
			d->loopwav_bank[chanNum] = 0;
			d->repeat_offset[chanNum] = wavDataOffset + repeatOffset;
			d->loopwav_fade_rate[chanNum] = 0;
			break;
		}
	if( chanNum >= MAX_LOOP_WAV_CH)
	{
		delete filePtr;
		return 0;
	}
	// Note : if not found, just play the sound, don't play the sound
	
	//------- copy sound data to DirectSoundBuffer--------//
	// unlock vga_front
	VgaFrontLock vgaLock;

	// load wave data into buffer
	// load data before lock DirectSoundBuffer in case the wave file
	// is very short
	if( !filePtr->file_read(wav_buf, OptBufferSize*LOOPWAV_BANKS))
	{
		// file error
		err_now("Missing wave file");
		return 0;
	}

	fill_buffer(lpDsb, wav_buf, OptBufferSize*LOOPWAV_BANKS);

	//------- Set volume -----------//
	set_volume(lpDsb, dsVolume);

	//------- Play wav file --------//
	if (!play_buffer(lpDsb, true)) return 0;
	d->run_yield = 1;
	return chanNum+1;
}
//------- End of function Audio::play_loop_wav ---------//


//------- Begin of function Audio::volume_loop_wav -------//
void	Audio::volume_loop_wav(int ch, DsVolume dsVolume)
{
	int chanNum = ch-1;
	if( chanNum < 0 || chanNum >= MAX_LOOP_WAV_CH)
		return;
	if(d->lp_loop_ch_dsb[chanNum])
	{
		set_volume(d->lp_loop_ch_dsb[chanNum], dsVolume);
		
		// stop fading
		d->loopwav_fade_rate[chanNum] = 0;
	}
}
//------- End of function Audio::volume_loop_wav -------//


//------- Begin of function Audio::fade_out_loop_wav -------//
//
// <int> fadeRate, time for volume 100 wave drop to slience
//
void Audio::fade_out_loop_wav(int ch, int fadeRate)
{
	int chanNum = ch-1;
	if( chanNum < 0 || chanNum >= MAX_LOOP_WAV_CH)
		return;
	if(d->lp_loop_ch_dsb[chanNum])
	{
		d->loopwav_fade_rate[chanNum] = fadeRate;
		d->loopwav_fade_time[chanNum] = m.get_time();
	}
}
//------- End of function Audio::fade_out_loop_wav -------//


//------- Begin of function Audio::get_loop_wav_volume -------//
DsVolume Audio::get_loop_wav_volume(int ch)
{
	int chanNum = ch-1;
	if( chanNum < 0 || chanNum >= MAX_LOOP_WAV_CH)
        {
                RelVolume rel = RelVolume(0,0);
		return DsVolume(rel);
        }

	LPDIRECTSOUNDBUFFER lpDsb= d->lp_loop_ch_dsb[chanNum];
	if( lpDsb)
		return DsVolume(get_volume(lpDsb), get_pan(lpDsb));
        RelVolume rel = RelVolume(0,0);
	return DsVolume(rel);
}
//------- End of function Audio::get_loop_wav_volume -------//


//------- Begin of function Audio::is_loop_wav_fading -------//
int Audio::is_loop_wav_fading(int ch)
{
	int chanNum = ch-1;
	if( chanNum < 0 || chanNum >= MAX_LOOP_WAV_CH)
		return 0;

	return d->lp_loop_ch_dsb[chanNum] && d->loopwav_fade_rate[chanNum];
}
//------- End of function Audio::is_loop_wav_fading -------//


//------- Begin of function Audio::yield ---------------//
void	Audio::yield()
{
	if( !d->run_yield)
		return;

	d->run_yield = 0;			// suspend recursive Audio::yield();

	// unlock vga_front
	VgaFrontLock vgaLock;

	// set break point beyond this point
	int i;
	for(i = 0; i < MAX_LONG_WAV_CH; ++i)
	{
		if( !d->lp_lwav_ch_dsb[i] )
			continue;

		// if a wav is not play, or buffer lost, stop it
		LPDIRECTSOUNDBUFFER& lpDsb = d->lp_lwav_ch_dsb[i];
		if( !buffer_playing(lpDsb))
		{
			release_buffer(&lpDsb);
			// mem_del(lwav_fileptr[i]);
			delete d->lwav_fileptr[i];
			d->lwav_fileptr[i] = NULL;
			continue;
		}

			// lock a channel for lwav_bufsiz[i]
			File *filePtr = d->lwav_fileptr[i];
			d->lwav_bank[i] = (d->lwav_bank[i] + 1) % LWAV_BANKS;	// next bank to fill
			bool loop = true;

			// write to pointers
			long startPos = filePtr->file_pos();

			filePtr->file_read(wav_buf, d->lwav_bufsiz[i]);
			long readStreamSize = filePtr->file_pos() - startPos;	// bytes read in
			fill_buffer(lpDsb, wav_buf, d->lwav_bufsiz[i], d->lwav_bank[i]*d->lwav_bufsiz[i]);

			if (d->lwav_bufsiz[i] > readStreamSize)
				loop = false;
			// load file into a channel, on last stream, don't loop back
			//------- Play wav file --------//
			if (!play_buffer(lpDsb, loop))
			{
				d->run_yield = 1;
				return;
			}
	}

	for(i = 0; i < MAX_LOOP_WAV_CH; ++i)
	{
		if( !d->lp_loop_ch_dsb[i] )
			continue;

		// if a channel is not playing, or can't restore release it
		LPDIRECTSOUNDBUFFER& lpDsb = d->lp_loop_ch_dsb[i];
		if (!buffer_playing(lpDsb))
		{
			release_buffer(&lpDsb);
			delete d->loopwav_fileptr[i];
			d->loopwav_fileptr[i] = NULL;
			continue;
		}

			File *filePtr = d->loopwav_fileptr[i];
			d->loopwav_bank[i] = (d->loopwav_bank[i] + 1) % LOOPWAV_BANKS;	// next bank to fill

			// write to pointers
			long startPos = filePtr->file_pos();
			filePtr->file_read(wav_buf, LOOPWAV_STREAM_BUFSIZ);
			long readStreamSize = filePtr->file_pos() - startPos;	// bytes read in
			if(LOOPWAV_STREAM_BUFSIZ > readStreamSize)
			{	// end of file, seek to beginning and read again
				filePtr->file_seek(d->repeat_offset[i]);
				filePtr->file_read(wav_buf+readStreamSize, LOOPWAV_STREAM_BUFSIZ-readStreamSize);
			}
			if (!fill_buffer(lpDsb, wav_buf, LOOPWAV_STREAM_BUFSIZ, d->loopwav_bank[i]*LOOPWAV_STREAM_BUFSIZ)) {
				// fail to unlock
				d->run_yield = 1;
				return;
			}

			// set volume if fading
			if( d->loopwav_fade_rate[i] )
			{
				DWORD nextFadeTime = m.get_time();
				int volume = get_volume(lpDsb);
				// calculate new volume
				volume -= (nextFadeTime - d->loopwav_fade_time[i]) * d->loopwav_fade_rate[i];
				if (set_volume(lpDsb, volume))
					d->loopwav_fade_time[i] = nextFadeTime;
			}

			// load file into a channel, on last stream, don't loop back
			//------- Play wav file --------//
			if (!play_buffer(lpDsb, true))
			{
				d->run_yield = 1;
				return;
			}
	}

	d->run_yield = 1;		// resume Audio::yield();
}

//------- End of function Audio::yield ---------------//


//------- Begin of function Audio::stop_wav -------//
//
void Audio::stop_wav()
{
	if( !wav_init_flag || !wav_flag )
		return;

	// ---------- stop all short wave  ------------- //
	int i;
	for(i = 0; i < MAX_WAV_CHANNEL; ++i)
		if(d->lp_wav_ch_dsb[i])
			{
			release_buffer(&d->lp_wav_ch_dsb[i]);
			}

	// ----------- stop all long wave ------------- //
	stop_long_wav();

	// ------------ stop all loop wave ------------//
	for(i = 1; i <= MAX_LOOP_WAV_CH; ++i)
	{
		stop_loop_wav(i);
	}

}
//------- End of function Audio::stop_wav -------//

//------- Begin of function Audio::stop_long_wav -------//
//
void Audio::stop_long_wav()
{
	if( !wav_init_flag || !wav_flag )
		return;

	for(int i = 0; i < MAX_LONG_WAV_CH; ++i)
		if(d->lp_lwav_ch_dsb[i])
		{
			release_buffer(&d->lp_lwav_ch_dsb[i]);
			delete d->lwav_fileptr[i];
			d->lwav_fileptr[i] = NULL;
		}
}
//------- End of function Audio::stop_long_wav -------//


//------- Begin of function Audio::stop_loop_wav -------//
void	Audio::stop_loop_wav(int ch)
{
	int chanNum = ch-1;
	if( chanNum < 0 || chanNum >= MAX_LOOP_WAV_CH)
		return;
	if(d->lp_loop_ch_dsb[chanNum])
	{
		release_buffer(&d->lp_loop_ch_dsb[chanNum]);
		delete d->loopwav_fileptr[chanNum];
		d->loopwav_fileptr[chanNum] = NULL;
	}
}
//------- End of function Audio::stop_loop_wav ---------//


//------- Begin of function Audio::is_wav_playing -------//
//
int Audio::is_wav_playing()
{
	int playingChannelCount = 0;
	if( !wav_init_flag || !wav_flag )   // a initialized and workable midi device can be disabled by user setting
		return 0;

	//------ find any wav channel is playing -----//
	// update d->lp_wav_ch_dsb[x] if necessary
	for(int ch=0; ch < MAX_WAV_CHANNEL; ++ch)
		if( buffer_playing(d->lp_wav_ch_dsb[ch]))
			playingChannelCount++;
		else
			// is not playing, clear it
			release_buffer(&d->lp_wav_ch_dsb[ch]);

	return (playingChannelCount > 0);
}
//------- End of function Audio::is_wav_playing -------//


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

	int dsVolume;
	long dsVolDiff = (wavVolume - d->wav_volume) * 100;

	// change volume for all channels
	int i;
	for( i = 0; i < MAX_WAV_CHANNEL; ++i)
	{
		if( d->lp_wav_ch_dsb[i])
		{
			dsVolume = get_volume(d->lp_wav_ch_dsb[i]) + dsVolDiff;
			set_volume(d->lp_wav_ch_dsb[i], dsVolume);
		}
	}

	for( i = 0; i < MAX_LONG_WAV_CH; ++i)
	{
		if( d->lp_lwav_ch_dsb[i])
		{
			dsVolume = get_volume(d->lp_lwav_ch_dsb[i]) + dsVolDiff;
			set_volume(d->lp_lwav_ch_dsb[i], dsVolume);
		}
	}

	for( i = 0; i < MAX_LOOP_WAV_CH; ++i)
	{
		if( d->lp_loop_ch_dsb[i])
		{
			dsVolume = get_volume(d->lp_loop_ch_dsb[i]) + dsVolDiff;
			set_volume(d->lp_loop_ch_dsb[i], dsVolume);
		}
	}

	d->wav_volume = wavVolume;
}
//--------------- End of Audio::set_wav_volume --------------//


// ------------ Begin of function Audio::volume_long_wav -------//
void Audio::volume_long_wav(int serial, DsVolume dsVolume)
{
	if( !is_long_wav_playing(serial) ) return;
	for( int chanNum = 0; chanNum < MAX_LONG_WAV_CH; ++chanNum)
	{
		if(d->lp_lwav_ch_dsb[chanNum] != NULL && d->lwav_serial_no[chanNum] == serial )
		{
			set_volume(d->lp_lwav_ch_dsb[chanNum], dsVolume.ds_vol); // no pan
			break;
		}
	}
}
// ------------ End of function Audio::volume_long_wav -------//

