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

//Filename    : OINFOECO.CPP
//Description : Economy information screen

#include <oinfo.h>
#include <ovga.h>
#include <omodeid.h>
#include <osys.h>
#include <ofont.h>
#include <oimgres.h>
#include <ovbrowif.h>
#include <obutton.h>
#include <ofirm.h>
#include <ofirmres.h>
#include <oraceres.h>
#include <onation.h> 
#include <ounit.h>
#include <ot_reps.h>

//------------- Define coordinations -----------//

#define INCOME_BROWSE_X1 (ZOOM_X1+6)
#define INCOME_BROWSE_Y1 (ZOOM_Y1+6)
#define INCOME_BROWSE_X2 (ZOOM_X2-6)
#define INCOME_BROWSE_Y2 (INCOME_BROWSE_Y1+186)

#define EXPENSE_BROWSE_X1 (ZOOM_X1+6)
#define EXPENSE_BROWSE_Y1 (INCOME_BROWSE_Y2+6)
#define EXPENSE_BROWSE_X2 (ZOOM_X2-6)
#define EXPENSE_BROWSE_Y2 (ZOOM_Y2-30)

//----------- Define static variables ----------//

static VBrowseIF browse_income, browse_expense;

//----------- Define static functions ----------//

static void put_income_rec(int recNo, int x, int y);
static void put_expense_rec(int recNo, int x, int y);

static void disp_total();

//--------- Begin of function Info::disp_economy ---------//
//
void Info::disp_economy()
{
	//------- display the income report -------//

	int x=INCOME_BROWSE_X1+9;
	int y=INCOME_BROWSE_Y1+4;

	vga_back.d3_panel_up(INCOME_BROWSE_X1, INCOME_BROWSE_Y1, INCOME_BROWSE_X2, INCOME_BROWSE_Y1+20 );

	font_bld.put( x	 , y, text_reports.str_income_item() ); // "Income Item" );
	font_bld.put( x+350, y, text_reports.str_yearly_income() ); // "Yearly Income" );

	int incomeCount;		// only display the cheat income if it amount is > 0

	if( nation_array[info.viewing_nation_recno]->income_365days(INCOME_CHEAT) > 0 &&
		 (sys.testing_session || info.viewing_nation_recno == nation_array.player_recno) )		// only display cheat amount in debug mode or cheat amount of the player's kingdom, do not display cheat amount on AI kingdoms
	{
		incomeCount = INCOME_TYPE_COUNT;
	}
	else
		incomeCount = INCOME_TYPE_COUNT-1;

	// ####### begin Gilbert 19/10 ######//
	browse_income.init( INCOME_BROWSE_X1, INCOME_BROWSE_Y1+22, INCOME_BROWSE_X2, INCOME_BROWSE_Y2-20,
							0, 18, incomeCount, put_income_rec, 1 );
	// ####### begin Gilbert 19/10 ######//

	browse_income.open(browse_income_recno);

	//------- display the expense report -------//

	x=EXPENSE_BROWSE_X1+9;
	y=EXPENSE_BROWSE_Y1+4;

	vga_back.d3_panel_up(EXPENSE_BROWSE_X1, EXPENSE_BROWSE_Y1, EXPENSE_BROWSE_X2, EXPENSE_BROWSE_Y1+20 );

	font_bld.put( x	 , y, text_reports.str_expense_item() ); // "Expense Item" );
	font_bld.put( x+350, y, text_reports.str_yearly_expense() ); // "Yearly Expense" );

	// ####### begin Gilbert 19/10 ######//
	browse_expense.init( EXPENSE_BROWSE_X1, EXPENSE_BROWSE_Y1+22, EXPENSE_BROWSE_X2, EXPENSE_BROWSE_Y2-20,
						0, 18, EXPENSE_TYPE_COUNT, put_expense_rec, 1 );
	// ####### end Gilbert 19/10 ######//

	browse_expense.open(browse_expense_recno);

	//--------- display total ----------//

	disp_total();
}
//----------- End of function Info::disp_economy -----------//


//--------- Begin of function Info::detect_economy ---------//
//
void Info::detect_economy()
{
	if( browse_income.detect() )
		browse_income_recno = browse_income.recno();

	if( browse_expense.detect() )
		browse_expense_recno = browse_expense.recno();
}
//----------- End of function Info::detect_economy -----------//


//--------- Begin of static function disp_total ---------//
//
static void disp_total()
{
	//--- calculate the total income and expense ----//

	float totalIncome  = (float) 0;
	float totalExpense = (float) 0;

	Nation* nationPtr = nation_array[info.viewing_nation_recno];

	int i;
	for( i=0 ; i<INCOME_TYPE_COUNT ; i++ )
		totalIncome  += nationPtr->income_365days(i);

	for( i=0 ; i<EXPENSE_TYPE_COUNT ; i++ )
		totalExpense += nationPtr->expense_365days(i);

	//---------- display total income ----------//

	vga_back.d3_panel_up(INCOME_BROWSE_X1, INCOME_BROWSE_Y2-18, INCOME_BROWSE_X2, INCOME_BROWSE_Y2 );

	int x=INCOME_BROWSE_X1+9;
	int y=INCOME_BROWSE_Y2-16;

	font_bld.put( x, y, text_reports.str_total_yearly_income() ); // "Total Yearly Income" );
	font_bld.put( x+370, y, m.format( (int) totalIncome, 2 ) );

	//---------- display total expense ----------//

	vga_back.d3_panel_up(EXPENSE_BROWSE_X1, EXPENSE_BROWSE_Y2-18, EXPENSE_BROWSE_X2, EXPENSE_BROWSE_Y2 );

	x=EXPENSE_BROWSE_X1+9;
	y=EXPENSE_BROWSE_Y2-16;

	font_bld.put( x, y, text_reports.str_total_yearly_expense() ); // "Total Yearly Expenses" );
	font_bld.put( x+370, y, m.format( (int) totalExpense, 2 ) );

	//----------- display the balance --------//

	y=EXPENSE_BROWSE_Y2+7;

	vga_back.d3_panel_up(EXPENSE_BROWSE_X1, EXPENSE_BROWSE_Y2+4, EXPENSE_BROWSE_X2, ZOOM_Y2-6 );

	font_bld.put( x, y, text_reports.str_yearly_balance() ); // "Yearly Balance" );
	font_bld.put( x+370, y, m.format( (int)(totalIncome-totalExpense), 2 ) );
}
//----------- End of static function disp_total -----------//


//-------- Begin of static function put_income_rec --------//
//
static void put_income_rec(int recNo, int x, int y)
{
	//----- define income descriptions ------//

//	static char* income_des_array[INCOME_TYPE_COUNT] =		// see TextResReports::str_income_desc
//	{
//		"Sale of Goods",
//		"Exports",
//		"Taxes",
//		"Recovered Treasure",
//		"Sale of Buildings",
//		"Aid/Tribute from Other Kingdoms",
//		"Conversion",
//		"Cheating",
//	};

	//---------------------------------//

	x+=3;
	y+=2;

	Nation* nationPtr = nation_array[info.viewing_nation_recno];

//	char* str;
//	if( nationPtr->is_monster() && recNo==6 )
//		str = "Aid/Tribute from Other Kingdoms/Slave Towns";
//	else
//		str = income_des_array[recNo-1];

	font_bld.put( x    , y, text_reports.str_income_desc(recNo-1, nationPtr->is_monster()) );
	font_bld.put( x+370, y, m.format( (int) nationPtr->income_365days(recNo-1), 2 ) );
}
//----------- End of static function put_income_rec -----------//


//-------- Begin of static function put_expense_rec --------//
//
static void put_expense_rec(int recNo, int x, int y)
{
	//----- define expense descriptions -------//

//	static char* expense_des_array[EXPENSE_TYPE_COUNT] =
//	{
//		"General Expenses",
//		"Spy Expenses",
//		"Other Mobile Human Unit Expenses",
//		"Caravan Expenses",
//		"Weapons Expenses",
//		"Construction and Repair Costs",
//		"Building Expenses",
//		"Town Expenses",
//		"Training Units",
//		"Hiring Units",
//		"Honoring Units",
//		"Grants to Your Citizens",
//		"Grants to Other People",
//		"Imports",
//		"Aid/Tribute to Other Kingdoms",
//		"Bribes",
//		"Conversion",
//	};

	//---------------------------------//

	x+=3;
	// ##### begin Gilbert 19/10 ######//
	y+=2;
	// ##### end Gilbert 19/10 ######//

	Nation* nationPtr = nation_array[info.viewing_nation_recno];

	// font_bld.put( x    , y, expense_des_array[recNo-1] );
	font_bld.put( x    , y, text_reports.str_expense_desc(recNo-1, nationPtr->is_monster()) ); // expense_des_array[recNo-1] );
	font_bld.put( x+370, y, m.format( (int) nationPtr->expense_365days(recNo-1), 2 ) );
}
//----------- End of static function put_expense_rec -----------//


