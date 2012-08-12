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

//Filename    : OBOX.CPP
//Description : A collection of box objects

#include <obox.h>
#include <key.h>
#include <osys.h>
#include <ovga.h>
#include <omodeid.h>
#include <omouse.h>
#include <ofont.h>
#include <obutton.h>
#include <opower.h>
#include <ot_basic.h>

#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <time.h>

//----------- Define constant -------------//

enum { BOX_LINE_SPACE    = 8,
       MAX_BOX_WIDTH     = 600,
       BOX_X_MARGIN      = 30,      // Horizontal margin
       BOX_TOP_MARGIN    = 20,
       BOX_BUTTON_MARGIN = 32,
		 BOX_BOTTOM_MARGIN = 42   };  // margin for display the box button

//----- Define static member variables ------//

char Box::opened_flag=0;


//----------- Begin of function Box::Box ---------//
//
Box::Box()
{
}
//----------- End of function Box::Box ---------//


//----------- Begin of function Box::~Box ---------//
//
Box::~Box()
{
  close();
}
//----------- End of function Box::~Box ---------//



//----------- Begin of function Box::open ---------//
//
// Open the box with the given coordinations
//
// <int> x1, y1, x2, y2 - coordination of the box
//                        (default : use current settings)
//
// [int] downCentre     - whether paint down the center of the box
//                        (default : 1)
//
void Box::open(int inX1, int inY1, int inX2, int inY2, int downCentre)
{
	box_x1 = inX1;
	box_y1 = inY1;
	box_x2 = inX2;
	box_y2 = inY2;
        down_centre = downCentre;
}
//------------ End of function Box::open -----------//


//----------- Begin of function Box::open ---------//
//
// Given the width & height of the box, and open the box
//
// <int> boxWidth, boxHeight - width & height of the box
//
// [int] downCentre - whether paint down the center of the box
//                    (default : 1)
//
void Box::open(int boxWidth, int boxHeight, int downCentre)
{
	box_x1 = (VGA_WIDTH-boxWidth) / 2;
	box_x2 = box_x1 + boxWidth - 1;

	box_y1 = (VGA_HEIGHT-boxHeight) / 2;
	box_y2 = box_y1 + boxHeight - 1;
}
//------------ End of function Box::open -----------//


//----------- Begin of function Box::paint ---------//

void Box::paint()
{
  if (!opened_flag) return;

	vga_buffer.d3_panel_up( box_x1, box_y1, box_x2, box_y2, 2 );

	if( down_centre )
		vga_buffer.d3_panel_down( box_x1+4, box_y1+4, box_x2-4, box_y2-4, 1 );
}
//------------ End of function Box::paint -----------//


//----------- Begin of function Box::close --------//
//
// Close the previously opened box
//
void Box::close()
{
	err_when(!opened_flag);		// double open

	mouse.get_event();   // post the click, prevent effect on other windows

	opened_flag = 0;
}
//------------ End of function Box::close -----------//


//----------- Begin of function Box::calc_size ------//
//
// Given a string and then calculate the size of the box
// which can contain the string.
//
// <char*> msgStr    = the message string
// <int>   minHeight = minimum height of the box
// [int]   x1, y1    = the top left corner of the box
//
void Box::calc_size(const char* msgStr, int minHeight, int x1, int y1)
{
	int width  = BOX_X_MARGIN*2 + font_bld.text_width(msgStr, -1, MAX_BOX_WIDTH-BOX_X_MARGIN*2);
	int height = minHeight + font_bld.text_height(BOX_LINE_SPACE);

   if( x1 < 0 )
   {
      box_x1 = (VGA_WIDTH  - width)  / 2;
      box_y1 = (VGA_HEIGHT - height) / 2;
   }
   else
   {
      box_x1 = x1;
      box_y1 = y1;
   }

   box_x2 = box_x1+width-1;
   box_y2 = box_y1+height-1;
}
//---------- End of function Box::calc_size -------------//


//---------- Begin of function Box::ok_button ---------//
//
// [int] timeOut - 1=enable inactive timeout
//                 0=disable inactive timeout
//                 (default : 1 )
//
void Box::ok_button(int timeOut)
{
	Button button;

	char* okStr = text_basic.is_inited() ? text_basic.str_box_ok() : (char*)"Ok";
   button.paint_text( box_x1+(box_x2-box_x1+1)/2-10, box_y2-BOX_BUTTON_MARGIN, okStr );

   vga.flip();
   button.wait_press(timeOut);
}
//------------ End of function Box::ok_button ---------//


//---------- Begin of function Box::ask_button ---------//
//
// Display two buttons, and wait for player to click one of them
//
// [char*] buttonDes1      = the description of button 1 (default : "Ok")
// [char*] buttonDes2 	   = the description of button 2 (default : "Cancel")
// [int]   rightClickClose = whether pressing the right button will close the window
//			     (default: 1)
//
int Box::ask_button(const char *msgStr, const char* buttonDes1, const char* buttonDes2, int rightClickClose)
{
	if( !buttonDes1 )
	{
		if( text_basic.is_inited() )
			buttonDes1 = text_basic.str_box_ok();
		else
			buttonDes1 = "Ok";
	}
	if( !buttonDes2 )
	{
		if( text_basic.is_inited() )
			buttonDes2 = text_basic.str_box_cancel();
		else
			buttonDes2 = "Cancel";
	}
  button1 = (char*) buttonDes1;
  button2 = (char*) buttonDes2;

  down_centre = 1;
   while( 1 )
   {

   int width;
   paint();

	font_bld.put_paragraph( box_x1+BOX_X_MARGIN, box_y1+BOX_TOP_MARGIN, box_x2-BOX_X_MARGIN,
			   box_y2-BOX_BOTTOM_MARGIN, msgStr, BOX_LINE_SPACE );
   width = box_x2-box_x1+1;

   Button buttonOk, buttonCancel;

   int buttonWidth1 = 20 + font_bld.text_width(button1);
   buttonOk.create_text( box_x1+width/2-buttonWidth1, box_y2-BOX_BUTTON_MARGIN, button1 );

   buttonCancel.create_text( box_x1+width/2+2 , box_y2-BOX_BUTTON_MARGIN, button2 );

   buttonOk.paint();      // paint button
   buttonCancel.paint();

   //..........................................//
   vga.flip();
		sys.yield();
		mouse.get_event();

      if( buttonOk.detect(buttonOk.str_buf[0], KEY_RETURN) )
		
			return 1;

		if( buttonCancel.detect(buttonCancel.str_buf[0], KEY_ESC)
			 || (rightClickClose && mouse.any_click(1)) ) 	  // detect right button only when the button is "Cancel"
		{
			mouse.get_event();
			return 0;
		}
	}
}
//--------- End of function Box::ask_button --------//

//------- Begin of function Box::ask ----------//
//
// Popup a message box and ask user 'Ok' or 'Cancel'
//
// Syntax :: ask( <char*> )
//
// <char*> msgStr     = pointer to the message, use '\n' to seperate lines
// [char*] buttonDes1 = the description of button 1 (default : "Ok")
// [char*] buttonDes2 = the description of button 2 (default : "Cancel")
// [int]   x1, y1     = the left corner of the box, if not given
//                      center the box on the screen
//
// Return : 1 - if user select "Ok" button
//          0 - if user select "Cancel" button
//
//
int Box::ask(char* msgStr, char* buttonDes1, char* buttonDes2, int x1, int y1)
{
   int rc;

   calc_size(msgStr,BOX_TOP_MARGIN+BOX_BOTTOM_MARGIN,x1,y1);   // calculate x1, y1, x2, y2 depended on the msgStr

   opened_flag = 1;
   rc = ask_button(msgStr, buttonDes1, buttonDes2);

   close();

   return rc;
}
//---------- End of function Box::ask ----------//


//------- Begin of function Box::msg ----------//
//
// Popup a message box and ask user to press the 'Ok' button
//
// Syntax :: msg( <char*> )
//
// <char*> msgStr 		 = pointer to the message, use '\n' to
//									seperate lines.
// [int]   enableTimeOut = enable time out or not (default: 1)
// [int]   x1, y1 		 = the left corner of the box, if not given
//                  		   center the box on the screen.
//
void Box::msg(const char* msgStr, int enableTimeOut, int x1, int y1)
{
	calc_size(msgStr,BOX_TOP_MARGIN+BOX_BOTTOM_MARGIN,x1,y1);   // calculate x1, y1, x2, y2 depended on the msgStr
  down_centre = 1;
  opened_flag = 1;
  paint();

	font_bld.put_paragraph( box_x1+BOX_X_MARGIN, box_y1+BOX_TOP_MARGIN, box_x2-BOX_X_MARGIN,
				box_y2-BOX_BOTTOM_MARGIN, msgStr, BOX_LINE_SPACE );

	ok_button(enableTimeOut);
	close();
}
//---------- End of function Box::msg ----------//


//------- Begin of function Box::print ----------//
//
// Popup a message box and ask user to press the 'Ok' button
//
// <char*> formatStr - formated message with % argument
// <....>  the argument list
//
void Box::print(char* formatStr, ... )
{
   //---- format the message and the arguments into one message ----//

   enum { RESULT_STR_LEN=200 };

   static char resultStr[RESULT_STR_LEN+1];

   err_when( strlen(formatStr) > RESULT_STR_LEN/2 );   // the length of the format string cannot > 1/2 of the result string buffer length, the remaining 1/2 is reserved for substituted values.

   va_list argPtr;        // the argument list structure

   va_start( argPtr, formatStr );
   vsprintf( resultStr, formatStr, argPtr );
   va_end( argPtr );

   //------------ display msg --------------//

   msg(resultStr);
}
//---------- End of function Box::print ----------//


//------- Begin of function Box::tell ----------//
//
// Popup a box and display a message. Call Box::close() to close the box.
//
// Syntax :: tell( <char*> )
//
// <char*> tellStr = pointer to the message, use '\n' to seperate lines
// [int]   x1, y1 = the left corner of the box, if not given
//                  center the box on the screen
//
void Box::tell(char* tellStr, int x1, int y1)
{
   calc_size(tellStr,BOX_TOP_MARGIN+BOX_BOTTOM_MARGIN,x1,y1);   // calculate x1, y1, x2, y2 depended on the tellStr

	font_bld.put_paragraph( box_x1+BOX_X_MARGIN, box_y1+BOX_TOP_MARGIN, box_x2-BOX_X_MARGIN,
			   box_y2-BOX_BOTTOM_MARGIN, tellStr, BOX_LINE_SPACE );
}
//---------- End of function Box::tell ----------//


