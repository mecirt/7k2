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

//Filename    : OHELP.CPP
//Description : Object Help

#include <all.h>
#include <ohelp.h>
#include <osys.h>
#include <ostr.h>
#include <ovga.h>
#include <omodeid.h>
#include <omusic.h>
#include <ofont.h>
#include <oimgres.h>
#include <oinfo.h>
#include <ogame.h>
#include <ounit.h>
#include <obox.h>
#include <omouse.h>
#include <oremote.h>
#include <ovbrowse.h>
#include <ofiletxt.h>
#include <oconfig.h>
#include <oblob2w.h>
#include <otutor.h>

//---------- Define constant -------------//

//#define HELP_BOX_COLOR      (VGA_GRAY+5)
#define HELP_BOX_COLOR      185

//#define HELP_INACTIVE_TIME  ((float) 0.8)		// when the mouse is inactive for one second, display help

//##### begin trevor 10-12 #####// 
// #define HELP_INACTIVE_TIME  ((float) 1.2)		// when the mouse is inactive for one second, display help
//##### end trevor 10-12 #####//
//##### begin Gilber 30/6 #####// 
#define HELP_INACTIVE_TIME  1500 // when the mouse is inactive for one second, display help
//##### end Gilbert 30/6 #####// 

enum { HELP_SCR_BUF_WIDTH  = 400,      // 400
		 HELP_SCR_BUF_HEIGHT = 200,      // 300
		 HELP_SCR_BUF_SIZE   = HELP_SCR_BUF_WIDTH * HELP_SCR_BUF_HEIGHT };

enum { MSG_LINE_SPACE = 4 };

enum { X_MARGIN = 10,
		 Y_MARGIN = 6  };

enum { MSG_WIN_WIDTH   = 390,
		 MSG_HEAD_HEIGHT = 16   };


short HelpInfo::calc_x1()
{
  // hack to properly support the existing data without having to convert it
  short diff = VGA_WIDTH - 800;
  if (area_x1 > 550) return area_x1 + diff;
  return area_x1;
}

short HelpInfo::calc_x2()
{
  // hack to properly support the existing data without having to convert it ... that x1 is intended
  short diff = VGA_WIDTH - 800;
  if (area_x1 > 550) return area_x2 + diff;
  return area_x2;
}

//------- Begin of function Help::Help ----------//

Help::Help()
{
	memset( this, 0, sizeof(Help) );

	help_info_array = (HelpInfo*) mem_add( sizeof(HelpInfo) * MAX_HELP_INFO );
}
//------- Begin of function Help::Help ----------//


//------- Begin of function Help::~Help ----------//

Help::~Help()
{
	deinit();
}
//------- Begin of function Help::~Help ----------//


//------- Begin of function Help::init ----------//

void Help::init(const char* resName)
{
	String str;

	str  = DIR_RES;
	str += resName;

	load( str );
}
//------- Begin of function Help::init ----------//


//------- Begin of function Help::deinit ----------//

void Help::deinit()
{
   if( help_info_array )
   {
		mem_del( help_info_array );
		help_info_array = NULL;
   }

	if( help_text_buf )
	{
		mem_del( help_text_buf );
		help_text_buf = NULL;
	}
}
//------- Begin of function Help::deinit ----------//


//------- Begin of function Help::load ----------//
//
// <char*> helpFileName = name of the help file.
//
void Help::load(char* helpFileName)
{
	//------ Open the file and allocate buffer -----//

	FileTxt fileTxt( helpFileName );

	int dataSize = fileTxt.file_size();

	if( dataSize > help_text_buf_size )
	{
		help_text_buf      = mem_resize( help_text_buf, dataSize );       // allocate a buffer larger than we need for the largest size possible
		help_text_buf_size = dataSize;
	}

	//-------- read in help info one by one --------//

	HelpInfo* iptr    = help_info_array;
	char*     textPtr = help_text_buf;
	int       readLen, totalReadLen=0;    // total length of text read into the help_text_buf
	int		 loopCount=0;
	char*		 tokenStr;
	
	help_info_count=0;

	first_help_by_help_code = first_help_by_area = -1;
	last_help_by_help_code = last_help_by_area = 0;

	while( !fileTxt.is_eof() )
	{
		err_when( loopCount++ > 10000 );

		tokenStr = fileTxt.get_token(0);		// don't advance the pointer

		if( !tokenStr )
			break;

		// ##### begin Gilbert 9/9 ######//
		iptr->help_code[0] = NULL;		// to identitfy search help text by help_code or coordinate
		// ##### end Gilbert 9/9 ######//

		//--------- if it's a help code ----------//

		if( tokenStr[0] >= 'A' && tokenStr[0] <= 'Z' )
		{
			strncpy( iptr->help_code, tokenStr, iptr->HELP_CODE_LEN );
			iptr->help_code[iptr->HELP_CODE_LEN] = NULL;

			// ###### begin Gilbert 9/9 #######//
			if( first_help_by_help_code == -1)
				first_help_by_help_code = help_info_count;
			last_help_by_help_code = help_info_count+1;		// record one after last
			// ###### end Gilbert 9/9 #######//
		}

		//------ if it's a help area position ------//

		else if( tokenStr[0] >= '0' && tokenStr[0] <= '9' )
		{
			iptr->area_x1 = (short) fileTxt.get_num();
			iptr->area_y1 = (short) fileTxt.get_num();
			iptr->area_x2 = (short) fileTxt.get_num();
			iptr->area_y2 = (short) fileTxt.get_num();

                        // skip the 1024x768 numbers
			fileTxt.get_num();
			fileTxt.get_num();
			fileTxt.get_num();
			fileTxt.get_num();
			iptr->monster_human_interface = (short) fileTxt.get_num();// 0 when display for both interfaces
																						 // 1 when display only for monster interface
																						 // 2 when display only for human interface
			
			// ###### begin Gilbert 15/9 #######//
			if( first_help_by_area == -1)
				first_help_by_area = help_info_count;
			last_help_by_area = help_info_count+1;			// record one after last
			// ###### end Gilbert 15/9 #######//
		}
		else
			err_here();

		//---------- next line -----------//

		fileTxt.next_line();

		if( fileTxt.is_eof() )
			break;

		//--------------------------------------------//
		// get help title
		//--------------------------------------------//

		fileTxt.read_line( iptr->help_title, iptr->HELP_TITLE_LEN );

		//---------------------------------------------------------//
		// get help description
		//---------------------------------------------------------//

		readLen = fileTxt.read_paragraph(textPtr, help_text_buf_size-totalReadLen);

		iptr->help_text_ptr = textPtr;
		iptr->help_text_len = readLen;

		textPtr      += readLen;
		totalReadLen += readLen;

		err_when( totalReadLen>help_text_buf_size );

		//----------- next help block -------------//

		fileTxt.next_line();      // pass the page break line

		help_info_count++;
		iptr++;

		err_when( help_info_count >= MAX_HELP_INFO );
	}

	if( first_help_by_help_code == -1 )
		first_help_by_help_code = 0;
	if( first_help_by_area == -1 )
		first_help_by_area = 0;

	err_when( last_help_by_help_code > help_info_count );
	err_when( last_help_by_area > help_info_count );
}
//--------- End of function Help::load ----------//


//----------- Begin of function Help::disp --------//
//
// Display help message on the given screen location.
//
void Help::disp()
{
	//---- first check if we should disp the help now ------//

	if( !should_disp() )
	{
		disp_short_help(&vga_back);  // check if we need the short one instead
		help_code[0] = NULL;	// reset it everytime after displaying, if the mouse is still on the button, help_code will be set again.
		custom_help_title[0] = NULL;
		return;
	}

	int i;
	HelpInfo* helpInfo = NULL;

	// ------ button help -------//

	if( help_code[0] )
	{
		//--------- locate the help and display it ----------//

		helpInfo = help_info_array + (i = first_help_by_help_code);
		for( ; i<last_help_by_help_code; i++, helpInfo++ )
		{
			if( helpInfo->help_code[0] == help_code[0] &&
				 strcmp( helpInfo->help_code, help_code )==0 )
			{
				break;
			}
		}

		if( i >= last_help_by_help_code )		// not found
			helpInfo = NULL;
	}

	//-------- custom help ---------//

	else if( custom_help_title[0] )
	{
	}

	//-------- other interface help ---------//

	else
	{
		//--------- locate the help and display it ----------//

		helpInfo = help_info_array + (i = first_help_by_area);
		for( ; i<last_help_by_area ; i++, helpInfo++ )
		{
			if( !helpInfo->help_code[0] && mouse.in_area( helpInfo->calc_x1(), helpInfo->area_y1,
				helpInfo->calc_x2(), helpInfo->area_y2) && 
				( (helpInfo->monster_human_interface == 1 && config.race_id < 0) ||
					(helpInfo->monster_human_interface == 2 && config.race_id > 0) ||
					(helpInfo->monster_human_interface == 0) ) )
			{
				break;
			}
		}

		if( i >= last_help_by_area )		// not found
			helpInfo = NULL;
	}

	//-------- button help ---------//

	if( help_code[0] && helpInfo )
	{
		disp_help( help_x, help_y,
			helpInfo->help_title, helpInfo->help_text_ptr );

//		help_code[0] = NULL;		// reset it everytime after displaying, if the mouse is still on the button, help_code will be set again.
	}

	//-------- custom help ---------//

	else if( custom_help_title[0] )
	{
		disp_help(help_x, help_y, custom_help_title, custom_help_detail);
//		custom_help_title[0] = NULL;
	}

	//-------- other interface help ---------//

	else if( helpInfo )
	{
		disp_help( (helpInfo->calc_x1()+helpInfo->calc_x2())/2,
					(helpInfo->area_y1+helpInfo->area_y2)/2,
					 helpInfo->help_title, helpInfo->help_text_ptr );
	}
}
//------------ End of function Help::disp ----------//


//---------- Begin of function Help::disp_help ----------//
//
// <int>   centerX, centerY - the center position of the help area.
// <char*> helpTitle  - title of the help
// [char*] helpDetail - detail of the help.
//
void Help::disp_help(int centerX, int centerY, char* helpTitle, char* helpDetail)
{
	if( config.help_mode == NO_HELP )
		return;

	//------ calculate the position of the help box ------//

	int winWidth, winHeight, dispHelpDetail=0;

	if( helpDetail && helpDetail[0] &&		  // with detailed help
		 config.help_mode == DETAIL_HELP )		  // Detailed Help
	{
		winWidth  = font_san.text_width(helpDetail, -1, MSG_WIN_WIDTH-X_MARGIN*2) + X_MARGIN*2;
		winHeight = Y_MARGIN*2 + font_san.height() + 11 + font_san.text_height(MSG_LINE_SPACE);    // text_width() must be called before calling text_height()

		int titleWidth = font_san.text_width(helpTitle, -1, MSG_WIN_WIDTH-X_MARGIN*2) + X_MARGIN*2;
		if( winWidth < titleWidth )
			winWidth = titleWidth;

		dispHelpDetail = 1;
	}
	else		// Help title only
	{
		winWidth  = font_san.text_width(helpTitle, -1, MSG_WIN_WIDTH-X_MARGIN*2) + X_MARGIN*2;
		winHeight = Y_MARGIN*2 + font_san.height() + 3;
	}

	//--- if the text is bigger than one text box can hold, use a scrollable text box ---//

	int x1, y1, x2, y2;

	if( winWidth * winHeight > HELP_SCR_BUF_SIZE )
	{
		x1 = max( 2, centerX - HELP_SCR_BUF_WIDTH  / 2 );
		y1 = max( 2, centerY - HELP_SCR_BUF_HEIGHT / 2 );

		x2 = x1 + HELP_SCR_BUF_WIDTH - 1;
		y2 = y1 + HELP_SCR_BUF_HEIGHT - 1;
	}
	else
	{
		x1 = max( 2, centerX - winWidth  / 2 );
		y1 = max( 2, centerY - winHeight / 2 );

		x2 = x1 + winWidth - 1;
		y2 = y1 + winHeight - 1;
	}

	if( x2 >= VGA_WIDTH )
	{
		x2 = VGA_WIDTH-10;
		x1 = x2-winWidth+1;
	}

	if( y2 >= VGA_HEIGHT )
	{
		y2 = VGA_HEIGHT-3;
		y1 = y2-winHeight+1;
	}

	//------------- save the area --------------//

	//------- Draw box (and arrow if specified object) ------//

	vga_buffer.bar( x1, y1, x2-3, y2-3, VgaBuf::color_up );
	vga_buffer.rect( x1, y1, x2-3, y2-3, 3, HELP_BOX_COLOR );
	vga_buffer.bar_alpha( x2-2, y1+2, x2, y2-3, 1, V_BLACK );
	vga_buffer.bar_alpha( x1+2, y2-2, x2, y2, 1, V_BLACK );

	//--------- disp help detail -----------//

	font_san.put( x1+X_MARGIN, y1+Y_MARGIN, helpTitle );

	if( dispHelpDetail )
	{
		int y = y1 + Y_MARGIN + font_san.height() + 4;

		vga_buffer.bar( x1, y, x2-3, y+1, 185 );  // line between description and help text

		font_san.put_paragraph( x1+X_MARGIN, y+4, x2-X_MARGIN, y2-Y_MARGIN, helpDetail, MSG_LINE_SPACE );
	}
}
//--------- End of function Help::disp_help ----------//


//--------- Begin of function Help::should_disp --------//
//
int Help::should_disp()
{
	if( config.help_mode < DETAIL_HELP || remote.is_enable())
		return 0;

	if( VBrowse::press_record )
		return 0;

	// ####### begin Gilbert 13/7 #########//
	if( last_mouse_x==mouse.cur_x && last_mouse_y==mouse.cur_y &&
		 !mouse.left_press && !mouse.right_press ) // && !mouse.any_click(2) )
	{
		if( m.get_time() - mouse_still_time >= HELP_INACTIVE_TIME )
	// ####### end Gilbert 13/7 #########//
		{
			return 1;
		}
	}
	else
	{
		last_mouse_x = mouse.cur_x;
		last_mouse_y = mouse.cur_y;
		mouse_still_time = m.get_time();
	}

	return 0;
}
//---------- End of function Help::should_disp ---------//


//--------- Begin of function Help::set_help --------//
//
void Help::set_help(int x1, int y1, int x2, int y2, const char* helpCode)
{
	err_when( strlen(helpCode) > 8 );

	if( !mouse.in_area(x1, y1, x2, y2) )
		return;

	strcpy( help_code, helpCode );

	help_x = (x1+x2)/2;
	help_y = (y1+y2)/2;
	help_x1 = x1;
	help_y1 = y1;
	help_x2 = x2;
	help_y2 = y2;
}
//---------- End of function Help::set_help ---------//


//--------- Begin of function Help::set_custom_help --------//
//
// <int>   x1, y1, x2, y2 - the coordination of the help
// <char*> helpTitle  - the title of the help
// [char*] helpDetail - the detailed text of the help
//
void Help::set_custom_help(int x1, int y1, int x2, int y2, const char* helpTitle, const char* helpDetail)
{
  printf ("set_custom_help %s, %s\n", helpTitle, helpDetail);
	if( !mouse.in_area(x1, y1, x2, y2) )
		return;

	// ##### begin Gilbert 10/12 ######//
	// clear help code
	help_code[0] = '\0';
	// ##### end Gilbert 10/12 ######//
	help_x = (x1+x2)/2;
	help_y = (y1+y2)/2;
	help_x1 = x1;
	help_y1 = y1;
	help_x2 = x2;
	help_y2 = y2;

	strncpy( custom_help_title, helpTitle, CUSTOM_HELP_TITLE_LEN );
	custom_help_title[CUSTOM_HELP_TITLE_LEN] = NULL;

	if( helpDetail )
	{
		strncpy( custom_help_detail, helpDetail, CUSTOM_HELP_DETAIL_LEN );
		custom_help_detail[CUSTOM_HELP_DETAIL_LEN] = NULL;
	}
	else
	{
		custom_help_detail[0] = NULL;
	}
}
//---------- End of function Help::set_custom_help ---------//


//--------- Begin of function Help::disp_short_help --------//
//
void Help::disp_short_help(VgaBuf *vgaBuf)
{
	// ###### begin Gilbert 9/9 #######//

	// ----- display help title in short bar under info -------//

	int i;
	HelpInfo* helpInfo = NULL;

	// ------ button help -------//

	if( help_code[0] )
	{
		//--------- locate the help and display it ----------//

		helpInfo = help_info_array + (i = first_help_by_help_code);
		for( ; i<last_help_by_help_code; i++, helpInfo++ )
		{
			if( helpInfo->help_code[0] == help_code[0] &&
				 strcmp( helpInfo->help_code, help_code )==0 )
			{
				break;
			}
		}

		if( i >= last_help_by_help_code )		// not found
			helpInfo = NULL;
	}

	//-------- custom help ---------//

	else if( custom_help_title[0] )
	{
	}

	//-------- other interface help ---------//

	else
	{
		//--------- locate the help and display it ----------//

		helpInfo = help_info_array + (i = first_help_by_area);
		for( ; i<last_help_by_area ; i++, helpInfo++ )
		{
			if( !helpInfo->help_code[0] && mouse.in_area( helpInfo->calc_x1(), helpInfo->area_y1,
				helpInfo->calc_x2(), helpInfo->area_y2)  &&
				!(game.game_mode == GAME_TUTORIAL && tutor.should_not_display_report_button_flag) &&		 
				( (helpInfo->monster_human_interface == 1 && config.race_id < 0) ||
					(helpInfo->monster_human_interface == 2 && config.race_id > 0) ||
					(helpInfo->monster_human_interface == 0) ) )
			{
				help_x1 = helpInfo->calc_x1();
				help_y1 = helpInfo->area_y1;
				help_x2 = helpInfo->calc_x2();
				help_y2 = helpInfo->area_y2;
				break;
			}
		}

		if( i >= last_help_by_area )		// not found
			helpInfo = NULL;
	}

	char *titleStr = NULL;

	if( helpInfo )
	{
		titleStr = helpInfo->help_title;
	}
	else if( custom_help_title[0] )
	{
		titleStr = custom_help_title;
	}

	if( titleStr )
	{
		short x1, y1, x2, y2;		// top left corner of the area
      //#### begin trevor 10-12 #####//
		int textWidth = font_san.text_width(titleStr) + X_MARGIN*2;
		int textHeight = font_san.text_height() + Y_MARGIN*2;
		//#### begin trevor 10-12 #####//
		x2 = help_x1 - 1;
		y2 = help_y1 - 1;
		x1 = x2 - textWidth + 1;
		y1 = y2 - textHeight + 1;
		if( y1 < 0 )
		{
			// try top border
			y1 = 0;
			y2 = y1 + textHeight - 1;
		}
		if( x1 < 0 )
		{
			// try right side
			x1 = help_x2 + 1;
			x2 = x1 + textWidth - 1;
		}

		if( config.help_mode >= BRIEF_HELP )
		{
			vgaBuf->bar( x1, y1, x2-3, y2-3, VgaBuf::color_up );
			vgaBuf->rect( x1, y1, x2-3, y2-3, 3, HELP_BOX_COLOR );
			vgaBuf->bar_alpha( x2-2, y1+2, x2, y2-3, 1, V_BLACK );
			vgaBuf->bar_alpha( x1+2, y2-2, x2, y2, 1, V_BLACK );

			font_san.center_put( x1, y1, x2-3, y2-3, titleStr );
		}
	}
	// ###### end Gilbert 9/9 #######//
}
//--------- End of function Help::disp_short_help --------//

