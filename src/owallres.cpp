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

//Filename    : OWALLRES.CPP
//Description : Wall resource object
//Ownership   : Gilbert

#include <osys.h>
#include <ovga.h>
#include <omodeid.h>
#include <ofont.h>
#include <ogameset.h>
#include <oworld.h>
// #include <oworldmt.h>
#include <onation.h>
#include <walltile.h>
#include <owallres.h>

//---------- #define constant ------------//

#define WALL_DB   		"WALL"

//------- Begin of function WallRes::WallRes -----------//

WallRes::WallRes()
{
	init_flag=0;

	selected_x_loc = -1;
	selected_y_loc = -1;
}
//--------- End of function WallRes::WallRes -----------//


//---------- Begin of function WallRes::init -----------//
//
// This function must be called after a map is generated.
//
void WallRes::init()
{
	deinit();

	//----- open wall material bitmap resource file -------//

	String str;

	str  = DIR_RES;
	str += "I_WALL.RES";

	res_bitmap.init_imported(str,1);  // 1-read all into buffer

	//------- load database information --------//

	load_wall_info();

	init_flag=1;
}
//---------- End of function WallRes::init -----------//


//---------- Begin of function WallRes::deinit -----------//

void WallRes::deinit()
{
	if( init_flag )
	{
		mem_del(wall_info_array);
		mem_del(wall_index);
		init_flag=0;
	}
}
//---------- End of function WallRes::deinit -----------//


//------- Begin of function WallRes::load_wall_info -------//
//
void WallRes::load_wall_info()
{
	WallRec		*wallRec;
	WallInfo		*wallInfo;
	int			i;
	long			bitmapOffset;
	
	max_wall_id = 0;

	//---- read in wall count and initialize wall info array ----//

	Database *dbWall = game_set.open_db(WALL_DB);	// only one database can be opened at a time, so we read FIRM.DBF first

	wall_count      = (short) dbWall->rec_count();
	wall_info_array = (WallInfo*) mem_add( sizeof(WallInfo)*wall_count );

	memset( wall_info_array, 0, sizeof(WallInfo) * wall_count );

	//---------- read in WALL.DBF ---------//

	for( i=0 ; i<wall_count ; i++ )
	{
		wallRec  = (WallRec*) dbWall->read(i+1);
		wallInfo = wall_info_array+i;

		wallInfo->wall_id = m.atoi( wallRec->wall_id, wallRec->WALL_ID_LEN);
		wallInfo->flags = 0;
		if( wallRec->gate_flag == 'Y' || wallRec->gate_flag == 'y')
			wallInfo->set_gate();
		if( wallRec->trans_flag == 'Y' || wallRec->trans_flag == 'y')
			wallInfo->set_trans();
		wallInfo->offset_x = m.atoi( wallRec->offset_x, wallRec->OFFSET_LEN);
		wallInfo->offset_y = m.atoi( wallRec->offset_y, wallRec->OFFSET_LEN);
		wallInfo->loc_off_x = m.atoi( wallRec->loc_off_x, wallRec->LOC_OFF_LEN);
		wallInfo->loc_off_y = m.atoi( wallRec->loc_off_y, wallRec->LOC_OFF_LEN);

		wallInfo->draw_wall_id = m.atoi( wallRec->draw_wall, wallRec->WALL_ID_LEN);

		memcpy( &bitmapOffset, wallRec->bitmap_ptr, sizeof(long) );
		wallInfo->bitmap_ptr	= res_bitmap.read_imported(bitmapOffset);

		if( wallInfo->wall_id > max_wall_id)
			max_wall_id = wallInfo->wall_id;
	}

	// --------- build wall_index ---------//
	if( max_wall_id > 0)
	{
		wall_index = (WallInfo **) mem_add(sizeof(WallInfo *) * max_wall_id);
		memset( wall_index, 0, sizeof(WallInfo *) * max_wall_id);
		for( i=0 ; i<wall_count ; i++ )
		{
			wall_index[wall_info_array[i].wall_id -1] = &wall_info_array[i];
		}
	}
	else
	{
		err.run("No wall database found");
	}
}
//--------- End of function WallRes::load_wall_info ---------//


//------- Begin of function WallRes::draw_selected -------//
//
void WallRes::draw_selected()
{
	if( selected_x_loc < 0 )
		return;
}
//--------- End of function WallRes::draw_selected ---------//


//------- Begin of function WallRes::disp_info -------//
//
void WallRes::disp_info(int refreshFlag)
{
	if( selected_x_loc < 0 )
		return;

	font_san.d3_put( INFO_X1, INFO_Y1, INFO_X2, INFO_Y1+17, "Defense Wall" );
	vga_buffer.d3_panel_up( INFO_X1, INFO_Y1+20, INFO_X2, INFO_Y1+40, 1 );

	int x=INFO_X1+3, y=INFO_Y1+23;
	Location* locPtr = world.get_loc( selected_x_loc, selected_y_loc );

	String str;

	str  = locPtr->wall_abs_hit_point();
	str += " / 100";

	font_san.field( x, y, "Hit Points",  x+80, str, INFO_X2-4, refreshFlag );
}
//--------- End of function WallRes::disp_info ---------//


//---------- Begin of function WallRes::operator[] -----------//

WallInfo* WallRes::operator[](int wallId)
{
	err_if( wallId<1 || wallId>max_wall_id )
		err_now( "WallRes::operator[]" );

	return wall_index[wallId-1];
}
//------------ End of function WallRes::operator[] -----------//

//------- Begin of function WallInfo::draw -----------//
//
// Draw the current wall on the map
//
void WallInfo::draw(int xLoc, int yLoc, short *remapTbl)
{
	return ;		// no wall in 7k2
}
//--------- End of function WallInfo::draw -----------//


//------- Begin of function WallInfo::draw_at -----------//
//
// Draw the wall on the zoom map, given the exact pixel position to put
// the bitmap.
//
// <int> absBaseX, absBaseY - the absolute base (center-bottom) coordination
//										of the building.
//
// Draw the current plant on the map
//
void WallInfo::draw_at(int absBaseX, int absBaseY, short *remapTbl)
{
	// no wall in 7k2
}
//--------- End of function WallInfo::draw_at -----------//

