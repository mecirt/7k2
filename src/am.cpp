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

//Filename    : AM.CPP
//Description : Ambition Entry Program

#define NEED_WINDOWS

#include <all.h>
#include <odplay.h>
#include <oanline.h>
#include <oaudio.h>
#include <obaseobj.h>
#include <obattle.h> 
#include <obox.h>
#include <obullet.h>
#include <oconfig.h> 
#include <odate.h>
#include <ofirm.h>
#include <oflame.h>
#include <ofont.h>
#include <ogame.h>
#include <ogameset.h>
#include <ogfile.h>
#include <ogodres.h>
#include <ohelp.h>
#include <oherores.h>
#include <otips.h>
#include <ohillres.h>
#include <oimgres.h>
#include <oinfo.h>
#include <omagic.h>
#include <omonsres.h>
#include <omouse.h>
#include <omousecr.h>
#include <onation.h>
#include <onews.h>
#include <ounit.h>
#include <oun_grp.h>
#include <opfind.h>
#include <opreuse.h>
#include <oplant.h>
#include <opower.h>
#include <oraceres.h>
#include <orebel.h>
#include <oremote.h>
#include <osite.h>
#include <osprtres.h>
#include <osfrmres.h>
#include <ospy.h>
#include <osys.h>
#include <otalkres.h>
#include <otechres.h>
#include <oterrain.h>
#include <otown.h>
#include <otransl.h>
#include <ounit.h>
#include <ovga.h>
#include <owallres.h>
#include <oworld.h>
#include <oweather.h>
#include <oevent.h>
#include <otornado.h>
#include <otutor.h>
#include <ose.h>
#include <oseres.h>
#include <osnowres.h>
#include <osnowg.h>
#include <ostate.h>
#include <orockres.h>
#include <orock.h>
#include <oeffect.h>
#include <oexpmask.h>
#include <oregion.h>
#include <owarpt.h>
#include <oerrctrl.h>
#include <omusic.h>
#include <olog.h>
#include <olonglog.h>
#include <ofirmdie.h>
#include <otwalres.h>
#include <ocrc_sto.h>
#include <ooptmenu.h>
#include <oingmenu.h>
#include <owaypnt.h>
#include <omodeid.h>
#include <ocampgn.h>
#include <oitemres.h>
#include <oprofile.h>
#include <oseditor.h>
#include <oserial.h>
#include <ot_basic.h>
#include <ot_gmenu.h>
#include <ot_tutor.h>
#include <ot_camp.h>
#include <ot_unit.h>
#include <ot_firm.h>
#include <ot_endc.h>
#include <ot_reps.h>
#include <ot_item.h>
#include <ot_talk.h>
#include <ot_news.h>
#include <ot_sedit.h>
#include <win32_compat.h>

//------- define game version constant --------//

const char *GAME_VERSION_STR = "1.20";
const int GAME_VERSION = 120;	// Version 1.00, don't change major version unless the format of save game files has been changed

//-------- System class ----------//

// global class object follows the order of init
// such that at error case exit() calls destructor in appropriate order

// e.g. Sys::~Sys calls game.deinit and game.deinit calls unit_array.deinit
// then unit_array construct before game, game before sys.

#ifndef NO_MEM_CLASS
   Mem   mem;              // constructor only init var and allocate memory
#endif

Error             err;              // constructor only call set_new_handler()d
Misc              m, m2;
DateInfo          date;
Config            config;
SerialRepository  serial_repository( 1000, "GAMESERL.DAT" );
Log               msg_log;
#ifdef DEBUG
LongLog *			long_log;
#endif

//-------- objects initialized in Sys::init_directx --------//

Audio             audio;
Music             music;
SECtrl            se_ctrl(&audio);
DisplayModeInfo	current_display_mode;
Vga               vga;
VgaBuf            vga_buffer;

//--------- objects initalized in Sys::init_objects --------//

MouseCursor       mouse_cursor;
Mouse             mouse;
Font					font_san, font_std, font_small, font_mid, font_news;
Font					font_hitpoint, font_bible, font_bard;
Font					font_whbl, font_zoom, font_blue;
Font					font_snds, font_blu2, font_red, font_gren, font_cara, font_cara_w;
Font					font_bld;
Font					font_vill, font_vilb, font_town;
Font					font_thin_black, font_bold_black, font_bold_red;
Font					font_sop;
Font					font_tut;
Font					font_scre, font_cmpa, font_cmph, font_cmpf, font_cmpo;

Font					font_hall;
ImageRes          image_icon, image_interface, image_menu, image_button, image_spict, image_gameif;
ImageRes          image_encyc;
ImageRes				image_tutorial;
ImageRes&			image_menu2 = image_menu;
PathFinder			path_finder;
PathReuse			path_reuse;
Flame             flame[FLAME_GROW_STEP];
AnimLine          anim_line;
Help              help;
Translate         translate;        // constructor only memset()
Tutor             tutor;
GameFileArray     game_file_array;

Box               box;

TextResBasic		text_basic;
TextResGameMenu	text_game_menu;
TextResTutorial	text_tutorial;
TextResCampaign	text_campaign;
TextResUnit			text_unit;
TextResFirm			text_firm;
TextResEndCondition text_end_condition;
TextResReports		text_reports;
TextResItem			text_item;
TextResTalk			text_talk;
TextResNews			text_news;
TextResEditor		text_editor;

//-------- multiplayer objects -----------//

MultiPlayerType	mp_obj;
// MultiPlayerDP	mp_dp;
// MultiPlayerIM	mp_im;
Remote            remote;
ErrorControl      ec_remote;
CrcStore				crc_store;

//-------- player profile --------//

PlayerProfile		player_profile;

//-------- resource initialized in Game::init --------//

ImageRes				image_tpict;
GameSet           game_set;         // no constructor
TerrainRes        terrain_res;
PlantRes          plant_res;
TechRes           tech_res;
GodRes            god_res;
SpriteRes         sprite_res;
SpriteFrameRes    sprite_frame_res;
UnitRes           unit_res;
MonsterRes        monster_res;
RawRes            raw_res;
RaceRes           race_res;
FirmRes           firm_res;
FirmDieRes			firm_die_res;
TownRes           town_res;
TownWallRes			town_wall_res;
HillRes           hill_res;
SnowRes           snow_res;
RockRes           rock_res;
ExploredMask      explored_mask;
SERes             se_res;
TalkRes           talk_res;
WallRes           wall_res;
ItemRes           item_res;
HeroRes				hero_res;
TipsRes				tips_res;

//--------- arrays initialized in Game::init -------//

NationArray       nation_array;
FirmArray         firm_array;
FirmDieArray		firm_die_array;
TownArray         town_array;
UnitArray         unit_array(100);        // 100-initial array size
BulletArray       bullet_array(100);
RebelArray        rebel_array;
SpyArray          spy_array;
SiteArray         site_array;
RockArray         rock_array;
RockArray         dirt_array;
SpriteArray       effect_array(50);
TornadoArray      tornado_array(10);
WarPointArray     war_point_array;
UnitGroup			unit_group, unit_group_land, unit_group_air, unit_group_remote;
UnitGroup			unit_group_array[MAX_DEFINED_UNIT_GROUP];
BaseObjArray		base_obj_array;
StateArray			state_array;
WayPointArray		way_point_array;

//-------- surface class initialized in Game::init -------//

Power             power;
World             world;
RegionArray       region_array;
Battle            battle;
NewsArray         news_array;
Info              info;
Weather           weather, weather_forecast[MAX_WEATHER_FORECAST];
Magic					magic;
MagicWeather      magic_weather;
RandomEvent			random_event;
SnowGroundArray   snow_ground_array;
OptionMenu			option_menu;
InGameMenu			in_game_menu;
ScenarioEditor		scenario_editor;

//--------------------------------------//

Game              game;

//--------------------------------------//

GameFile          game_file;

//--------------------------------------//

Sys               sys;

//--------------------------------------//



//----------- Global Variables -----------//

char game_design_mode=0;
char game_demo_mode=0;
char debug2_enable_flag=0;
File seedCompareFile;
char debug_seed_status_flag=0;
int  debug_sim_game_type = 0;
int  unit_search_node_used=0;
short nation_hand_over_flag=0;
int     unit_search_tries = 0;        // the number of tries used in the current searching
char    unit_search_tries_flag = 0;   // indicate num of tries is set, reset after searching

char 	new_config_dat_flag=0;

#ifdef DEBUG
int check_unit_dir1, check_unit_dir2;
unsigned long	last_unit_ai_profile_time = 0L;
unsigned long	unit_ai_profile_time = 0L;
unsigned long	last_unit_profile_time = 0L;
unsigned long	unit_profile_time = 0L;
unsigned long pathfind_profile_time = 0L;
unsigned long last_pathfind_profile_time = 0L;
// ######## begin Gilbert 27/4 #######//
unsigned long pathfind_count = 0L;
unsigned long last_pathfind_count = 0L;
unsigned long longest_pathfind_profile_time = 0L;
unsigned long last_longest_pathfind_profile_time = 0L;
unsigned long longest_pathfind_unit_recno = 0L;
unsigned long last_longest_pathfind_unit_recno = 0L;
// ######## end Gilbert 27/4 #######//
unsigned long	path_reuse_profile_time = 0L;
unsigned long last_path_reuse_profile_time = 0L;
unsigned long last_sprite_array_profile_time = 0L;
unsigned long sprite_array_profile_time = 0L;
unsigned long last_sprite_idle_profile_time = 0L;
unsigned long sprite_idle_profile_time = 0L;
unsigned long last_sprite_move_profile_time = 0L;
unsigned long sprite_move_profile_time = 0L;
unsigned long last_sprite_wait_profile_time = 0L;
unsigned long sprite_wait_profile_time = 0L;
unsigned long last_sprite_attack_profile_time = 0L;
unsigned long sprite_attack_profile_time = 0L;
unsigned long last_unit_attack_profile_time = 0L;
unsigned long unit_attack_profile_time = 0L;
unsigned long last_unit_assign_profile_time = 0L;
unsigned long unit_assign_profile_time = 0L;
unsigned long	last_unit_process_town_defender_profile_time = 0L;
unsigned long	unit_process_town_defender_profile_time = 0L;
unsigned long	last_unit_process_camp_defender_profile_time = 0L;
unsigned long	unit_process_camp_defender_profile_time = 0L;
unsigned long	last_unit_process_rebel_profile_time = 0L;
unsigned long	unit_process_rebel_profile_time = 0L;
unsigned long	last_unit_execute_move_profile_time = 0L;
unsigned long	unit_execute_move_profile_time = 0L;
unsigned long	last_unit_execute_attack_profile_time = 0L;
unsigned long	unit_execute_attack_profile_time = 0L;
unsigned long	last_unit_execute_build_firm_profile_time = 0L;
unsigned long	unit_execute_build_firm_profile_time = 0L;
unsigned long	last_unit_execute_settle_town_profile_time = 0L;
unsigned long	unit_execute_settle_town_profile_time = 0L;
unsigned long	last_unit_execute_assign_profile_time = 0L;
unsigned long	unit_execute_assign_profile_time = 0L; 
unsigned long	unit_check_build_firm_profile_time = 0L;
unsigned long	last_unit_check_build_firm_profile_time = 0L;
unsigned long	unit_cast_power_profile_time = 0L;
unsigned long	last_unit_cast_power_profile_time = 0L;
unsigned long	unit_transform_fortress_profile_time = 0L;
unsigned long	last_unit_transform_fortress_profile_time = 0L;


#endif


//------- Define static functions --------//

static void extra_error_handler();

//---------- Begin of function WinMain ----------//
//��
// WinMain - initialization, message loop
//
// Compilation constants:
//
// DEBUG  - normal debugging
// DEBUG2 - shortest path searching and unit action debugging
// DEBUG3 - debugging some functions (e.g. Location::get_loc()) which
//          will cause major slowdown.
//
int PASCAL WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance,
						  LPSTR lpCmdLine, int nCmdShow)
{
	//try to read from CONFIG.DAT, moved to AM.CPP

	// ####### patch begin Gilbert 14/1 ########//
	// validate
	if( !config.load("CONFIG.DAT") || !config.validate() )
	{
		new_config_dat_flag = 1;
		config.init();
   }
	// ####### patch end Gilbert 14/1 ########//

	// try to read serial_repository
	if( !serial_repository.load() )
		serial_repository.init();

	//--------------------------------------//

#ifdef IMAGICMP
	static char lobbyLaunchCmdLine[] = "IM";
#else
	static char lobbyLaunchCmdLine[] = "-!lobby!";
#endif

	//----------- play movie ---------------//

	sys.set_game_dir();
	if( strstr(lpCmdLine, lobbyLaunchCmdLine) == NULL )	// skip if launch from lobby
	{
		if( !m.is_file_exist("SKIPAVI.SYS") )
		{
//			play_video(hInstance, 0);
		}
	}

	if( !sys.init(hInstance) )
		return FALSE;

   err.set_extra_handler( extra_error_handler );   // set extra error handler, save the game when a error happens

	// ######## begin Gilbert 2/7 #######//
	if( strstr(lpCmdLine, lobbyLaunchCmdLine) != NULL )
	{
		mp_obj.pre_init();
		mp_obj.init_lobbied( MAX_NATION, lpCmdLine );
		if( mp_obj.init_flag )
		{
			// player register
			game.multi_player_menu(lpCmdLine);		// if detect launched from lobby
			mp_obj.deinit();
		}
		else
		{
			mp_obj.deinit();
			game.main_menu();
		}
	}
	else
	{
		game.main_menu();
	}
	// ######## end Gilbert 24/2 #######//

   sys.deinit();

   return 1;
}
//---------- End of function WinMain ----------//


//----------- Begin of function Msg ----------//
//
// Routine for displaying debug messages
//

#ifdef DEBUG

void __cdecl debug_msg( const char* fmt, ... )
{
    char buff[256];

    lstrcpy( buff, "SEVEN KINGDOMS: " );
    wvsprintf( buff+lstrlen(buff), fmt, (char*)(&fmt+1) );
    lstrcat( buff, "\r\n" );

    OutputDebugString( buff );
}

#endif

//----------- End of function Msg ----------//


//------- Begin of function extra_error_handler -----------//

static void extra_error_handler()
{
//	if( game.game_mode != GAME_SINGLE_PLAYER )
//		return;
//	game_file_array.save_new_game("ERROR.SAV");  // save a new game immediately without prompting menu
//	box.msg( "Error encountered. The game has been saved to ERROR.SAV" );
}
//----------- End of function extra_error_handler -------------//



