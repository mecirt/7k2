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

//Filename    : OSYSDISP.CPP
//Description : class Sys - display functions

#include <osys.h>
#include <ovga.h>
#include <oinfo.h>
#include <obox.h>
#include <omodeid.h>
#include <oconfig.h>
#include <oingmenu.h>
#include <ooptmenu.h>
#include <obutt3d.h>
#include <ofirm.h>
#include <ocampgn.h>
#include <otown.h>
#include <olog.h>
#include <otornado.h>
#include <owarpt.h>
#include <osite.h>
#include <ofont.h>
#include <omusic.h>
#include <obutton.h>
#include <omouse.h>
#include <ohelp.h>
#include <owaypnt.h>
#include <oanline.h>
#include <onews.h>
#include <ogame.h>
#include <oworldmt.h>
#include <oremote.h>
#include <oaudio.h>
#include <onation.h>
#include <ovgalock.h>
#include <otutor.h>
#include <ot_gmenu.h>

//---------- define static variables ----------//

static Button3D  button_menu;

//-------- Begin of function Sys::disp_button --------//
//
void Sys::disp_button()
{
	if (current_display_mode.mode_id == MODE_ID_800x600x16)
		button_menu.paint( MENU_BUTTON_X1, MENU_BUTTON_Y1, "MENU-U", "MENU-D" );
	else
		button_menu.paint( MENU_BUTTON_X1, MENU_BUTTON_Y1, "MU_1024", "MD_1024" );
	button_menu.set_help_code( "GAMEMENU" );
}
//--------- End of function Sys::disp_button ---------//


//-------- Begin of function Sys::detect_button --------//
//
void Sys::detect_button()
{
	//--------- detect menu button -------//

	if( button_menu.detect() )
	{
		in_game_menu.enter(!remote.is_enable());
		return;
	}

	//------- detect scroll menu buttons -----//

	#define VIEW_MODE_BUTTON_WIDTH   75 //58
	#define VIEW_MODE_BUTTON_HEIGHT  22 //16

	int VIEW_MODE_BUTTON_X_SPACE; 
	int i, x, y;
	if (current_display_mode.mode_id == MODE_ID_800x600x16)
	{
		VIEW_MODE_BUTTON_X_SPACE = VIEW_MODE_BUTTON_WIDTH + 2;
		x = 6;
		y = 7;
	}
	else if (current_display_mode.mode_id == MODE_ID_1024x768x16)
	{
		VIEW_MODE_BUTTON_X_SPACE = VIEW_MODE_BUTTON_WIDTH + 37;
		x = 819;
		y = 80;
	}
	else
		err_here();

	
	static char viewModeArray[] =
	{
		MODE_NATION, MODE_TOWN, MODE_ECONOMY, MODE_TRADE, MODE_MILITARY, MODE_TECH, MODE_SPY, MODE_RANK
	};
	for( i=0 ; i<8 ; i++, x+=VIEW_MODE_BUTTON_X_SPACE )
	{
		if(( ((i-1) % 2) == 1) && (current_display_mode.mode_id == MODE_ID_1024x768x16))
		{
			x = 819;
			y = y + VIEW_MODE_BUTTON_HEIGHT;
		}

		if(( i==4 ) && (current_display_mode.mode_id == MODE_ID_800x600x16))		// the second row
		{
			x = 6;
			y = 29;
		}

		if( nation_array.player_recno==0 && i<7 )		// when the player has lost the game, the only report available is ranking report only
			continue;

		if( mouse.single_click( x, y, x+VIEW_MODE_BUTTON_WIDTH-1, y+VIEW_MODE_BUTTON_HEIGHT-1 ) )
		{
			int newMode = viewModeArray[i];

			if( view_mode == newMode )       	// when click on the same mode again, go to the normal mode
				set_view_mode(MODE_NORMAL);
			else
				set_view_mode(newMode);

			break;
		}
	}
}
//--------- End of function Sys::detect_button ---------//


//-------- Begin of function Sys::disp_frame --------//
//
// [int] dispCampaignMsg - whether display a campaign message in the beginning of the game or not 
//									(default: 0)
// 
void Sys::disp_frame(int dispCampaignMsg)
{
	if( sys.signal_exit_flag )
		return;

#ifdef DEBUG
	unsigned long startTime = m.get_time();
	unsigned long updateViewTime = 0;
	unsigned long infoUpdateTime = 0;
	unsigned long dispMapTime = 0;
	unsigned long flipTime = 0;
#endif

	if( option_menu.is_active() )
	{
		option_menu.disp(1);
	}
	else
	{
			info.disp_panel();
			world.paint();
			disp_button();
			world.refresh();
			disp_view();
			disp_map();

			if( in_game_menu.is_active() )
				in_game_menu.disp();

			//------ if this is a campaign game -------//

			if( dispCampaignMsg )
			{
				//---- display campaign game intro ----//

				game.campaign()->disp_in_game_msg( game.campaign()->intro_text() );

				//------- display plot ---------//

				// ######## begin Gilbert 1/4 #########//
				if( game.campaign()->plot_id )
				{
					char *plotTextPtr = game.campaign()->plot_text();
					if( plotTextPtr )
					{
					//	vga.flip();
						game.campaign()->disp_in_game_msg( plotTextPtr );
					}
				}
				// ######## end Gilbert 1/4 #########//
			}

			//------------------------------------//

			info.update();

			disp_view_mode();

			info.disp();

			if( game.game_mode == GAME_TUTORIAL )
				tutor.disp();

		//--------- display the map and info area --------//

		help.disp();
        }
	#ifdef DEBUG
	flipTime = m.get_time();
	#endif
	vga.flip();
	#ifdef DEBUG
	flipTime = m.get_time() - flipTime;
	#endif

	anim_line.inc_phase();		// originally in Sys::process()

#ifdef DEBUG
	startTime = m.get_time() - startTime;
//	{
//		VgaFrontLock vgaLock;
		startTime = 0;			// set break point here
//	}
#endif
}
//-------- End of function Sys::disp_frame --------//


//-------- Begin of function Sys::disp_map --------//

void Sys::disp_map()
{
	//------ draw and display the map -------//

	if( map_need_redraw )		// requested by other modules to redraw the pre-drawn map background
	{
		world.map_matrix->draw();
		map_need_redraw = 0;
	}

	world.map_matrix->disp();

	//-------- draw dots on the map ---------//

	MapMatrix* mapMatrix = world.map_matrix;

	if( !mapMatrix->filter_object_flag || mapMatrix->filter_object_type == OBJ_FIRM )
		firm_array.draw_dot(mapMatrix->filter_object_para);

	if( !mapMatrix->filter_object_flag || mapMatrix->filter_object_type == OBJ_TOWN )
		town_array.draw_dot(mapMatrix->filter_object_para);

	if( !mapMatrix->filter_object_flag && !mapMatrix->filter_nation_flag )
		site_array.draw_dot();

	if( !mapMatrix->filter_object_flag || mapMatrix->filter_object_type == OBJ_UNIT )
	{
		unit_array.draw_dot(mapMatrix->filter_object_para);
		way_point_array.draw(1);		// draw on the mini-map
	}

	war_point_array.draw_dot();	// draw on the mini-map

	if( !mapMatrix->filter_object_flag && !mapMatrix->filter_nation_flag )
		tornado_array.draw_dot();

	mapMatrix->draw_square();		// draw a square on the map for current zoomed area
}
//-------- End of function Sys::disp_map --------//


//-------- Begin of function Sys::disp_zoom --------//

void Sys::disp_zoom()
{
	//--------- set zoom window ----------//

	ZoomMatrix* zoomMatrix = world.zoom_matrix;

	//--------- draw map area ---------//

	if( zoom_need_redraw )		// requested by other modules to redraw the pre-drawn zoom background
	{
		long backupSeed = m.get_random_seed();

		// #### begin Gilbert 8/2 #####//
		world.zoom_matrix->just_drawn_flag = 0;	// request world.zoom_matrix->disp() to call draw()
		// #### end Gilbert 8/2 #####//
		// world.zoom_matrix->draw();
		zoom_need_redraw = 0;
	}

	//-------- disp zoom area --------//

#ifdef DEBUG
	unsigned long dispTime = m.get_time();
#endif
	world.zoom_matrix->disp();
#ifdef DEBUG
	dispTime = m.get_time() - dispTime;
#endif

	//------- draw foreground objects --------//

#ifdef DEBUG
	unsigned long drawFrameTime = m.get_time();
#endif
	world.zoom_matrix->draw_frame();
#ifdef DEBUG
	drawFrameTime = m.get_time() - drawFrameTime;
#endif


#ifdef DEBUG
	unsigned long miscTime = m.get_time();
#endif

	//----- draw the frame of the selected firm/town -----//

//	info.draw_selected();		// moved to world_z.cpp

	//-------- display news messages ---------//

	news_array.disp();

	//----- next frame, increase the frame counter -----//

	sys.frames_in_this_second++;		// no. of frames displayed in this second

	if( view_mode==MODE_NORMAL )
		disp_frames_per_second();

#ifdef DEBUG
	miscTime = m.get_time() - miscTime;
	vga_buffer.temp_unlock();
	// set break point here
	vga_buffer.temp_restore_lock();
#endif
}
//-------- End of function Sys::disp_zoom --------//


//-------- Begin of function Sys::disp_frames_per_second --------//
//
void Sys::disp_frames_per_second()
{
	if( !config.show_debug_info )			// only display this in a debug session
		return;

	if( game.game_mode == GAME_TUTORIAL )		// don't display in tutorial mode as it overlaps with the tutorial text
		return;

	//------- get the curren system time ---------//

	DWORD curTime = m.get_time();		// in millisecond

	//----------- first time calling -------------//

	if( last_second_time==0 )
	{
		last_second_time  = curTime;
		frames_in_this_second = 0;		// no. of frames displayed in this second
		return;
	}

	//------ when passes to the next second -----//

	if( curTime >= last_second_time+1000 )  // 1 second = 1000 millisecond
	{
		frames_per_second = frames_in_this_second;

		//------ update var and reset counter -----//

		last_second_time += 1000;
		frames_in_this_second = 0;
	}

	//---------- display frame count -----------//

	String str;

	str  = "Frames per second: ";
	str += frames_per_second;

	font_news.disp( ZOOM_X1+10, ZOOM_Y1+10, str, MAP_X2);
}
//--------- End of function Sys::disp_frames_per_second ---------//


//-------- Begin of function Sys::change_mode --------//

int Sys::change_display_mode(int modeId)
{
	if( modeId == current_display_mode.mode_id )
		return 0;
	
	UCHAR	tempVgaLockStackCount = vga_buffer.lock_stack_count;
	WORD	tempVgaLockBitStack =   vga_buffer.lock_bit_stack;
	BOOL    tempVgaBufLocked = vga_buffer.buf_locked;

	if( mouse.init_flag )
	{
		mouse.hide();
	}

	// unlock surface

	if( vga_buffer.vptr_dd_buf && vga_buffer.buf_locked )
	{
		DEBUG_LOG("Attempt vga_buffer.unlock_buf()");
		vga_buffer.unlock_buf();
		DEBUG_LOG("vga_buffer.unlock_buf() finish");
	}

	// deinit surface

	DEBUG_LOG("Attempt vga_buffer.deinit()");
	vga_buffer.deinit();
   DEBUG_LOG("vga_buffer.deinit() finish");

	// ##### begin Gilbert 30/10 ######//
	// show mouse
	ShowMouseCursor(true);
	// ##### end Gilbert 30/10 ######//

	// change display mode

	int oldMode = current_display_mode.mode_id;
	DisplayModeInfo *displayModeInfo = DisplayModeInfo::get_display_info(modeId);
	int rc = 0;

	if( displayModeInfo )
	{
		DEBUG_LOG("Attemp vga.set_mode()");
		if( vga.set_mode(displayModeInfo->screen_width, displayModeInfo->screen_height) )
			rc = 1;
		DEBUG_LOG("Attemp vga.set_mode() finish");
	}

	if( rc )
	{
		DisplayModeInfo::set_current_display_mode(modeId);
	}
	else
	{
		// cannot change mode, remain at the same mode
		modeId = oldMode;
		displayModeInfo = DisplayModeInfo::get_display_info(modeId);
	}

	// ##### begin Gilbert 30/10 ######//
	// show mouse
	ShowMouseCursor(false);
	// ##### end Gilbert 30/10 ######//

	// re-create surface

  init_display();

	if( mouse.init_flag )
	{
		mouse.reset_boundary();		// since screen dimension changed
		mouse.show();
	}

	// update the boundary of anim_line
   anim_line.init(ZOOM_X1, ZOOM_Y1, ZOOM_X2, ZOOM_Y2);

	vga_buffer.lock_stack_count = tempVgaLockStackCount;
	vga_buffer.lock_bit_stack =   tempVgaLockBitStack;
	if ((tempVgaBufLocked) && (!vga_buffer.buf_locked))
		vga_buffer.lock_buf();
	if ((!tempVgaBufLocked) && (vga_buffer.buf_locked))
		vga_buffer.unlock_buf();

	return rc;
}
//-------- End of function Sys::change_mode --------//


// ###### begin Gilbert 4/11 ######//
// ------ begin of function Sys::capture_screen ------//
//
void Sys::capture_screen()
{
   String str("7K");

   int i;
   for( i=0 ; i<=99 ; i++ )
   {
      str  = "7K";

      if( i<10 )
         str += "0";

      str += i;
      str += ".BMP";

      if( !m.is_file_exist(str) )
         break;
   }

   if( i>99 )        // all file names from DWORLD00 to DWORLD99 have been occupied
      return;

   vga_buffer.write_bmp_file(str);

   //------ display msg --------//

	box.msg( text_game_menu.str_capture_screen((char *)str) );
}
// ------ end of function Sys::capture_screen ------//
// ###### end Gilbert 4/11 ######//
