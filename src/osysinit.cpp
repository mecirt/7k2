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

//Filename    : OSYSINIT.CPP
//Description : class Sys - initialization functions

#include <resource.h>
#include <osys.h>
#include <omodeid.h>
#include <oconfig.h> 
#include <omouse.h>
#include <omousecr.h>
#include <ogame.h>
#include <oflame.h>
#include <ohelp.h>
#include <ogameset.h>
#include <opfind.h>
#include <oanline.h>
#include <ogfile.h>
#include <oimgres.h>
#include <oraceres.h>
#include <omonsres.h>
#include <ogodres.h>
#include <oherores.h>
#include <otechres.h>
#include <otutor.h>
#include <oaudio.h>
#include <omusic.h>
#include <ose.h>
#include <ofont.h>
#include <olog.h>
#include <ovga.h>
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


// -------- define constant -----//

#define USE_TRUE_FRONT_BUFFER 1

//----------- Begin of function Sys::Sys -----------//

Sys::Sys()
{
   memset(this, 0, sizeof(Sys) );

   common_data_buf = mem_add( COMMON_DATA_BUF_SIZE );

   view_mode = MODE_NORMAL;         // the animation mode
}
//----------- End of function Sys::Sys -----------//


//----------- Begin of function Sys::~Sys -----------//

Sys::~Sys()
{
   deinit();
   mem_del(common_data_buf);
}
//----------- End of function Sys::~Sys -----------//


//------------ Begin of function Sys::init ----------//
//
int Sys::init( HANDLE hInstance )
{
   err_when( init_flag );

   //------- initialize basic vars --------//

   app_hinstance = hInstance;

	// ##### begin Gilbert 15/2 ########//
#ifdef USE_FLIP
	use_true_front         = USE_TRUE_FRONT_BUFFER;
#else
	use_true_front         = debug_session;
#endif
	// ##### begin Gilbert 15/2 ########//

	set_game_dir();      // set game directories names and game version

   //------- initialize more stuff ---------//

   if( !init_win() )
      return FALSE;

   if( !init_directx() )
      return FALSE;

   if( !init_objects() )   // initialize system objects which do not change from games to games.
      return FALSE;

   init_flag = 1;

   return TRUE;
}
//------------ End of function Sys::init ----------//


//-------- Begin of function Sys::deinit --------//
//
// Finished with all objects we use; release them
//
void Sys::deinit()
{
   if( !init_flag )
      return;

	signal_exit_flag = 1;

   game.deinit();    // actually game.deinit() will be called by main_win_proc() and calling it here will have no effect

   deinit_objects();

   //-------------------------------------//

   if( vga_back.buf_locked )
      vga_back.unlock_buf();

   if( vga_front.buf_locked )
      vga_front.unlock_buf();

   init_flag = 0;
   CloseMainWindow();
}
//--------- End of function Sys::deinit ---------//


//-------- Begin of function Sys::init_win --------//
//
int Sys::init_win()
{
  return CreateMainWindow();
}
//-------- End of function Sys::init_win --------//


//-------- Begin of function Sys::init_directx --------//
//
int Sys::init_directx()
{
   DEBUG_LOG("Attempt audio.init()");
   audio.init();
   DEBUG_LOG(audio.wav_init_flag);
   music.init();
   se_ctrl.init();

   //---------------------------------------//

   ShowMouseCursor(false);

	DisplayModeInfo::set_current_display_mode(MODE_ID_DEFAULT);

   //-------- initialize DirectDraw --------//

   DEBUG_LOG("Attempt vga.init()");
   if( !vga.init() )
      return FALSE;
   DEBUG_LOG("vga.init() ok");

  init_display();

   return TRUE;
}
//-------- End of function Sys::init_directx --------//

int Sys::init_display()
{

//   if( sys.debug_session )                // if we are currently in a debug session, don't lock the front buffer otherwise the system will hang up
   if( use_true_front )                // if we are currently in triple buffer mode, don't lock the front buffer otherwise the system will hang up
   {
      DEBUG_LOG("Attempt vga_true_front.init_front()");
      vga_true_front.init_front();
      DEBUG_LOG("Attempt vga_front.init_back()");
      vga_front.init_back();		// create in video memory
      vga_front.is_front = 1;       // set it to 1, overriding the setting in init_back()
		DEBUG_LOG("Attempt vga_back.init_back()");
		vga_back.init_back();
		DEBUG_LOG("vga_back.init_back() finish");
   }
   else
   {
      vga_front.init_front();
#if(!defined(USE_FLIP))
		vga_back.init_back();		// create in system memory
#else
		vga_back.init_back( 0, 0, 1 );		// create in video memory
#endif
   }

#if(defined(USE_FLIP))
	vga_front.attach_surface( &vga_back );
#endif

   DEBUG_LOG("Attempt vga_front.lock_buf()");
   vga_front.lock_buf();
   DEBUG_LOG("vga_front.lock_buf() finish");

   DEBUG_LOG("Attempt vga_back.lock_buf()");
   vga_back.lock_buf();
   DEBUG_LOG("vga_back.lock_buf() finish");

   return 1;
}

//-------- Begin of function Sys::deinit_directx --------//
//
void Sys::deinit_directx()
{
   if( vga_back.vptr_dd_buf && vga_back.buf_locked )
   {
      DEBUG_LOG("Attempt vga_back.unlock_buf()");
      vga_back.unlock_buf();
      DEBUG_LOG("vga_back.unlock_buf() finish");
   }

   if( vga_front.vptr_dd_buf && vga_front.buf_locked )
   {
      DEBUG_LOG("Attempt vga_front.unlock_buf()");
      vga_front.unlock_buf();
      DEBUG_LOG("vga_front.unlock_buf() finish");
   }

	// usually get surface lost error
	//vga_front.detach_surface( &vga_back );

   DEBUG_LOG("Attempt vga_back.deinit()");
   vga_back.deinit();
   DEBUG_LOG("vga_back.deinit() finish");

//   if( sys.debug_session )
   if( use_true_front )
   {
      DEBUG_LOG("Attempt vga_true_front.deinit()");
      vga_true_front.deinit();
      DEBUG_LOG("vga_true_front.deinit() finish");
   }

   DEBUG_LOG("Attempt vga_front.deinit()");
   vga_front.deinit();
   DEBUG_LOG("Attempt vga_front.deinit() finish");

   DEBUG_LOG("Attempt vga.deinit()");
   vga.deinit();
   DEBUG_LOG("vga.deinit() finish");

   //------------------------------//

   se_ctrl.deinit();
   music.deinit();
   DEBUG_LOG("Attempt audio.deinit()");
   audio.deinit();
   DEBUG_LOG("audio.deinit() finish");
}
//--------- End of function Sys::deinit_directx ---------//


//------- Begin of function Sys::init_objects -----------//
//
// Initialize system objects which do not change from games to games.
//
int Sys::init_objects()
{
   //--------- init system class ----------//

	game.init_remap_table();			// moved from Game::Game since vga haven't been initializeded there

   mouse_cursor.init();
   mouse_cursor.set_frame_border(ZOOM_X1,ZOOM_Y1,ZOOM_X2,ZOOM_Y2);

   mouse.init( app_hinstance, NULL);
   FocusMainWindow();

   //------- init resource class ----------//

	// ####### begin Gilbert 24/5 ########//
	font_san.init("SAN", 0);      // 0-zero inter-character space
	font_std.init("STD", 2);
	font_small.init("SMAL");
	font_mid.init("MID");
	font_news.init("NEWS");
	font_hitpoint.init("HITP");
	font_bible.init("CASA", 1, 3);
	font_bard.init("CASA", 0);

	font_whbl.init("WHBL", 1);
	font_zoom.init("WHBL", 1);
	font_blue.init("BLUE");

	font_snds.init("SNDS", 0);			// orange background
	font_blu2.init("BLU2", 0);		
	font_red.init("RED", 0);		
	font_gren.init("GREN", 0);		
	font_cara.init("CARA", 0);			// white background of font_snds
	font_cara_w.init("CARA", 1);		// wide space font_cara

	font_bld.init("BLD", 0);
	font_bld.space_width -= 2;		// reduce the width of space for this font

	font_vill.init("VILL", 0);
	font_vilb.init("VILB", 0);
	font_town.init("TOWN", 0);

	font_thin_black.init( "TBLK", 3 );		// wider for the width to close to bold_black
	font_thin_black.space_width -= 2;		// reduce the width of space for this font
	font_bold_black.init( "BBLK", 1 );
	font_bold_black.space_width -= 2;		// reduce the width of space for this font
	font_bold_red.init( "BRED", 1 );
	font_bold_red.space_width -= 2;		// reduce the width of space for this font

	font_sop.init("BOOT", 0);

	font_tut.init("TUT", 0);

	font_scre.init("SCRE", 0);		// ending score screen 
	font_cmpa.init("CMPA", 1, 1);		// italic font
	font_cmph.init("CMPH", 1, 1);		// italic font
	font_cmpf.init("CMPF", 1);
	font_cmpo.init("CMPO", 1);
	font_cmpo.space_width += 2;		// reduce the width of space for this font

	font_hall.init("HALL", 1);
	// ####### end Gilbert 24/5 ########//

   image_icon.init(DIR_RES"I_ICON.RES",1,0);       // 1-read into buffer
   image_interface.init(DIR_RES"I_IF.RES",0,0);    // 0-don't read into the buffer, don't use common buffer

   #ifndef DEMO         // do not load these in the demo verison
      image_menu.init(DIR_RES"I_MENU.RES",0,0);       // 0-don't read into the buffer, don't use common buffer
   // image_encyc.init(DIR_RES"I_ENCYC.RES",0,0); // 0-don't read into the buffer, don't use common buffer
   #endif

	image_gameif.init(DIR_RES"I_GAMEIF.RES",1,0);
   image_button.init(DIR_RES"I_BUTTON.RES",1,0);
   image_spict.init(DIR_RES"I_SPICT.RES",1,0);
	image_tutorial.init(DIR_RES"TUT_PICT.RES",0,0);

	// ------ init text resource ------- //

	text_basic.init();
	text_game_menu.init();
	text_tutorial.init();
	text_campaign.init();
	text_unit.init();
	text_firm.init();
	text_end_condition.init();
	text_reports.init();
	text_item.init();
	text_talk.init();
	text_news.init();
	text_editor.init();

	// --------------------------------//

	path_finder.init();

	//------------ init flame ------------//

   for(int i = 0; i < FLAME_GROW_STEP; ++i)
      flame[i].init(Flame::default_width(i), Flame::default_height(i), Flame::base_width(i), FLAME_WIDE);

   //------------ init animated line drawer -------//

	anim_line.init(ZOOM_X1, ZOOM_Y1, ZOOM_X2, ZOOM_Y2);

   //---------- init other objects ----------//

   game_set.init();     // this must be called before game.init() as game.init() assume game_set has been initialized

	game_set.open_set(1);             // open the default game set

	unit_res.init();
	race_res.init();		// race_res is initialized here instead of in race_res because campaign will use it (before a game is started and game.init() is called.)
	monster_res.init();
	god_res.init();
	hero_res.init();
	tech_res.init();

	//----------------------------------------//

	help.init("help.res");

	tutor.init();

	//---------- init game_set -----------//

	DEBUG_LOG("Sys::init_objects finish");

	return TRUE;
}
//------- End of function Sys::init_objects -----------//


//------- Begin of function Sys::deinit_objects -----------//

void Sys::deinit_objects()
{
   //--------- deinit system class ----------//

   mouse.deinit();            // mouse must be deinitialized first
   mouse_cursor.deinit();

   //------- deinit resource class ----------//

	font_san.deinit();
	font_std.deinit();
	font_small.deinit();
	font_mid.deinit();
   font_news.deinit();

   font_hitpoint.deinit();
   font_bible.deinit();
   font_bard.deinit();

	font_whbl.deinit();
	font_zoom.deinit();
	font_blue.deinit();

   font_snds.deinit();
	font_blu2.deinit();
	font_red.deinit();
	font_gren.deinit();
	font_cara.deinit();
	font_cara_w.deinit();

	font_bld.deinit();

   font_vill.deinit();
	font_vilb.deinit();
	font_town.deinit();

	font_thin_black.deinit();
	font_bold_black.deinit();
	font_bold_red.deinit();

	font_sop.deinit();

	font_tut.deinit();

	font_scre.deinit();
	font_cmpa.deinit();
	font_cmph.deinit();
	font_cmpf.deinit();
	font_cmpo.deinit();
	
	font_hall.deinit();

   image_gameif.deinit();
	image_icon.deinit();
   image_interface.deinit();
   image_menu.deinit();
   image_button.deinit();
   image_spict.deinit();
// image_encyc.deinit();
   image_tutorial.deinit();

	// ------ deinit text resource ------- //

	text_basic.deinit();
	text_game_menu.deinit();
	text_tutorial.deinit();
	text_campaign.deinit();
	text_unit.deinit();
	text_firm.deinit();
	text_end_condition.deinit();
	text_reports.deinit();
	text_item.deinit();
	text_talk.deinit();
	text_news.deinit();
	text_editor.deinit();

	// -----------------------------------//

	path_finder.deinit();

	//-------- deinit flame ----------//

	for(int i = 0; i < FLAME_GROW_STEP; ++i)
		flame[i].deinit();

	//--------- deinit other objects ----------//

	game_set.deinit();

	unit_res.deinit();
	race_res.deinit();		// race_res is initialized here instead of in race_res because campaign will use it (before a game is started and game.init() is called.)
	monster_res.deinit();
	god_res.deinit();
	hero_res.deinit();
	tech_res.deinit();

	//-----------------------------//

	help.deinit();
	tutor.deinit();
   config.deinit();
   game_file_array.deinit();
}
//------- End of function Sys::deinit_objects -----------//


//-------- Begin of function Sys::set_game_dir ----------//
//
// Set all game directories.
//
void Sys::set_game_dir()
{
   //------- If it should run from the CDROM ------//

   get_cdrom_drive();

	set_one_dir( "Image/Halfame1.jpg"       , "Image/" , dir_image );
	set_one_dir( "movie/intro.mpg"          , "movie/" , dir_movie );
	set_one_dir( "Campaign/EAST.TXR"	     , "Campaign/", dir_campaign );

   set_one_dir( "Music/Norman.wav"      , "Music/", dir_music );
	set_one_dir( "Tutorial/Standard.tut" , "Tutorial/", dir_tutorial );
   set_one_dir( "Scenario/scenario.sys"    , "Scenario/", dir_scenario );

	// ####### begin Gilbert 26/5 #########//
#if(MAX_SCENARIO_PATH >= 2)
   set_one_dir( "Scenario/Premade/scenario.sys"    , "Scenario/Premade/", dir_scenario_path[1] );
#endif
	// ####### end Gilbert 26/5 #########//

   //-------- set game version ---------//

            game_version = VERSION_FULL;     // single player game is not available when game_version == VERSION_FULL
}
//----------- End of function Sys::set_game_dir ----------//


//-------- Begin of function Sys::set_one_dir ----------//
//
int Sys::set_one_dir( const char* checkFileName, const char* defaultDir, char* trueDir )
{
   if( m.is_file_exist( checkFileName ) )
   {
      strcpy( trueDir, defaultDir );
   }
   else
   {
      if( cdrom_drive )
      {
         strcpy( trueDir, "D:\\" );
         strcat( trueDir, defaultDir );

         trueDir[0] = cdrom_drive;
      }
      else
      {
         strcpy( trueDir, "" );
         return 0;
      }
   }

   return 1;
}
//----------- End of function Sys::set_one_dir ----------//


//-------- Start of function Sys::get_cdrom_drive -------------//
//
// Get the drive letter of the CDROM and restore the result in cdrom_drive.
//
void Sys::get_cdrom_drive()
{
  cdrom_drive = 0;  // no CD
}
//--------- End of function Sys::get_cdrom_drive ---------------//

//-------- Begin of function Sys::pause --------//
//
void Sys::pause()
{
   if( paused_flag )
      return;

   InvalidateMainWindow();

   paused_flag = TRUE;
}
//--------- End of function Sys::pause ---------//



