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

//Filename	  : OSYS.H
//Description : Header file for Game class

#ifndef __OSYS_H
#define __OSYS_H

#include <win32_compat.h>

// ------- declare type --------//

typedef long LPARAM;

#define MAX_PATH 260

class File;

//------ Define common data buffer size  ------//

enum { COMMON_DATA_BUF_SIZE = 64*1024 };			// keep a 64K common buffer for all temporary memory storage like reading files temporarily into memory

//-------------- define constant ------------//

#define FRAMES_PER_DAY	10			// no. of frames per day

#define MAX_SCENARIO_PATH 2

//------------ sys_flag --------------//

enum { SYS_PREGAME=0, SYS_RUN };

//--------- game version ---------//

enum { VERSION_FULL,
		 VERSION_DEMO,
		 VERSION_MULTIPLAYER_ONLY };

//--------- Define info modes --------//

enum { VIEW_MODE_COUNT=10 };

enum { MODE_NORMAL,
		 MODE_NATION,
		 MODE_TOWN,
		 MODE_ECONOMY,
		 MODE_TRADE,
		 MODE_MILITARY,
		 MODE_TECH,
		 MODE_SPY,
		 MODE_RANK,
		 MODE_NEWS_LOG,
		 MODE_AI_ACTION,
	  };

//------------------ define debug_seed_status_flag ----------------//
enum	{	NO_DEBUG_SYN = 0,
			DEBUG_SYN_LOAD_AND_COMPARE_ONCE,
			DEBUG_SYN_AUTO_SAVE,
			DEBUG_SYN_AUTO_LOAD,
		};

//-------- Define class Sys -----------//

class Sys
{
public:
	char		game_version;			// VERSION_???

	char     active_flag;    		// we are the active app.
	char     paused_flag;    		// whether the game is paused
	char		sys_flag;
	char		init_flag;

	char		signal_exit_flag;		//	1-quit to windows
											// 2-quit to main menu
											// 3-quit a campaign game only, but still remain in the campaign
											// 4-this is set after loading a game for exiting the current main_loop() so that it can enter the correct main_loop() (campaign & game) later.

	char		quick_exit_flag()  	{ return signal_exit_flag; }

	char		need_redraw_flag;    // set to 1 if task switched back. After redraw, clear it
	char		cheat_enabled_flag;

	char 		view_mode;				// the view mode can be MODE_???

	char		map_need_redraw;
	char		zoom_need_redraw;

	short		last_speed_mode;

	//------ frame related vars -----//

	int 		day_frame_count;
	DWORD		next_frame_time;		// next frame's time for maintaining a specific game speed

	//----- multiplayer vars ----//

	DWORD 	frame_count;  			// frame count, for is_sync_frame only
	char		is_sync_frame;			// whether sync should take place at the current frame (for handling one sync per n frames)
	char		mp_save_flag;			// indicate a request to save game in multi-player game
	DWORD		mp_save_frame;			// save game in which frame

	//---- continous key string -----//

	enum { MAX_KEY_STR = 10 };       // Maximum 10 different key string

	int  key_str_pos[MAX_KEY_STR];  // for detecting cheating codes

	//-------- statistic --------//

	DWORD		last_second_time;
	int		frames_in_this_second;
	int	 	frames_per_second;   // the actual frames per second

	//------ view report var -----//

	//------- file paths -------//

	char  	cdrom_drive;

	char  	dir_image[MAX_PATH+1];
	char  	dir_music[MAX_PATH+1];
	char  	dir_movie[MAX_PATH+1];
	char  	dir_tutorial[MAX_PATH+1];
	char		dir_campaign[MAX_PATH+1];

	union
	{
		char dir_scenario[MAX_PATH+1];
		char dir_scenario_path[MAX_SCENARIO_PATH][MAX_PATH+1];
	};

	//------- other vars --------//

	char*		common_data_buf;

	char		debug_session;				// for debugging games
	char		testing_session;			// for testing games (kings never die)
	char		scenario_cheat_flag;		// for creating scenarios

public:
	Sys();
	~Sys();

	int		init();
	int		change_display_mode(int modeId);		// see omodeid.h for modeId
	void		deinit();
	void		deinit_directx();
	void		deinit_objects();

	void		run(int=0);
	void		yield();

	void 		set_speed(int frameSpeed, int remoteCall=0);
	void 		set_view_mode(int viewMode, int viewingNationRecno=0, int viewingSpyRecno=0);
	void		disp_view_mode(int observeMode=0);

	void 		disp_frame(int dispCampaignMsg=0);
	void		capture_screen();

	void		pause();
	void		unpause();
	void		sleep(int time);

	void		mp_request_save(DWORD frame);
	void		mp_clear_request_save();

	//---- for setting game directories ----//

	void 		set_game_dir();
	int 		set_one_dir( const char* checkFileName, const char* defaultDir, char* trueDir );
	void 		get_cdrom_drive();

	//-------- for load/save games --------//

	int 		write_file(File* filePtr);
	int		read_file(File* filePtr);
	void		save_game();
	void		load_game();

private:
	int		init_win();
	int		init_directx();
	int 		init_objects();
	int 		init_display();

	void		main_loop(int);
	void		detect();
	void		process();

	void		disp_button();
	void 		detect_button();

	void		disp_view();
	void		update_view();
	void		detect_view();

	void 		disp_map();
	void 		disp_zoom();

	int		should_next_frame();
	int		is_mp_sync( int *unreadyPlayerFlag );
	void		auto_save();

	void 		blt_next_frame();
	void		disp_frames_per_second();

	BOOL		restore();

	void		process_key(unsigned scanCode, unsigned skeyState);

	void		detect_letter_key(unsigned scanCode, unsigned skeyState);
	void		detect_function_key(unsigned scanCode, unsigned skeyState);
	void		detect_cheat_key(unsigned scanCode, unsigned skeyState);
	void		detect_debug_cheat_key(unsigned scanCode, unsigned skeyState);
	int 		detect_set_speed(unsigned scanCode, unsigned skeyState);
	void		detect_campaign_cheat_key(unsigned scanCode, unsigned skeyState);

	int 		detect_key_str(int keyStrId, const char* keyStr);
};

extern Sys sys;
#ifdef DEBUG
extern char debug2_enable_flag;
extern File seedCompareFile;
extern char debug_seed_status_flag;
extern int	debug_sim_game_type;
#endif

//-------------------------------------//

#endif


