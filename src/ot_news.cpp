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

// Filename    : OT_NEWS.H
// Description : text resource news

#include <ot_news.h>
#include <all.h>


void TextResNews::init()
{
	TextResource::init( DIR_RES"t_news.res" );
}

char* TextResNews::str_town_of_nation( char* townName, char* nationName)
{
	return process( get_format_str( 1, "TOFK"), townName, nationName );
}

char* TextResNews::str_firm_near_town( char* firmName, char* townName )
{
	return process( get_format_str( 2, "FOFT"), firmName, townName );
}

char* TextResNews::str_firm_of_nation( char* firmName, char* nationName)
{
	return process( get_format_str( 3, "FOFK"), firmName, nationName );
}

char* TextResNews::str_firm_near_town_of_nation( char *firmName, char *townName, char* nationName )
{
	return process( get_format_str( 4, "FOTK"), firmName, townName, nationName );
}


char* TextResNews::str_time_left_ym( int years, int months )
{
	return process( get_format_str( 5, "TLEF"), years, years && months, months );
}

