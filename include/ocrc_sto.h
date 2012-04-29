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

// Filename    : OCRC_STO.H
// Description : store of crc of objects


#ifndef __OCRC_STO_H
#define __OCRC_STO_H

#include <ovqueue.h>
#include <ostr.h>

class CrcStore
{
public:
	VLenQueue nations;
	VLenQueue units;
	VLenQueue firms;
	VLenQueue towns;
	VLenQueue bullets;
	VLenQueue rebels;
	VLenQueue spies;
	VLenQueue sites;
	VLenQueue talk_msgs;

	// #### patch begin Gilbert 23/1 #####//
	String	crc_error_string;
	// #### patch end Gilbert 23/1 #####//

public:
	CrcStore();
	void	init();
	void	deinit();

	void	record_nations();
	void	record_units();
	void	record_firms();
	void	record_towns();
	void	record_bullets();
	void	record_rebels();
	void	record_spies();
	void	record_sites();
	void	record_talk_msgs();

	void	record_all();
	void	send_all();
	int	compare_remote(DWORD remoteMsgId, char *);
};

extern CrcStore crc_store;

#endif 
