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

//Filename    : OIMAGERES.H
//Description : Object ImageRes

#ifndef __OIMAGERES_H
#define __OIMAGERES_H

#ifndef  __ORESX_H
#include <oresx.h>
#endif

//----------------------------------------//

class VgaBuf;

class ImageRes : public ResourceIdx
{
public:
	ImageRes()	{;}
	ImageRes(const char* resFile, int readAll=0, int useVgaBuf=0);

	void  put_front(int,int,const char*, int compressFlag=0);
	void  put_back(int,int,const char*, int compressFlag=0);

	void  put_front(int,int,int, int compressFlag=0);
	void  put_back(int,int,int, int compressFlag=0);

	char* get_ptr(const char* imageName)   { return ResourceIdx::read(imageName); }

	void  put_large(VgaBuf*,int,int,const char*);  // put a large image, over 64K
	void  put_large(VgaBuf*,int,int,int);

	void  put_to_buf(VgaBuf* vgaBufPtr, const char* imageName);
	void  put_to_buf(VgaBuf* vgaBufPtr, int bitmapId);
};

extern ImageRes image_icon, image_interface, image_menu, image_button, image_spict, image_gameif;
extern ImageRes image_encyc, image_tpict, image_tutorial;

extern ImageRes& image_menu2;
//--------------------------------------------//

#endif
