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

//Filename    : OVBROWIF.CPP
//Description : Object Visual Browse for the Interface

#include <osys.h>
#include <oinfo.h>
#include <ovga.h>
#include <ovbrowif.h>

//--------- Begin of function VBrowseIF::init --------//
void VBrowseIF::init(int inX1, int inY1, int inX2, int inY2,
						 int recWidth, int recHeight,
						 int totalRec, BrowDispFP dispFunc,
						 int dispFrame, int minInterSpace, int dispFrameRect)
{
	VBrowse::init(inX1, inY1, inX2, inY2, 0,
						 recWidth, recHeight,
						 totalRec, dispFunc,
						 dispFrame, minInterSpace, dispFrameRect);
	disp_frame_rect = dispFrameRect;
}
//--------- End of function VBrowseIF::init --------//


//---------- Begin of function VBrowseIF::VBrowseIF -------//
//
VBrowseIF::VBrowseIF()
{
}
//----------- End of function VBrowseIF::VBrowseIF ----------//


//---------- Begin of function VBrowseIF::init_var -------//
//
void VBrowseIF::init_var(int totalRec, int recNo)
{
	VBrowse::init_var(totalRec, recNo);
	scroll_bar.init( 1, x2-SCROLL_BAR_WIDTH, y1, x2, y2, disp_max_rec,
			 (disp_frame && x_max_rec==1 ? 1 : disp_max_rec),    // only use record as scroller unit when it's a vertical browser with frame
			 total_rec_num, 1 );		// last 1 - Interface mode if_flag is 1
}
//----------- End of function VBrowseIF::init_var ----------//


//--------- Begin of function VBrowseIF::paint ---------//
//
void VBrowseIF::paint()
{
	vga.d3_panel_down( x1, y1, x2-SCROLL_BAR_WIDTH-3, y2, 0 );  // the list content box

	scroll_bar.paint();

}
//--------- End of function VBrowseIF::paint ---------//


//--------- Begin of function VBrowseIF::refresh ---------//
//
// Reset the no. of record and current record pointer and cause displaying
//
// [int] newRecNo      = new current record no.
//                       (default : -1, no change)
// [int] newTotalRec   = new total no. of records
//                       (default : -1, no change)
//
void VBrowseIF::refresh(int newRecNo, int newTotalRec)
{
	if( !vga.use_back_buf )
		vga.blt_buf( x1, y1, x2-SCROLL_BAR_WIDTH-3, y2, 0 );  // the list content box

	VBrowse::refresh(newRecNo, newTotalRec);
}
//--------- End of function VBrowseIF::refresh ---------//


//-------- Begin of function VBrowseIF::disp_all ----------//
//
// Display all records on screen, highlight cur_rec_no
//
void VBrowseIF::disp_all()
{
	int recNo;

	if( !vga.use_back_buf )
		vga.blt_buf( ix1, iy1, ix2, iy2, 0 ); // clear background

	int scrollRecno = (disp_frame && x_max_rec==1) ? rec_no : top_rec_no;

	scroll_bar.refresh( scrollRecno, 1, disp_max_rec,
				 (disp_frame && x_max_rec==1 ? 1 : disp_max_rec),
				 total_rec_num );
	
	for( recNo=top_rec_no ; recNo<=total_rec_num && recNo<top_rec_no+disp_max_rec ; recNo++ )
	{
		disp_one( recNo, DISP_REC );

		if( recNo == rec_no )
			disp_one( recNo, DISP_HIGH );
	}
}
//--------- End of function VBrowseIF::disp_all ----------//


//-------- Begin of function VBrowseIF::disp_one ----------//
//
// <int> recNo     = the no. of the record to display
// <int> putType   = DISP_REC  - display record
//                   DISP_HIGH - highlight rect
//                   CLEAR_HIGH- clear highlight rect
//
void VBrowseIF::disp_one(int recNo, int dispType)
{
	if( none_record )
		return;

	int x,y;

	y = iy1 + (recNo-top_rec_no)/x_max_rec * (rec_height+rec_y_space);
	x = ix1 + (recNo-top_rec_no)%x_max_rec * (rec_width+rec_x_space);

	if (!disp_frame_rect)
		return;

	//---- put a outline rect around the record if it is highlight ---//

	if( disp_frame && dispType == CLEAR_HIGH && !vga.use_back_buf )
	{
		vga.blt_buf( x-2, y-2			  , x+rec_width+1, y-2				, 0 );	// top
		vga.blt_buf( x-2, y+rec_height+1, x+rec_width+1, y+rec_height+1, 0 );	// bottom
		vga.blt_buf( x-2, y-2			  , x-2		     , y+rec_height+1, 0 );	// left
		vga.blt_buf( x+rec_width+1, y-2 , x+rec_width+1, y+rec_height+1, 0 );	// right
	}

	if( dispType == DISP_REC )
		disp_rec( recNo, x, y, INFO_REPAINT );  // call user defined function

	if( disp_frame && dispType == DISP_HIGH )
	{
		//-------- draw d3 up border --------//

		Vga::active_buf->draw_d3_up_border(x-2, y-2, x+rec_width+1, y+rec_height+1);

		//--------- display the record --------//

		int scrollRecno = (disp_frame && x_max_rec==1) ? recNo : top_rec_no;

		scroll_bar.refresh( scrollRecno, 0, disp_max_rec,
			 (disp_frame && x_max_rec==1 ? 1 : disp_max_rec),
					total_rec_num );
	}
}
//--------- End of function VBrowseIF::disp_one ----------//


//-------- Begin of function VBrowseIF::disp_rec --------//
//
void VBrowseIF::disp_rec(int recNo, int x, int y, int refreshFlag)
{
	(*disp_rec_func)( recNo, x, y, refreshFlag );  // call user defined function

	sys.yield();
}
//---------- End of function VBrowseIF::disp_rec -----------//
