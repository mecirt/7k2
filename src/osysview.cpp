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

//Filename    : OSYSVIEW.CPP
//Description : class Sys - view report functions

#include <ovga.h>
#include <osys.h>
#include <oinfo.h>
#include <omodeid.h>
#include <oconfig.h>
#include <ofirm.h>
#include <otown.h>
#include <ospy.h>
#include <ofont.h>
#include <oimgres.h>
#include <oingmenu.h>
#include <otutor.h>
#include <otips.h>
// ##### begin Gilbert 13/10 ######//
#include <oseditor.h>
// ##### end Gilbert 13/10 ######//
#include <omouse.h>
#include <omousecr.h>
#include <ose.h>
#include <oseres.h>
#include <opower.h>
#include <ohelp.h>
#include <onews.h>
#include <ogame.h>
#include <oworldmt.h>
#include <oremote.h>
#include <oaudio.h>
#include <onation.h>
#include <ounit.h>
#include <oun_grp.h>
#include <ot_reps.h>

//--------- Begin of funtion Sys::disp_view_mode ---------//
//
// <int> observeMode		- force observe mode display (darken view mode 1 - 7)
//									needed in Game::game_end, nation_array.player_recno has not yet set to 0
void Sys::disp_view_mode(int observeMode)
{
	if (game.game_mode == GAME_TUTORIAL && 
		// (tutor.should_not_display_report_button_flag || 
		 tutor.cur_tutor_id == 2)
		return;

	// ------- display highlight ----------//
	const int MIN_MODE_TO_DISPLAY = 1;
	const int MAX_MODE_TO_DISPLAY = 8;
	const int MODE_TO_DISPLAY_COUNT = MAX_MODE_TO_DISPLAY - MIN_MODE_TO_DISPLAY + 1;

	static short darkenX[MODE_TO_DISPLAY_COUNT] = {  6,  82, 158, 234,   6,  82, 158, 234};
	static short darkenY[MODE_TO_DISPLAY_COUNT] = {  7,   7,   7,   7,  29,  29,  29,  29};

	const int darkenWidth = 75;
	const int darkenHeight = 21;
	char scrollName[] = "SR1024-B";
	
	for (int i = 0 ; i < 8 ; i ++)
	{
		image_button.put_back( darkenX[i], darkenY[i], "SR800-UP");
		font_bld.center_put(darkenX[i], darkenY[i], darkenX[i] + darkenWidth -1,
								 darkenY[i] + darkenHeight -3, text_reports.str_report_mode(i+1)); // scroll_name[i]);
	}
	// highlight of the mode after
	if( view_mode >= MIN_MODE_TO_DISPLAY && view_mode <= MAX_MODE_TO_DISPLAY )
	{
		image_button.put_back( darkenX[view_mode-1], darkenY[view_mode-1], "SR800-DW");
		font_bld.center_put(darkenX[view_mode-1]+1, darkenY[view_mode-1]+1, darkenX[view_mode-1] + darkenWidth,
								 darkenY[view_mode-1] + darkenHeight -2, text_reports.str_report_mode(view_mode));	// scroll_name[view_mode-1]);
	}

	// darken buttons of view mode 1-7 if nation_array.player_recno == 0
	if( observeMode || !nation_array.player_recno )
	{
		for( int j = 1; j <= 7; ++j )
		{
			vga_back.adjust_brightness(
				darkenX[j-MIN_MODE_TO_DISPLAY], darkenY[j-MIN_MODE_TO_DISPLAY],
				darkenX[j-MIN_MODE_TO_DISPLAY]+darkenWidth-1,
				darkenY[j-MIN_MODE_TO_DISPLAY]+darkenHeight-1, -5 );
		}
	}
}
//--------- End of funtion Sys::disp_view_mode ---------//


//-------- Begin of function Sys::disp_view --------//
//
// Display the view area.
//
void Sys::disp_view()
{
  disp_zoom();

  if( view_mode!=MODE_NORMAL )
  {
    //------- blt the zoom area to the front screen --------//

    Vga::opaque_flag = config.opaque_report;

    switch( view_mode )
    {
      case MODE_TRADE:
        info.disp_trade();
        break;

      case MODE_MILITARY:
        info.disp_military();
        break;

      case MODE_ECONOMY:
        info.disp_economy();
        break;

      case MODE_TOWN:
        info.disp_town();
        break;

      case MODE_NATION:
        info.disp_nation();
        break;

      case MODE_TECH:
        info.disp_tech();
        break;

      case MODE_SPY:
        info.disp_spy();
        break;

      case MODE_RANK:
        info.disp_rank();
        break;

      case MODE_NEWS_LOG:
        info.disp_news_log();
        break;

      case MODE_AI_ACTION:
        info.disp_ai_action();
        break;
    }
    Vga::opaque_flag = 0;
  }
  else
  {
    // display tips box
    tips_res.disp( ZOOM_X1+50, ZOOM_Y1+8);
  }

  if( scenario_editor.is_enable() )
  {
    scenario_editor.disp_view();
    scenario_editor.disp_menu_bar();
  }

  if( config.show_debug_info )
  {
    nation_array.draw_profile();
    firm_array.draw_profile();
    town_array.draw_profile();
    unit_array.draw_profile();
    unit_group.draw_profile();
  }

  if (in_game_menu.is_active())
    in_game_menu.disp();
}
//-------- End of function Sys::disp_view --------//

//-------- Begin of function Sys::detect_view --------//
//
void Sys::detect_view()
{
	int enableAction;			// some action is not enabled, when paused.
	enableAction = config.frame_speed > 0 || !remote.is_enable();

	if( enableAction )
		info.detect();

	switch( view_mode )
	{
		case MODE_TRADE:
			info.detect_trade();
			break;

		case MODE_MILITARY:
			info.detect_military();
			break;

		case MODE_ECONOMY:
			info.detect_economy();
			break;

		case MODE_TOWN:
			info.detect_town();
			break;

		case MODE_NATION:
			info.detect_nation();
			break;

		case MODE_TECH:
			info.detect_tech();
			break;

		case MODE_SPY:
			info.detect_spy();
			break;

		case MODE_RANK:
			info.detect_rank();
			break;

		case MODE_NEWS_LOG:
			info.detect_news_log();
			break;

		case MODE_AI_ACTION:
			info.detect_ai_action();
			break;
	}

	if( view_mode == MODE_NORMAL && scenario_editor.is_enable() )
	{
		if( scenario_editor.detect_menu_bar() || scenario_editor.detect_view() )
			return;
	}

	//------ detect tutorial controls -------//

	if( view_mode==MODE_NORMAL && game.game_mode==GAME_TUTORIAL )		// tutorial text is only displayed in non-report mode
	{
		if( tutor.detect() )
			return;
	}

	//---- no normal news when the game is displaying the news log ----//

	if( news_array.detect() )
		return;

	//---- pressing right button in command mode -> cancel command mode ----//

	if( mouse.any_click(RIGHT_BUTTON) && power.command_id )
	{
		power.command_id = 0;
		mouse.reset_click();
		info.disp();
		return;
	}

	//-------- detect world ----------//

	if( world.detect() )
		return;

	//----- detect right mouse button to select defined unit groups -----//

	if( !(mouse.event_skey_state & SHIFT_KEY_MASK) )
	{
//		if( mouse.press_area( ZOOM_X1, ZOOM_Y1, ZOOM_X2, ZOOM_Y2, RIGHT_BUTTON) )
		if( mouse.any_click( ZOOM_X1, ZOOM_Y1, ZOOM_X2, ZOOM_Y2, RIGHT_BUTTON) )
		{
//			if( power.detect_select(mouse.cur_x, mouse.cur_y, mouse.cur_x, mouse.cur_y, 1, 0) )		// 1 - recall group
			if( power.detect_select(mouse.click_x(RIGHT_BUTTON), mouse.click_y(RIGHT_BUTTON), 
				mouse.click_x(RIGHT_BUTTON), mouse.click_y(RIGHT_BUTTON), 1, 0) )		// 1 - recall group
				return;
		}
	}

	//-------- detect main zoom area -------------//

	if( view_mode == MODE_NORMAL )
	{
		if( world.detect_firm_town() )
			return;

		//------ detect selecting objects and laying tracks ------//

		if( power.detect_frame() )
			return;
	}
	else
	{
		mouse_cursor.set_frame(0);
	}

	//----------- detect action ------------//

	if( enableAction && power.detect_action() )
	{
		if(unit_array.selected_recno )
			unit_array[unit_array.selected_recno]->sound("ACK");
		return;
	}
}
//-------- End of function Sys::detect_view --------//


//-------- Begin of function Sys::set_view_mode --------//
//
// <int> viewMode 			 - id. of the view mode.
// [int] viewingNationRecno - which nation the player is viewing at with the reports.
//										(default: the player nation)
// [int] viewingSpyRecno 	 - >0 if the spy is viewing secret reports of other nations
//
void Sys::set_view_mode(int viewMode, int viewingNationRecno, int viewingSpyRecno)
{
	if( view_mode == viewMode )
		return;

	info.display_hot_key = 0;
	//---- if the player's kingdom has been destroyed ----//

	err_when( viewingNationRecno && nation_array.is_deleted(viewingNationRecno) );

	if( nation_array.is_deleted(info.default_viewing_nation_recno) )
	{
		if( viewMode != MODE_NORMAL && viewMode != MODE_RANK )		// other reports are not available except the normal and rank report
			return;
	}

	//---- a spy is exposed when he has finished viewing the secrets ----//

	if( info.viewing_spy_recno )
	{
		if( !spy_array.is_deleted(info.viewing_spy_recno) )
		{
			Spy* spyPtr = spy_array[info.viewing_spy_recno];

			int needViewSecretSkill = spy_array.needed_view_secret_skill(info.viewing_spy_recno);
			int escapeChance = spyPtr->spy_skill - needViewSecretSkill;
			int killFlag = 0;

			if( escapeChance > 0 )
			{
				if( m2.random( escapeChance/15 )==0  )		// use m2 instead of m to maintain mulitplayer sync
					killFlag = 1;
			}

			if( killFlag )
				spyPtr->set_exposed(COMMAND_PLAYER);
		}

		info.viewing_spy_recno = 0;
	}

	//----------------------------------------------------//

	if( viewMode == MODE_NORMAL )
	{
		info.viewing_nation_recno = info.default_viewing_nation_recno;
	}
	else
	{
		if( viewingNationRecno )
			info.viewing_nation_recno = viewingNationRecno;
		else
			info.viewing_nation_recno = info.default_viewing_nation_recno;

		info.viewing_spy_recno = viewingSpyRecno;
	}

	view_mode = viewMode;
	disp_view_mode();

	disp_view();
}
//--------- End of function Sys::set_view_mode ---------//

