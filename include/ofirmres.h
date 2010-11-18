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

//Filename    : OFIRMRES.H
//Description : Header file of object FirmRes

#ifndef __OFIRMRES_H
#define __OFIRMRES_H

#ifndef __GAMEDEF_H
#include <gamedef.h>
#endif

#ifndef __OFIRM_H
#include <ofirm.h>
#endif

#ifndef __ORESDB_H
#include <oresdb.h>
#endif

#include <oattack.h>

//------- Define constant ----------//

#define MAX_FIRM_FRAME			50		// Maximum frames for firm animation

#define MAX_FIRM_LOC_WIDTH  	6	 	// Maximum no. of locations a firm can occupy
#define MAX_FIRM_LOC_HEIGHT 	6

//------------- Firm Mode ------------//

enum { FIRM_UNDER_CONSTRUCTION='U',		// definitions of FirmBuild::mode
		 FIRM_IDLE='I',
		 FIRM_ACTIVE='A',
	  };

//------------ Define struct FirmRec ---------------//

struct FirmRec
{
	enum { CODE_LEN=8, NAME_LEN=20, SHORT_NAME_LEN=12, TITLE_LEN=10, FIRST_BUILD_LEN=3, BUILD_COUNT_LEN=3,
			 FIRST_GROUP_LEN=3, GROUP_COUNT_LEN=3, HIT_POINTS_LEN=5, COST_LEN=5 };

	char code[CODE_LEN];
	char name[NAME_LEN];
	char short_name[SHORT_NAME_LEN];

	char overseer_title[TITLE_LEN];
	char worker_title[TITLE_LEN];

	char tera_type;
	
	char all_know;
	char live_in_town;

	char hit_points[HIT_POINTS_LEN];

	char is_linkable_to_town;

	char setup_cost[COST_LEN];
	char setup_live_points_cost[COST_LEN];
	char year_cost[COST_LEN];
	char year_live[COST_LEN];

	char is_military;				// whether this is a military related building

	char first_build[FIRST_BUILD_LEN];
	char build_count[BUILD_COUNT_LEN];
	
	char first_group[FIRST_GROUP_LEN];
	char group_count[GROUP_COUNT_LEN];
};

//------------ Define struct FirmBuildRec ---------------//

struct FirmBuildRec
{
	enum { FIRM_CODE_LEN=8, GROUP_LEN=2, BITMAP_RECNO_LEN=5, FIRST_FRAME_LEN=5, FRAME_COUNT_LEN=2,
			 PAL_NAME_LEN=8, PAL_OFFSET_LEN=4, NAME_LEN=20, ATTRIBUTE_LEN=3, BULLET_SPRITE_LEN=8, 
			 BULLET_ID_LEN=3, IS_WOOD_LEN=3, OFFSET_LEN=4};

	char firm_code[FIRM_CODE_LEN];
	char group[GROUP_LEN];

	char animate_full_size;

	char use_first_frame;

	char pal_file_name[PAL_NAME_LEN];
	char pal_offset[PAL_OFFSET_LEN];
 
	char name[NAME_LEN];

	char under_construction_bitmap_recno[BITMAP_RECNO_LEN];
	char under_construction_bitmap_count[FRAME_COUNT_LEN];
	char idle_bitmap_recno[BITMAP_RECNO_LEN];
	char ground_bitmap_recno[BITMAP_RECNO_LEN];

	char first_frame[FIRST_FRAME_LEN];
	char frame_count[FRAME_COUNT_LEN];
	char ani_part_count[FRAME_COUNT_LEN];
	char sturdiness[ATTRIBUTE_LEN];
	char is_wood[IS_WOOD_LEN];

	char firm_resource_name[PAL_NAME_LEN];

	char bullet_sprite_code[BULLET_SPRITE_LEN];
	char bullet_sprite_id[BULLET_ID_LEN];

//	char min_offset_y[OFFSET_LEN];
};

//------------ Define struct FirmFrameRec ---------------//

struct FirmFrameRec
{
	enum { FIRM_CODE_LEN=8, GROUP_LEN=2, FRAME_ID_LEN=2, DELAY_LEN=2, FIRST_BITMAP_LEN=5, BITMAP_COUNT_LEN=2 };

	char firm_code[FIRM_CODE_LEN];
	char group[GROUP_LEN];

	char frame_id[FRAME_ID_LEN];

	char delay[DELAY_LEN];		// unit: 1/10 second

	char first_bitmap[FIRST_BITMAP_LEN];
	char bitmap_count[BITMAP_COUNT_LEN];
};

//------------ Define struct FirmBitmapRec ---------------//

struct FirmBitmapRec
{
	enum { FIRM_CODE_LEN=8, GROUP_LEN=2, FRAME_ID_LEN=2, RANDOM_FLAG_LEN=1, LOC_LEN=3, OFFSET_LEN=4, DELAY_LEN=2, FILE_NAME_LEN=8, BITMAP_PTR_LEN=4 };

	char firm_code[FIRM_CODE_LEN];
	char group[GROUP_LEN];
	char mode;

	char frame_id[FRAME_ID_LEN];
	char ani_part[FRAME_ID_LEN];
	char random_flag[RANDOM_FLAG_LEN];

	char loc_width[LOC_LEN];
	char loc_height[LOC_LEN];
	char layer;

	char offset_x[OFFSET_LEN];
	char offset_y[OFFSET_LEN];

	char delay[DELAY_LEN];	  // unit: 1/10 second

	char width[LOC_LEN];
	char height[LOC_LEN];

	char file_name[FILE_NAME_LEN];
	char bitmap_ptr[BITMAP_PTR_LEN];
};

//------------ Define struct FirmGroupRec ---------------//

struct FirmGroupRec
{
	enum { FIRM_CODE_LEN=8, GROUP_LEN=2, RACE_CODE_LEN=8, RACE_ID_LEN=3 };

	char firm_code[FIRM_CODE_LEN];
	char group[GROUP_LEN];

	char race_code[RACE_CODE_LEN];
	char race_id[RACE_ID_LEN];
	
};

//------------- Define struct FirmInfo --------------//

struct FirmInfo
{
	enum { NAME_LEN=20, SHORT_NAME_LEN=12, TITLE_LEN=10 };

	char	firm_id;
	char 	name[NAME_LEN+1];
	char  short_name[SHORT_NAME_LEN+1];

	char  overseer_title[TITLE_LEN+1];
	char  worker_title[TITLE_LEN+1];
	
	char  only_build_race_id;		// if this > 0, then only unit of this race can build the firm 

	char  tera_type;
	char  buildable;					// whether this building can be built by the player or it exists in the game since the beginning of the game. If setup_cost==0, this firm is not buildable
	char  live_in_town;		// whether the workers of the firm lives in towns or not.
	short	max_hit_points;

	char	need_overseer;
	char	need_worker;
	char	need_unit()		{ return need_overseer || need_worker; }
	char	same_race_worker;

	short setup_cost;
	short	setup_live_points_cost;
	short year_cost;
	short year_live;

	short	first_build_id;
	short build_count;

	short	first_group_id;
	short group_count;

	short loc_width;
	short loc_height;

	int	can_build(int unitRecno, int checkCash=1);
	int 	nation_can_build(int nationRecno, int raceId, int checkCash=1);

	char  is_linkable_to_town;
	int 	is_linkable_to_firm(int linkFirmId);
	int 	default_firm_link_status(int linkFirmId);

	char  is_military;			// whether this is a military related building

	//---------- game vars -----------//

	short total_firm_count;								// total no. of this firm type on the map
	short nation_firm_count_array[MAX_NATION];
	int	get_nation_firm_count(int nationRecno) 						{ return nation_firm_count_array[nationRecno-1]; }

	char  nation_tech_level_array[MAX_NATION];
	int   get_nation_tech_level(int nationRecno) 						{ return nation_tech_level_array[nationRecno-1]; }
	void  set_nation_tech_level(int nationRecno, char techLevel) 	{ nation_tech_level_array[nationRecno-1] = techLevel; }

public:
	int	get_build_id(char* buildCode);
	int	get_build_id(int raceId);
	char	get_race_id(char* buildCode);
	void	inc_nation_firm_count(int nationRecno);
	void	dec_nation_firm_count(int nationRecno);
};

//------------- Define struct FirmBuild --------------//

struct FirmBuild
{
	enum { BUILD_CODE_LEN=8, NAME_LEN=20, FIRM_RES_NAME_LEN = 8, BULLET_SPRITE_LEN = 8};

	char	firm_id; 
//	char  build_code[BUILD_CODE_LEN+1];		// building code, either a race code or a custom code for each firm's own use, it is actually read from FirmBuildRec::race_code[] 
//	char  race_id;
	
	char  group;
	
	char  animate_full_size;
	
	char  use_first_frame;

	char	name[NAME_LEN+1];

	char bullet_sprite_code[BULLET_SPRITE_LEN+1];

	short	bullet_sprite_id;

	char firm_resource_name[FIRM_RES_NAME_LEN+1];

	DefenseAttribute defense_attribute;

	//----- info of the first frame -----//

	char	loc_width;			// no. of locations it takes horizontally and vertically
	char	loc_height;

	short min_offset_x, min_offset_y;
	short max_bitmap_width, max_bitmap_height;

//	short min_offset_y;

	// ------- palette info -----------//

	BYTE*  pal_ptr;			// coded palette pointer, owned by FirmBuild
	short* palw_ptr;

	// ------- dynamic loading info -----------//
	int  			 loaded_count;			// if it >= 1, it has been loaded into the memory
	ResourceDb	 res_bitmap;			// frame bitmap resource

	//----------- frame info ------------//

	short frame_count;
	short ani_part_count;

	short first_bitmap_array[MAX_FIRM_FRAME];
	short bitmap_count_array[MAX_FIRM_FRAME];
	short frame_delay_array[MAX_FIRM_FRAME];      // unit: 1/10 second

	short	under_construction_bitmap_recno;		// bitmap recno of the firm that is under construction
	short	under_construction_bitmap_count;
	short	idle_bitmap_recno;						// bitmap recno of the firm that is idle
	short ground_bitmap_recno;

	short first_bitmap(int frameId) 	{ return first_bitmap_array[frameId-1]; }
	short bitmap_count(int frameId) 	{ return bitmap_count_array[frameId-1]; }
	short frame_delay(int frameId) 	{ return frame_delay_array[frameId-1];  }

	short* get_color_remap_table(int nationRecno, int selectedFlag);

	// ------- dynamic loading subroutine -----------//
	FirmBuild();
	~FirmBuild();
	void	load_bitmap_res();
	void	free_bitmap_res();
	int	is_loaded()		{ return loaded_count>0; }

};

//------------ Define struct FirmGroup ---------------//

struct FirmGroup
{
	enum { FIRM_CODE_LEN=8, RACE_CODE_LEN=8 };

	// ##### begin Gilbert 15/10 #####//
	char	firm_id;
	// ##### end Gilbert 15/10 #####//
	char	firm_code[FIRM_CODE_LEN+1];
	char  group;

	char  race_code[RACE_CODE_LEN+1];
	char  race_id;
};

//------------- Define struct FirmBitmap --------------//

struct FirmBitmap
{
//	char* bitmap_ptr;
	long	bitmap_ptr; 
	short width, height;

	char	ani_part;
	char	random_flag;

	char	loc_width;			// no. of locations it takes horizontally and vertically
	char	loc_height;
	char	delay;
	short	offset_x, offset_y;
	char	display_layer;
	char	mode;

	void	draw_at(int curX, int curY, int curZ, short *colorTable, int displayLayer, FirmBuild* firmBuild);
};

//----------- Define class FirmRes ---------------//

class FirmRes
{
public:
	short    	firm_count;
	short			firm_build_count;
	short			firm_bitmap_count;
	short			firm_group_count;

	FirmInfo* 	firm_info_array;
	FirmBuild*  firm_build_array;
	FirmBitmap* firm_bitmap_array;
	FirmGroup*  firm_group_array;

	char	   	init_flag;
//	ResourceDb	res_bitmap;
	ResourceDb	res_pal;

	// ##### begin Gilbert 21/9 ######//
	static short *cache_color_remap_table;
	// ##### end Gilbert 21/9 ######//

public:
	FirmRes();

	void 		 	init();
	void 		 	deinit();

	int 			write_file(File* filePtr);
	int			read_file(File* filePtr);

	FirmBitmap* get_bitmap(int bitmapId) 	{ return firm_bitmap_array+bitmapId-1; }
	FirmBuild*	get_build(int buildId)		{ return firm_build_array+buildId-1; }
	FirmGroup*	get_group(int buildId)		{ return firm_group_array+buildId-1; }
	FirmInfo*   operator[](int firmId);

	short*		calc_color_remap_table(short *, float completion);

	int			is_linkable_to_town( int firmId, int firmRaceId, int townRaceId );
	int 			is_linkable_to_firm( int firmId1, int firmRaceId1, int firmNationRecno1, int firmId2, int firmRaceId2, int firmNationRecno2 );

private:
	void 		   load_firm_info();
	void			load_firm_build();
	void 		   load_firm_bitmap();
	void 		   load_firm_group();
	void			process_firm_info();
};

extern FirmRes firm_res;

//----------------------------------------------------//

#endif
