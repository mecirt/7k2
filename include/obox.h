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

//Filename    : OBOX.H
//Description : Header file of button object

#ifndef __OBOX_H
#define __OBOX_H

#include <stdlib.h>

//------- constant for arrow box ----------//

enum { ARROW_BOX_WIDTH        = 390,
       ARROW_BOX_X_MARGIN     = 15,
       ARROW_BOX_Y_MARGIN     = 15,
       ARROW_BOX_TITLE_HEIGHT = 20,
       ARROW_BOX_LINE_SPACE   = 4   };

// ------- declare class ----------//

class Button;

//--------- Define class Box ------------//

class Box
{
public:
   int  box_x1, box_y1, box_x2, box_y2;
   int down_centre;
   char *button1, *button2;

	static char opened_flag;

public:
	Box();
	~Box();
   // ready to use one-step call functions

	int  ask(char*, char* =NULL, char* =NULL, int= -1, int= -1);
	void msg(const char* msgStr, int enableTimeOut=1, int x1= -1, int y1= -1);
	void print(char*, ... );
   void tell(char*,int= -1, int= -1);

   // lower level box control functions

   void open(int,int,int,int,int=1);
   void open(int,int,int=1);
   void paint();
   void close();

   void calc_size(const char*,int,int= -1, int= -1);

   int  ask_button(const char*,const char* =NULL,const char* =NULL,int=1);
   void ok_button(int=1);

   // arrow box

   void arrow_box(char*, char* =NULL, int=1);
   void close_arrow_box();
   void calc_arrow_box(char*, int, int, int=0);
   void calc_arrow_box(int, int, int, int);
   void draw_arrow();
};

extern Box box;

//----------------------------------------------//

#endif
