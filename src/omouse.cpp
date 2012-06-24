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

//Filename    : OMOUSE.CPP
//Description : Mouse handling Object

#include <all.h>

#include <omouse.h>
#include <osys.h>
#include <ovga.h>
#include <omodeid.h>
#include <opower.h>
#include <omousecr.h>
#include <key.h>
#include <ovgalock.h>
#include <otutor.h>
#include <ogame.h>

//--- define the size of a buffer for real-time vga screen updating ---//

#define VGA_UPDATE_BUF_SIZE	(100*100*sizeof(short))		// 100 x 100

//--------- Define Click Threshold -----------//
//
// Clock tick is incremented 1000 times per second or once per millisecond.
//
// The minimum time interval between consequent mouse click is set to
// = 0.3 seconds
//
//--------------------------------------------//

static DWORD click_threshold = (int)(0.3 * 1000);

//------- define constant --------//

#define DOUBLE_SPEED_THREHOLD 8

//------- define static storage -----------//

static int update_x1, update_y1, update_x2, update_y2;		// coordination of the last double-buffer update area

//--------- Start of Mouse::Mouse ---------//
//
Mouse::Mouse()
{
	memset( this, 0, sizeof(Mouse) );
	double_speed_threshold = DEFAULT_DOUBLE_SPEED_THRESHOLD;
	triple_speed_threshold = DEFAULT_TRIPLE_SPEED_THRESHOLD;
	first_repeat_key_time = DEFAULT_FIRST_REPEAT_KEY_TIME;
	repeat_rate = DEFAULT_REPEAT_RATE;		// millisecond
}
//---------- End of Mouse::Mouse ---------//


//---------- Begin of Mouse::~Mouse --------//
//
// Deinitialize the mouse driver, reset event handler
//
Mouse::~Mouse()
{
	deinit();
}
//------------ End of Mouse::~Mouse --------//


//------------ Start of Mouse::init ------------//
//
void Mouse::init()
{
  //-------- set starting position ---------//
  GetMousePos(&cur_x, &cur_y);

  InitInputDevices (0);

  init_keyboard();

  // ------ initialize mouse boundary ---------//
  reset_boundary();

  // ------- initialize event queue ---------//
  head_ptr = tail_ptr = 0;

  //--------------------------------------------//

  init_flag = 1;
}
//------------- End of Mouse::init -------------//


//------------ Start of Mouse::deinit ------------//
//
void Mouse::deinit()
{
	init_flag = 0;

  UninitInputDevices ();
}
//------------- End of Mouse::deinit -------------//


//--------- Start of Mouse::hide -------//
//
// Suspend the mouse function, use resume() to resume to function
//
void Mouse::hide()
{
	mouse_cursor.hide_all_flag=1;

	mouse_cursor.process(cur_x, cur_y);
}
//---------- End of Mouse::hide --------//


//--------- Start of Mouse::show -------//
//
// Resume the mouse function which is previously hideed by hide()
//
void Mouse::show()
{
	mouse_cursor.hide_all_flag=0;

	mouse_cursor.process(cur_x, cur_y);
}
//---------- End of Mouse::show --------//


//--------- Begin of Mouse::hide_area ----------//
//
void Mouse::hide_area(int x1, int y1, int x2, int y2)
{
	mouse_cursor.hide_area_flag++;

	if( mouse_cursor.hide_area_flag!=1 )		// only process for the first call of hide_area()
		return;

	mouse_cursor.hide_x1 = x1;
	mouse_cursor.hide_y1 = y1;
	mouse_cursor.hide_x2 = x2;
	mouse_cursor.hide_y2 = y2;

	int curX = cur_x - mouse_cursor.hot_spot_x;
	int curY = cur_y - mouse_cursor.hot_spot_y;

	if( m.is_touch( x1, y1, x2, y2, curX, curY,
						 curX+mouse_cursor.icon_width-1,
						 curY+mouse_cursor.icon_height-1 ) )
	{
		//------ hide up the mouse cursor --------//

		mouse_cursor.process(cur_x, cur_y);
	}
}
//--------- End of Mouse::hide_area --------------//


//--------- Begin of Mouse::show_area ----------//
//
void Mouse::show_area()
{
	mouse_cursor.hide_area_flag--;

	if( mouse_cursor.hide_area_flag!=0 )		// only process for the first call of hide_area()
		return;

	int curX = cur_x - mouse_cursor.hot_spot_x;
	int curY = cur_y - mouse_cursor.hot_spot_y;

	if( m.is_touch( mouse_cursor.hide_x1, mouse_cursor.hide_y1,
						 mouse_cursor.hide_x2, mouse_cursor.hide_y2,
						 curX, curY, curX+mouse_cursor.icon_width-1,
						 curY+mouse_cursor.icon_height-1 ) )
	{
		//----- redisplay the mouse cursor ------//

		mouse_cursor.process(cur_x, cur_y);
	}
}
//--------- End of Mouse::show_area --------------//


//--------- Start of Mouse::add_event ---------//
//
// Called by handler interrupt to procss the state
//
void Mouse::add_event(MouseEvent *mouseEvent)
{
	//---- call the game object to see if the mouse cursor icon needs to be changed, or if the nation selection square needs to be activated ----//

	power.mouse_handler();

	//--------- update the mouse cursor ----------//

	mouse_cursor.process(cur_x, cur_y);     // repaint mouse cursor

	//-------- save state into the event queue --------//

	if((head_ptr == tail_ptr-1) ||            // see if the buffer is full
		(head_ptr == EVENT_BUFFER_SIZE-1 && tail_ptr == 0))
	{
		return;
	}

	event_buffer[head_ptr] = *mouseEvent;

	if(++head_ptr >= EVENT_BUFFER_SIZE)       // increment the head ptr
		head_ptr = 0;
}
//----------- End of Mouse::add_event ----------//


//--------- Start of Mouse::add_key_event ---------//
//
// Called by key handler to save the key pressed
//
void Mouse::add_key_event(unsigned scanCode, DWORD timeStamp)
{
	if((head_ptr == tail_ptr-1) ||               // see if the buffer is full
		(head_ptr == EVENT_BUFFER_SIZE-1 && tail_ptr == 0))
	{
		return;
	}

	MouseEvent *ev = event_buffer + head_ptr;

	ev->event_type = KEY_PRESS;
	ev->scan_code = scanCode;
	ev->skey_state = skey_state;
	ev->time = timeStamp;

	// put mouse state
	// ev->state = 0;			//ev->state = left_press | right_press;
	ev->x = cur_x;
	ev->y = cur_y;

	if(++head_ptr >= EVENT_BUFFER_SIZE)  // increment the head ptr
		head_ptr = 0;
}
//----------- End of Mouse::add_key_event ----------//


//--------- Start of Mouse::add_key_event ---------//
//
// Called by key handler to save the key pressed
//
void Mouse::add_key_release_event(unsigned scanCode, DWORD timeStamp)
{
	if((head_ptr == tail_ptr-1) ||               // see if the buffer is full
		(head_ptr == EVENT_BUFFER_SIZE-1 && tail_ptr == 0))
	{
		return;
	}

	MouseEvent *ev = event_buffer + head_ptr;

	ev->event_type = KEY_RELEASE;
	ev->scan_code = scanCode;
	ev->skey_state = skey_state;
	ev->time = timeStamp;

	// put mouse state
	// ev->state = 0;			//ev->state = left_press | right_press;
	ev->x = cur_x;
	ev->y = cur_y;

	if(++head_ptr >= EVENT_BUFFER_SIZE)  // increment the head ptr
		head_ptr = 0;
}
//----------- End of Mouse::add_key_release_event ----------//


//--------- Start of Mouse::get_event ---------//
//
// Get next event from the event buffer
//
// return : <int> 1 - event fetched from the event queue
//                0 - not event remained in the buffer
//
// to know what type of event :
// 1. check is_mouse_event() or is_key_event() 
// 2. if is_mouse_event(), check mouse_event_type
//			if( LEFT_BUTTON or LEFT_BUTTON_RELEASE, read click_buffer[LEFT_BUTTON]
//			if( RIGHT_BUTTON or RIGHT_BUTTON_RELEASE, read click_buffer[RIGHT_BUTTON]
// 3. if is_key_event(), check event_skey_state, scan_code and key_code 
//
int Mouse::get_event()
{
	int rc = 0;

	do
	{
		if(head_ptr == tail_ptr)     // no event queue left in the buffer
		{
			scan_code      =0;        // no keyboard event
			key_code       =0;
			has_mouse_event=0;        // no mouse event
			break;
		}

		//--------- get event from queue ---------//

		MouseEvent* eptr = event_buffer + tail_ptr;
		MouseClick* cptr;

		event_skey_state = eptr->skey_state;
		mouse_event_type = eptr->event_type;
		event_time       = eptr->time;

		// set rc to 1 if any event had happen

		switch( eptr->event_type )
		{
		case LEFT_BUTTON:
		case RIGHT_BUTTON:
			// set count of other button to zero
			click_buffer[LEFT_BUTTON+RIGHT_BUTTON-eptr->event_type].count = 0;
			cptr = click_buffer + eptr->event_type;
			if( //eptr->event_type == LEFT_BUTTON	&&		// left button has double click
				 eptr->time - cptr->time < click_threshold )
				cptr->count++;
			else
				cptr->count = 1;

			cptr->time = eptr->time;
			cptr->x    = eptr->x;
			cptr->y    = eptr->y;
			scan_code       = 0;
			key_code        = 0;
			has_mouse_event = 1;

			// --------- set holding_button and press_button_time -----//
			holding_button[eptr->event_type] = 1;
			press_button_time[eptr->event_type] = eptr->time;

			rc = 1;
			break;

		case KEY_PRESS:
			scan_code = eptr->scan_code;
			key_code = mouse.is_key(scan_code, event_skey_state, (WORD) 0, K_CHAR_KEY);
			has_mouse_event = 0;

			// ------- set holding scan code ------//
			holding_scan_code = scan_code;
			holding_key_code = key_code;
			press_key_time = eptr->time;

			rc = 1;
			break;

		case LEFT_BUTTON_RELEASE:
		case RIGHT_BUTTON_RELEASE:
			cptr = click_buffer + eptr->event_type - LEFT_BUTTON_RELEASE;
			cptr->release_time = eptr->time;
			cptr->release_x    = eptr->x;
			cptr->release_y    = eptr->y;
			scan_code          = 0;
			key_code           = 0;
			has_mouse_event    = 1;

			// --------- set holding_button and press_button_time -----//
			// ##### patch begin Gilbert 4/10 ######//
			holding_button[eptr->event_type - LEFT_BUTTON_RELEASE] = 0;
			// ##### patch end Gilbert 4/10 ######//

			rc = 1;
			break;

		case KEY_RELEASE:
			// scan_code = 0;
			// key_code = 0;
			// has_mouse_event = 0;

			// ------- clear holding scan code ------//
			holding_scan_code = 0;
			holding_key_code = 0;

			// rc = 1;		// key release is not an event to outside, so don't set rc
			break;

		default:
			err_here();
		}

		if(++tail_ptr >= EVENT_BUFFER_SIZE)
			tail_ptr = 0;

	} while( !rc );

   return rc;
}
//----------- End of Mouse::get_event ----------//


//--------- Begin of Mouse::in_area ----------//
//
// <Real-time access>
//
// Detect whether mouse cursor is in the specified area
//
// <int> x1,y1,x2,y2 = the coordinations of the area
//
// Return : 1 - if the mouse cursor is in the area
//          0 - if not
//
int Mouse::in_area(int x1, int y1, int x2, int y2)
{
	return( cur_x >= x1 && cur_y >= y1 && cur_x <= x2 && cur_y <= y2 );
}
//--------- End of Mouse::in_area --------------//


//--------- Begin of Mouse::press_area ----------//
//
// <Real-time access>
//
// Detect whether the specified area has been pressed by mouse
//
// <int> x1,y1,x2,y2 = the coordinations of the area
// <int> buttonId    = which button ( 0=left, 1-right, 2-left or right )
//
// Return : 1 - if the area has been pressed (left button)
//			   1 - if the area has been pressed (right button)
//          0 - if not
//
int Mouse::press_area(int x1, int y1, int x2, int y2, int buttonId)
{
	if( cur_x >= x1 && cur_y >= y1 && cur_x <= x2 && cur_y <= y2 )
	{
		if( left_press && (buttonId==0 || buttonId==2) )    // Left button
			return 1;

		if( right_press && (buttonId==1 || buttonId==2) )   // Right button
			return 2;
	}

	return 0;
}
//--------- End of Mouse::press_area --------------//


//--------- Begin of Mouse::set_boundary ----------//
//
// for each parameter, put -1 to mean unchange
//
// ###### begin Gilbert 9/10 #######//
void Mouse::set_boundary(int x1, int y1, int x2, int y2, int boundType)
{
	if( x1 >= 0)
		bound_x1 = x1 < MOUSE_X_LOWER_LIMIT ? MOUSE_X_LOWER_LIMIT : x1;
	if( y1 >= 0)
		bound_y1 = y1 < MOUSE_Y_LOWER_LIMIT ? MOUSE_Y_LOWER_LIMIT : y1;
	if( x2 >= 0)
		bound_x2 = x2 > MOUSE_X_UPPER_LIMIT ? MOUSE_X_UPPER_LIMIT : x2;
	if( y2 >= 0)
		bound_y2 = y2 > MOUSE_Y_UPPER_LIMIT ? MOUSE_Y_UPPER_LIMIT : y2;
	if( boundType >= 0)
		bound_type = boundType;

	// ------- check inside boundary now -----//

	int oldCurX = cur_x;
	int oldCurY = cur_y;

	switch( bound_type )
	{
	case 0:		// rectangular boundary
		if(cur_x < bound_x1)
			cur_x = bound_x1;
		if(cur_x > bound_x2)
			cur_x = bound_x2;
		if(cur_y < bound_y1)
			cur_y = bound_y1;
		if(cur_y > bound_y2)
			cur_y = bound_y2;
		break;
	case 1:		// rhombus boundary
		{
			err_when( bound_y2 <= bound_y1);
			err_when( bound_x2 <= bound_x1);

			// let vector I is unit vector x-axis, J is unit vector y-axis
			// let vector U is from origin to upper right corner of Rectangle (bound_x1,y1,x2,y2)
			// let vector V is from origin to upper left coner of the Rectangle
			// a, b is width and height of the Rectangle
			// U = (a/2)I + (b/2)J
			// V = (-a/2)I + (b/2)J
			// => aI = U-V		 bJ = U+V
			// cur_x, cur_Y is limited to ±U/2 and ±V/2		
			//
			// let dx = (cur_x - (bound_x1+bound_y2))/2
			// let dy = (cur_y - (bound_y1+bound_y2))/2
			// A = dx I + dy J
			//   = (dx/a)(U-V) + (dy/b)(U+V)
			//   = (dx/a + dy/b) U + (dy/b - dx/a) V
			// let A = Au U + Av V
			// limit Au and Av between -1/2 and 1/2, call them Au' and Av'
			// A' is Au' U + Av' V
			// change back to I, J form
			// A' = Au'*((a/2)I + (b/2)J) + Av'*((-a/2)I + (b/2)J)
			//    = (Au' - Av')(a/2)I + (Au' + Av')(b/2)J

			int a = bound_x2-bound_x1;
			int b = bound_y2-bound_y1;
			int dx2 = 2*cur_x - bound_x1 - bound_x2;
			int dy2 = 2*cur_y - bound_y1 - bound_y2;
			int aU2ab = dx2 * b;
			int aV2ab = dy2 * a;
			aU2ab += aV2ab;		// 2dx/a + 2dy/b
			aV2ab += aV2ab - aU2ab;		// 2dy/b - 2dx/a
			int ab = a * b;

			if( aU2ab > ab )
				aU2ab = ab;
			if( aU2ab < -ab )
				aU2ab = -ab;
			if( aV2ab > ab )
				aV2ab = ab;
			if( aV2ab < -ab )
				aV2ab = -ab;

			cur_x = (bound_x1 + bound_x2 + (aU2ab - aV2ab) / b / 2) / 2;
			cur_y = (bound_y1 + bound_y2 + (aU2ab + aV2ab) / a / 2) / 2;
		}
		break;
	default:
		err_here();
	}

	if( vga_front.vptr_dd_buf			// front buffer inited
		&& (oldCurX != cur_x || oldCurY != cur_y) )
	{
		mouse_cursor.process(cur_x, cur_y);     // repaint mouse cursor
		power.mouse_handler();
	}
}
// ###### end Gilbert 9/10 #######//
//--------- End of Mouse::set_boundary ----------//


//--------- Begin of Mouse::reset_boundary ----------//
void Mouse::reset_boundary()
{
	bound_x1 = MOUSE_X_LOWER_LIMIT;
	bound_y1 = MOUSE_Y_LOWER_LIMIT;
	bound_x2 = MOUSE_X_UPPER_LIMIT;
	bound_y2 = MOUSE_Y_UPPER_LIMIT;
	bound_type = 0;
	// ###### begin Gilbert 14/12 #######//
	if( cur_x < bound_x1 )
		cur_x = bound_x1;
	if( cur_x > bound_x2 )
		cur_x = bound_x2;
	if( cur_y < bound_y1 )
		cur_y = bound_y1;
	if( cur_y > bound_y2 )
		cur_y = bound_y2;
	// ###### end Gilbert 14/12 #######//
}
//--------- End of Mouse::set_boundary ----------//


//--------- Begin of Mouse::single_click ----------//
//
// <Event queue access>
//
// Detect whether the specified area has been single clicked by mouse
//
// <int> x1,y1,x2,y2 = the coordinations of the area
// [int] buttonId    = which button ( 0=left, 1-right, 2-both)
//                     (default:0)
//
// Return : 1 - if the area has been clicked (left click)
//				2 - if the area has been clicked (right click)
//          0 - if not
//
int Mouse::single_click(int x1, int y1, int x2, int y2,int buttonId)
{
	if( !has_mouse_event )
		return 0;

	MouseClick* cptr;

	if( buttonId==0 || buttonId==2 )     // left button
	{
		cptr = click_buffer+LEFT_BUTTON;

		if( mouse_event_type == LEFT_BUTTON
			 && cptr->count == 1
			 && cptr->x >= x1 && cptr->y >= y1
			 && cptr->x <= x2 && cptr->y <= y2 )
		{
			if (game.game_mode != GAME_TUTORIAL ||
				 (game.game_mode == GAME_TUTORIAL && tutor.allow_mouse_click(cptr->x, cptr->y, 1, 1)))
				return 1;
		}
	}

	if( buttonId==1 || buttonId==2 )     // right button
	{
		cptr = click_buffer+RIGHT_BUTTON;

		if( mouse_event_type == RIGHT_BUTTON
			 && cptr->count == 1
			 && cptr->x >= x1 && cptr->y >= y1
			 && cptr->x <= x2 && cptr->y <= y2 )
		{
			if (game.game_mode != GAME_TUTORIAL ||
				 (game.game_mode == GAME_TUTORIAL && tutor.allow_mouse_click(cptr->x, cptr->y, 1, 2)))
				return 2;
      }
   }

   return 0;
}
//--------- End of Mouse::single_click --------------//


//--------- Begin of Mouse::double_click ----------//
//
// <Event queue access>
//
// Detect whether the specified area has been double clicked by mouse
//
// Note : when a mouse double click, it will FIRST generate a SINGLE
//        click signal and then a DOUBLE click signal.
//        Because a double click is consisted of two single click
//
// <int> x1,y1,x2,y2 = the coordinations of the area
// [int] buttonId    = which button ( 0=left, 1-right, 2-left or right)
//                     (default:0)
//
// Return : 1 - if the area has been clicked
//          0 - if not
//
int Mouse::double_click(int x1, int y1, int x2, int y2,int buttonId)
{
	if( !has_mouse_event )
      return 0;

   MouseClick* cptr;

   if( buttonId==0 || buttonId==2 )     // left button
   {
      cptr = click_buffer+LEFT_BUTTON;

      if( mouse_event_type == LEFT_BUTTON
			 && cptr->count == 2
			 && cptr->x >= x1 && cptr->y >= y1
          && cptr->x <= x2 && cptr->y <= y2 )
      {
			if (game.game_mode != GAME_TUTORIAL ||
				 (game.game_mode == GAME_TUTORIAL && tutor.allow_mouse_click(cptr->x, cptr->y, 1, 1)))
				return 1;
      }
   }

	if( buttonId==1 || buttonId==2 )     // right button
   {
		cptr = click_buffer+RIGHT_BUTTON;

      if( mouse_event_type == RIGHT_BUTTON
			 && cptr->count == 2
          && cptr->x >= x1 && cptr->y >= y1
          && cptr->x <= x2 && cptr->y <= y2 )
      {
			if (game.game_mode != GAME_TUTORIAL ||
				 (game.game_mode == GAME_TUTORIAL && tutor.allow_mouse_click(cptr->x, cptr->y, 1, 2)))
				return 1;
      }
   }

   return 0;
}
//--------- End of Mouse::double_click --------------//


//--------- Begin of Mouse::any_click ----------//
//
// <Event queue access>
//
// Detect whether the specified area has been single or double clicked by mouse
//
// <int> x1,y1,x2,y2 = the coordinations of the area
// [int] buttonId    = which button ( 0=left, 1-right, 2-left or right)
//                     (default:0)
//
// Return : >0 - the no. of click if the area has been clicked
//          0  - if not
//
int Mouse::any_click(int x1, int y1, int x2, int y2,int buttonId)
{
   if( !has_mouse_event )
      return 0;

	MouseClick* cptr;

	if( buttonId==0 || buttonId==2 )     // left button
   {
      cptr = click_buffer+LEFT_BUTTON;

      if( mouse_event_type == LEFT_BUTTON
			 && cptr->count >= 1
          && cptr->x >= x1 && cptr->y >= y1
          && cptr->x <= x2 && cptr->y <= y2 )
      {
			if (game.game_mode != GAME_TUTORIAL ||
				 (game.game_mode == GAME_TUTORIAL && tutor.allow_mouse_click(cptr->x, cptr->y, 1, 1)))
				return cptr->count;
      }
   }

   if( buttonId==1 || buttonId==2 )     // right button
   {
      cptr = click_buffer+RIGHT_BUTTON;

      if( mouse_event_type == RIGHT_BUTTON
			 && cptr->count >= 1
          && cptr->x >= x1 && cptr->y >= y1
          && cptr->x <= x2 && cptr->y <= y2 )
      {
			if (game.game_mode != GAME_TUTORIAL ||
				 (game.game_mode == GAME_TUTORIAL && tutor.allow_mouse_click(cptr->x, cptr->y, 1, 2)))
				return cptr->count;
      }
   }

	return 0;
}
//--------- End of Mouse::any_click --------------//


//--------- Begin of Mouse::any_click ----------//
//
// <Event queue access>
//
// Detect whether the specified area has been single or double clicked by mouse
// Only check button, don't check mouse coordination
//
// [int] buttonId = which button ( 0=left, 1-right, 2-left or right)
//                  (default:0)
//
// Return : >0 - the no. of click if the area has been clicked
//          0  - if not
//
int Mouse::any_click(int buttonId)
{
	if( !has_mouse_event )
      return 0;

   MouseClick* cptr;

   if( buttonId==0 || buttonId==2 )     // left button
   {
      cptr = click_buffer+LEFT_BUTTON;

		if( mouse_event_type == LEFT_BUTTON && cptr->count >= 1 )
			if (game.game_mode != GAME_TUTORIAL ||
				 (game.game_mode == GAME_TUTORIAL && tutor.allow_mouse_click(cptr->x, cptr->y, 1, 1)))
				return cptr->count;
   }

   if( buttonId==1 || buttonId==2 )     // right button
   {
      cptr = click_buffer+RIGHT_BUTTON;

      if( mouse_event_type == RIGHT_BUTTON && cptr->count >= 1 )
			if (game.game_mode != GAME_TUTORIAL ||
				 (game.game_mode == GAME_TUTORIAL && tutor.allow_mouse_click(cptr->x, cptr->y, 1, 2)))
				return cptr->count;
   }

	return 0;
}
//--------- End of Mouse::any_click --------------//


//--------- Begin of Mouse::release_click ----------//
//
// <Event queue access>
//
// Detect whether the specified area has been released button by mouse
//
// <int> x1,y1,x2,y2 = the coordinations of the area
// [int] buttonId    = which button ( 0=left, 1-right, 2-both)
//                     (default:0)
//
// Return : 1 - if the area has been clicked (left click)
//				2 - if the area has been clicked (right click)
//          0 - if not
//
int Mouse::release_click(int x1, int y1, int x2, int y2,int buttonId)
{
	if( !has_mouse_event )
		return 0;

	MouseClick* cptr;

	if( buttonId==0 || buttonId==2 )     // left button
	{
		cptr = click_buffer+LEFT_BUTTON;

		if( mouse_event_type == LEFT_BUTTON_RELEASE
			 && cptr->release_x >= x1 && cptr->release_y >= y1
			 && cptr->release_x <= x2 && cptr->release_y <= y2 )
		{
			if (game.game_mode != GAME_TUTORIAL ||
				 (game.game_mode == GAME_TUTORIAL && tutor.allow_mouse_click(cur_x, cur_y, 2, 1)))
				return 1;
		}
	}

	if( buttonId==1 || buttonId==2 )     // right button
	{
		cptr = click_buffer+RIGHT_BUTTON;

		if( mouse_event_type == RIGHT_BUTTON_RELEASE
			 && cptr->release_x >= x1 && cptr->release_y >= y1
			 && cptr->release_x <= x2 && cptr->release_y <= y2 )
		{
			if (game.game_mode != GAME_TUTORIAL ||
				 (game.game_mode == GAME_TUTORIAL && tutor.allow_mouse_click(cur_x, cur_y, 2, 2)))
				return 2;
      }
   }

   return 0;
}
//--------- End of Mouse::release_click --------------//


//--------- Begin of Mouse::release_click ----------//
//
// <Event queue access>
//
// Detect whether the specified area has been released button by mouse
//
// [int] buttonId    = which button ( 0=left, 1-right, 2-both)
//                     (default:0)
//
// Return : 1 - if the area has been clicked (left click)
//				2 - if the area has been clicked (right click)
//          0 - if not
//
int Mouse::release_click(int buttonId)
{
	if( !has_mouse_event )
		return 0;

	MouseClick* cptr;

	if( buttonId==0 || buttonId==2 )     // left button
	{
		cptr = click_buffer+LEFT_BUTTON;

		if( mouse_event_type == LEFT_BUTTON_RELEASE )
		{
			if (game.game_mode != GAME_TUTORIAL ||
				 (game.game_mode == GAME_TUTORIAL && tutor.allow_mouse_click(cur_x, cur_y, 2, 1)))
				return 1;
		}
	}

	if( buttonId==1 || buttonId==2 )     // right button
	{
		cptr = click_buffer+RIGHT_BUTTON;

		if( mouse_event_type == RIGHT_BUTTON_RELEASE )
		{
			if (game.game_mode != GAME_TUTORIAL ||
				 (game.game_mode == GAME_TUTORIAL && tutor.allow_mouse_click(cur_x, cur_y, 2, 2)))
				return 2;
      }
   }

   return 0;
}
//--------- End of Mouse::release_click --------------//


//--------- Begin of Mouse::poll_event ----------//
//
// Poll mouse events from the direct mouse VXD.
//
int Mouse::poll_event()
{
  if (!init_flag) return 0;

  return PollInputDevices ();
}
//--------- End of Mouse::poll_event --------------//


//--------- Begin of Mouse::clear_event ----------//
//
void Mouse::clear_event()
{
	int loopCount=0;

	while( poll_event() )
	{
		err_when( loopCount++ > 10000 );
	}

	head_ptr = 0;
	tail_ptr = 0;
}
//--------- End of Mouse::clear_event --------------//


//--------- Begin of Mouse::wait_press ----------//
//
// Wait until one of the mouse buttons is pressed.
//
// [int] timeOutSecond - no. of seconds to time out. If not
//								 given, there will be no time out.
//
// return: <int> 1-left mouse button
//					  2-right mouse button
//
int Mouse::wait_press(int timeOutSecond)
{
	// ####### begin Gilbert 7/7 #######//
	// BUGHERE : handled not so good but safe
	if( sys.paused_flag || !sys.active_flag )		// return immediately if switched task
		return 0;
	// ####### end Gilbert 7/7 #######//
puts("wait_press is running!");

	while( mouse.left_press || mouse.any_click() || mouse.key_code )		// avoid repeat clicking
	{
game.process_messages();
		sys.yield();
		mouse.get_event();
	}

	int rc=0;
	unsigned int timeOutTime = m.get_time() + timeOutSecond*1000;

	while(1)
	{
game.process_messages();
		sys.yield();
		mouse.get_event();

		if( right_press || mouse.key_code==KEY_ESC )
		{
			rc = 2;
			break;
		}

		if( left_press || mouse.key_code )
		{
			rc = 1;
			break;
		}

		if( timeOutSecond && m.get_time() > timeOutTime )
			break;
	}

	while( mouse.left_press || mouse.any_click() || mouse.key_code )		// avoid repeat clicking 
	{
		sys.yield();
		mouse.get_event();
	}

	return rc;
}
//--------- End of Mouse::wait_press --------------//


//--------- Begin of Mouse::get_key ----------//
//
// Get a key from the user.
//
// return: <int> 1-left mouse button
//					  2-right mouse button
//
int Mouse::get_key()
{
	int keyCode;

	while(1)
	{
		sys.yield();
		mouse.get_event();

		if( mouse.key_code )
		{
			keyCode = mouse.key_code;
			break;
		}
	}

	while( mouse.key_code )		// flush the key code
	{
		sys.yield();
		mouse.get_event();
	}

	return keyCode;
}
//--------- End of Mouse::get_key --------------//


//--------- Begin of Mouse::reset_click ----------//
//
// Reset queued mouse clicks.
//
void Mouse::reset_click()
{
	has_mouse_event = 0;
	click_buffer[0].count=0;
	click_buffer[1].count=0;
}
//--------- End of Mouse::reset_click --------------//


// ------ Begin of Mouse::mickey_to_displacment -------//
long Mouse::micky_to_displacement(DWORD w)
{
	long d = (long)w ;
	return abs(d) >= double_speed_threshold ? d+d : d;
}
// ------ End of Mouse::mickey_to_displacment -------//


// --------- begin of function Mouse::is_holding ------//
//
// return whether a button is holding
// true if since LEFT_BUTTON (RIGHT_BUTTON) and before LEFT_BUTTON_RELEASE (RIGHT_BUTTON_RELEASE)
//
// remark : press_button_time[buttonId] indicates time when button is pressed
int Mouse::is_holding(int buttonId)
{
	return holding_button[buttonId];
}

void Mouse::init_keyboard()
{
  update_skey_state();
}
// ------- end of Mouse::update_skey_state --------//


//--------- Begin of Mouse::update_skey_state ----------//
// called after task switch to get the lastest state of ctrl/alt/shift key
void Mouse::update_skey_state()
{
  UpdateSkeyState();
}
//--------- End of Mouse::update_skey_state ----------//


// ------ Begin of Mouse::is_key -------//
// compare key with key code
// e.g. to test a key is alt-a,
// call mouse.is_key(mouse.scan_code, mouse.event_skey_state, 'a', K_CHAR_KEY | K_IS_ALT)
//
// pass 0 as charValue to disable checking in charValue
// e.g pressed key is 'a'
// mouse.is_key(mouse.scan_code, mouse.event_skey_state, (WORD) 0, K_CHAR_KEY) returns 'a'
// but if key pressed is alt-a
// the same function call returns 0
// use mouse.is_key(mouse.scan_code, mouse.event_skey_state, (WORD) 0, K_CHAR_KEY | K_IS_ALT ) instead
//
int Mouse::is_key(unsigned scanCode, unsigned short skeyState, unsigned short charValue, unsigned flags)
{
  return IsKey (scanCode, skeyState, charValue, flags);
}
// ------ End of Mouse::is_key -------//


// return number of keystrokes made by auto repeat
// return 0 if no key is pressed
// return 1 if a key is first hit
// return 2 and above for auto repeat
//
// then use holding_scan_code/ holding_key_code and skey_state
//
int Mouse::key_auto_repeat_count()
{
	if( holding_scan_code )
	{
		DWORD holdTime = m.get_time() - press_key_time;
		if( holdTime >= first_repeat_key_time )
			return (holdTime - first_repeat_key_time) / repeat_rate + 2;
		else
			return 1;
	}
	return 0;
}

MouseDispCount::MouseDispCount()
{
	// set cursor position
	SetMousePos( mouse.cur_x, mouse.cur_y);

	// show cursor
	mouse.hide();
	// #### begin Gilbert 9/1 #######//
	vga_front.temp_unlock();
	vga_back.temp_unlock();
	// #### end Gilbert 9/1 #######//
	ShowMouseCursor(true);
}

MouseDispCount::~MouseDispCount()
{
	// set cursor position
	GetMousePos(&mouse.cur_x, &mouse.cur_y);

	// hide cursor
	ShowMouseCursor(false);
	// #### begin Gilbert 9/1 #######//
	vga_front.temp_restore_lock();
	vga_back.temp_restore_lock();
	// #### end Gilbert 9/1 #######//
	mouse.show();
	int ev = mouse.get_event();
	ev = mouse.get_event();

}

