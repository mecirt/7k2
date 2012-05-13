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

// Filename    : OSNOWRES.CPP
// Description : Snow Resource
// Onwer       : Gilbert


#include <all.h>
#include <osnowres.h>
#include <ovgabuf.h>
#include <ogameset.h>
#include <oworld.h>
#include <oworldmt.h>


// --------- define constant --------//

#define SNOW_DB	"SNOWG"


// ------- Begin of function SnowInfo::draw_at ------//
void SnowInfo::draw_at(short absX, short absY)
{
	// no snow in 7k2

}
// ------- End of function SnowInfo::draw_at ------//


// ------- Begin of function SnowRes::SnowRes ------//
SnowRes::SnowRes()
{
	init_flag = 0;
}
// ------- End of function SnowRes::SnowRes ------//


// ------- Begin of function SnowRes::~SnowRes ------//
SnowRes::~SnowRes()
{
	deinit();
}
// ------- End of function SnowRes::~SnowRes ------//


// ------- Begin of function SnowRes::init ------//
void SnowRes::init()
{
	deinit();

	//----- open snow ground bitmap resource file -------//

	String str;

	str  = DIR_RES;
	str += "I_SNOW.RES";

	res_bitmap.init_imported(str,1);  // 1-read all into buffer

	//------- load database information --------//

	load_info();

	init_flag=1;
}
// ------- End of function SnowRes::init ------//


// ------- Begin of function SnowRes::deinit ------//
void SnowRes::deinit()
{
	if( init_flag )
	{
		mem_del( snow_info_array );
		mem_del( root_info_array );
		init_flag = 0;
	}
}
// ------- End of function SnowRes::deinit ------//


// ------- Begin of function SnowRes::load_info ------//
void SnowRes::load_info()
{
	SnowRec		*snowRec;
	SnowInfo		*snowInfo;
	int			i;
	long			bitmapOffset;
	
	//---- read in snow map count and initialize snow info array ----//

	Database *dbSnow = game_set.open_db(SNOW_DB);	// only one database can be opened at a time

	snow_info_count = (int) dbSnow->rec_count();
	snow_info_array = (SnowInfo *) mem_add(sizeof(SnowInfo) * snow_info_count );

	memset( snow_info_array, 0, sizeof(SnowInfo) * snow_info_count );
	root_count = 0;

	//---------- read in SNOWG.DBF ---------//

	for( i=0 ; i<snow_info_count; i++ )
	{
		snowRec = (SnowRec *) dbSnow->read(i+1);
		snowInfo = snow_info_array+i;

		snowInfo->snow_map_id = i+1;
		memcpy( &bitmapOffset, snowRec->bitmap_ptr, sizeof(long) );
		snowInfo->bitmap_ptr	= res_bitmap.read_imported(bitmapOffset);
		if( snowRec->offset_x[0] != '\0' && snowRec->offset_x[0] != ' ')
			snowInfo->offset_x = (short) m.atoi(snowRec->offset_x, snowRec->OFFSET_LEN);
		else
			snowInfo->offset_x = -(snowInfo->bitmap_width() / 2);
		if( snowRec->offset_y[0] != '\0' && snowRec->offset_y[0] != ' ')
			snowInfo->offset_y = (short) m.atoi(snowRec->offset_y, snowRec->OFFSET_LEN);
		else
			snowInfo->offset_y = -(snowInfo->bitmap_height() / 2);

		snowInfo->next_count = 0;
		snowInfo->prev_count = 0;

		if( snowRec->next_file1[0] != '\0' && snowRec->next_file1[0] != ' ')
		{
			snowInfo->next_file[snowInfo->next_count++] = snow_info_array + m.atoi( snowRec->next_ptr1, snowRec->RECNO_LEN) -1;
		}
		if( snowRec->next_file2[0] != '\0' && snowRec->next_file2[0] != ' ')
		{
			snowInfo->next_file[snowInfo->next_count++] = snow_info_array + m.atoi( snowRec->next_ptr2, snowRec->RECNO_LEN) -1;
		}
		if( snowRec->next_file3[0] != '\0' && snowRec->next_file3[0] != ' ')
		{
			snowInfo->next_file[snowInfo->next_count++] = snow_info_array + m.atoi( snowRec->next_ptr3, snowRec->RECNO_LEN) -1;
		}
		if( snowRec->next_file4[0] != '\0' && snowRec->next_file4[0] != ' ')
		{
			snowInfo->next_file[snowInfo->next_count++] = snow_info_array + m.atoi( snowRec->next_ptr4, snowRec->RECNO_LEN) -1;
		}
		if( snowRec->prev_file1[0] != '\0' && snowRec->prev_file1[0] != ' ')
		{
			snowInfo->prev_file[snowInfo->prev_count++] = snow_info_array + m.atoi( snowRec->prev_ptr1, snowRec->RECNO_LEN) -1;
		}
		if( snowRec->prev_file2[0] != '\0' && snowRec->prev_file2[0] != ' ')
		{
			snowInfo->prev_file[snowInfo->prev_count++] = snow_info_array + m.atoi( snowRec->prev_ptr2, snowRec->RECNO_LEN) -1;
		}

		if(snowInfo->is_root())
		{
			root_count++;
		}
	}

	root_info_array = (SnowInfo **)mem_add(sizeof(SnowInfo *) * root_count);
	unsigned j = 0;
	for( i=0 ; i<snow_info_count; i++ )
	{
		snowInfo = snow_info_array+i;
		if(snowInfo->is_root())
		{
			root_info_array[j++] = snowInfo;
			if( j >= root_count)
				break;
		}
	}
}
// ------- End of function SnowRes::load_info ------//


// ------- Begin of function SnowRes::rand_root ------//
int SnowRes::rand_root(unsigned rand)
{
	err_when(!init_flag);
	if( root_count > 0)
	{
		return root_info_array[rand % root_count]->snow_map_id;
	}
	return NULL;
}
// ------- End of function SnowRes::rand_root ------//

#ifdef DEBUG
// ------- Begin of function SnowRes::operator[] ------//
SnowInfo * SnowRes::operator[](int snowMapId)
{
	err_when( snowMapId < 1 || snowMapId > snow_info_count );
	err_when( snow_info_array[snowMapId-1].snow_map_id != snowMapId);
	return snow_info_array + snowMapId -1;
}
// ------- End of function SnowRes::operator[] ------//
#endif
