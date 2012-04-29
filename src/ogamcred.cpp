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

// Filename    : OGAMCRED.CPP
// Description : viewing credits

#include <osys.h>
#include <ovga.h>
#include <ovgalock.h>
#include <ofont.h>
#include <omouse.h>
#include <omusic.h>
#include <oimgres.h>
#include <ogame.h>
#include <ofiletxt.h>

//------ Declare static functions --------//

/*
static void disp_credits_2();
static void disp_credits_3();
static void disp_credits_4();
static void disp_credits_5();
static void disp_credits(int y, char** creditNameArray);
*/

static ResourceIdx res_credit;

//------ Begin of function Game::view_credits ------//

void Game::view_credits()
{
	//----- display the first page ------//
	vga.disp_image_file("CREDITS1");

	String str;
	str  = DIR_RES;
	str += "CREDIT.RES";
	res_credit.init(str, 0);

	int   dataSize;
	char	*credit_buf = NULL;
	int	credit_buf_size = 0;

	File* filePtr = res_credit.get_file("CREDIT1", dataSize);
	if( !filePtr )       // if error getting the tutor resource
		err_here();

	//------ Open the file and allocate buffer -----//
	FileTxt fileTxt( filePtr, dataSize );  // initialize fileTxt with an existing file stream
	if( dataSize > credit_buf_size )
	{
		credit_buf      = mem_resize( credit_buf, dataSize );       // allocate a buffer larger than we need for the largest size possible
		credit_buf_size = dataSize;
	}
	char	*textPtr = credit_buf;
	fileTxt.read_paragraph(textPtr, credit_buf_size);
//	font_mid.center_put_paragraph( 0, 0, VGA_WIDTH, VGA_HEIGHT, textPtr, 2, 0, 0 );
	font_mid.put_paragraph( 50, 150, VGA_WIDTH-1, VGA_HEIGHT-1, textPtr );

	if( credit_buf )
	{
		mem_del(credit_buf);
		credit_buf = NULL;
	}

	if( mouse.wait_press(60)==2 )		// return 2 if pressed ESC or right mouse click
	{
		res_credit.deinit();
		vga.finish_disp_image_file();
		return;								// 60 seconds to time out
	}
	
	//----- display the 2nd page ------//
	vga.disp_image_file("CREDITS1");

	credit_buf = NULL;
	credit_buf_size = 0;

	filePtr = res_credit.get_file("CREDIT2", dataSize);
	if( !filePtr )       // if error getting the tutor resource
		err_here();

	//------ Open the file and allocate buffer -----//
	FileTxt fileTxt2( filePtr, dataSize );  // initialize fileTxt2 with an existing file stream
	if( dataSize > credit_buf_size )
	{
		credit_buf      = mem_resize( credit_buf, dataSize );       // allocate a buffer larger than we need for the largest size possible
		credit_buf_size = dataSize;
	}
	textPtr = credit_buf;
	fileTxt2.read_paragraph(textPtr, credit_buf_size);
//	font_mid.center_put_paragraph( 0, 0, VGA_WIDTH, VGA_HEIGHT, textPtr, 2, 0, 0 );
	font_mid.put_paragraph( 250, 200, VGA_WIDTH-1, VGA_HEIGHT-1, textPtr );

	if( credit_buf )
	{
		mem_del(credit_buf);
		credit_buf = NULL;
	}

	if( mouse.wait_press(60)==2 )		// return 2 if pressed ESC or right mouse click
	{
		res_credit.deinit();
		vga.finish_disp_image_file();
		return;								// 60 seconds to time out
	}

	//----- display the 3nd page ------//
	vga.disp_image_file("CREDITS1");

	credit_buf = NULL;
	credit_buf_size = 0;

	filePtr = res_credit.get_file("CREDIT3", dataSize);
	if( !filePtr )       // if error getting the tutor resource
		err_here();

	//------ Open the file and allocate buffer -----//
	FileTxt fileTxt3( filePtr, dataSize );  // initialize fileTxt2 with an existing file stream
	if( dataSize > credit_buf_size )
	{
		credit_buf      = mem_resize( credit_buf, dataSize );       // allocate a buffer larger than we need for the largest size possible
		credit_buf_size = dataSize;
	}
	textPtr = credit_buf;
	fileTxt3.read_paragraph(textPtr, credit_buf_size);
//	font_mid.center_put_paragraph( 0, 0, VGA_WIDTH, VGA_HEIGHT, textPtr, 2, 0, 0 );
	font_mid.put_paragraph( 250, 150, VGA_WIDTH-1, VGA_HEIGHT-1, textPtr );

	if( credit_buf )
	{
		mem_del(credit_buf);
		credit_buf = NULL;
	}

	if( mouse.wait_press(60)==2 )		// return 2 if pressed ESC or right mouse click
	{
		res_credit.deinit();
		vga.finish_disp_image_file();
		return;								// 60 seconds to time out
	}

	//----- display the 4th page ------//
	vga.disp_image_file("CREDITS1");

	credit_buf = NULL;
	credit_buf_size = 0;

	filePtr = res_credit.get_file("CREDIT4", dataSize);
	if( !filePtr )       // if error getting the tutor resource
		err_here();

	//------ Open the file and allocate buffer -----//
	FileTxt fileTxt4( filePtr, dataSize );  // initialize fileTxt2 with an existing file stream
	if( dataSize > credit_buf_size )
	{
		credit_buf      = mem_resize( credit_buf, dataSize );       // allocate a buffer larger than we need for the largest size possible
		credit_buf_size = dataSize;
	}
	textPtr = credit_buf;
	fileTxt4.read_paragraph(textPtr, credit_buf_size);
//	font_mid.center_put_paragraph( 0, 0, VGA_WIDTH, VGA_HEIGHT, textPtr, 2, 0, 0 );
	font_mid.put_paragraph( 250, 150, VGA_WIDTH-1, VGA_HEIGHT-1, textPtr );

	if( credit_buf )
	{
		mem_del(credit_buf);
		credit_buf = NULL;
	}

	if( mouse.wait_press(60)==2 )		// return 2 if pressed ESC or right mouse click
	{
		res_credit.deinit();
		vga.finish_disp_image_file();
		return;								// 60 seconds to time out
	}

#if(defined(CHINESE))
	//SXM:Credits
	//----- display the 5th page ------//
	vga.disp_image_file("CREDITS1");

	credit_buf = NULL;
	credit_buf_size = 0;

	filePtr = res_credit.get_file("CREDIT5", dataSize);
	if( !filePtr )       // if error getting the tutor resource
		err_here();

	//------ Open the file and allocate buffer -----//
	FileTxt fileTxt5( filePtr, dataSize );  // initialize fileTxt2 with an existing file stream
	if( dataSize > credit_buf_size )
	{
		credit_buf      = mem_resize( credit_buf, dataSize );       // allocate a buffer larger than we need for the largest size possible
		credit_buf_size = dataSize;
	}
	textPtr = credit_buf;
	fileTxt5.read_paragraph(textPtr, credit_buf_size);
//	font_mid.center_put_paragraph( 0, 0, VGA_WIDTH, VGA_HEIGHT, textPtr, 2, 0, 0 );
	font_mid.put_paragraph( 250, 150, VGA_WIDTH-1, VGA_HEIGHT-1, textPtr );

	if( credit_buf )
	{
		mem_del(credit_buf);
		credit_buf = NULL;
	}

	if( mouse.wait_press(60)==2 )		// return 2 if pressed ESC or right mouse click
	{
		res_credit.deinit();
		vga.finish_disp_image_file();
		return;								// 60 seconds to time out
	}
	//SXM
#endif // defined(CHINESE)
/*
#if(defined(FRENCH))
	//------ display the 5th page -----//

	vga.disp_image_file("CREDITS5");

	if( mouse.wait_press(60)==2 )		// return 2 if pressed ESC or right mouse click
	{
		vga.finish_disp_image_file();
		return;								// 60 seconds to time out
	}
#endif*/

	vga.finish_disp_image_file();
	res_credit.deinit();
}
//------ End of function Game::view_credits ------//

/*

//------ Begin of static function disp_credits_2 ------//

static void disp_credits_2()
{
	static char* credit_des_array[] =
	{
		"Executive Producers",
		"Project Manager",
		"Quality Manager",
		"Play Test Coordinator",
		"Manual Editing",
		"Manual Editing and Layout",
		"Product Marketing Manager",
		NULL
	};

	static char* credit_name_array[] =
	{
		"Ray Rutledge and Joe Rutledge",
		"Steve Wartofsky",
		"David Green",
		"Brain K. Davis VII",
		"Arnold Hendrick",
		"Sarah O'Keefe and Alan Pringle von 'Scriptorium Publishing Services, Inc.",
		"Angela Lipscomb",
	};

	//----------------------------------//

	int y=100;

	font_bible.center_put( 0, y, VGA_WIDTH-1, y+font_bible.height()-1, "Interactive Magic" );
	y+=font_bible.height()+20;

	for( int i=0 ; credit_des_array[i] ; i++ )
	{
		font_bible.center_put( 0, y, VGA_WIDTH-1, y+font_bible.height()-1, credit_des_array[i] );
		y+=font_bible.height()+3;

		font_bible.center_put( 0, y, VGA_WIDTH-1, y+font_bible.height()-1, credit_name_array[i] );
		y+=font_bible.height()+16;
	}
}
//------ End of static function disp_credits_2 ------//


//------ Begin of static function disp_credits_3 ------//

static void disp_credits_3()
{
	static char* credit_name_array[] =
	{
		"Joe Allen",
		"Ismini Boinodiris",
		"James Cowgill",
		"Chris Gardner",
		"Carlin Gartrell",
		"Anthony Lazaro",
		"Mike Metrosky",
		"Mike Pearson",
		"Marc Racine",
		"Jason Sircy",
		"Adam Turner",
		"Ted Wagoner",
		"Greg Young",
		NULL
	};

	//----------------------------------//

	int y=100;

	font_bible.center_put( 0, y, VGA_WIDTH-1, y+font_bible.height()-1,
		"Internal Beta-Testers" );

	disp_credits(y, credit_name_array);
}
//------ End of static function disp_credits_3 ------//


//------ Begin of static function disp_credits_4 ------//

static void disp_credits_4()
{
	static char* credit_name_array[] =
	{
		"Richard Arnesen"
		"JP Bernard",
		"Bryan Caldwell",
		"Kent Coleman",
		"Sorin Cristescu",
		"Al Demauro",
		"Troy Denkinger",
		"Chris Edwards",
		"Drew Fudenberg",
		"Michael Garrett",
		"Raymond Graham",
		"Tom Harlin",
		"Leonard Hemsen",
		"Chris Hepner",
		"Tom Hepner",
		"Ben Herd",
		"Benjamin Van Hoeson",
		"Allen Holland",
		"Brian Lander",
		"Steve Lieb",
		"mark Logsdon",
		"Crist-Jan Mannien",
		"David Newman",
		"Tomi J Nissinen",
		"Sven Johansson",
		"Tim Jordan",
		"Greg Ottoman",
		"Ralf Papen",
		"Jim Pedicord",
		"Gaspar Peixoto",
		"David Poythress",
		"Louis Rhodes",
		"Dean Robb",
		"Anthony Sage",
		"Todd Strobl",
		"Bjorn Tidal",
		"Ron Williams",
		"Christopher Yoder",
		NULL
	};

	//----------------------------------//

	int y=260;

	font_bible.center_put( 0, y, VGA_WIDTH-1, y+font_bible.height()-1,
		"External Beta-Testers" );

	disp_credits(y, credit_name_array);
}
//------ End of static function disp_credits_4 ------//


//------ Begin of static function disp_credits ------//

static void disp_credits(int y, char** creditNameArray)
{
	y+=font_bible.height()+13;

	String str;

	for( int i=0 ; creditNameArray[i] ; )
	{
		str = creditNameArray[i];
		i++;

		if( creditNameArray[i] )
		{
			str += ", ";
			str += creditNameArray[i];
			i++;

			if( creditNameArray[i] )
			{
				str += ", ";
				str += creditNameArray[i];
				i++;

				if( creditNameArray[i] )
				{
					str += ", ";
					str += creditNameArray[i];
					i++;

					if( creditNameArray[i] )
						str += ",";
				}
			}
		}

		font_bible.center_put( 0, y, VGA_WIDTH-1, y+font_bible.height()-1, str );
		y+=font_bible.height()+3;
	}
}
//------ End of static function disp_credits ------//

*/
