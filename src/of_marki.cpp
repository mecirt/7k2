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

//Filename    : OF_MARKI.CPP
//Description : Firm Market Place - interface functions

#include <of_mark.h>
#include <of_work.h>
#include <oinfo.h>
#include <ovga.h>
#include <omodeid.h>
#include <ostr.h>
#include <obitmap.h>
#include <omouse.h>
#include <obutton.h>
#include <obutt3d.h>
#include <ofont.h>
#include <oimgres.h>
#include <ohelp.h>
#include <oconfig.h>
#include <orawres.h>
#include <oraceres.h>
#include <otown.h>
#include <onation.h>
#include <oworldmt.h>
#include <osys.h>
#include <oremote.h>
#include <ose.h>
#include <oconfig.h>
#include <ot_firm.h>

// ------- define constant --------//

#define COIN_PER_SALES 200		// for display sales amount indicators
#define MAX_COIN 10


//------- define static vars -------//

struct Point
{
	short	x;
	short y;
};

static Point section_point_array[] =
{
	{   8, 110 },
	{   8, 130 },
	{   8, 150 },
	{  30, 110 },
	{  30, 130 },
	{  30, 150 },

	{   2, 65 },
	{   2, 75 },
	{   2, 85 },
	{  15, 65 },
	{  15, 75 },
	{  15, 85 }
};

static Point slot_point_array[] =
{
	{  0,  0 },
	{  6,  0 },
	{  0,  6 },
	{  6,  6 },
	{ 12,  0 },
	{ 12,  6 },
	{ -2, -1 },
	{  4, -1 },
	{ -4, -7 },

	{  0,  0 },
	{  3,  0 },
	{  0,  3 },
	{  3,  3 },
	{  6,  0 },
	{  6,  3 },
	{ -1, -1 },
	{  2, -1 },
	{ -2, -3 }
};

static Button3D button_hire_caravan;
static Button 	 button_clear_stock[MAX_MARKET_GOODS];
static const char show_sale_debug_info=0;		// for debugging

static short edit_stock_enable[MAX_MARKET_GOODS];
static short edit_stock_x1[MAX_MARKET_GOODS];
static short edit_stock_y1[MAX_MARKET_GOODS];
static short edit_stock_x2[MAX_MARKET_GOODS];
static short edit_stock_y2[MAX_MARKET_GOODS];

//--------- Begin of function FirmMarket::put_info ---------//
//
void FirmMarket::put_info(int refreshFlag)
{
	Firm::put_info(refreshFlag);

	vga.active_buf->put_bitmap( INFO_X1, INFO_Y1, image_gameif.read("MARKBASE") );
	disp_market_info( INFO_Y1, refreshFlag );

	//--- only display market info if the player is allowed to trade with this market ---//

	if( config.show_ai_info || is_own() )
	{
		disp_income(INFO_Y1+209, refreshFlag );	  // 1-display income figure
	}

	button_hire_caravan.create( INFO_X1+13, INFO_Y1+281, 'A', "HIRECARA" );
	button_hire_caravan.enable_flag = 0;

	if( is_own() )
	{
		button_hire_caravan.enable_flag = can_hire_caravan();
		button_hire_caravan.paint();
	}

/*
	disp_basic_info(INFO_Y1, refreshFlag);

	//--- only display market info if the player is allowed to trade with this market ---//

	put_market_info(INFO_Y1+50, refreshFlag);

	//------------------------------------------------//

	if( !config.show_ai_info && nation_recno!=nation_array.player_recno )
		return;

	disp_income(INFO_Y1+209, refreshFlag );	  // 1-display income figure

	button_hire_caravan.paint( INFO_X1, INFO_Y1+236, 'A', "HIRECARA" );

	if( can_hire_caravan() )
		button_hire_caravan.enable();
	else
		button_hire_caravan.disable();
*/
}
//----------- End of function FirmMarket::put_info -----------//


//--------- Begin of function FirmMarket::detect_info ---------//
//
void FirmMarket::detect_info()
{
	Firm::detect_info();

	detect_market_info();

	if( is_own() )
	{
		if( button_hire_caravan.detect() )
		{
			hire_caravan(COMMAND_PLAYER);
			se_ctrl.immediate_sound("TURN_ON");
		}
	}

/*
	if( detect_basic_info() )
		return;

	if( !config.show_ai_info && nation_recno!=nation_array.player_recno )
		return;

	if( nation_recno != nation_array.player_recno )		// the following controls are only available for player's firms
		return;

	//----- detect clear stock buttons -------//

	for( int i=0 ; i<MAX_MARKET_GOODS ; i++ )
	{
		if( button_clear_stock[i].detect() )
		{
			if( !remote.is_enable() )
			{
				MarketGoods* marketGoods = market_goods_array+i;
				
				clear_market_goods(i+1);
				info.disp();
				return;
			}
			else
			{
				// message structure : <firm recno> <cell no 0-3>
				short *shortPtr = (short *)remote.new_send_queue_msg(MSG_F_MARKET_SCRAP, sizeof(short)+sizeof(short) );
				shortPtr[0] = firm_recno;
				shortPtr[1] = i;
			}
			se_ctrl.immediate_sound("TURN_OFF");
		}
	}

	//----- detect hire caravan button -------//

	if( button_hire_caravan.detect() )
		hire_caravan(COMMAND_PLAYER);

*/
}
//----------- End of function FirmMarket::detect_info -----------//


// -------- begin of function FirmMarket::should_show_info -------//
//
int FirmMarket::should_show_info()
{
	return Firm::should_show_info() ||
		( nation_array.player_recno 
		&& nation_array[nation_recno]->get_relation(nation_array.player_recno)->trade_treaty );
}
// -------- end of function FirmMarket::should_show_info -------//


//--------- Begin of function FirmMarket::put_market_info ---------//
//
void FirmMarket::disp_market_info(int dispY1, int refreshFlag)
{
	if( !should_show_info() )
	{
		font_snds.center_put_paragraph( INFO_X1+16, INFO_Y1+8, INFO_X2-10, INFO_Y1+42,
			text_firm.str_no_trade(), // "You're not permitted to trade with this market.",
			1, 0, 0 );
		return;
	}

	int i = 0;
	MarketGoods *marketGoods = market_goods_array;
	for(  ; i<MAX_MARKET_GOODS; ++i, ++marketGoods )
	{
		int x = (i%3) * 72 + 18 + INFO_X1;
		int y = (i/3) * 89 + 60 + INFO_Y1;

		if( marketGoods->raw_id )
		{
		}
		else if( marketGoods->product_raw_id )
		{
		}
		else
		{
			// empty
			continue;
		}

		// ------- disp sale ----------//

		if( !show_sale_debug_info )
		{
			int yearSales = (int) marketGoods->sales_365days();

			if( yearSales > 0 )
			{
				int coinCount = yearSales / COIN_PER_SALES;

				coinCount = max( 1, coinCount );
				coinCount = min( MAX_COIN, coinCount );

				char bitmapName[] = "DOLLAR00";
				bitmapName[6] = '0' + coinCount / 10;
				bitmapName[7] = '0' + coinCount % 10;

				vga.active_buf->put_bitmap_trans( x-4, y, image_gameif.read(bitmapName) );
			}
		}
		else
		{
			font_snds.put( x, y+16, "Sales" );
			font_snds.right_put( x+56, y+16, m.format((int)marketGoods->sales_365days()) );
		}

		// ------- display quantity -----//

		if( !show_sale_debug_info )
		{
			font_snds.center_put( x, y+20, x+56, y+36, m.format((int)marketGoods->stock_qty) );
		}
		else
		{
			font_snds.put( x, y, "Stock" );
			font_snds.right_put( x+56, y, m.format((int)marketGoods->stock_qty) );
		}

		// -------- disp demand -------//

		if( show_sale_debug_info )
		{
			font_snds.put( x, y+32, "Demand" );
			font_snds.right_put( x+56, y+32, m.format((int)marketGoods->month_demand) );
		}

		// ----- display "full" icon -------//

		if( marketGoods->stock_qty >= MAX_MARKET_STOCK-10 )
		{
			char *fullBitmap = image_icon.read("FULLVILL");
			vga.active_buf->put_bitmap(x+8, y+48+RAW_LARGE_ICON_HEIGHT-1- ((Bitmap *)fullBitmap)->get_height(), 
				fullBitmap);
		}
	}
}
//----------- End of function FirmMarket::disp_market_info -----------//


//----------- Begin of function FirmMarket::detect_market_info -------//
//
void FirmMarket::detect_market_info()
{
	if( !should_show_info() )
	{
		return;
	}

	int i = 0;
	MarketGoods *marketGoods = market_goods_array;
	for(  ; i<MAX_MARKET_GOODS; ++i, ++marketGoods )
	{
		int x = (i%3) * 72 + INFO_X1 + 18;
		int y = (i/3) * 89 + INFO_Y1 + 60;

		String str;

		if( marketGoods->raw_id )
		{
			str = raw_res[marketGoods->raw_id]->name;
		}
		else if( marketGoods->product_raw_id )
		{
			str = text_firm.str_product_name( marketGoods->product_raw_id );
		}
		else
		{
			// empty
			continue;
		}

		// ------- detect sale ----------//

		if( !show_sale_debug_info )
			help.set_help( x-4, y, x+56, y+18, "MK_SALES" );
		else
			help.set_help( x, y+16, x+56, y+31, "MK_SALES" );

		// ------- detect quantity -----//

		if( !show_sale_debug_info )
			help.set_help( x, y+20, x+56, y+36, "MK_STOCK" );
		else
			help.set_help( x, y, x+56, y+15, "MK_STOCK" );

		// ------- detect demand ----------//

		if( show_sale_debug_info )
		{
			help.set_help( x, y+32, x+56, y+47, "MK_DEMAN" );
		}

		// ------ detect icon ---------//

		help.set_custom_help(x+6, y+48, x+50, y+79, str);
	}
}
//----------- End of function FirmMarket::detect_market_info -------//



//--------- Begin of function FirmMarket::disp_income ---------//
//
// Display monthly expense information.
//
void FirmMarket::disp_income(int dispY1, int refreshFlag)
{
//	int x2 = font_snds.put( INFO_X1+26, INFO_Y1+21, "Yearly Income" ) + 12;
//	x2 = font_snds.put( x2, INFO_Y1+21, (int) income_365days() );

	font_snds.put( INFO_X1+26, INFO_Y1+21, text_firm.str_yearly_income((int) income_365days()) );
}
//----------- End of function FirmMarket::disp_income -----------//


//------- Begin of function FirmMarket::draw -----------//
//
// Draw product stocks.
//
void FirmMarket::draw(int displayLayer)
{
	Firm::draw(displayLayer);

	if( under_construction )
		return;

	if( displayLayer == 1)
	{
		//------- draw market goods cargoes ---------//

		int   	 		i, j, x, y, cargoCount, sectionId=0;
		MarketGoods*	marketGoods;
		char* 			iconPtr;

		if (config.building_size != 1)	
			sectionId = 6;

		for( i=0, marketGoods=market_goods_array ; i<MAX_MARKET_GOODS ; i++, marketGoods++ )
		{
			if( marketGoods->raw_id )
				iconPtr = raw_res.small_raw_icon(marketGoods->raw_id);

			else if( marketGoods->product_raw_id )
				iconPtr = raw_res.small_product_icon(marketGoods->product_raw_id);

			else
				continue;

			//------- draw cargo on the firm bitmap buffer --------//

			cargoCount = MAX_CARGO * (int)marketGoods->stock_qty/(int)max_stock_qty;
			cargoCount = (marketGoods->stock_qty > 0) ? max(1, cargoCount) : 0;

			x = loc_x1*LOCATE_WIDTH + section_point_array[sectionId].x;
			y = loc_y1*LOCATE_HEIGHT + section_point_array[sectionId].y;

			sectionId++;

			for( j=0 ; j<cargoCount ; j++ )
			{
				if (config.building_size == 1)	
					world.zoom_matrix->put_bitmap( x + slot_point_array[j].x, y + slot_point_array[j].y,
						altitude, iconPtr, NULL, 0, 0);
				else
					world.zoom_matrix->put_bitmap( x + slot_point_array[j+9].x, y + slot_point_array[j+9].y,
						altitude, iconPtr, NULL, 0, 0);
			}
		}
	}
}
//--------- End of function FirmMarket::draw -----------//


// --------- begin of function FirmMarket::disp_edit_mode -------//
//
void FirmMarket::disp_edit_mode(int& refreshFlag, int& y, int dispBG)
{
	// if display background here, set dispBG to 0

	Firm::disp_edit_mode( refreshFlag, y, dispBG);

	int x;
	int x1 = INFO_X1 + 12;
	int x2 = INFO_X2 - 10;

	// edit hit point
	for( int i = 0; i < MAX_MARKET_GOODS; ++i )
	{
		MarketGoods* marketGoods = &market_goods_array[i];
		edit_stock_enable[i] = marketGoods->raw_id != 0 || marketGoods->product_raw_id != 0;

		if( edit_stock_enable[i] )
		{
			if( marketGoods->product_raw_id )
				x = font_whbl.put( x1, y, text_firm.str_factory_ware(marketGoods->product_raw_id), 0, x2 );
			else if( marketGoods->raw_id )
				x = font_whbl.put( x1, y, text_firm.str_factory_raw(marketGoods->raw_id), 0, x2 );
			else
			{
				err_here();
			}

			x += 20;
			if( x < INFO_X1+120 )
				x = INFO_X1+120;
			x = edit_stock_x2[i] = font_blue.put( (edit_stock_x1[i]=x), (edit_stock_y1[i]=y), (int)marketGoods->stock_qty, 0, x2 );
			edit_stock_y2[i] = edit_stock_y1[i]+font_blue.max_font_height;
		}
		y += 18;
	}
}
// --------- end of function FirmMarket::disp_edit_mode -------//


// --------- begin of function FirmMarket::detect_edit_mode -------//
//
int FirmMarket::detect_edit_mode()
{
	int rc;
	if( (rc = Firm::detect_edit_mode()) )
		return rc;

	for( int i = 0; i < MAX_MARKET_GOODS; ++i )
	{
		MarketGoods* marketGoods = &market_goods_array[i];
		if( edit_stock_enable[i] )
		{
			if( mouse.any_click(edit_stock_x1[i], edit_stock_y1[i], edit_stock_x2[i], edit_stock_y2[i], LEFT_BUTTON) )
			{
				marketGoods->stock_qty += 100.0f;
				if( marketGoods->stock_qty > MAX_MARKET_STOCK )
					marketGoods->stock_qty = MAX_MARKET_STOCK;
				return 1;
			}
			if( mouse.any_click(edit_stock_x1[i], edit_stock_y1[i], edit_stock_x2[i], edit_stock_y2[i], RIGHT_BUTTON) )
			{
				marketGoods->stock_qty -= 100.0f;
				if( marketGoods->stock_qty < 0.0f )
					marketGoods->stock_qty = 0.0f;
				return 1;
			}
		}
	}

	return 0;
}
// --------- end of function FirmMarket::detect_edit_mode -------//

