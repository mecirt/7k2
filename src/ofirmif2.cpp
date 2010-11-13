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

//Filename    : OFIRMIF2.CPP
//Description : Firm interface functions - part 2

#include <ofirm.h>
#include <oinfo.h>
#include <ovga.h>
#include <omodeid.h>
#include <osys.h>
#include <ospy.h>
#include <ostr.h>
#include <ohelp.h>
#include <ofont.h>
#include <omouse.h>
#include <ovbrowif.h>
#include <ogame.h>
#include <onation.h>
#include <obutt3d.h>
#include <oimgres.h>
#include <orawres.h>
#include <oraceres.h>
// #include <oworld.h>
#include <ounit.h>
#include <oremote.h>

//------------- Define coordinations -----------//

//enum { SPY_BROWSE_X1 = INFO_X1,
//		 SPY_BROWSE_Y1 = INFO_Y1+75,
//		 SPY_BROWSE_X2 = INFO_X2,
//		 SPY_BROWSE_Y2 = SPY_BROWSE_Y1+130,
//	  };
#define SPY_BROWSE_X1 INFO_X1
#define SPY_BROWSE_Y1 (INFO_Y1+75)
#define SPY_BROWSE_X2 INFO_X2
#define SPY_BROWSE_Y2 (SPY_BROWSE_Y1+130)

#define BUTTON_X1 INFO_X1
#define BUTTON_Y1 (SPY_BROWSE_Y2+28)
#define BUTTON_X2 INFO_X2
#define BUTTON_Y2 (BUTTON_Y1+50)

//----------- Define static variables ----------//

static VBrowseIF  browse_spy;
static Button3D   button_spy_menu, button_spy_mobilize, button_spy_action, button_spy_reward,
						button_bribe, button_assassinate, button_capture, button_view_secret, button_cancel;
static Firm*      firm_ptr;

//----------- Define static functions ----------//

static int  spy_filter(int recNo=0);
static void put_spy_rec(int recNo, int x, int y, int refreshFlag);
static int  get_player_spy_recno(int firmRecno);

//--------- Begin of function Firm::disp_spy_menu ---------//
//
void Firm::disp_spy_menu(int refreshFlag)
{
	static int lastSpyCount;

	firm_ptr = this;

	disp_basic_info(INFO_Y1, refreshFlag);

	//---------- paint controls -----------//

	if( refreshFlag == INFO_REPAINT )
	{
		//------ display browser field description -------//

		int x=SPY_BROWSE_X1+2;
		int y=SPY_BROWSE_Y1-23;

		vga.d3_panel_up( SPY_BROWSE_X1, y, SPY_BROWSE_X2, SPY_BROWSE_Y1-3 );

		font_san.put( x+4  , y+4, "Spy Skill" );
		font_san.put( x+70 , y+4, "Loyalty" );
		font_san.put( x+130, y+4, "Action" );

		//------------ create browser ------------//

		browse_spy.init( SPY_BROWSE_X1, SPY_BROWSE_Y1, SPY_BROWSE_X2, SPY_BROWSE_Y2,
							  0, 25, player_spy_count, put_spy_rec );

		browse_spy.open(1);

		lastSpyCount = player_spy_count;

		err_when( player_spy_count != spy_filter() );
	}
	else
	{
		//---------- update controls -----------//

		if( player_spy_count != lastSpyCount )
		{
			lastSpyCount = player_spy_count;
			info.disp();
			return;
		}
		else
			browse_spy.update();
	}

	if( spy_filter()==0 )
		return;

	//----------- create the paint button ----------//

	if( refreshFlag == INFO_REPAINT )
	{
		int x=BUTTON_X1;
		int y=SPY_BROWSE_Y2+5;

		//--------- spy menu mode -----------//

		if( firm_menu_mode == FIRM_MENU_SPY )
		{
			//--------- mobilize spy button --------//

			button_spy_mobilize.paint( x, y, 'A', "MOBILSPY" );
			x+=BUTTON_ACTION_WIDTH;

			//--------- reward spy button --------//

			button_spy_reward.paint( x, y, 'A', "REWARD" );
			x+=BUTTON_ACTION_WIDTH;

			//------ change spy action button ------//

			if( firm_id != FIRM_INN && nation_recno != nation_array.player_recno )		// cannot change action in inns
			{
				button_spy_action.paint( x, y, 'A', "CHG-TASK" );
				x+=BUTTON_ACTION_WIDTH;
			}
			else
				button_spy_action.reset();

			//---------- capture button -----------//

			if( can_player_spy_capture() )
			{
				button_capture.paint( x, y, 'A', "SPYCAPT" );
				x+=BUTTON_ACTION_WIDTH;

				if( x+BUTTON_ACTION_WIDTH-5 > INFO_X2 )
				{
					x  = BUTTON_X1;
					y += BUTTON_ACTION_HEIGHT;
				}
			}
			else
				button_capture.reset();

			//---------- view secret button -----------//

			if( nation_recno && nation_recno != nation_array.player_recno )
			{
				button_view_secret.paint( x, y, 'A', "VSECRET" );
				x+=BUTTON_ACTION_WIDTH;

				if( x+BUTTON_ACTION_WIDTH-5 > INFO_X2 )
				{
					x  = BUTTON_X1;
					y += BUTTON_ACTION_HEIGHT;
				}
			}
			else
				button_view_secret.reset();

			//---------- assassination button -----------//

			if( nation_recno && nation_recno != nation_array.player_recno &&
				 firm_res[firm_id]->need_overseer )
			{
				button_assassinate.paint( x, y, 'A', "ASSASSIN" );
				x+=BUTTON_ACTION_WIDTH;

				if( x+BUTTON_ACTION_WIDTH-5 > INFO_X2 )
				{
					x  = BUTTON_X1;
					y += BUTTON_ACTION_HEIGHT;
				}
			}
			else
				button_assassinate.reset();
		}

		//--------- select briber mode --------//

		else if( firm_menu_mode == FIRM_MENU_SELECT_BRIBER )
		{
			button_bribe.paint( x, y, 'A', "BRIBE" );
			x+=BUTTON_ACTION_WIDTH;
		}
		else
			err_here();

		//----------- cancel button -----------//

		button_cancel.paint( x, y, 'A', "PREVMENU" );
	}

	//---- enable/disable view secret button ----//

	//### begin alex 20/3 ###//
	if( button_view_secret.init_flag && firm_menu_mode==FIRM_MENU_SPY)
	//#### end alex 20/3 ####//
	{
		Spy* spyPtr = spy_array[ spy_filter( browse_spy.recno() ) ];

		if( tech_res[TECH_INFO_THEFT]->get_nation_tech_level(spyPtr->true_nation_recno) 
			&& spyPtr->spy_skill >= MIN_VIEW_SECRET_SPYING_SKILL )
			button_view_secret.enable();
		else
			button_view_secret.disable();
	}

	//---- enable/disable assassinate button ----//

	if( button_assassinate.init_flag )
	{
		if( overseer_recno &&
			 unit_array[overseer_recno]->true_nation_recno() != nation_array.player_recno )		// don't assassinate your own spy
		{
			button_assassinate.enable();
		}
		else
		{
			button_assassinate.disable();
		}
	}
}
//----------- End of function Firm::disp_spy_menu -----------//


//--------- Begin of function Firm::can_player_spy_capture ---------//
//
// The player's spy can only capture a firm if the player's spy
// is the commander of the firm.
//
// return: <int> 0  - if the player can't capture this firm
//					  >0 - the recno of the player's spy who can capture this
//							 firm.
//
int Firm::can_player_spy_capture()
{
	if( nation_recno == nation_array.player_recno )		// this is the player's own firm.
		return 0;

	firm_ptr = this;

	if( spy_filter()==0 )
		return 0;

	//---- if the overseer is one of the player's spies -----//

	if( overseer_recno )
	{
		int spyRecno = unit_array[overseer_recno]->spy_recno;

		if( spyRecno )
		{
			if( spy_array[spyRecno]->true_nation_recno == nation_array.player_recno )
			{
				return spyRecno;
			}
		}
	}

	return 0;
}
//----------- End of function Firm::can_player_spy_capture -----------//


//--------- Begin of function Firm::detect_spy_menu ---------//
//
void Firm::detect_spy_menu()
{
	firm_ptr = this;

	if( spy_filter()==0 )
		return;

	browse_spy.detect();

	Spy* spyPtr = spy_array[ spy_filter( browse_spy.recno() ) ];

	//--------- detect buttons --------//

	//--------- spy menu mode -----------//

	if( firm_menu_mode == FIRM_MENU_SPY )
	{
		//------ mobilize spy ---------//

		if( button_spy_mobilize.detect() )
		{
			if( !remote.is_enable() )
			{
				if( spyPtr->mobilize_firm_spy() )
				{
					spyPtr->notify_cloaked_nation_flag = 0;		// reset it so the player can control it
					info.disp();
					return;
				}
			}
			else
			{
				// packet structure <spy recno>
				short *shortPtr = (short *)remote.new_send_queue_msg(MSG_SPY_LEAVE_FIRM, sizeof(short) );
				*shortPtr = spyPtr->spy_recno;
			}
		}

		//------ reward spy ---------//

		else if( button_spy_reward.detect() )
		{
			spyPtr->reward(COMMAND_PLAYER);
		}

		//------- change spy action ---------//

		else if( button_spy_action.detect() )		// set action mode
		{
			if( !remote.is_enable() )
			{
				spyPtr->set_next_action_mode();
				disp_spy_menu( INFO_UPDATE );
			}
			else
			{
				// packet structure <spy recno>
				short *shortPtr = (short *)remote.new_send_queue_msg(MSG_SPY_CYCLE_ACTION, sizeof(short) );
				*shortPtr = spyPtr->spy_recno;
			}
		}

		//------ capture firm ---------//

		else if( button_capture.detect() )
		{
			int spyRecno = can_player_spy_capture();

			if( spyRecno )
			{
				Spy* capturerSpy = spy_array[spyRecno];

				if( !remote.is_enable() )
				{
					capturerSpy->capture_firm();
				}
				else
				{
					// packet structure <spy recno>
					short *shortPtr = (short *)remote.new_send_queue_msg(MSG_SPY_CAPTURE_FIRM, sizeof(short) );
					*shortPtr = capturerSpy->spy_recno;
				}
			}
		}

		//------ view secret ---------//

		else if( button_view_secret.detect() )
		{
			action_spy_recno = spyPtr->spy_recno;
			firm_menu_mode   = FIRM_MENU_VIEW_SECRET;
			info.disp();
		}

		//-------- assassinate ------//

		else if( button_assassinate.detect() )
		{
			spyPtr->assassinate( overseer_recno, COMMAND_PLAYER );
		}
	}

	//--------- select briber mode --------//

	else if( firm_menu_mode == FIRM_MENU_SELECT_BRIBER )
	{
		if( button_bribe.detect() )
		{
			action_spy_recno = spyPtr->spy_recno;
			firm_menu_mode   = FIRM_MENU_SET_BRIBE_AMOUNT;
			info.disp();
		}
	}
	else
	{
		err_here();
	}

	//--------- detect cancel button --------//

	if( button_cancel.detect() )
	{
		firm_menu_mode = FIRM_MENU_MAIN;
		info.disp();
	}
}
//----------- End of function Firm::detect_spy_menu -----------//


//--------- Begin of function spy_filter ---------//
//
static int spy_filter(int recNo)
{
	Spy* spyPtr;
	int  curFirmRecno = firm_ptr->firm_recno;
	int  recCount=0;

	for( int i=spy_array.size() ; i>=1 ; i-- )
	{
		if( spy_array.is_deleted(i) )
			continue;

		spyPtr = spy_array[i];

		if( spyPtr->spy_place==SPY_FIRM &&
			 spyPtr->spy_place_para==curFirmRecno &&
			 spyPtr->true_nation_recno==nation_array.player_recno )
		{
			recCount++;
		}

		if( recNo && recCount==recNo )
			return i;
	}

	err_when( recNo );

	return recCount;
}
//----------- End of function spy_filter -----------//


//-------- Begin of static function put_spy_rec --------//
//
static void put_spy_rec(int recNo, int x, int y, int refreshFlag)
{
	int x2 = x+browse_spy.rec_width-1;

	//-------- display icon of the spy unit -----//

	Spy* spyPtr = spy_array[ spy_filter(recNo) ];

	//-------- get the rank of the spy --------//

	int unitId = race_res[spyPtr->race_id]->basic_unit_id;
	int rankId = RANK_SOLDIER;

	if( spyPtr->spy_place == SPY_FIRM )
	{
		int unitRecno = firm_array[spyPtr->spy_place_para]->overseer_recno;

		if( unitRecno && unit_array[unitRecno]->spy_recno == spyPtr->spy_recno )
			rankId = RANK_GENERAL;
	}

	//---------- display unit icon ---------//

	if( refreshFlag == INFO_REPAINT )
	{
		vga.d3_panel_down(x+1, y+1, x+UNIT_SMALL_ICON_WIDTH+4, y+UNIT_SMALL_ICON_HEIGHT+4 );

		vga_front.put_bitmap(x+3, y+3, unit_res[unitId]->get_small_icon_ptr(rankId) );
	}

	//--------- set help parameters --------//

	if( mouse.in_area(x+1, y+1, x+RACE_ICON_WIDTH+4, y+RACE_ICON_HEIGHT+4) )
	{
		help.set_unit_help( unitId, rankId, x+1, y+1, x+RACE_ICON_WIDTH+4, y+RACE_ICON_HEIGHT+4 );
	}

	//-------- display spy skill -------//

	font_san.put( x+40, y+6, spyPtr->spy_skill, 1, x+66 );

	//-------- display spy loyalty -------//

	font_san.put( x+67, y+6, spyPtr->spy_loyalty, 1, x+94 );

	//------ display the action mode of the spy ------//

	vga.blt_buf( x+95, y+6, x2, y+5+font_san.height(), 0 );

   font_san.use_max_height();
	font_san.center_put( x+95, y+6, x2, y+5+font_san.height(), spyPtr->action_str() );
	font_san.use_std_height();
}
//----------- End of static function put_spy_rec -----------//


//--------- Begin of function Firm::disp_spy_button --------//
//
void Firm::disp_spy_button(int x, int y, int refreshFlag)
{
	if( !is_own() )		// if not own firm, there is not other button other than the spy button, so always display it left-aligned
		x = INFO_X1;		// if own firm, the x passed will be space position on the interface already, taking into account of the other buttons on interface

	//-------------------------------------//

	if( player_spy_count > 0 )
	{
		//------------ spy menu -----------//

		if( refreshFlag == INFO_REPAINT )
			button_spy_menu.paint( x, y, 'A', "SPYMENU" );

		x += BUTTON_ACTION_WIDTH;

		//---------- bribe button -----------//

		if( nation_recno != nation_array.player_recno ) 	// only display the bribe button for non-player owned towns
		{
			int canBribe=0;

			if( selected_worker_id )
				canBribe = can_spy_bribe(selected_worker_id, nation_array.player_recno );

			else if( overseer_recno )
				canBribe = can_spy_bribe(0, nation_array.player_recno );

			//-------- display the bribe button -------//

			if( refreshFlag == INFO_REPAINT )
				button_bribe.paint( x, y, 'A', "SELBRIBE" );

			if( canBribe )
				button_bribe.enable();
			else
				button_bribe.disable();

			x += BUTTON_ACTION_WIDTH;
		}
	}
}
//--------- End of function Firm::disp_spy_button --------//


//--------- Begin of function Firm::detect_spy_button --------//

void Firm::detect_spy_button()
{
	if( player_spy_count>0 )
	{
		if( button_spy_menu.detect() )
		{
			firm_menu_mode = FIRM_MENU_SPY;
			info.disp();
		}

		if( nation_recno != nation_array.player_recno )		// only display the bribe button for non-player towns
		{
			if( button_bribe.detect() )
			{
				if( player_spy_count > 1 )
				{
					firm_menu_mode = FIRM_MENU_SELECT_BRIBER;
				}
				else
				{
					action_spy_recno = get_player_spy_recno(firm_recno);		// the player has only one spy in this firm
					firm_menu_mode   = FIRM_MENU_SET_BRIBE_AMOUNT;
				}

				info.disp();
			}
		}
	}
}
//--------- End of function Firm::detect_spy_button --------//


//--------- Begin of function get_player_spy_recno --------//
//
static int get_player_spy_recno(int firmRecno)
{
	int  spyCount=spy_array.size();
	Spy* spyPtr;

	for( int i=1 ; i<=spyCount ; i++ )
	{
		if( spy_array.is_deleted(i) )
			continue;

		spyPtr = spy_array[i];

		if( spyPtr->spy_place==SPY_FIRM &&
			 spyPtr->spy_place_para==firmRecno &&
			 spyPtr->true_nation_recno==nation_array.player_recno )
		{
			return i;
		}
	}

	return 0;
}
//------- End of function Firm::get_player_spy_recno ---------//

