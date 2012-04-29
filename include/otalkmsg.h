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

//Filename	  : OTALKMSG.H
//Description : Header file for class TalkMsg

#ifndef __OTALKMSG_H
#define __OTALKMSG_H

//-------- define talk id. --------//

enum { MAX_TALK_TYPE	= 20 };

enum { TALK_PROPOSE_TRADE_TREATY=1,
		 TALK_PROPOSE_FRIENDLY_TREATY,
		 TALK_PROPOSE_ALLIANCE_TREATY,
		 TALK_END_TRADE_TREATY,
		 TALK_END_FRIENDLY_TREATY,
		 TALK_END_ALLIANCE_TREATY,
		 TALK_REQUEST_MILITARY_AID,
		 TALK_REQUEST_TRADE_EMBARGO,
		 TALK_REQUEST_CEASE_WAR,
		 TALK_REQUEST_DECLARE_WAR,
		 TALK_REQUEST_BUY_FOOD,
		 TALK_DECLARE_WAR,
		 TALK_GIVE_TRIBUTE,
		 TALK_DEMAND_TRIBUTE,
		 TALK_GIVE_AID,
		 TALK_DEMAND_AID,
		 TALK_EXCHANGE_TECH,
		 TALK_REQUEST_SURRENDER,
		 TALK_SURRENDER,
	  };

//----------------------------------//

#endif
