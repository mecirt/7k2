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

//Filename    : OR_NAT.CPP
//Description : Nation Report

#include <stdlib.h>
#include <win32_compat.h>
#include <key.h>
#include <oconfig.h>
#include <oraceres.h>
#include <omonsres.h>
#include <ovga.h>
#include <omodeid.h>
#include <ofont.h>
#include <ostr.h>
#include <ogeta.h>
#include <odate.h>
#include <omouse.h>
#include <oimgres.h>
#include <ovbrowif.h> 
#include <obutton.h>
#include <osys.h>
#include <onation.h>
#include <otalkres.h>
#include <oinfo.h>
#include <onews.h>
#include <oremote.h>
// #include <oworldmt.h>
#include <ot_reps.h>
#include <ot_talk.h>

//------------- Define coordinations -----------//

//enum { REPORT_BROWSE_X1 = ZOOM_X1+6,
//		 REPORT_BROWSE_Y1 = ZOOM_Y1+6,
//		 REPORT_BROWSE_X2 = ZOOM_X2-6,
//		 REPORT_BROWSE_Y2 = REPORT_BROWSE_Y1+206,

//		 REPORT_BUTTON_X1 = ZOOM_X1+8,
//		 REPORT_BUTTON_Y1 = REPORT_BROWSE_Y2+6,
//		 REPORT_BUTTON_WIDTH = 135,
//		 REPORT_BUTTON_Y2 = REPORT_BUTTON_Y1+20,
//		 REPORT_BUTTON_X_SPACE = 130,
//
//		 REPORT_DET_X1 = ZOOM_X1+6,
//		 REPORT_DET_Y1 = REPORT_BUTTON_Y2+4,
//		 REPORT_DET_X2 = ZOOM_X2-6,
//		 REPORT_DET_Y2 = ZOOM_Y2-6,
//		 REPORT_TALK_Y2 = ZOOM_Y2-86,		// use a shorter height for the diplomacy menu as it needs to let the player looks at the messages received.
//	 };

#define REPORT_BROWSE_X1 (ZOOM_X1+6)
#define REPORT_BROWSE_Y1 (ZOOM_Y1+6)
#define REPORT_BROWSE_X2 (ZOOM_X2-6)
#define REPORT_BROWSE_Y2 (REPORT_BROWSE_Y1+206)

#define REPORT_BUTTON_X1 (ZOOM_X1+8)
#define REPORT_BUTTON_Y1 (REPORT_BROWSE_Y2+6)
#define REPORT_BUTTON_WIDTH 135
#define REPORT_BUTTON_Y2 (REPORT_BUTTON_Y1+20)
#define REPORT_BUTTON_X_SPACE 130

#define REPORT_DET_X1 (ZOOM_X1+6)
#define REPORT_DET_Y1 (REPORT_BUTTON_Y2+4)
#define REPORT_DET_X2 (ZOOM_X2-6)
#define REPORT_DET_Y2 (ZOOM_Y2-6)
#define REPORT_TALK_Y2 (ZOOM_Y2-86)		// use a shorter height for the diplomacy menu as it needs to let the player looks at the messages received.

#define RECIPIENT_BUTTON_X1 (REPORT_DET_X1 + 10)
#define RECIPIENT_BUTTON_Y1 (REPORT_TALK_Y2 - 50)
#define RECIPIENT_BUTTON_HEIGHT 20
#define RECIPIENT_BUTTON_Y2 (RECIPIENT_BUTTON_Y1+RECIPIENT_BUTTON_HEIGHT-1)

//---------- Define report mode strings ---------//

static char report_mode_disp_order[MAX_NATION_REPORT_MODE] =
{
	NATION_REPORT_TALK,
	NATION_REPORT_INFO,
	NATION_REPORT_CHAT,
	NATION_REPORT_TALK_LOG,
	NATION_REPORT_DEBUG
};

//static char* report_mode_str_array[MAX_NATION_REPORT_MODE] =
//{
//	"Information",
//	"Diplomacy",
//	"Chat",
//	"Diplomatic Log",
//	"Debug",
//};

//---------- Define chat receiver str strings ---------//

//static char* chat_receiver_str_array[MAX_CHAT_RECEIVER_TYPE] =
//{
//	"Send to the selected kingdom",
//	"Send to all allied human controlled kingdoms",
//	"Send to all human controlled kingdoms",
//};

//----------- Define static variables ----------//

static VBrowseIF 	 browse_nation, browse_talk_msg;
static ButtonGroup button_allow_attack(2);
static ButtonGroup button_chat_receiver(MAX_CHAT_RECEIVER_TYPE);

// -------- chat variables -------- //
static int init_get_chat = 0;
static GetA 		 get_chat;
static int hide_nation_browse = 0;		// only hide in chat mode
//static Button button_hide_nation_browse;
//static Button button_receiver_all;
//static Button button_receiver_allies;
//static ButtonCustom button_receiver[MAX_NATION];
// ##### patch begin Gilbert 26/8 ########//
static int last_nation_report_mode = 0;
// ##### end begin Gilbert 26/8 ########//


//----------- Define static functions ----------//

static void put_nation_rec(int recNo, int x, int y);
static void put_talk_msg_rec(int recNo, int x, int y);
// #### begin Gilbert 19/4 #######//
// static int  nation_filter(int recNo=0);
// #### end Gilbert 19/4 #######//
static void disp_button();
static int  detect_button();
static void disp_detail(int refreshFlag);
static void detect_detail();
static void disp_nation_info();
static void detect_nation_info();
static void disp_debug_info();
static void disp_nation_talk();
static void detect_nation_talk();
static void disp_nation_chat(int refreshFlag);
static void detect_nation_chat();
static void disp_talk_msg_sent(int refreshFlag);

static int  sort_talk_msg( const void *a, const void *b );

//--------- Begin of function Info::disp_nation ---------//
//
// <int> refreshFlag - INFO_REPAINT - the user has just switched
//												  to this report.
//							  INFO_UPDATE  - the report is already in this mode,
//												  just keep displaying and updating.
//
void Info::disp_nation(int refreshFlag)
{
	int hideNationBrowse = nation_report_mode == NATION_REPORT_CHAT
		&& hide_nation_browse;

	//--------- Create nation income and expenses browsers -------//

	if( !hideNationBrowse )
	{
		int x=REPORT_BROWSE_X1+9, y=REPORT_BROWSE_Y1+4;

		vga_back.d3_panel_up(REPORT_BROWSE_X1, REPORT_BROWSE_Y1, REPORT_BROWSE_X2, REPORT_BROWSE_Y1+33 );

		font_bld.put( x	 , y+7, text_reports.str_nat_kingdom()); // "Kingdom" );
		font_bld.right_put( x+235, y+7, text_reports.str_nat_reputation());
		font_bld.put( x+265, y+7, text_reports.str_nat_status()); // "Status" );

		font_bld.put_paragraph( x+343, y, x+405, y+28, text_reports.str_nat_allow_attack(), 0 );

		font_bld.put_paragraph( x+405, y, x+465, y+28, text_reports.str_nat_trade_treaty(), 0 );
		font_bld.put_paragraph( x+465, y, x+525, y+28, text_reports.str_nat_trade_amount(), 0 );
	}

	if( refreshFlag == INFO_REPAINT )
	{
		browse_nation.init( REPORT_BROWSE_X1, REPORT_BROWSE_Y1+35, REPORT_BROWSE_X2, REPORT_BROWSE_Y2,
								  0, 20, info.nation_filter(), put_nation_rec, 1 );

		if( !hideNationBrowse )
			browse_nation.open(browse_nation_recno);
	}
	else
	{
		if( !hideNationBrowse )
		{
			browse_nation.paint();
			browse_nation.open(browse_nation_recno, nation_filter());
		}
	}

	if( !hideNationBrowse )
		browse_nation_recno = browse_nation.recno();
	else
	{
		int nCount;
		if( browse_nation_recno > (nCount = nation_filter()) )
			browse_nation_recno = nCount;		// avoid browse_nation_recno too large
	}


	//------- disp report buttons --------//

	disp_button();

	//--------- display detail info --------//

	if( refreshFlag==INFO_REPAINT )
	{
		info.last_talk_nation_recno = 0;
		browse_talk_msg_recno = 1;
	}

	disp_detail(refreshFlag);
}
//----------- End of function Info::disp_nation -----------//


//--------- Begin of function Info::detect_nation ---------//
//
void Info::detect_nation()
{
	int hideNationBrowse = nation_report_mode == NATION_REPORT_CHAT
		&& hide_nation_browse;

	//------- detect nation browser ------//

	if( !hideNationBrowse && browse_nation.detect() )
	{
		browse_nation_recno = browse_nation.recno();
		return;
	}

	//------- detect report buttons --------//

	if( detect_button() )
		return;

	//--------- detect detail info -------//

	detect_detail();
}
//----------- End of function Info::detect_nation -----------//


//--------- Begin of function Info::init_player_reply ---------//
//
// This function is called by TalkRes::player_reply() when the
// player is going to reply to a diplomatic message from another
// nation.
//
// <int> talkToNationRecno - recno of the nation the player is
//									  going to talk to.
//
void Info::init_player_reply(int talkToNationRecno)
{
	nation_report_mode = NATION_REPORT_TALK;

	info.player_reply_mode = 1;

	//--- go to the nation in the browser which the player is going to talk to ---//

	int nationCount = nation_filter();

	int i;
	for( i=1 ; i<=nationCount ; i++ )
	{
		if( nation_filter(i) == talkToNationRecno )
		{
			browse_nation_recno = i;
			break;
		}
	}

	err_when( i>nationCount );

	//----- if currently it's in nation report mode -----//

	if( sys.view_mode == MODE_NATION )
		browse_nation.refresh(browse_nation_recno);
}
//----------- End of function Info::init_player_reply -----------//


//-------- Begin of static function nation_filter --------//
//
// This function has dual purpose :
//
// 1. when <int> recNo is not given :
//    - return the total no. of nations of this nation
//
// 2. when <int> recNo is given :
//    - return the nation recno in nation_array of the given recno.
//
int Info::nation_filter(int recNo)
{
	int    	i, nationCount=0;
//	Nation*  viewingNation = nation_array[info.viewing_nation_recno];

	Nation*  viewingNation = NULL;
	
	if( nation_array.player_recno )
		viewingNation = nation_array[info.viewing_nation_recno];

	for( i=1 ; i<=nation_array.size() ; i++ )
	{
		if( nation_array.is_deleted(i) )
			continue;

		if( i==info.viewing_nation_recno ||
			 !viewingNation ||
			 viewingNation->get_relation(i)->has_contact )
		{
			nationCount++;
		}

		if( recNo && nationCount==recNo )
			return i;
	}

	err_when( recNo );   // the recNo is not found, it is out of range

	return nationCount;
}
//----------- End of static function nation_filter -----------//


//-------- Begin of static function put_nation_rec --------//
//
static void put_nation_rec(int recNo, int x, int y)
{
	int	  nationRecno = info.nation_filter(recNo);
	Nation* nationPtr   = nation_array[nationRecno];
	Nation* viewingNation = nation_array[info.viewing_nation_recno];
	NationRelation* nationRelation = viewingNation->get_relation(nationRecno);

	x+=3;
	// ###### begin Gilbert 19/10 ######//
	y+=3;

	nationPtr->disp_nation_color(x, y+3);
	// ###### end Gilbert 19/10 ######//

	font_bld.put( x+20 , y, nationPtr->nation_name(), 0, x+215 );

	if( nationPtr->is_human() )
		font_bld.put( x+220, y, m.format((int) nationPtr->reputation) );

	//------- display relation parameters --------//

	if( nationRecno != info.viewing_nation_recno )
	{
		String str;

		str = nationRelation->status_str();

		if( config.show_ai_info )
		{
			if( nationPtr->nation_type == NATION_AI )
			{
				str += " (";
				str += nationRelation->ai_relation_level;
				str += ")";
			}
		}

		font_bld.put( x+262, y, str, 0, x+350 );
		// font_bld.put( x+355, y, nationRelation->should_attack ? "Yes" : "No" );
		font_bld.put( x+355, y, text_reports.str_no_yes(nationRelation->should_attack) );
		// font_bld.put( x+412, y, nationRelation->trade_treaty ? "Yes" : "No" );
		font_bld.put( x+412, y, text_reports.str_no_yes(nationRelation->trade_treaty) );

		// ###### begin Gilbert 11/5 ########//
		// if the player has lost connection, display "disconnect" instead
		if( remote.is_enable() && nationPtr->is_ai() && nationPtr->player_id != 0 )
		{
			font_red.put( x+465, y, text_reports.str_disconnect() ); //"Disconnect" );
		}
		else
		{
			font_bld.put( x+465, y, m.format( (int) viewingNation->total_year_trade(nationRecno),2) );
		}
		// ###### end Gilbert 11/5 ########//

		if( config.show_debug_info )
		{
			font_bld.put( x+510, y, m.format(nationPtr->get_relation(info.viewing_nation_recno)->ai_relation_level) );
			font_bld.put( x+180, y, m.format( (int) nationPtr->get_relation(info.viewing_nation_recno)->good_relation_duration_rating) );
		}
	}
}
//----------- End of static function put_nation_rec -----------//


//--------- Begin of static function disp_button ---------//
//
static void disp_button()
{
	int x=REPORT_BUTTON_X1;

	// order changed
	for( int j=1 ; j<=MAX_NATION_REPORT_MODE ; j++ )
	{
		int i = report_mode_disp_order[j-1];

		if( i==NATION_REPORT_DEBUG )
		{
			if( remote.is_enable() ||
				 ( !sys.debug_session && !sys.testing_session ) )
			{
				continue;
			}
		}
		else if( !sys.debug_session && i==NATION_REPORT_CHAT )
		{
			if( !remote.is_enable() )
				continue;
		}

		//-----------------------------------------//

		if( info.nation_report_mode == i )
			vga.d3_panel_down( x, REPORT_BUTTON_Y1, x+REPORT_BUTTON_WIDTH-1, REPORT_BUTTON_Y2 );
		else
			vga.d3_panel_up( x, REPORT_BUTTON_Y1, x+REPORT_BUTTON_WIDTH-1, REPORT_BUTTON_Y2 );

		font_bld.center_put( x, REPORT_BUTTON_Y1, x+REPORT_BUTTON_WIDTH-1, REPORT_BUTTON_Y2, 
			text_reports.str_nat_sub_mode(i) ); // report_mode_str_array[i-1] );

		x+=REPORT_BUTTON_WIDTH;
	}
}
//----------- End of static function disp_button -----------//


//--------- Begin of static function detect_button ---------//
//
static int detect_button()
{
	int x=REPORT_BUTTON_X1;

	for( int j=1 ; j<=MAX_NATION_REPORT_MODE ; j++ )
	{
		int i = report_mode_disp_order[j-1];

		if( i==NATION_REPORT_DEBUG )
		{
			if( remote.is_enable() ||
				 ( !sys.debug_session && !sys.testing_session ) )
			{
				continue;
			}
		}
		else if( !sys.debug_session && i==NATION_REPORT_CHAT )
		{
			if( !remote.is_enable() )
				continue;
		}

		//-----------------------------------------//

		if( mouse.single_click( x, REPORT_BUTTON_Y1, x+REPORT_BUTTON_WIDTH-1, REPORT_BUTTON_Y2 ) )
		{
			info.nation_report_mode = i;
			return 1;
		}

		x+=REPORT_BUTTON_WIDTH;
	}

	return 0;
}
//----------- End of static function detect_button -----------//


//--------- Begin of static function disp_detail ---------//
//
static void disp_detail(int refreshFlag)
{
	if( browse_nation.recno()==0 )		// no records in the list
		return;

	// ##### patch begin Gilbert 26/8 ########//
	if( refreshFlag == INFO_REPAINT
		|| info.nation_report_mode != last_nation_report_mode )
	{
		refreshFlag = INFO_REPAINT;
		last_nation_report_mode = info.nation_report_mode;
	}
	// ##### end begin Gilbert 26/8 ########//

	switch( info.nation_report_mode )
	{
		case NATION_REPORT_INFO:
			disp_nation_info();
			break;

		case NATION_REPORT_TALK:
			disp_nation_talk();
			break;

		case NATION_REPORT_CHAT:
			disp_nation_chat(refreshFlag);
			break;

		case NATION_REPORT_TALK_LOG:
			disp_talk_msg_sent(refreshFlag);
			break;

		case NATION_REPORT_DEBUG:
			disp_debug_info();
			break;
	}
}
//----------- End of static function disp_detail -----------//


//--------- Begin of static function detect_detail ---------//
//
static void detect_detail()
{
	switch( info.nation_report_mode )
	{
		case NATION_REPORT_INFO:
			detect_nation_info();
			break;

		case NATION_REPORT_TALK:
			detect_nation_talk();
			break;

		case NATION_REPORT_CHAT:
			detect_nation_chat();
			break;

		case NATION_REPORT_TALK_LOG:
			if( browse_talk_msg.detect() )
				info.browse_talk_msg_recno = browse_talk_msg.recno();
			break;
	}
}
//----------- End of static function detect_detail -----------//


//--------- Begin of static function disp_nation_info ---------//
//
static void disp_nation_info()
{
	vga.d3_panel_down( REPORT_DET_X1, REPORT_DET_Y1, REPORT_DET_X2, REPORT_DET_Y2 );

	//----------- display info ------------//

	int	  nationRecno  = info.nation_filter(browse_nation.recno());
	Nation* nationPtr    = nation_array[nationRecno];
	Nation* viewingNation = nation_array[info.viewing_nation_recno];
	NationRelation* nationRelation = viewingNation->get_relation(nationRecno);

	int x1=REPORT_DET_X1+6, x2=REPORT_DET_X1+380, x3=REPORT_DET_X2-10;
	int y=REPORT_DET_Y1+6;
	const int lineSpacing = 19;

	//---- display the nationality of the king ----//

	if( nationPtr->is_human() )
		font_bld.put_field( x1, y, text_reports.str_king_race(nationPtr->race_id), x2, 
		nationPtr->is_human() ? race_res[nationPtr->race_id]->name : monster_res[nationPtr->monster_id()]->name );

	y += lineSpacing;

		//-------- display economic data ----------//

	String str;

	if( nationRecno == info.viewing_nation_recno )
	{
		// font_bld.put_field( x1, y, "Your Food", x2, nationPtr->food_str() );
		font_bld.put_field( x1, y, text_reports.str_your_food(), x2, nationPtr->food_str() );

		str  = "$";
		str += nationPtr->cash_str();

		font_bld.put_field( x1, y+=lineSpacing, text_reports.str_your_treasure(), x2, str );

		if( nationPtr->is_monster() )
			font_bld.put_field( x1, y+=lineSpacing, text_reports.str_your_live_points(), x2, (int) nationPtr->live_points );

		font_bld.field( x1, y+=lineSpacing, text_reports.str_your_peace_time(), x2, nationPtr->peace_duration_str(), x3, INFO_REPAINT, "PEACE" );
	}
	else
	{
		//str  = translate.process("Your Yearly Import from ");
		//str += nationPtr->nation_name();
		// font_bld.field( x1, y    , str, x2, (int) viewingNation->get_relation(nationRecno)->import_365days(IMPORT_TOTAL), 2, x3, INFO_REPAINT, "IMPORT" );
		font_bld.field( x1, y    , text_reports.str_enemy_import_from(nationPtr->nation_name()),
			x2, (int) viewingNation->get_relation(nationRecno)->import_365days(IMPORT_TOTAL), 2, x3, INFO_REPAINT, "IMPORT" );

		// str  = translate.process("Your Yearly Export to ");
		// str += nationPtr->nation_name();
		// font_bld.field( x1, y+=lineSpacing, str, x2, (int) nationPtr->get_relation(info.viewing_nation_recno)->import_365days(IMPORT_TOTAL), 2, x3, INFO_REPAINT, "EXPORT" );
		font_bld.field( x1, y+=lineSpacing, text_reports.str_enemy_export_to(nationPtr->nation_name()),
			x2, (int) nationPtr->get_relation(info.viewing_nation_recno)->import_365days(IMPORT_TOTAL), 2, x3, INFO_REPAINT, "EXPORT" );

		// str  = translate.process("Continuous Peace Time of ");
		// str += nationPtr->nation_name();
		// font_bld.field( x1, y+=lineSpacing, str, x2, nationPtr->peace_duration_str(), x3, INFO_REPAINT, "PEACE" );
		font_bld.field( x1, y+=lineSpacing, text_reports.str_enemy_peace_time(nationPtr->nation_name()),
			x2, nationPtr->peace_duration_str(), x3, INFO_REPAINT, "PEACE" );

		//--------- duration of current status ----------//

//		str  = translate.process("Duration of ");
//		str += translate.process(nationRelation->status_str());
//		str += " Status";
//		font_bld.field( x1, y+=lineSpacing, str, x2, nationRelation->status_duration_str(), x3, INFO_REPAINT, "STATTIME" );
		font_bld.field( x1, y+=lineSpacing, text_reports.str_enemy_status_duration(nationRelation->status_str()),
			x2, nationRelation->status_duration_str(), x3, INFO_REPAINT, "STATTIME" );

		//------- display the allow_attack field --------//

		// str  = "Allow Your Units to Attack ";
		// str += nationPtr->nation_name();
		// font_bld.field( x1, y+=lineSpacing, str, x2, "", x3, INFO_REPAINT, "ALLOWATK" );
		font_bld.field( x1, y+=lineSpacing, text_reports.str_enemy_allow_attack(nationPtr->nation_name()),
			x2, "", x3, INFO_REPAINT, "ALLOWATK" );

//		button_allow_attack[0].set_font( &font_bld );
//		button_allow_attack[1].set_font( &font_bld );
		button_allow_attack[1].create_text( x2+6 , y, x2+50 , y+lineSpacing-1, text_reports.str_no_yes(1)); // "Yes" );
		button_allow_attack[0].create_text( x2+54, y, x2+100, y+lineSpacing-1, text_reports.str_no_yes(0)); // "No"  );

		button_allow_attack.paint( viewingNation->get_relation(nationRecno)->should_attack );

		//---- if this nation is our ally, display its cash and food ----//

		if( viewingNation->get_relation(nationRecno)->status == RELATION_ALLIANCE )
		{
			// str  = nationPtr->nation_name();
			// str += " 's Treasure";
			// font_bld.field( x1, y+=lineSpacing, str, x2, nationPtr->cash, 2, x3, INFO_REPAINT );
			font_bld.field( x1, y+=lineSpacing, text_reports.str_ally_cash(nationPtr->nation_name()),
				x2, nationPtr->cash, 2, x3, INFO_REPAINT );

			// str  = nationPtr->nation_name();
			// str += " 's Food";
			// font_bld.field( x1, y+=lineSpacing, str, x2, nationPtr->food, 1, x3, INFO_REPAINT );
			font_bld.field( x1, y+=lineSpacing, text_reports.str_ally_food(nationPtr->nation_name()),
				x2, nationPtr->food, 1, x3, INFO_REPAINT );
		}
	}

	y+=lineSpacing+10;

	//----- display its relation status with other nations -----//

	if( info.viewing_nation_recno != nationRecno )
	{
		// String str;

		nationPtr->disp_nation_color(x1, y+1);
		font_bld.put(x1+20, y, text_reports.str_enemy_relation(nationPtr->nation_name()));
		y+=20;

		for( int i=1 ; i<=nation_array.size() ; i++ )
		{
			if( nation_array.is_deleted(i) ||
				 i == nationRecno ||
				 i == info.viewing_nation_recno )
			{
				continue;
			}

			nation_array[i]->disp_nation_color(x1, y+1);

			font_bld.put( x1+20, y, nation_array[i]->nation_name() );
			font_bld.put( x1+240, y, nationPtr->get_relation(i)->status_str() );

			if( nationPtr->get_relation(i)->trade_treaty )
			{
				int x2 = font_bld.put( x1+330, y, text_talk.str_trade_treaty()); // "Trade Treaty" );
				// font_bld.put( x1+450, y, m.format((int)nationPtr->total_year_trade(i),2) );
				font_bld.put( x2+10, y, m.format((int)nationPtr->total_year_trade(i),2) );
			}

			if( config.show_debug_info )
			{
				font_bld.put( x1+500, y, nationPtr->get_relation(i)->ai_relation_level );
				font_bld.put( x1+530, y, (int) nationPtr->get_relation(i)->good_relation_duration_rating );
			}

			y+=16;
		}
	}
}
//----------- End of static function disp_nation_info -----------//


//--------- Begin of static function detect_nation_info ---------//
//
static void detect_nation_info()
{
	int nationRecno = info.nation_filter(browse_nation.recno());

	Nation* viewingNation = nation_array[info.viewing_nation_recno];

	if( button_allow_attack.detect() >= 0 )
		viewingNation->set_relation_should_attack(nationRecno, button_allow_attack.button_pressed, COMMAND_PLAYER);
}
//----------- End of static function detect_nation_info -----------//


//--------- Begin of static function disp_debug_info ---------//
//
static void disp_debug_info()
{
	vga.d3_panel_down( REPORT_DET_X1, REPORT_DET_Y1, REPORT_DET_X2, REPORT_DET_Y2 );

	//----------- display info ------------//

	int	  nationRecno  = info.nation_filter(browse_nation.recno());
	Nation* nationPtr    = nation_array[nationRecno];

	int x1=REPORT_DET_X1+6, x2=REPORT_DET_X1+160;
	int y=REPORT_DET_Y1+6;

	int refreshFlag = INFO_REPAINT;

	//------------ display AI info ----------//

	font_bld.put_field( x1, y    , "Food    ", x2, nationPtr->food_str() );
	font_bld.put_field( x1, y+=16, "Treasure", x2, nationPtr->cash_str() );
	font_bld.put_field( x1, y+=16, "Life points", x2, (int) nationPtr->live_points, 1 );
	font_bld.put_field( x1, y+=16, "ai_capture_enemy_town_recno", x2, nationPtr->ai_capture_enemy_town_recno, 1 );
	font_bld.put_field( x1, y+=16, "Surplus supply rating", x2, nationPtr->surplus_supply_rating() );
	y+=48;

	//--------- display AI preference ----------//

	x2 += 60;

	font_bld.put_field( x1, y+=16, "Unit Chase Distance"     , x2, nationPtr->pref_unit_chase_distance     , 1 );
	font_bld.put_field( x1, y+=16, "Military Development"	   , x2, nationPtr->pref_military_development	 , 1 );
	font_bld.put_field( x1, y+=16, "Economic Development"	   , x2, nationPtr->pref_economic_development	 , 1 );
	font_bld.put_field( x1, y+=16, "Increase Pop by Capture" , x2, nationPtr->pref_inc_pop_by_capture      , 1 );
	font_bld.put_field( x1, y+=16, "Increase Pop by Growth"  , x2, nationPtr->pref_inc_pop_by_growth   	 , 1 );
	font_bld.put_field( x1, y+=16, "Peacefulness" 		      , x2, nationPtr->pref_peacefulness        	 , 1 );
	font_bld.put_field( x1, y+=16, "Military Courage"    	   , x2, nationPtr->pref_military_courage    	 , 1 );
	font_bld.put_field( x1, y+=16, "Territorial Cohesiveness", x2, nationPtr->pref_territorial_cohesiveness, 1 );
	font_bld.put_field( x1, y+=16, "Trading Tendency"        , x2, nationPtr->pref_trading_tendency        , 1 );

	x1 += (REPORT_DET_X2-REPORT_DET_X1)/2;
	x2 += (REPORT_DET_X2-REPORT_DET_X1)/2;

	y=REPORT_DET_Y1+6;

	font_bld.put_field( x1, y    , "Allying Tendency"        , x2, nationPtr->pref_allying_tendency        , 1 );
	font_bld.put_field( x1, y+=16, "Honesty"                 , x2, nationPtr->pref_honesty                 , 1 );
	font_bld.put_field( x1, y+=16, "Town Defense"         	, x2, nationPtr->pref_town_defense			    , 1 );
	font_bld.put_field( x1, y+=16, "Citizen Loyalty Concern" , x2, nationPtr->pref_loyalty_concern  		 , 1 );
	font_bld.put_field( x1, y+=16, "Forgiveness"             , x2, nationPtr->pref_forgiveness      		 , 1 );
	font_bld.put_field( x1, y+=16, "Collect Tax Tendency"    , x2, nationPtr->pref_collect_tax      		 , 1 );
	font_bld.put_field( x1, y+=16, "Build Inn Tendency"      , x2, nationPtr->pref_hire_unit       		    , 1 );
	font_bld.put_field( x1, y+=16, "Use Weapon Tendency"     , x2, nationPtr->pref_use_weapon       		 , 1 );
	font_bld.put_field( x1, y+=16, "Keep Generals Tendency"  , x2, nationPtr->pref_keep_general            , 1 );
	font_bld.put_field( x1, y+=16, "Keep Skilled Units Tendency", x2, nationPtr->pref_keep_skilled_unit    , 1 );
	font_bld.put_field( x1, y+=16, "Attack Monster"          , x2, nationPtr->pref_attack_monster          , 1 );
	font_bld.put_field( x1, y+=16, "Use Spies"               , x2, nationPtr->pref_spy                     , 1 );
	font_bld.put_field( x1, y+=16, "Use Counter Spies"       , x2, nationPtr->pref_counter_spy             , 1 );
	font_bld.put_field( x1, y+=16, "Food Reserve"            , x2, nationPtr->pref_food_reserve            , 1 );
	font_bld.put_field( x1, y+=16, "Cash Reserve"            , x2, nationPtr->pref_cash_reserve            , 1 );
	font_bld.put_field( x1, y+=16, "Use Marine"              , x2, nationPtr->pref_use_marine              , 1 );
}
//----------- End of static function disp_debug_info -----------//


//--------- Begin of static function disp_nation_talk ---------//
//
static void disp_nation_talk()
{
	int nationRecno = info.nation_filter(info.browse_nation_recno);

	//----- if the selected nation is the viewing nation ----//

	if( nationRecno != info.viewing_nation_recno &&
		 info.viewing_nation_recno == nation_array.player_recno )
	{
		vga.d3_panel_down( REPORT_DET_X1, REPORT_DET_Y1, REPORT_DET_X2, REPORT_TALK_Y2 );

		if( info.last_talk_nation_recno != nationRecno && !info.player_reply_mode )
			talk_res.init_conversion(nationRecno);

		info.player_reply_mode = 0;
		info.last_talk_nation_recno = nationRecno;

		talk_res.disp_talk();
	}
}
//----------- End of static function disp_nation_talk -----------//


//--------- Begin of static function detect_nation_talk ---------//
//
static void detect_nation_talk()
{
	int nationRecno = info.nation_filter(info.browse_nation_recno);

	//----- if the selected nation is the viewing nation ----//

	if( nationRecno != info.viewing_nation_recno &&
		 info.viewing_nation_recno == nation_array.player_recno )
	{
		talk_res.detect_talk();
	}
}
//----------- End of static function detect_nation_talk -----------//


//--------- Begin of static function disp_talk_msg_sent ---------//
//
static void disp_talk_msg_sent(int refreshFlag)
{
	//--- filter out talk messages sent by this nation and sort them by date ---//

	TalkMsg* 	talkMsg;
	TalkMsgDisp talkMsgDisp;

	static short lastNationRecno=0;
	int nationRecno = info.nation_filter(info.browse_nation_recno);

	if( lastNationRecno != nationRecno )
	{
		info.browse_talk_msg_recno = 1;		// reset the browser recno of the viewing nation has been changed
		lastNationRecno = nationRecno;
	}

	//--------------------------------------------//

	info.talk_msg_disp_array.zap();

	for( short i=1 ; i<=talk_res.talk_msg_count() ; i++ )
	{
		if( talk_res.is_talk_msg_deleted(i) )
			continue;

		talkMsg = talk_res.get_talk_msg(i);

		if( !talkMsg->is_valid_to_disp() )		// don't link it out, otherwise it may cause multiplayer sync problem. 
			continue;

		// ##### begin Gilbert 11/12 ########//
		if( talkMsg->from_nation_recno == nationRecno && !talkMsg->from_nation_invisible ||
		// ##### end Gilbert 11/12 ########//
			 talkMsg->to_nation_recno == nationRecno )
		{
			// ##### begin Gilbert 11/12 ########//
			if( talkMsg->from_nation_recno == info.viewing_nation_recno && !talkMsg->from_nation_invisible ||
			// ##### end Gilbert 11/12 ########//
				 talkMsg->to_nation_recno == info.viewing_nation_recno )
			{
				if( !nation_array.is_deleted(talkMsg->from_nation_recno) &&
					 !nation_array.is_deleted(talkMsg->to_nation_recno) )
				{
					talkMsgDisp.recno = i;
					talkMsgDisp.date  = talkMsg->date;
					talkMsgDisp.is_reply = 0;

					info.talk_msg_disp_array.linkin(&talkMsgDisp);

					if( talkMsg->reply_date )
					{
						talkMsgDisp.recno = i;
						talkMsgDisp.date  = talkMsg->reply_date;
						talkMsgDisp.is_reply = 1;

						info.talk_msg_disp_array.linkin(&talkMsgDisp);
					}
				}
			}
		}
	}

	if( info.talk_msg_disp_array.size() > 0 )
		info.talk_msg_disp_array.quick_sort(sort_talk_msg);

	//----- display a browser of the talk msg sent -----//

	if( refreshFlag == INFO_REPAINT || !browse_talk_msg.init_flag )
	{
		browse_talk_msg.init( REPORT_DET_X1, REPORT_DET_Y1, REPORT_DET_X2, REPORT_DET_Y2,
									 0, 30, info.talk_msg_disp_array.size(), put_talk_msg_rec, 1 );

		browse_talk_msg.open(info.browse_talk_msg_recno);
	}
	else
	{
		browse_talk_msg.paint();
		browse_talk_msg.open(info.browse_talk_msg_recno, info.talk_msg_disp_array.size());
	}

	info.browse_talk_msg_recno = browse_talk_msg.recno();
}
//----------- End of static function disp_talk_msg_sent -----------//


//------ Begin of function sort_talk_msg ------//
//
static int sort_talk_msg( const void *a, const void *b )
{
	if( ((TalkMsgDisp*)b)->recno == ((TalkMsgDisp*)a)->recno )
		return ((TalkMsgDisp*)b)->is_reply ? 1 : -1;

	return ((TalkMsgDisp*)b)->date - ((TalkMsgDisp*)a)->date;
}
//------- End of function sort_talk_msg ------//


//-------- Begin of static function put_talk_msg_rec --------//
//
static void put_talk_msg_rec(int recNo, int x1, int y)
{
	TalkMsgDisp* talkMsgDisp = (TalkMsgDisp*) info.talk_msg_disp_array.get(recNo);
	TalkMsg*		 talkMsg = talk_res.get_talk_msg(talkMsgDisp->recno);

	int x=x1, isTo;

	x+=3;
	y+=2;

	if( !talkMsgDisp->is_reply )		// if not a replying message
		isTo = talkMsg->from_nation_recno == info.viewing_nation_recno;	// you send a message to a nation
	else
		isTo = talkMsg->from_nation_recno != info.viewing_nation_recno;

	//---------------------------------------//

//	char* str1;
//	if( isTo )
//		str1 = translate.process("To");
//	else
//		str1 = translate.process("From");
//	font_bld.put( x , y, str1 );
//	x += font_bld.text_width(str1)+5;

	x = font_bld.put( x, y, isTo?text_reports.str_send_to():text_reports.str_send_from() );
	x += 5;

	//---------------------------------------//

	if( talkMsg->from_nation_recno == info.viewing_nation_recno )
		nation_array[talkMsg->to_nation_recno]->disp_nation_color(x, y+2);
	else
		nation_array[talkMsg->from_nation_recno]->disp_nation_color(x, y+2);

	x+=18;

	String str;
//	str = translate.process("on ");
//	if( talkMsgDisp->is_reply )
//		str += date.date_str(talkMsg->reply_date);
//	else
//		str += date.date_str(talkMsg->date);
//	str += " :";
//	font_bld.put( x , y, str );

	if( talkMsgDisp->is_reply )
		str = date.date_str(talkMsg->reply_date);
	else
		str = date.date_str(talkMsg->date);
	font_bld.put( x, y, text_reports.str_on_date((char *)str) );

	font_bld.put( x1, y+13, talkMsg->msg_str(info.viewing_nation_recno, talkMsgDisp->is_reply), 0, browse_talk_msg.ix2 );
}
//----------- End of static function put_talk_msg_rec -----------//


//--------- Begin of static function disp_nation_chat ---------//
//
static void disp_nation_chat(int refreshFlag)
{
	// init button

	if( !init_get_chat || get_chat.x != REPORT_DET_X1+10 || get_chat.y != REPORT_TALK_Y2 - 26 )		// re-init after resolution change
	{
		get_chat.init( REPORT_DET_X1 + 10, REPORT_TALK_Y2 - 26, REPORT_DET_X2-10,
			info.player_chat_str, CHAT_STR_LEN, &font_san, 0, 0 );
		get_chat.clear();

		init_get_chat = 1;
	}

	// disp frame
	vga.d3_panel_down( REPORT_DET_X1, REPORT_DET_Y1, REPORT_DET_X2, REPORT_TALK_Y2 );

	font_bld.put( REPORT_DET_X1+10, REPORT_TALK_Y2 - 66,
					  // "Please enter your chat message and press <Enter> to send." );
		text_reports.str_chat_inst() );

	// display receiver button 

	int x = RECIPIENT_BUTTON_X1;
	int y1 = RECIPIENT_BUTTON_Y1;
	int y2 = RECIPIENT_BUTTON_Y2;
	int i;

	if( info.chat_receiver_type == CHAT_RECEIVER_ALL_HUMAN )
	{
		vga.d3_panel_down( x, y1, x+68, y2 );
		font_bld.center_put( x, y1+2, x+68, y2-1, text_reports.str_chat_recipient(CHAT_RECEIVER_ALL_HUMAN) );
	}
	else
	{
		vga.d3_panel_up( x, y1, x+68, y2, 0, 1 );
		font_bld.center_put( x, y1+1, x+68, y2-2, text_reports.str_chat_recipient(CHAT_RECEIVER_ALL_HUMAN) );
	}
	x += 70;

	if( info.chat_receiver_type == CHAT_RECEIVER_ALL_ALLY )
	{
		vga.d3_panel_down( x, y1, x+68, y2 );
		font_bld.center_put( x, y1+2, x+68, y2-1, text_reports.str_chat_recipient(CHAT_RECEIVER_ALL_ALLY) );
	}
	else
	{
		vga.d3_panel_up( x, y1, x+68, y2, 0, 1 );
		font_bld.center_put( x, y1+1, x+68, y2-2, text_reports.str_chat_recipient(CHAT_RECEIVER_ALL_ALLY) );
	}
	x += 70;

	// draw nation color button
	int nc = info.nation_filter();
	int nationRecno;
	if( info.chat_receiver_type == CHAT_RECEIVER_CURRENT
		&& info.browse_nation_recno >= 1 && info.browse_nation_recno <= nc )
		nationRecno = info.nation_filter(info.browse_nation_recno);
	else
		nationRecno = 0;
	for( i = 1; i <= nc; ++i )
	{
		int n = info.nation_filter(i);
		if( nation_array[n]->is_remote() )
		{
			if( nationRecno == n )
			{
				vga.d3_panel_down( x, y1, x+24, y2 );
				vga.active_buf->bar( x+7, y1+7, x+18, y2-5, nation_array[n]->nation_color );
			}
			else
			{
				vga.d3_panel_up( x, y1, x+24, y2, 0 ,1 );
				vga.active_buf->bar( x+6, y1+6, x+17, y2-6, nation_array[n]->nation_color );
			}
			x += 26;
		}
	}

	// draw hide nation button

	x = REPORT_DET_X2 - 200 - 8 ;
	if( hide_nation_browse )
	{
		vga.d3_panel_down( x, y1, x+200-1, y2 );
		font_bld.center_put( x, y1+2, x+200-1, y2-1, text_reports.str_hide_nation_detail() );
	}
	else
	{
		vga.d3_panel_up( x, y1, x+200-1, y2, 0, 1 );
		font_bld.center_put( x, y1+1, x+200-1, y2-2, text_reports.str_hide_nation_detail() );
	}

	// draw get field

	get_chat.paint(1);			// 1-paint cursor

	// display message log, display news in reversed order, from down to up

	int chatMsgCount = info.chat_msg_filter();
	int msgY = REPORT_TALK_Y2 - 80;
	for( i = -1; i >= -chatMsgCount && msgY >= REPORT_DET_Y1+4 ; --i )
	{
		ChatInfo *chatInfo = &info.remote_chat_array[info.chat_msg_filter(i)-1];
		if( !nation_array.is_deleted( chatInfo->from_nation_recno ) )
		{
			nation_array[chatInfo->from_nation_recno]->disp_nation_color( REPORT_DET_X1+10, msgY+2 );
			font_cara_w.put( REPORT_DET_X1+25, msgY, chatInfo->chat_str, 0, REPORT_DET_X2-6 );
			msgY -= font_cara_w.max_font_height;
		}
	}
}
//----------- End of static function disp_nation_chat -----------//


//--------- Begin of static function detect_nation_chat ---------//
//
static void detect_nation_chat()
{
	// init button

	if( !init_get_chat || get_chat.x != REPORT_DET_X1+10 || get_chat.y != REPORT_TALK_Y2 - 26 )		// re-init after resolution change
	{
		get_chat.init( REPORT_DET_X1 + 10, REPORT_TALK_Y2 - 26, REPORT_DET_X2-10,
			info.player_chat_str, CHAT_STR_LEN, &font_san, 0, 0 );
		get_chat.clear();

		init_get_chat = 1;
	}

	// detect receiver button 

	int x = RECIPIENT_BUTTON_X1;
	int y1 = RECIPIENT_BUTTON_Y1;
	int y2 = RECIPIENT_BUTTON_Y2;
	int i;

	if( mouse.single_click( x, y1, x+68, y2 ) )
		info.chat_receiver_type = CHAT_RECEIVER_ALL_HUMAN;
	x += 70;

	if( mouse.single_click( x, y1, x+68, y2 ) )
		info.chat_receiver_type = CHAT_RECEIVER_ALL_ALLY;
	x += 70;

	// draw nation color button
	int nc = info.nation_filter();
	int nationRecno;
	if( info.chat_receiver_type == CHAT_RECEIVER_CURRENT
		&& info.browse_nation_recno >= 1 && info.browse_nation_recno <= nc )
		nationRecno = info.nation_filter(info.browse_nation_recno);
	else
		nationRecno = 0;
	for( i = 1; i <= nc; ++i )
	{
		int n = info.nation_filter(i);
		if( nation_array[n]->is_remote() )
		{
			if( mouse.single_click( x, y1, x+24, y2 ) )
			{
				info.browse_nation_recno = i;
				info.chat_receiver_type = CHAT_RECEIVER_CURRENT;
			}
			x += 26;
		}
	}

	// draw hide nation button

	x = REPORT_DET_X2 - 200 - 8 ;
	if( mouse.single_click( x, y1, x+200-1, y2 ) )
		hide_nation_browse = !hide_nation_browse;

	// detect get, detect max 5 times

	for( i=0 ; i<5 ; i++ )
	{
		int keyCode = get_chat.detect();

		if( !keyCode )
			break;

		//---- press <Enter> to send the chat message ----//

		if( keyCode == KEY_RETURN && info.player_chat_str[0] != '\0')
		{
			if( remote.is_enable() )
			{
				// use new_msg instead of new_send_queue_msg, so chat message can send immediately
				// packet structure : <to nation recno> <from nation recno> <char[CHAT_STR_LEN+1]>
				// short *shortPtr = (short *)remote.new_send_queue_msg(MSG_CHAT, 2*sizeof(short) + strlen(info.player_chat_str)+1 );

				RemoteMsg *remoteMsg = remote.new_msg(MSG_CHAT, 2*sizeof(short) + strlen(info.player_chat_str)+1 );
				short *shortPtr = (short *)remoteMsg->data_buf;

				switch( info.chat_receiver_type )
				{
				case CHAT_RECEIVER_CURRENT:
					shortPtr[0] = nationRecno;
					break;
				case CHAT_RECEIVER_ALL_ALLY:
					shortPtr[0] = -1;
					break;
				case CHAT_RECEIVER_ALL_HUMAN:
					shortPtr[0] = 0;
					break;
				}
				shortPtr[1] = info.viewing_nation_recno;

				strcpy(2*sizeof(short)+(char *)shortPtr, info.player_chat_str);
		      remote.send_free_msg( remoteMsg );     // send out the message and free it after finishing sending
			}

			{
				// add a copy to sender
				int chatIndex = info.append_chat( NULL );
				if( chatIndex )
				{
					ChatInfo *chatInfo = &info.remote_chat_array[chatIndex-1];
					chatInfo->received_date = info.game_date;
					chatInfo->from_nation_recno = (char)info.viewing_nation_recno;
					strcpy( chatInfo->chat_str, info.player_chat_str );
				}
			}

			get_chat.clear();
			break;
		}

		mouse.get_event();
	}
}
//----------- End of static function detect_nation_chat -----------//


//------- Begin of function Info::player_reply_chat --------//
//
// <int> withNationRecno - player reply a chat message to this nation.
//
void Info::player_reply_chat(int withNationRecno)
{
	nation_report_mode = NATION_REPORT_CHAT;

	//--- go to the nation in the browser which the player is going to talk to ---//

	int nationCount = nation_filter();

	int i;
	for( i=1 ; i<=nationCount ; i++ )
	{
		if( nation_filter(i) == withNationRecno )
		{
			browse_nation_recno = i;
			break;
		}
	}

	err_when( i>nationCount );

	sys.set_view_mode(MODE_NATION);
}
//-------- End of function Info::player_reply_chat ---------//


// ----- begin of function Info::append_chat -------//
//
// similar to circular queue
// but at least one record remote_chat_array[i].received_date must be blank
//
// <ChatInfo *>chatInfo    record to be added, NULL is valid but not copied to the array
//
// return the slot available (plus 1)
// e.g. if info.remote_chat_array[i] is free then return i+1
// 
int Info::append_chat( ChatInfo *chatInfo )
{

	// find place to insert

	int firstBlank = -1;
	if( remote_chat_array[0].received_date != 0 )
	{
		// case broken non-blank (e.g. 4,5,6,0*,0,0,0,1,2,3)  0* indicate the first blank
		// all blank then all non-blank (e.g. 1,2,3,4,5,6,0*,0,0,0)
		int i;
		for( i = 0; i < MAX_REMOTE_CHAT_STR && remote_chat_array[i].received_date != 0; ++i );
		err_when( i >= MAX_REMOTE_CHAT_STR );		 // no blank found
		firstBlank = i;
	}
	else if( remote_chat_array[MAX_REMOTE_CHAT_STR-1].received_date == 0 )
	{
		// case broken blank ( e.g. 0,0,1,2,3,4,5,6,0*,0 )	 0* indicate the first blank
		// or empty case (e.g. 0,0,0,0,0,0,0,0,0,0 )
		int i;
		for( i = MAX_REMOTE_CHAT_STR-1; i >= 0 && remote_chat_array[i].received_date == 0; --i );
		firstBlank = i+1;			// if i is -1, firstBlank is 0
	}
	else
	{
		// case all blank then all non-blank (0,0,0,0,1,2,3,4,5,6)
		firstBlank = 0;
	}

	if( firstBlank == -1 )
	{
		return 0;
	}

	int nextPos = firstBlank + 1;
	if( nextPos >= MAX_REMOTE_CHAT_STR )
		nextPos = 0;

	if( chatInfo )
	{
		remote_chat_array[firstBlank] = *chatInfo;
	}
	
	// remove chat message in news_array first
	news_array.remove( NEWS_CHAT_MSG, nextPos+1 );

	remote_chat_array[nextPos].received_date = 0;		// clear next record
	return firstBlank + 1;
}
// ----- end of function Info::append_chat -------//

// ----- begin of function Info::chat_msg_filter -------//
//
// recno = 0 , count number of record
// recno positive  get recno from the head
// recno negative get recno from the last
//
int Info::chat_msg_filter(int recno)
{
	int i;

	// count record

	int count = 0;
	for( i = 0; i < MAX_REMOTE_CHAT_STR; ++i )
	{
		if( remote_chat_array[i].received_date )
			++count;
	}

	if( recno == 0 )
		return count;

	// blank                0,0,0,0,0,0,0,0,0,0
	err_when( count == 0 );	// no record, don't call with recno != 0

	// find first non blank


	int firstNonBlank = -1;
	if( remote_chat_array[0].received_date == 0 )
	{
		// case broken blank 0,0,1,2,3,4,5,6,0,0
		// blank then non-blank 0,0,0,0,1,2,3,4,5,6
		int i;
		for( i = 0; i < MAX_REMOTE_CHAT_STR && remote_chat_array[i].received_date == 0; ++i );
		firstNonBlank = i;
	}
	else if( remote_chat_array[MAX_REMOTE_CHAT_STR-1].received_date != 0 )
	{
		// case broken non-blank 4,5,6,0,0,0,0,1,2,3
		int i;
		for( i = MAX_REMOTE_CHAT_STR-1; i >= 0 && remote_chat_array[i].received_date != 0; --i );
		err_when( i < 0 );			// no blank found
		firstNonBlank = i+1;			// if i is -1, firstBlank is 0
	}
	else
	{
		// non-blank then blank 1,2,3,4,5,6,0,0,0,0
		firstNonBlank = 0;
	}

	if( recno < 0 )
	{
		err_when( -recno > count );
		recno += count + 1;		// -1 means the last, -count means the first
	}

	err_when( recno <= 0 || recno > count );			// valid recno is 1 to count
	int rc = firstNonBlank + recno;
	if( rc > MAX_REMOTE_CHAT_STR )
		rc -= MAX_REMOTE_CHAT_STR;
	return rc;
}
// ----- end of function Info::chat_msg_filter -------//
