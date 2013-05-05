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

//Filename    : ORAWRES.CPP
//Description : Raw material resource object

#include <oinfo.h>
#include <ovga.h>
#include <ohelp.h>
#include <ofont.h>
#include <osys.h>
#include <ogameset.h>
#include <oworldmt.h>
#include <of_fact.h>
#include <of_mine.h>
#include <of_mark.h>
#include <orawres.h>
#include <oconfig.h>
#include <ot_firm.h>

//---------- #define constant ------------//

#define RAW_DB   "RAW"

//------- Begin of function RawRes::RawRes -----------//

RawRes::RawRes()
{
	init_flag=0;
}

//--------- End of function RawRes::RawRes -----------//


//---------- Begin of function RawRes::init -----------//
//
// This function must be called after a map is generated.
//
void RawRes::init()
{
	deinit();

	//----- open unit bitmap resource file -------//

	String str;

	str  = DIR_RES;
	str += "I_RAW.RES";

//	res_icon.init(str, 1);  // 1-read all into buffer
	res_map_icon.init_imported(str, 1);  // 1-don't read all into buffer

	res_small_raw.init_imported(DIR_RES"I_RAWRS.RES", 1);
	res_small_product.init_imported(DIR_RES"I_RAWPS.RES" ,1);
	res_large_raw.init_imported(DIR_RES"I_RAWRL.RES" ,1);
	res_large_product.init_imported(DIR_RES"I_RAWPL.RES" ,1);
	res_interface_raw.init_imported(DIR_RES"I_RAWRI.RES" ,1);

	//------- load database information --------//

	load_all_info();

	init_flag=1;
}
//---------- End of function RawRes::init -----------//


//---------- Begin of function RawRes::deinit -----------//

void RawRes::deinit()
{
	if( init_flag )
	{
		delete [] raw_info_array;
		init_flag=0;
   }
}
//---------- End of function RawRes::deinit -----------//


//------- Begin of function RawRes::load_all_info -------//
//
// Read in information of RAW.DBF into memory array
//
void RawRes::load_all_info()
{
   RawRec  *rawRec;
   RawInfo *rawInfo;
	int      i;
	Database *dbRaw = game_set.open_db(RAW_DB);

	raw_count = (short) dbRaw->rec_count();
	raw_count = raw_count >> 1;

	raw_info_array = new RawInfo[raw_count];

	//------ read in raw information array -------//

	for( i=0 ; i<raw_count ; i++ )
	{
		if (config.building_size == 1)	
			rawRec  = (RawRec*) dbRaw->read(i+1);
		else
			rawRec  = (RawRec*) dbRaw->read(i+1+raw_count);

		rawInfo = raw_info_array+i;

		m.rtrim_fld( rawInfo->name, rawRec->name, rawRec->NAME_LEN );
		translate.multi_to_win(rawInfo->name, rawInfo->NAME_LEN);

		rawInfo->raw_id    = i+1;
		rawInfo->tera_type = m.atoi( rawRec->tera_type, rawRec->TERA_TYPE_LEN );

		long bitmapOffset;

		memcpy( &bitmapOffset, rawRec->map_icon_ptr, sizeof(long) );
		rawInfo->map_icon_ptr = res_map_icon.read_imported(bitmapOffset);

		rawInfo->map_loc_width = (char) m.atoi(rawRec->map_loc_width, rawRec->LOC_LEN );
		rawInfo->map_loc_height = (char) m.atoi(rawRec->map_loc_height, rawRec->LOC_LEN );
		rawInfo->map_icon_offset_x = m.atoi(rawRec->map_icon_offset_x, rawRec->OFFSET_LEN);
		rawInfo->map_icon_offset_y = m.atoi(rawRec->map_icon_offset_y, rawRec->OFFSET_LEN);

		memcpy( &bitmapOffset, rawRec->small_raw_ptr, sizeof(long) );
		rawInfo->small_raw_ptr = res_small_raw.read_imported(bitmapOffset);

		memcpy( &bitmapOffset, rawRec->small_product_ptr, sizeof(long) );
		rawInfo->small_product_ptr = res_small_product.read_imported(bitmapOffset);

		memcpy( &bitmapOffset, rawRec->large_raw_ptr, sizeof(long) );
		rawInfo->large_raw_ptr = res_large_raw.read_imported(bitmapOffset);

		memcpy( &bitmapOffset, rawRec->large_product_ptr, sizeof(long) );
		rawInfo->large_product_ptr = res_large_product.read_imported(bitmapOffset);

		memcpy( &bitmapOffset, rawRec->interface_raw_ptr, sizeof(long) );
		rawInfo->interface_raw_ptr = res_interface_raw.read_imported(bitmapOffset);

		// modify offset_x/y for 7k2
		rawInfo->map_icon_offset_x += -rawInfo->map_loc_width*LOCATE_WIDTH/2 - (-rawInfo->map_loc_width*ZOOM_LOC_X_WIDTH/2 + -rawInfo->map_loc_height*ZOOM_LOC_Y_WIDTH/2);
		rawInfo->map_icon_offset_y += -rawInfo->map_loc_height*LOCATE_HEIGHT/2 - (-rawInfo->map_loc_width*ZOOM_LOC_X_HEIGHT/2 + -rawInfo->map_loc_height*ZOOM_LOC_Y_HEIGHT/2);
	}
}
//--------- End of function RawRes::load_all_info ---------//


//---------- Begin of function RawRes::next_day -----------//

void RawRes::next_day()
{
	if( info.game_date%15==0 )
		update_supply_firm();
}
//---------- End of function RawRes::next_day -----------//


//------- Begin of function RawRes::update_supply_firm -------//

void RawRes::update_supply_firm()
{
	//----- reset the supply array of each raw and product ----//

	for( int i=0 ; i<MAX_RAW ; i++ )
	{
		raw_info_array[i].raw_supply_firm_array.zap();
		raw_info_array[i].product_supply_firm_array.zap();
	}

	//---- locate for suppliers that supply the products needed ----//

	Firm* 		 firmPtr;
	FirmMine* 	 firmMine;
	FirmFactory* firmFactory;
	FirmMarket*  firmMarket;

	for( short firmRecno=firm_array.size() ; firmRecno>0 ; firmRecno-- )
	{
		if( firm_array.is_deleted(firmRecno) )
			continue;

		firmPtr = (Firm*) firm_array[firmRecno];

		//-------- factory as a potential supplier ------//

		if( firmPtr->firm_id == FIRM_FACTORY )
		{
			firmFactory = (FirmFactory*) firmPtr;

			if( firmFactory->product_raw_id &&
				 firmFactory->stock_qty > firmFactory->max_stock_qty / 5 )
			{
				raw_res[firmFactory->product_raw_id]->add_product_supply_firm(firmRecno);
			}
		}

		//-------- mine as a potential supplier ------//

		if( firmPtr->firm_id == FIRM_MINE )
		{
			firmMine = (FirmMine*) firmPtr;

			if( firmMine->raw_id &&
				 firmMine->stock_qty > firmMine->max_stock_qty / 5 )
			{
				raw_res[firmMine->raw_id]->add_raw_supply_firm(firmRecno);
			}
		}

		//-------- market place as a potential supplier ------//

		else if( firmPtr->firm_id == FIRM_MARKET )
		{
			firmMarket = (FirmMarket*) firmPtr;

			MarketGoods* marketGoods = firmMarket->market_goods_array;

			for( int i=0 ; i<MAX_MARKET_GOODS ; i++, marketGoods++ )
			{
				if( marketGoods->stock_qty > MAX_MARKET_STOCK / 5 )
				{
					if( marketGoods->product_raw_id )
						raw_res[marketGoods->product_raw_id]->add_product_supply_firm(firmRecno);

					else if( marketGoods->raw_id )
						raw_res[marketGoods->raw_id]->add_raw_supply_firm(firmRecno);
				}
			}
		}
	}
}
//-------- End of function RawRes::update_supply_firm --------//


//------- Begin of function RawInfo::RawInfo -----------//

RawInfo::RawInfo() : raw_supply_firm_array(sizeof(short), 70), product_supply_firm_array(sizeof(short), 70)
{
}

//--------- End of function RawInfo::RawInfo -----------//


//------- Begin of function RawInfo::add_raw_supply_firm -----------//

void RawInfo::add_raw_supply_firm(short firmRecno)
{
	err_when( firm_array.is_deleted(firmRecno) );

	raw_supply_firm_array.linkin(&firmRecno);
}

//--------- End of function RawInfo::add_raw_supply_firm --------//


//------- Begin of function RawInfo::add_product_supply_firm -----------//

void RawInfo::add_product_supply_firm(short firmRecno)
{
	err_when( firm_array.is_deleted(firmRecno) );

	product_supply_firm_array.linkin(&firmRecno);
}

//--------- End of function RawInfo::add_product_supply_firm --------//


//---------- Begin of function RawRes::operator[] -----------//

RawInfo* RawRes::operator[](int rawId)
{
	err_if( rawId<1 || rawId>raw_count )
		err_now( "RawRes::operator[]" );

	return raw_info_array+rawId-1;
}

//------------ End of function RawRes::operator[] -----------//


// --------- begin of function RawRes::small_raw_icon --------//
//
char *RawRes::small_raw_icon(int rawId)
{
	return operator[](rawId)->small_raw_ptr;
}
// --------- end of function RawRes::small_raw_icon --------//


// --------- begin of function RawRes::small_product_icon --------//
//
char *RawRes::small_product_icon(int rawId)
{
	return operator[](rawId)->small_product_ptr;
}
// --------- end of function RawRes::small_product_icon --------//


// --------- begin of function RawRes::large_raw_icon --------//
//
char *RawRes::large_raw_icon(int rawId)
{
	return operator[](rawId)->large_raw_ptr;
}
// --------- end of function RawRes::large_raw_icon --------//


// --------- begin of function RawRes::large_product_icon --------//
//
char *RawRes::large_product_icon(int rawId)
{
	return operator[](rawId)->large_product_ptr;
}
// --------- end of function RawRes::large_product_icon --------//

// --------- begin of function RawRes::interface_raw_icon --------//
//
char *RawRes::interface_raw_icon(int rawId)
{
	return operator[](rawId)->interface_raw_ptr;
}
// --------- end of function RawRes::interface_raw_icon --------//


