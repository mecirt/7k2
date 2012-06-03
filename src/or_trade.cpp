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

//Filename    : OR_TOWN.CPP
//Description : Town Report

#include <oinfo.h>
#include <ovga.h>
#include <omodeid.h>
#include <ostr.h>
#include <ofont.h>
#include <oimgres.h>
#include <ovbrowif.h>
#include <obutton.h>
#include <oraceres.h>
#include <ofirm.h>
#include <oworld.h>
#include <onation.h> 
#include <ou_cara.h>
// #include <ou_mari.h>
// #include <oworldmt.h>
#include <ot_reps.h>
#include <ot_unit.h>


//------------- Define coordinations -----------//

//enum { CARAVAN_BROWSE_X1 = ZOOM_X1+6,
//		 CARAVAN_BROWSE_Y1 = ZOOM_Y1+6,
//		 CARAVAN_BROWSE_X2 = ZOOM_X2-6,
//		 CARAVAN_BROWSE_Y2 = CARAVAN_BROWSE_Y1+260
//	  };
#define CARAVAN_BROWSE_X1 (ZOOM_X1+6)
#define CARAVAN_BROWSE_Y1 (ZOOM_Y1+6)
#define CARAVAN_BROWSE_X2 (ZOOM_X2-6)
#define CARAVAN_BROWSE_Y2 (CARAVAN_BROWSE_Y1+260)

//enum { SHIP_BROWSE_X1 = ZOOM_X1+6,
//		 SHIP_BROWSE_Y1 = CARAVAN_BROWSE_Y2+6,
//		 SHIP_BROWSE_X2 = ZOOM_X2-6,
//		 SHIP_BROWSE_Y2 = ZOOM_Y2-6,
//	  };
#define SHIP_BROWSE_X1 (ZOOM_X1+6)
#define SHIP_BROWSE_Y1 (CARAVAN_BROWSE_Y2+6)
#define SHIP_BROWSE_X2 (ZOOM_X2-6)
#define SHIP_BROWSE_Y2 (ZOOM_Y2-6)

//----------- Define static variables ----------//

static VBrowseIF browse_caravan, browse_ship;

//----------- Define static functions ----------//

static void create_caravan_list();
static void create_ship_list();
static void put_caravan_rec(int recNo, int x, int y, int refreshFlag);
static void put_ship_rec(int recNo, int x, int y, int refreshFlag);
static void	disp_total();
static void put_stop_info(int x, int y, TradeStop* tradeStop);

static int  sort_unit( const void *a, const void *b );

//--------- Begin of function Info::disp_trade ---------//
//
void Info::disp_trade(int refreshFlag)
{
	create_caravan_list();
//	create_ship_list();

	//-------- display the caravan browser --------//

	int x=CARAVAN_BROWSE_X1+9;
	int y=CARAVAN_BROWSE_Y1+4;

	vga_back.d3_panel_up(CARAVAN_BROWSE_X1, CARAVAN_BROWSE_Y1, CARAVAN_BROWSE_X2, CARAVAN_BROWSE_Y1+20 );

	font_bld.put( x	 , y, unit_res[UNIT_CARAVAN]->name ); // "Caravan" );
	font_bld.put( x+78 , y, text_unit.str_hit_points() ); // "Hit Points" );
	font_bld.put( x+160, y, text_reports.str_trade_stop(1) ); // "Stop 1" );
	font_bld.put( x+250, y, text_reports.str_trade_stop(2) ); // "Stop 2" );
	font_bld.put( x+340, y, text_reports.str_goods_carried() ); //"Goods Carried" );

	if( refreshFlag == INFO_REPAINT )
	{
		// ###### begin Gilbert 19/10 #######//
		browse_caravan.init( CARAVAN_BROWSE_X1, CARAVAN_BROWSE_Y1+22, CARAVAN_BROWSE_X2, CARAVAN_BROWSE_Y2-20,
									0, 18, report_array.size(), put_caravan_rec, 1 );
		// ###### end Gilbert 19/10 #######//

		browse_caravan.open(browse_caravan_recno);
	}
	else
	{
		browse_caravan.paint();
		browse_caravan.open(browse_caravan_recno, report_array.size());
	}

	//------------ display total -------------//

	disp_total();
}
//----------- End of function Info::disp_trade -----------//


//--------- Begin of function Info::detect_trade ---------//
//
void Info::detect_trade()
{
	//-------- detect the caravan browser ---------//

	if( browse_caravan.detect() )
	{
		browse_caravan_recno = browse_caravan.recno();

		if( browse_caravan.double_click )
		{
			Unit* unitPtr = unit_array[ get_report_data(browse_caravan_recno) ];

			world.go_loc(unitPtr->next_x_loc(), unitPtr->next_y_loc(), 1);
		}
	}

	//-------- detect the ship browser ---------//

/*
	if( browse_ship.detect() )
	{
		browse_ship_recno = browse_ship.recno();

		if( browse_ship.double_click )
		{
			Unit* unitPtr = unit_array[ get_report_data2(browse_ship_recno) ];

			world.go_loc(unitPtr->next_x_loc(), unitPtr->next_y_loc(), 1);
		}
	}
*/
}
//----------- End of function Info::detect_trade -----------//


//-------- Begin of static function disp_total --------//

static void disp_total()
{
	//------- display caravan total --------//

	int x=CARAVAN_BROWSE_X1+9;
	int y=CARAVAN_BROWSE_Y2-16;

	vga_back.d3_panel_up(CARAVAN_BROWSE_X1, CARAVAN_BROWSE_Y2-18, CARAVAN_BROWSE_X2, CARAVAN_BROWSE_Y2 );

//	String str;
//	if( info.report_array.size() > 1 )
//		str = "Total Caravans";
//	else
//		str = "Total Caravan";
//	str  = translate.process(str);
//	str += ": ";
//	str += info.report_array.size();
	font_bld.put( x, y, text_reports.str_total_caravan(info.report_array.size()) );

/*
	//-------- display ship total --------//

	x=SHIP_BROWSE_X1+9;
	y=SHIP_BROWSE_Y2-16;

	vga_back.d3_panel_up(SHIP_BROWSE_X1, SHIP_BROWSE_Y2-18, SHIP_BROWSE_X2, SHIP_BROWSE_Y2 );

	if( info.report_array2.size() > 1 )
		str = "Total Ships";
	else
		str = "Total Ship";

	str  = translate.process(str);
	str += ": ";
	str += info.report_array2.size();

	font_bld.put( x, y, str );
*/
}
//----------- End of static function disp_total -----------//


//-------- Begin of static function create_caravan_list --------//
//
static void create_caravan_list()
{
	int   totalUnit = unit_array.size();
	Unit* unitPtr;

	info.report_array.zap();

	for( short unitRecno=1 ; unitRecno<=totalUnit ; unitRecno++ )
	{
		if( unit_array.is_deleted(unitRecno) )
			continue;

		unitPtr = unit_array[unitRecno];

		if( unitPtr->nation_recno == info.viewing_nation_recno &&
			 unitPtr->unit_id == UNIT_CARAVAN )
		{
			info.report_array.linkin(&unitRecno);
		}
	}

	info.report_array.quick_sort(sort_unit);
}
//----------- End of static function create_caravan_list -----------//


//-------- Begin of static function create_ship_list --------//
//
static void create_ship_list()
{
	int   totalUnit = unit_array.size();
	Unit* unitPtr;

	info.report_array2.zap();

	for( short unitRecno=1 ; unitRecno<=totalUnit ; unitRecno++ )
	{
		if( unit_array.is_deleted(unitRecno) )
			continue;

		unitPtr = unit_array[unitRecno];

		if( unitPtr->nation_recno == info.viewing_nation_recno &&
			 unit_res[unitPtr->unit_id]->carry_goods_capacity > 0 )
		{
			info.report_array2.linkin(&unitRecno);
		}
	}

	info.report_array2.quick_sort(sort_unit);
}
//----------- End of static function create_ship_list -----------//


//-------- Begin of static function put_caravan_rec --------//
//
static void put_caravan_rec(int recNo, int x, int y, int refreshFlag)
{
	int   		 unitRecno = info.get_report_data(recNo);
	UnitCaravan* unitPtr   = (UnitCaravan*) unit_array[unitRecno];

	int x2 = x+browse_caravan.rec_width-3;

	//---------- display info ----------//

	x+=3;
	// ##### begin Gilbert 19/10 #######//
	y+=2;
	// ##### end Gilbert 19/10 #######//

	String str;
	str  = (int) unitPtr->hit_points;
	str += "/";
	str += unitPtr->max_hit_points();

	font_bld.put( x    , y, unitPtr->unit_name() );
	font_bld.put( x+90 , y, str );

	//------- display pick up type of each stop -------//

	if( unitPtr->stop_defined_num >= 1 )
		put_stop_info( x+160, y, unitPtr->stop_array );

	if( unitPtr->stop_defined_num >= 2 )
		put_stop_info( x+250, y, unitPtr->stop_array+1 );
//		put_stop_info( x+220, y, unitPtr->stop_array+1 );

//	if( unitPtr->stop_defined_num >= 3 )
//		put_stop_info( x+280, y, unitPtr->stop_array+2 );

	//------- display goods carried -------//

	x += 340;

	char *bitmapPtr;
	int i;

	for( i=0; i<MAX_PRODUCT; i++)
	{
		if( unitPtr->product_raw_qty_array[i]==0 )
			continue;

		bitmapPtr = raw_res.small_product_icon(i+1);
		vga_back.put_bitmap( x, y, bitmapPtr );

		font_bld.disp( x+14, y-1, unitPtr->product_raw_qty_array[i], 1, x+45 );
		x+=36;

		if( x+36 > x2 )
			return;
	}

	for(i=0; i<MAX_RAW; i++)
	{
		if( unitPtr->raw_qty_array[i]==0 )
			continue;

		bitmapPtr = raw_res.small_raw_icon(i+1);
		vga_back.put_bitmap( x, y, bitmapPtr );

		font_bld.disp( x+14, y-1, unitPtr->raw_qty_array[i], 1, x+45 );
		x+=36;

		if( x+36 > x2 )
			return;
	}
}
//----------- End of static function put_caravan_rec -----------//


//-------- Begin of static function put_ship_rec --------//
//
static void put_ship_rec(int recNo, int x, int y, int refreshFlag)
{
	/*
	int   		unitRecno = info.get_report_data2(recNo);
	UnitMarine* unitPtr   = (UnitMarine*) unit_array[unitRecno];

	int x2 = x+browse_ship.rec_width-3;

	//---------- display info ----------//

	x+=3;
	y+=3;

	String str;
	str  = (int) unitPtr->hit_points;
	str += "/";
	str += unitPtr->max_hit_points();

	font_bld.put( x    , y, unitPtr->unit_name() );
	font_bld.put( x+90 , y, str );

	//------- display pick up type of each stop -------//

	if( unitPtr->stop_defined_num >= 1 )
		put_stop_info( x+160, y, unitPtr->stop_array );

	if( unitPtr->stop_defined_num >= 2 )
		put_stop_info( x+220, y, unitPtr->stop_array+1 );

	if( unitPtr->stop_defined_num >= 3 )
		put_stop_info( x+280, y, unitPtr->stop_array+2 );

	//------- display goods carried -------//

	x += 340;

	char *bitmapPtr;

	for(int i=0; i<MAX_RAW; i++)
	{
		if( unitPtr->raw_qty_array[i]==0 )
			continue;

		bitmapPtr = raw_res.small_raw_icon(i+1);
		vga_back.put_bitmap( x, y, bitmapPtr );

		font_bld.disp( x+14, y-1, unitPtr->raw_qty_array[i], 1, x+45 );
		x+=36;

		if( x+36 > x2 )
			return;
	}

	for( i=0; i<MAX_PRODUCT; i++)
	{
		if( unitPtr->product_raw_qty_array[i]==0 )
			continue;

		bitmapPtr = raw_res.small_product_icon(i+1);
		vga_back.put_bitmap( x, y, bitmapPtr );

		font_bld.disp( x+14, y-1, unitPtr->product_raw_qty_array[i], 1, x+45 );
		x+=36;

		if( x+36 > x2 )
			return;
	}
	*/
}
//----------- End of static function put_ship_rec -----------//


//-------- Begin of static function put_stop_info --------//
//
static void put_stop_info(int x, int y, TradeStop* tradeStop)
{
//	int x2=x+58;
	int x2=x+88;

	//----- display the color of the stop ----//

	if( firm_array.is_deleted(tradeStop->firm_recno) )
		return;

	Firm* firmPtr = firm_array[tradeStop->firm_recno];

	nation_array[firmPtr->nation_recno]->disp_nation_color( x, y );

	x+=15;

	//------ display pick up type icon ------//

	int i, pickUpType = tradeStop->pick_up_type;

	switch( pickUpType )
	{
		case AUTO_PICK_UP:
			vga_back.put_bitmap( x, y, image_icon.get_ptr("AUTOPICK") );
			break;

		case NO_PICK_UP:
//			vga_back.put_bitmap( x, y, image_icon.get_ptr("NOPICK") );
			break;

		default:
			for( i=PICK_UP_PRODUCT_FIRST ; i<=PICK_UP_PRODUCT_LAST ; i++ )
			{
				if( tradeStop->pick_up_array[i-1] )
				{
					vga_back.put_bitmap( x, y, raw_res.small_product_icon(i-PICK_UP_PRODUCT_FIRST+1) );
					x+=12;

					if( x+10 > x2 )
						return;
				}
			}

			for( i=PICK_UP_RAW_FIRST ; i<=PICK_UP_RAW_LAST ; i++ )
			{
				if( tradeStop->pick_up_array[i-1] )
				{
					vga_back.put_bitmap( x, y, raw_res.small_raw_icon(i-PICK_UP_RAW_FIRST+1) );
					x+=12;

					if( x+10 > x2 )
						return;
				}
			}

	}
}
//----------- End of static function put_stop_info -----------//


//------ Begin of function sort_unit ------//
//
static int sort_unit( const void *a, const void *b )
{
	Unit* unitPtr1 = (Unit*) unit_array[*((short*)a)];
	Unit* unitPtr2 = (Unit*) unit_array[*((short*)b)];

	return unitPtr1->name_id - unitPtr2->name_id;
}
//------- End of function sort_unit ------//

