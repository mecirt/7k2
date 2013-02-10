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

//Filename    : OFIRMIF.CPP
//Description : Firm interface routines

#include <ofirm.h>
#include <ostr.h>
#include <key.h>
#include <ovga.h>
#include <omodeid.h>
#include <ohelp.h>
#include <omouse.h>
#include <ofont.h>
#include <obutt3d.h>
#include <obutton.h>
#include <oslider.h>
#include <oconfig.h>
#include <onation.h>
#include <oinfo.h>
#include <oraceres.h>
#include <ounit.h>
#include <ospy.h>
#include <otown.h>
#include <oworld.h>
#include <oremote.h>
#include <oimgres.h>
#include <ogame.h>
#include <osys.h>
#include <ose.h>
#include <oseres.h>
#include <ot_firm.h>
#include <ot_unit.h>

//---------- Define static variables ------------//

static Button3D button_sell, button_destruct, button_builder;
static short  	 pop_disp_y1;

static int	edit_mode_y1;
static short edit_hp_x1, edit_hp_y1, edit_hp_x2, edit_hp_y2, edit_hp_enable;

//---------- Declare static function ------------//

// static void disp_worker_hit_points(int x1, int y1, int x2, int hitPoints, int maxHitPoints);

//--------- Begin of function Firm::disp_info_both ---------//
//
void Firm::disp_info_both(int refreshFlag)
{
	disp_basic_info(INFO_Y1, refreshFlag);

	// ##### begin Gilbert 7/9 ########//

	if( under_construction )
	{
		// disp firm name
		vga.active_buf->put_bitmap( INFO_X1, INFO_Y1, image_gameif.read("VILLCONS") );

		// font_vill.center_put( INFO_X1+113, INFO_Y1+152, INFO_X1+205, INFO_Y1+165, "Under" );
		// font_vill.center_put( INFO_X1+99, INFO_Y1+165, INFO_X1+217, INFO_Y1+185, "Construction" );
		font_vill.center_put_paragraph( INFO_X1+99, INFO_Y1+152, INFO_X1+217, INFO_Y1+185,
			text_firm.str_under_construction(), -5, 0, 0 );
		font_vill.center_put_paragraph( INFO_X1+20, INFO_Y1+103, INFO_X1+125, INFO_Y1+147, 
			firm_name(), -5, 0, 0 );

		// disp completion percentage
		int ratio( (int)hit_points * 100 / max_hit_points() );
		if( ratio < 0 )
			ratio = 0;
		if( ratio > 100 )
			ratio = 100;
		font_vill.center_put( INFO_X1+20, INFO_Y1+139, INFO_X1+125, INFO_Y1+171, 
			m.format(ratio, 3) );
	}
	else
		put_info(refreshFlag);			// virtual function
}
//----------- End of function Firm::disp_info_both -----------//


//--------- Begin of function Firm::detect_info_both ---------//
//
// Called by Info::detect(). Detect both cases when the firm is
// under construction and is normal. 
//
void Firm::detect_info_both()
{
	// ###### begin Gilbert 24/2 ########//
	// detect_basic_info();
	if( detect_basic_info() )		// in case the firm is removed
		return;
	// ###### end Gilbert 24/2 ########//

	if( !under_construction )
		detect_info();
}
//----------- End of function Firm::detect_info_both -----------//


//--------- Begin of function Firm::disp_basic_info ---------//
//
void Firm::disp_basic_info(int dispY1, int refreshFlag)
{
	//------- display the name of the firm --------//
//	String str;
	
	// ------- display town name and hit point bar --------//
	if (firm_id != FIRM_WILDE_LISHORR && 
		 firm_id != FIRM_LISHORR &&
		 firm_id != FIRM_OFFENSIVE_BUILDING_3)
	{
//		if ( firm_id == FIRM_FORT )
//			str = firm_name(0);
//		else
//			str = firm_name();	
		font_snds.center_put( INFO_X1+15, INFO_Y1-19, INFO_X2, INFO_Y1-4, 
			firm_name(), 0, 1 );
		disp_hit_point(0);
		help.set_help( INFO_X1 +173, INFO_Y1 -62, INFO_X1 +202, INFO_Y1 -29, "F_ENENGY" );
	}

	char *nationPict = image_spict.get_ptr("V_COLCOD");
	vga.active_buf->put_bitmap_trans_remap_decompress(INFO_X1+16, INFO_Y1-28, nationPict, game.get_color_remap_table(nation_recno, 0) );

	// ------- display button ------- //
	int x1 = INFO_X1 +13;
	int y1 = INFO_Y1 +235;
	int x2 = INFO_X1 +13;
	int y2 = INFO_Y1 +281;

	// ------- create button ------- //
	int showRepairIcon = builder_recno && !under_construction && should_show_info() && hit_points < max_hit_points();

//	set_repair_flag( button_builder.pushed_flag, COMMAND_PLAYER );

	button_destruct.create( INFO_X1+16, INFO_Y1-61, "V_DEM-U", "V_DEM-D" );
	button_sell.create( INFO_X1+16, INFO_Y1-61, "V_SEL-U", "V_SEL-D" );
	button_sell.set_help_code( "SELLFIRM" );

	if( nation_recno && nation_array.player_recno == nation_recno )
	{
		button_destruct.visible_flag = 0;
		button_sell.visible_flag = 0;

		if( under_construction || !can_sell() )
		{
			if( under_construction )
				button_destruct.set_help_code( "CANCELBLD" );
			else
				button_destruct.set_help_code( "DESTFIRM" );

			button_destruct.visible_flag = 1;
			button_destruct.paint();
		}
		else
		{
			button_sell.visible_flag = 1;
			button_sell.paint();
		}
	}
	// ####### begin Gilbert 30/11 #######//
	button_builder.create( INFO_X1+13, INFO_Y1-96, "REPAIRU", "REPAIRD", 0 );
	button_builder.set_help_code( "REPAIR" );

	if( should_show_info() )
	{
		if( (button_builder.visible_flag = !under_construction && hit_points < max_hit_points() && firm_id != FIRM_LISHORR) )
		{
			if( repair_flag && hit_points < max_hit_points() )
			{
				button_builder.pushed_flag = repair_flag;
				button_builder.update_bitmap( ((sys.frame_count % 5) < 3) ? (char*)"REPAIRU" : (char*)"REPAIRD");
				button_builder.set_help_code( "REPAIRING" );
			}
			else
			{
				button_builder.pushed_flag = repair_flag;
				button_builder.update_bitmap( repair_flag ? (char*)"REPAIRU" : (char*)"REPAIRD");
				button_builder.set_help_code( "REPAIR" );
			}
		}
	}
	// ####### end Gilbert 30/11 #######//
}
//----------- End of function Firm::disp_basic_info -----------//


//--------- Begin of function Firm::detect_basic_info ---------//
//
int Firm::detect_basic_info()
{
	//--- click on the name area to center the map on it ---//

	if( mouse.single_click( INFO_X1+15, INFO_Y1-18, INFO_X2, INFO_Y1-3 ) )
	{
		world.go_loc( center_x, center_y );
		return 1;
	}
	
	//----------- Mobilize the builder ---------//

	// ###### begin Gilbert 30/11 ######//
//	int detectBuilder = builder_recno && !under_construction &&
//							  unit_array[builder_recno]->is_own();			// this is your unit in your firm or it is a spy of yours in an enemy firm

	int rc;
	if( is_own() && (rc = button_builder.detect(0, 0, 1)) )
	{
		if( under_construction )
		{
			if( !remote.is_enable() )
				set_builder(0);
			else
			{
				// packet structure : <firm recno>
				short *shortPtr = (short *)remote.new_send_queue_msg(MSG_FIRM_MOBL_BUILDER, sizeof(short));
				*shortPtr = firm_recno;
			}
		}
		else
		{
			// ####### begin Gilbert 21/4 ##########//
			if( rc != 2 )
			{
				set_repair_flag( button_builder.pushed_flag ? 1 : 0, COMMAND_PLAYER );
			}
			else	// right click to repair all buildings
			{
				nation_array[nation_recno]->repair_all_building( button_builder.pushed_flag ? 1 : 0, COMMAND_PLAYER );
			}
			// ####### end Gilbert 21/4 ##########//
		}
		se_ctrl.immediate_sound("REPAIR");
		return 1;
	}
	// ###### end Gilbert 30/11 ######//

	//---------------------------------------//

	if( !is_own() )
		return 0;

	//---------- "Destruct" button -----------//

	// ####### begin Gilbert 27/1 #######//
	// if( button_destruct.detect(KEY_DEL) )
	if( button_destruct.detect() )	// if press del during chat, the firm destructed too
	// ####### end Gilbert 27/1 #######//
	{
		if( under_construction )
			cancel_construction(COMMAND_PLAYER);
		else
			destruct_firm(COMMAND_PLAYER);

		se_ctrl.immediate_sound("TURN_ON");

		return 1;
	}

	//------------ "Sell" button -------------//

	// ###### begin gilbert 27/1 ######//
	// if( button_sell.detect(KEY_DEL) )
	if( button_sell.detect() )
	// ###### end gilbert 27/1 ######//
	{
		sell_firm(COMMAND_PLAYER);
		se_ctrl.immediate_sound("TAXGRANT");
		return 1;
	}

	help.set_help( INFO_X1+16, INFO_Y1-28, INFO_X1+45, INFO_Y1+1, "COLCODE" );
	
	return 0;
}
//----------- End of function Firm::detect_basic_info -----------//

//----------- Begin of function Firm::disp_hit_point -----------//
void Firm::disp_hit_point(int dispY1)
{
	float hitPoints;

	if( hit_points > (float)0 && hit_points < (float)1 )
		hitPoints = (float) 1;
	else
		hitPoints = hit_points;
	int ratio = (int)hitPoints *25 / (int)max_hit_points();
	char hitPointName[] = "BB-00";
	hitPointName[3] = '0' + ratio / 10;
	hitPointName[4] = '0' + ratio % 10;
	image_spict.put_back( INFO_X1 +140, INFO_Y1 -106, hitPointName);

	// ###### begin Gilbert 19/1 ########//
//	if( config.show_debug_info )
//	{
		if( max_hit_points() < 1000 )
			image_icon.put_back( INFO_X1 +173, INFO_Y1 -62, "REPAIRBX");
		font_san.put( INFO_X1+177, INFO_Y1-59, (int) hitPoints );
		font_san.put( INFO_X1+177, INFO_Y1-45, (int) max_hit_points() );
//	}
	// ###### end Gilbert 19/1 ########//
}
//----------- End of function Firm::disp_hit_point -----------//

//----------- Begin of function Firm::put_info -----------//
void Firm::put_info(int refreshFlag)
{
	vga.active_buf->put_bitmapW( INFO_X1, INFO_Y1, info.info_background_bitmap );
}
//----------- End of function Firm::put_info -----------//


// -------- begin of function Firm::disp_edit_mode -------//
//
// <int&> refreshFlag  INFO_REPAINT or INFO_UPDATE
// <int&> dispY1,      start y1 of editing
// [int]  dispBG,      whether to display background (default:true)
//
// beware int& refreshFlag, so it can inform subclass disp_edit_mode to change to INFO_REPAINT
//
void Firm::disp_edit_mode(int& refreshFlag, int& y, int dispBG)
{
	// if subclass disp_edit_mode already painted background,
	// pass dispBG=0
	if( dispBG )
	{
		vga.active_buf->put_bitmap( INFO_X1, INFO_Y1, image_gameif.read("SCEN-ED") );
	}

	// put nation color code

	char *nationPict = image_spict.get_ptr("V_COLCOD");
	vga.active_buf->put_bitmap_trans_remap_decompress(INFO_X1+16, INFO_Y1-28, nationPict, game.get_color_remap_table(nation_recno, 0) );

	int x;
	int x1 = INFO_X1 + 12;
	int x2 = INFO_X2 - 10;

	// set edit_mode_y1 for detect

	edit_mode_y1 = y;
	y += 15;

	// disp firm name

	disp_basic_info(INFO_Y1, refreshFlag);

	font_zoom.put( x1, y, firm_name(), 0, x2 );
	y += 18;

	// edit hit point

	edit_hp_enable = 1;
	x = font_whbl.put( x1, y, text_unit.str_hit_points(), 0, x2 );	// "Hit Points"
	x = edit_hp_x2 = font_blue.put( (edit_hp_x1=x+20), (edit_hp_y1=y), (int)hit_points, 0, x2 );
	edit_hp_y2 = edit_hp_y1+font_blue.max_font_height;
	x = font_whbl.put( x, y, " / ", 0, x2 );
	x = font_whbl.put( x, y, max_hit_points(), 0, x2 );
	y += 18;

	// disp left click to inc, right click to dec.

	font_blue.put( x1, INFO_Y2-28, text_unit.str_left_inc() );  // "Left click to increase" );
	font_blue.put( x1, INFO_Y2-14, text_unit.str_right_dec() ); // "Right click to decrease" );
}
// -------- end of function Firm::disp_edit_mode -------//


// -------- begin of function Firm::detect_edit_mode -------//
//
int Firm::detect_edit_mode()
{
	if( edit_hp_enable )
	{
		if( mouse.any_click(edit_hp_x1, edit_hp_y1, edit_hp_x2, edit_hp_y2, LEFT_BUTTON) )
		{
			hit_points += 50.0f;
			if( hit_points > (float) max_hit_points() )
				hit_points = (float) max_hit_points();
			return 1;
		}
		if( mouse.any_click(edit_hp_x1, edit_hp_y1, edit_hp_x2, edit_hp_y2, RIGHT_BUTTON) )
		{
			hit_points -= 50.0f;
			if( hit_points < 1.0f )
				hit_points = 1.0f;
			return 1;
		}
	}
	return 0;
}
// -------- end of function Firm::detect_edit_mode -------//

