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

// Filename    : OVOLUME.CPP
// Description : volume unit type

#include <win32_compat.h>
#include <oaudio.h>
#include <oconfig.h>
#include <ovolume.h>

const int DEFAULT_VOL_DROP = 100;	// distance 100, no sound
const int DEFAULT_DIST_LIMIT = 40;
const int DEFAULT_PAN_DROP = 100;	// distance 100, extreme left or extreme right
RelVolume DEF_REL_VOLUME(100,0);


DsVolume::DsVolume(long dsVol, long dsPan) : ds_vol(dsVol), ds_pan(dsPan)
{
}

DsVolume::DsVolume(AbsVolume &absVolume) : ds_vol(absVolume.abs_vol), ds_pan(absVolume.ds_pan)
{
}

DsVolume::DsVolume(RelVolume &relVolume)
	: ds_vol(relVolume.rel_vol), ds_pan(relVolume.ds_pan)
{
}


AbsVolume::AbsVolume(long absVol, long dsPan)
	: abs_vol(absVol), ds_pan(dsPan)
{
}

AbsVolume::AbsVolume(DsVolume &dsVolume)
	: abs_vol(dsVolume.ds_vol), ds_pan(dsVolume.ds_pan)
{
}

RelVolume::RelVolume(long relVol, long dsPan)
	: rel_vol(relVol), ds_pan(dsPan)
{
}

RelVolume::RelVolume(PosVolume &posVolume)
{
	int x = posVolume.x - posVolume.y;
	int y = posVolume.x + posVolume.y;
	long absX = x >= 0 ? x : -x;
	long absY = y >= 0 ? y : -y;
	long dist = absX >= absY ? absX :absY;
	if( dist <= DEFAULT_DIST_LIMIT )
		rel_vol = rel_vol = 100 - dist * 100 / DEFAULT_VOL_DROP;
	else
		rel_vol = 0;

	if( x >= DEFAULT_PAN_DROP )
		ds_pan = 10000;
	else if( x <= -DEFAULT_PAN_DROP )
		ds_pan = -10000;
	else
		ds_pan = 10000 / DEFAULT_PAN_DROP * x;
}


RelVolume::RelVolume(PosVolume &posVolume, int drop, int limit)
{
	int x = posVolume.x - posVolume.y;
	int y = posVolume.x + posVolume.y;
	long absX = x >= 0 ? x : -x;
	long absY = y >= 0 ? y : -y;
	long dist = absX >= absY ? absX :absY;
	if( dist <= limit )
		rel_vol = 100 - dist * 100 / drop;
	else
		rel_vol = 0;
	
	if( x >= DEFAULT_PAN_DROP )
		ds_pan = 10000;
	else if( x <= -DEFAULT_PAN_DROP )
		ds_pan = -10000;
	else
		ds_pan = 10000 / DEFAULT_PAN_DROP * x;
}


PosVolume::PosVolume(long relLocX, long relLocY) : x(relLocX), y(relLocY)
{
}
