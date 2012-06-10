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

// Filename    : OMUSIC.CPP
// Description : music

#include <gamedef.h>
#include <all.h>
#include <oaudio.h>
#include <omusic.h>
#include <osys.h>
#include <oconfig.h>

// -------- define constant --------//
// random select 2 - 8 for background music
#define LOW_RANDOM_SONG 6
#define HIGH_RANDOM_SONG 12//8

// -------- define song name --------//

static const char *music_file[] =
{
	"CBRIEF.WAV",		// opening
	"CFRYWIN.WAV",
	"MENU.WAV",
	"LOSE.WAV",
	"WIN.WAV",
	"BKMUSIC1.WAV",
	"BKMUSIC2.WAV",
	"BKMUSIC3.WAV",
	"BKMUSIC4.WAV",
	"BKMUSIC5.WAV",
	"BKMUSIC6.WAV",
	"BKMUSIC7.WAV",
};

// -------- begin of function Music::Music ---------//
Music::Music()
{
	init_flag = 0;
	song_id = 0;
	music_channel = 0;
}
// -------- end of function Music::Music ---------//


// -------- begin of function Music::~Music ---------//
Music::~Music()
{
	deinit();
}
// -------- end of function Music::~Music ---------//


// -------- begin of function Music::init ---------//
// note : call Music::init after audio.init
void Music::init()
{
	deinit();
	init_flag = audio.init_flag;
}
// -------- end of function Music::init ---------//


// -------- begin of function Music::deinit ---------//
// call deinit before audio.deinit
void Music::deinit()
{
	if( init_flag )
	{
		stop();
	}
}
// -------- end of function Music::deinit ---------//


// -------- begin of function Music::stop ---------//
int Music::stop()
{
	if( init_flag )
	{
		if(music_channel)
		{
			if( play_type & MUSIC_PLAY_LOOPED )
				audio.stop_loop_wav(music_channel);
			else
				audio.stop_long_wav(music_channel);
			music_channel = 0;
			song_id = 0;
		}
		return 1;
	}
	else
	{
		return -1;
	}
}
// -------- end of function Music::stop ---------//


// -------- begin of function Music::play ---------//
// <int> songId
// <int> playType   0 = non-looped, 1 = looped
int Music::play(int songId, int playType)
{
	if( !init_flag )
		return 0;

	stop();
		if( audio.wav_init_flag )
		{
			String waveFileStr(DIR_MUSIC);
			waveFileStr += music_file[songId-1];
			if( !DIR_MUSIC[0] || !m.is_file_exist(waveFileStr) || !audio.wav_init_flag )
				return 0;
			if( playType & MUSIC_PLAY_LOOPED )
                        {
                                AbsVolume absv(config.wav_music_volume,0);
                                music_channel = audio.play_loop_wav(waveFileStr, 0, absv );
                        }
			else
                        {
                                AbsVolume absv(config.wav_music_volume,0);
                                music_channel = audio.play_long_wav(waveFileStr, absv );
                        }
			play_type = playType;
			song_id = songId;
			return music_channel != 0;
		}
		return 0;
}
// -------- end of function Music::play ---------//


// -------- begin of function Music::is_playing ---------//
// [int] songId        id of the song (default=0, don't care)
int Music::is_playing(int songId)
{
	if( !init_flag )
		return 0;

	if( !music_channel )
		return 0;

	if( play_type & MUSIC_PLAY_LOOPED )
	{
		return (!songId || songId == song_id);		// loop wav always playing
	}
	else
	{
		return audio.is_long_wav_playing(music_channel) && (!songId || songId == song_id);
	}

	return 0;
}
// -------- end of function Music::is_playing ---------//


// -------- begin of function Music::max_song --------//
// return no. of songs
int Music::max_song()
{
	return sizeof(music_file) / sizeof(char *);
}
// -------- end of function Music::max_song --------//


// -------- begin of function Music::random_bgm_track --------//
int Music::random_bgm_track(int excludeSong)
{
	int s = LOW_RANDOM_SONG + m.get_time() % (HIGH_RANDOM_SONG - LOW_RANDOM_SONG + 1);
	if( s == excludeSong )
	{
		// avoid selecting excludeSong if possible
		if( ++s > HIGH_RANDOM_SONG )
			s = LOW_RANDOM_SONG;
	}
	err_when(s < 1 || s > max_song() );
	return s;
}
// -------- end of function Music::random_bgm_track --------//


// -------- begin of function Music::change_volume --------//
void Music::change_volume(int vol)
{
	if( !init_flag )
		return;

	if( is_playing() )
	{
                AbsVolume absv(vol,0);
                audio.volume_long_wav(music_channel, DsVolume(absv));
	}
}
//-------- end of function Music::change_volume --------//


//-------- begin of function Music::yield --------//

void Music::yield()
{
	if( !init_flag )
		return;

	int oldSongId = song_id;

	if( config.music_flag )
	{
		if( !is_playing() )
			play(random_bgm_track(oldSongId), sys.cdrom_drive ? MUSIC_CD_THEN_WAV : 0 );
	}
	else
	{
		stop();			// stop any music playing
	}
}
//-------- end of function Music::yield --------//
