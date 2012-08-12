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

//Filename    : OFIRMA.H
//Description : Object Firm Array

#ifndef __OFIRMA_H
#define __OFIRMA_H

#ifndef __ODYNARRB_H
#include <odynarrb.h>
#endif

//--------- Define class FirmArray ----------//

class Firm;

class FirmArray : public DynArrayB
{
public:
	int	 selected_recno;		// the firm current being selected

private:
	int    process_recno;

public:
   FirmArray();
   ~FirmArray();

	void  init();
   void  deinit();

	int 	build_firm(int xLoc, int yLoc, int nationRecno, int firmId, char* buildCode=NULL, short builderRecno=0, bool isUpgrade=false, bool noCost=false);
	int 	generate_firm(int xLoc, int yLoc, int nationRecno, int firmId, char* buildCode=NULL);
	int   create_firm(int);
   void  del_firm(int);
	int   firm_class_size(int);
	void 	upgrade_firm(int oldFirmRecno, int newFirmId);

	int   process();
	void  next_day();
	void  next_month();
	void  next_year();

	void	draw_dot(int filterFirmId=0);
	void	draw_profile();
	void  skip(int);

	int   write_file(File*);
	int   read_file(File*);

	#ifdef DEBUG
		Firm* operator()();             // reference to current Firm record
		Firm* operator[](int recNo);
	#else
		Firm* operator()()	    		{ return (Firm*) get_ptr(); }
		Firm* operator[](int recNo)   { return (Firm*) get_ptr(recNo); }
	#endif

	int   is_deleted(int recNo)    // whether the item is deleted or not
			{ return get_ptr(recNo) == NULL; }

};

//---------------------------------------------//

extern FirmArray firm_array;

//---------------------------------------------//

#endif
