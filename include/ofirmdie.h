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

// Filename    : OFIRMDIE.H
// Description : destructing firm


#ifndef __OFIRMDIE_H
#define __OFIRMDIE_H

#include <odynarrb.h>
#include <oresdb.h>

struct FirmBuild;
struct FirmBitmap;
struct FirmDieBitmap;

// -------- define class FirmDieRes --------//

class FirmDieRes
{
public:
	short			firm_build_count;
	short			firm_bitmap_count;
	FirmBuild *firm_build_array;
	FirmDieBitmap *firm_bitmap_array;

	char	   	init_flag;
	ResourceDb	res_bitmap;
	ResourceDb	res_pal;

public:
	FirmDieRes();
	~FirmDieRes();
	void	init();
	void	deinit();

	FirmBuild*	get_build(int buildId);
	FirmDieBitmap* get_bitmap(int bitmapId);

private:
	void	load_build_info();
	void	load_bitmap_info();
};

// -------- define class FirmDie --------//

class Firm;

class FirmDie
{
public:
	short	firm_id;
	short	firm_build_id;
	short	nation_recno;
	short	frame;
	short	frame_delay_count;
	short	loc_x1, loc_y1, loc_x2, loc_y2;
	short	parent_loc_x1, parent_loc_y1, parent_loc_x2, parent_loc_y2;
	short	altitude;
	short die_flag;	// start from 1
	short start_frame;// start from 1
	short end_frame;	// start from 1
	short frame_looping;	// start from 1

public:
	void	init(short firmId, short firmBuildId, short nationRecno, 
		short	locX1, short locY1, short locX2, short locY2, short alt);
	void	init(Firm *firmPtr, short die_flag, int random = 0);	
	void	pre_process();
	int	process();
	void	draw(int displayLayer);

	int	is_stealth();
};

// -------- define class FirmDieArray --------//

class FirmDieArray : public DynArrayB
{
public:
	FirmDieArray();
	~FirmDieArray();
	void	init();
	void	deinit();

	int	add(FirmDie *r);
	void	del(int i);
	int	is_deleted(int recno);
	void	process();

	FirmDie *operator[](int recNo);
};

extern FirmDieRes firm_die_res;
extern FirmDieArray firm_die_array;

#endif
