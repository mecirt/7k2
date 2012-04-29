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

//Filename    : OTALKFRE.CPP
//Description : French version of the talk messages

#ifdef FRENCH

#include <omouse.h>
#include <osys.h>
#include <ovga.h>
#include <oinfo.h>
#include <ofont.h>
#include <otechres.h>
#include <ogameset.h>
#include <onews.h>
#include <onation.h>
#include <otalkres.h>

//-------- define static vars --------//

static String str;						// these vars don't have to be saved as their are only used by msg_str() for passing vars to other functions
static short  viewing_nation_recno;
static char   should_disp_reply;
static char   disp_second_line;


//------- Begin of function TalkMsg::msg_str --------//
//
// Return the text of this message.
//
// <int> viewingNationRecno = the recno of the nation viewing this message
// [int] dispReply 			 = whether display the reply if there is one
//										(default: 1)
// [int] dispSecondLine		 = whether should display the second line of the
//										message (default: 0)
//
char* TalkMsg::msg_str(int viewingNationRecno, int dispReply, int dispSecondLine)
{
	viewing_nation_recno = viewingNationRecno;
	should_disp_reply		= dispReply;
	disp_second_line		= dispSecondLine;

	//-------- compose the message str -------//

	switch(talk_id)
	{
		case TALK_PROPOSE_TRADE_TREATY:
			propose_treaty("accord commercial");
			break;

		case TALK_PROPOSE_FRIENDLY_TREATY:
			propose_treaty("pacte de non-agression");
			break;

		case TALK_PROPOSE_ALLIANCE_TREATY:
			propose_treaty("trait� d'alliance");
			break;

		case TALK_END_TRADE_TREATY:
			end_treaty("accord commercial");
			break;

		case TALK_END_FRIENDLY_TREATY:
			end_treaty("pacte de non-agression");
			break;

		case TALK_END_ALLIANCE_TREATY:
			end_treaty("trait� d'alliance");
			break;

		case TALK_REQUEST_MILITARY_AID:
			request_military_aid();
			break;

		case TALK_REQUEST_TRADE_EMBARGO:
			request_trade_embargo();
			break;

		case TALK_REQUEST_CEASE_WAR:
			request_cease_war();
			break;

		case TALK_REQUEST_DECLARE_WAR:
			request_declare_war();
			break;

		case TALK_REQUEST_BUY_FOOD:
			request_buy_food();
			break;

		case TALK_DECLARE_WAR:
			declare_war();
			break;

		case TALK_GIVE_TRIBUTE:
			give_tribute("tribut");
			break;

		case TALK_DEMAND_TRIBUTE:
			demand_tribute(0);		// 1-is tribute, not aid
			break;

		case TALK_GIVE_AID:
			give_tribute("aide");
			break;

		case TALK_DEMAND_AID:
			demand_tribute(1);		// 1-is aid, not tribute
			break;

		case TALK_EXCHANGE_TECH:
			exchange_tech();
			break;

		case TALK_REQUEST_SURRENDER:
			request_surrender();
			break;

		case TALK_SURRENDER:
			surrender();
			break;

		default:
			err_here();
	}

	return str;
}
//-------- End of function TalkMsg::msg_str ---------//


//----- Begin of function TalkMsg::propose_treaty ------//
//
// talk_para1 - duration of the treaty (no. of years).
//
void TalkMsg::propose_treaty(char* treatyTypeStr)
{
	//---------------------------------------------//
	//
	// Send:
	//
	// <King>'s Kingdom proposes a/an friendly/alliance treaty to you.
	// You propose a/an friendly/alliance treaty to <King>'s Kingdom.
	//
	// Reply:
	//
	// <King>'s Kingdom accepts/rejects your proposed
	// friendly/alliance treaty.
	//
	// You accept/reject the friendly/alliance treaty
	// proposed by <King>'s Kingdom.
	//
	//---------------------------------------------//
	//
	// Envoyer:
	//
	// Le Royaume de <King> vous propose un pacte_de_non-agression/trait�_d'alliance.
	//
	// Vous proposez un pacte_de_non-agression/trait�_d'alliance au Royaume de
	// <King>.
	//
	// R�pondre:
	//
	// Le Royaume de <King> accepte/rejette votre proposition de
	// pacte_de_non-agression/trait�_d'alliance.
	//
	// Vous acceptez/rejetez le pacte_de_non-agression/trait�_d'alliance
	// propos� par le Royaume de <King>.
	//
	//---------------------------------------------//

	//--------------------------------------//

	if( reply_type == REPLY_WAITING || !should_disp_reply )
	{
		if( viewing_nation_recno == from_nation_recno )
		{
			str  = "Vous proposez un ";
			str += treatyTypeStr;
			str += " au ";
			str += to_nation_name();
			str += ".";
		}
		else
		{
			str  = "Le ";
			str += from_nation_name();
			str += " vous propose un ";
			str += treatyTypeStr;
			str += ".";
		}
	}
	else
	{
		if( viewing_nation_recno == from_nation_recno )
		{
			str  = "Le ";
			str += to_nation_name();

			if( reply_type == REPLY_ACCEPT )
				str += " accepte votre proposition de ";
			else
				str += " rejette votre proposition de ";

			str += treatyTypeStr;
			str += ".";
		}
		else
		{
			if( reply_type == REPLY_ACCEPT )
				str = "Vous acceptez le ";
			else
				str = "Vous acceptez le ";

			str += treatyTypeStr;

			str += " propos� par le ";
			str += from_nation_name();
			str += ".";
		}
	}
}
//------- End of function TalkMsg::propose_treaty ------//


//----- Begin of function TalkMsg::end_treaty ------//
//
// talk_para1 - treaty type, RELATION_FRIENDLY or RELATION_ALLIANCE.
//
void TalkMsg::end_treaty(char* treatyTypeStr)
{
	//---------------------------------------------//
	//
	// Send:
	// <King>'s Kingdom terminates its friendly/alliance treaty with you.
	// You terminate your friendly/alliance treaty with <King>'s Kingdom.
	//
	//---------------------------------------------//
	//
	// Envoyer:
	// Le Royaume de <King> met fin � son  pacte_de_non-agression/trait�_d'alliance 
	// avec vous.
	//
	// Vous mettez fin au pacte_de_non-agression/trait�_d'alliance
	// avec le Royaume de <King>.
	//
	//---------------------------------------------//

	if( viewing_nation_recno == from_nation_recno )
	{
		str  = "Vous mettez fin au ";
		str += treatyTypeStr;
		str += " avec le ";
		str +=  to_nation_name();
		str += ".";
	}
	else
	{
		str =  "Le ";
		str += from_nation_name();
		str += " met fin � son ";
		str += treatyTypeStr;
		str += " avec vous.";
	}
}
//------- End of function TalkMsg::end_treaty ------//


//----- Begin of function TalkMsg::request_cease_war ------//
//
void TalkMsg::request_cease_war()
{
	//---------------------------------------------//
	//
	// Send:
	// <King>'s Kingdom requests a cease-fire.
	// You request a cease-fire with <King>'s Kingdom.
	//
	// Reply:
	// <King>'s Kingdom agrees to a cease-fire.
	// <King>'s Kingdom refuses a cease-fire.
	// You agree to a cease-fire with <King>'s Kingdom.
	// You refuse a cease-fire with <King>'s Kingdom.
	//
	//---------------------------------------------//
	//
	// Envoyer:
	// Le Royaume de <King> demande un cessez-le-feu.
	// Vous demandez un cessez-le-feu au Royaume de <King>.
	//
	// R�pondre:
	// Le Royaume de <King> accepte un cessez-le-feu.
	// Le Royaume de <King> refuse un cessez-le-feu.
	// Vous acceptez un cessez-le-feu avec le Royaume de <King>.
	// Vous refusez un cessez-le-feu avec le Royaume de <King>.
	//
	//---------------------------------------------//

	if( reply_type == REPLY_WAITING || !should_disp_reply )
	{
		if( viewing_nation_recno == from_nation_recno )
		{
			str  = "Vous demandez un cessez-le-feu au ";
			str +=  to_nation_name();
			str += ".";
		}
		else
		{
			str  = "Le ";
			str +=  from_nation_name();
			str += " demande un cessez-le-feu.";
		}
	}
	else
	{
		if( viewing_nation_recno == from_nation_recno )
		{
			str = "Le ";
			str += to_nation_name();

			if( reply_type == REPLY_ACCEPT )
				str += " accepte";
			else
				str += " refuse";

			str += " cessez-le-feu.";
		}
		else
		{
			if( reply_type == REPLY_ACCEPT )
				str = "Vous acceptez ";
			else
				str = "Vous refusez";

			str += " un cessez-le-feu avec le ";
			str += from_nation_name();
			str += ".";
		}
	}
}
//------- End of function TalkMsg::request_cease_war ------//


//----- Begin of function TalkMsg::request_declare_war ------//
//
// talk_para1 - the recno of the nation to declare war with.
//
void TalkMsg::request_declare_war()
{
	//---------------------------------------------//
	//
	// Send:
	// <King>'s Kingdom requests that you declare war on <King B>'s Kingdom.
	// You request <King>'s Kingdom to declare war on <King B>'s Kingdom.
	//
	// Reply:
	// <King>'s Kingdom agrees/refuses to declare war on <King B>'s Kingdom.
	// You agree/refuse to declare war on <King B>'s Kingdom.
	//
	//---------------------------------------------//
	//
	// Envoyer:
	// Le Royaume de <King> demande que vous d�clariez la guerre au
	// Royaume de <King B>.
	//
	// Vous demandez au Royaume de <King> de d�clarer la guerre au
	// Royaume de <King B>.
	//
	// R�pondre:
	// Le Royaume de <King> accepte/refuse de d�clarer la guerre au
	// Royaume de <King B>.
	//
	// Vous acceptez/refusez de d�clarer la guerre au Royaume de <King B>.
	//
	//---------------------------------------------//


	if( reply_type == REPLY_WAITING || !should_disp_reply )
	{
		if( viewing_nation_recno == from_nation_recno )
		{
			str  = "Vous demandez au ";
			str +=  to_nation_name();
			str += " de";
		}
		else
		{
			str = "Le ";
			str += from_nation_name();
			str += " demande que vous";
		}

		str += " d�clarer la guerre au ";
		if (nation_array.is_deleted(talk_para1))
		{
			str += "the destroyed ";
			str += nation_array.custom_nation_name_array[talk_para1];
		}
		else
		{
			str += nation_array.custom_nation_name_array[talk_para1];
				//nation_array[talk_para1]->nation_name();
			str += nation_color_code_str(talk_para1);
		}
		str += ".";
	}
	else
	{
		if( viewing_nation_recno == from_nation_recno )
		{
			str  = "Le ";
			str += to_nation_name();

			if( reply_type == REPLY_ACCEPT )
				str += " accepte";
			else
				str += " refuse";
		}
		else
		{
			if( reply_type == REPLY_ACCEPT )
				str = "Vous acceptez";
			else
				str = "Vous refusez";
		}

		str += " de d�clarer la guerre au ";
		if (nation_array.is_deleted(talk_para1))
		{
			str += "the destroyed ";
			str += nation_array.custom_nation_name_array[talk_para1];
		}
		else
		{
			str += nation_array.custom_nation_name_array[talk_para1];
				//nation_array[talk_para1]->nation_name();
			str += nation_color_code_str(talk_para1);
		}
		str += ".";
	}
}
//------- End of function TalkMsg::request_declare_war ------//


//----- Begin of function TalkMsg::request_buy_food ------//
//
// talk_para1 - the qty of food the nation wants to buy.
// talk_para2 - price offered for 10 qty of food.
//
void TalkMsg::request_buy_food()
{
	//---- display the second line in the reply question ----//
	//
	// <King>'s Kingdom offers $10 for 10 units of food.
	//
	//-------------------------------------------------------//
	//
	// Le Royaume de <King> vous offre $10 pour 10 unit�s de nourriture.
	//
	//-------------------------------------------------------//

	if( disp_second_line )
	{
		str  = "Le ";
		str += from_nation_name();
		str += " vous offre ";
		str += m.format(talk_para2,2);
		str += " pour 10 unit�s de nourriture.";

		return;
	}

	//---------------------------------------------//
	//
	// Send:
	// <King>'s Kingdom requests to purchase <amount>
	// units of food from you.
	//
	// You request to purchase <amount> units of food
	// from <King>'s Kingdom.
	//
	// Reply:
	// <King>'s Kingdom agrees/refuses to sell <amount> units
	// of food to you.
	//
	// You agree/refuse to sell <amount> units of food to
	// <King>'s Kingdom.
	//
	//---------------------------------------------//
	//
	// Envoyer:
	// Le Royaume de <King> demande que vous lui vendiez <amount>
	// unit�s de nourriture.
	//
	// Vous demandez � acheter <amount> unit�s de nourriture
	// au Royaume de <King>.
	//
	// R�pondre:
	// Le Royaume de <King> accepte/refuse de vous vendre <amount> unit�s
	// de nourriture.
	//
	// Vous acceptez/refusez de vendre <amount> unit�s de nourriture
	// au Royaume de <King>.
	//
	//---------------------------------------------//

	if( reply_type == REPLY_WAITING || !should_disp_reply )
	{
		if( viewing_nation_recno == from_nation_recno )
		{
			str  = "Vous demandez � acheter ";
			str += talk_para1;
			str += " unit�s de nourriture au ";
			str += to_nation_name();
			str += ".";
		}
		else
		{
			str  = "Le ";
			str += from_nation_name();
			str += " demande que vous lui vendiez ";
			str += talk_para1;
			str += " unit�s de nourriture.";
		}
	}
	else
	{
		if( viewing_nation_recno == from_nation_recno )
		{
			str  = "Le ";
			str += to_nation_name();

			if( reply_type == REPLY_ACCEPT )
				str += " accepte";
			else
				str += " refuse";

			str += " de vous vendre ";
			str += talk_para1;
			str += " unit�s of de nourriture.";
		}
		else
		{
			if( reply_type == REPLY_ACCEPT )
				str = "Vous acceptez";
			else
				str = "Vous refusez";

			str += " de vendre ";
			str += talk_para1;
			str += " unit�s de nourriture au ";
			str += from_nation_name();
			str += ".";
		}
	}
}
//------- End of function TalkMsg::request_buy_food ------//


//----- Begin of function TalkMsg::declare_war ------//
//
void TalkMsg::declare_war()
{
	//---------------------------------------------//
	//
	// Send:
	// <King>'s Kingdom declares war on you.
	// You declare war on <King>'s Kingdom.
	//
	//---------------------------------------------//
	//
	// Envoyer:
	// Le Royaume de <King> vous d�clare la guerre.
	// Vous d�clarez la guerre au Royaume de <King>.
	//
	//---------------------------------------------//


	if( viewing_nation_recno == from_nation_recno )
	{
		str  = "Vous d�clarez la guerre au ";
		str += to_nation_name();
		str += ".";
	}
	else
	{
		str  = "Le ";
		str += from_nation_name();
		str += " vous d�clare la guerre.";
	}
}
//------- End of function TalkMsg::declare_war ------//


//----- Begin of function TalkMsg::give_tribute ------//
//
// <char*> tributeStr - either "tribute" or "aid".
//
// talk_para1 - amount of the tribute.
//
void TalkMsg::give_tribute(char* tributeStr)
{
	char *article = "un";
	char *verb = "paie";
	char *youVerb = "payez";
	if(tributeStr[0] == 'a')		// aide
	{
		article = "une";
		verb = "offre";
		youVerb = "offrez";
	}

	//---------------------------------------------//
	//
	// Send:
	// <King>'s Kingdom offers you <$999> in aid/tribute.
	// You offer <King>'s Kingdom <$999> in aid/tribute.
	//
	// Reply:
	// <King>'s Kingdom accepts/rejects your aid/tribute of <$999>.
	// You accept/reject the <$999> in aid/tribute from <King>'s Kingdom.
	//
	//---------------------------------------------//
	//
	// Envoyer:
	// Le Royaume de <King> vous offre/paie une/un aide/tribut de <$999>.
	// Vous offrez/payez une/un aide/tribut de <$999> au Royaume de <King>.
	//
	// R�pondre:
	// Le Royaume de <King> accepte/refuse votre aide/tribut de <$999>.
	//
	// Vous acceptez/refusez une/un aide/tribut de <$999> de la part du
	// Royaume de <King>.
	//
	//---------------------------------------------//

	if( reply_type == REPLY_WAITING || !should_disp_reply )
	{
		if( viewing_nation_recno == from_nation_recno )
		{
			str  = "Vous ";
			str += youVerb;
			str += " ";
			str += article;
			str += " ";
			str += tributeStr;
			str += " de ";
			str += m.format(talk_para1, 2);
			str += " au ";
			str += to_nation_name();
			str += ".";
		}
		else
		{
			str  = "Le ";
			str += from_nation_name();
			str += " vous ";
			str += verb;
			str += " ";
			str += article;
			str += " ";
			str += tributeStr;
			str += " de ";
			str += m.format(talk_para1, 2);
			str += ".";
		}
	}
	else
	{
		if( viewing_nation_recno == from_nation_recno )
		{
			str  = "Le ";
			str += to_nation_name();

			if( reply_type == REPLY_ACCEPT )
				str += " accepte votre ";
			else
				str += " refuse votre ";

			str += tributeStr;
			str += " de ";
			str += m.format(talk_para1, 2);
			str += ".";
		}
		else
		{
			if( reply_type == REPLY_ACCEPT )
				str = "Vous acceptez ";
			else
				str = "Vous refusez ";
			str += article;
			str += " ";
			str += tributeStr;
			str += " de ";
			str += m.format(talk_para1, 2);
			str += " de la part du ";
			str += from_nation_name();
			str += ".";
		}
	}
}
//------- End of function TalkMsg::give_tribute ------//


//----- Begin of function TalkMsg::demand_tribute ------//
//
// <int> isAid - 1 if it's a aid, 0 if it's a tribute.
//
// talk_para1 - the amount of the tribute.
//
void TalkMsg::demand_tribute(int isAid)
{
	//---------------------------------------------//
	//
	// Send:
	// <King>'s Kingdom requests/demands <tribute amount> in aid/tribute
	// from you.
	//
	// You request/demand <tribute amount> in aid/tribute from
	// <King>'s Kingdom.
	//
	// Reply:
	// <King>'s Kingdom agrees/refuses to give/pay you <tribute amount>
	// in aid/tribute.
	//
	// You agree/refuse to give/pay <King>'s Kingdom <tribute amount>
	// in aid/tribute.
	//
	//---------------------------------------------//
	//
	// Envoyer:
	// Le Royaume de <King> demande/exige une/un aide/tribut de 
	// <tribute amount> de votre part.
	//
	// Vous demandez/exigez une/un aide/tribut de <tribute amount> de la 
	// part du Royaume de <King>.
	//
	// R�pondre:
	// Le Royaume de <King> accepte/refuse de vous donner/payer une/un
	// aide/tribut de <tribute amount>.
	//
	// Vous acceptez/refusez de donner/payer une/un aide/tribut de 
	// <tribute amount> au Royaume de <King>.
	//
	//---------------------------------------------//

//	char* aidStr;

//	if( isAid )
//		aidStr = "aid";
//	else
//		aidStr = "tribute";

	if( reply_type == REPLY_WAITING || !should_disp_reply )
	{
		if( viewing_nation_recno == from_nation_recno )
		{
			if( isAid )
				str = "Vous demandez une aide de ";
			else
				str = "Vous exigez un tribut de ";
			str += m.format(talk_para1,2);
			str += " de la part du ";
			str += to_nation_name();
			str += ".";
		}
		else
		{
			str  = "Le ";
			str += from_nation_name();

			if( isAid )
				str += " demande une aide de ";
			else
				str += " exige un tribut de ";

			str += m.format(talk_para1,2);
			str += " de votre part.";
		}
	}
	else
	{
		if( viewing_nation_recno == from_nation_recno )
		{
			str  = "Le ";
			str += to_nation_name();

			if( reply_type == REPLY_ACCEPT )
				str += " accepte";
			else
				str += " refuse";

			if( isAid )
				str += " de vous donner une aide de ";
			else
				str += " de vous payer un tribut de ";
			str += m.format(talk_para1,2);
			str += ".";
		}
		else
		{
			if( reply_type == REPLY_ACCEPT )
				str = "Vous acceptez";
			else
				str = "Vous refusez";

			if( isAid )
				str += " de donner une adie de ";
			else
				str += " de payer un tribut de ";
			str += m.format(talk_para1,2);
			str += " au ";
			str += from_nation_name();
			str += ".";
		}
	}
}
//------- End of function TalkMsg::demand_tribute ------//


//----- Begin of function TalkMsg::give_tech ------//
//
// talk_para1 - id. of the tech given.
// talk_para2 - version of the tech.
//
void TalkMsg::give_tech()
{
	//---------------------------------------------//
	//
	// Send:
	// <King>'s Kingdom offers <tech><version> technology to you.
	//
	// You offer <tech><version> technology to <King>'s Kingdom.
	//
	// Reply:
	// <King>'s Kingdom accepts/rejects your gift of <tech><version>
	// technology.
	//
	// You accept/reject the gift of <tech><version> technology
	// from <King>'s Kingdom.
	//
	//---------------------------------------------//
	//
	// Envoyer:
	// Le Royaume de <King> vous offre la technologie <tech><version>.
	//
	// Vous offrez la technologie <tech><version> au Royaume de <King>.
	//
	// R�pondre:
	// Le Royaume de <King> accepte/refuse votre offre de la technologie 
	// <tech><version>.
	//
	// Vous acceptez/refusez l'offre du Royaume de <King> concernant la 
	// technologie <tech><version>.
	//
	//---------------------------------------------//

	if( reply_type == REPLY_WAITING || !should_disp_reply )
	{
		if( viewing_nation_recno == from_nation_recno )
		{
			str  = "Vous offrez la technologie ";
			str += tech_res[talk_para1]->tech_des();

			if( talk_para2 )		// Ships do not have different versions 
			{
				str += " ";
				str += m.roman_number(talk_para2);
			}

			str += " au ";
			str += to_nation_name();
			str += ".";
		}
		else
		{
			str  = "Le ";
			str += from_nation_name();
			str += " vous offre la technologie ";
			str += tech_res[talk_para1]->tech_des();

			if( talk_para2 )		// Ships do not have different versions
			{
				str += " ";
				str += m.roman_number(talk_para2);
			}

			str += ".";
		}
	}
	else
	{
		if( viewing_nation_recno == from_nation_recno )
		{
			str  = "Le ";
			str += to_nation_name();

			if( reply_type == REPLY_ACCEPT )
				str += " accepte";
			else
				str += " refuse";
			str += " votre offre de la technologie ";

			str += tech_res[talk_para1]->tech_des();

			if( talk_para2 )		// Ships do not have different versions
			{
				str += " ";
				str += m.roman_number(talk_para2);
			}

			str += ".";
		}
		else
		{
			if( reply_type == REPLY_ACCEPT )
				str = "Vous acceptez l'offre du ";
			else
				str = "Vous refusez l'offre du ";
			str += from_nation_name();
			str += " concernant la technologie ";
			str += tech_res[talk_para1]->tech_des();

			if( talk_para2 )		// Ships do not have different versions
			{
				str += " ";
				str += m.roman_number(talk_para2);
			}
			str += ".";
		}
	}

}
//------- End of function TalkMsg::give_tech ------//


//----- Begin of function TalkMsg::demand_tech ------//
//
// Demand for the latest version of the technology.
//
// talk_para1 - id. of the tech demanded.
// talk_para2 - version of the tech if the nation agrees to transfer
//					 technology.
//
void TalkMsg::demand_tech()
{
	//---------------------------------------------//
	//
	// Send:
	// <King>'s Kingdom demands/requests the latest
	// <tech> technology from you.
	//
	// You demand/request the latest <tech> technology from
	// <King>'s Kingdom.
	//
	// Reply:
	// <King>'s Kingdom agrees/refuses to transfer its latest <tech>
	// technology to you.
	//
	// You agree/refuse to transfer your <tech> technology to
	// <King>'s Kingdom.
	//
	//---------------------------------------------//
	//
	// Envoyer:
	// Le Royaume de <King> exige/demande votre derni�re version de 
	// <tech>.
	//
	// Vous exigez/demandez sa derni�re version de <tech> au 
	// Royaume de <King>.
	//
	// R�pondre:
	// Le Royaume de <King> accepte/refuse de vous livrer sa derni�re version 
	// de <tech>.
	//
	// Vous acceptez/refusez de livrer votre derni�re version de <tech> au
	// Royaume de <King>.
	//
	//---------------------------------------------//

	char* requestStr;

	if( nation_array[from_nation_recno]->get_relation_status(to_nation_recno)->is_friendly() )
	{
		requestStr = "demande";
	}
	else
	{
		requestStr = "exige";
	}

	//------------------------------------------//

	if( reply_type == REPLY_WAITING || !should_disp_reply )
	{
		if( viewing_nation_recno == from_nation_recno )
		{
			str  = "Vous ";
			str += requestStr;
			str += "z sa derni�re version de ";
			str += tech_res[talk_para1]->tech_des();
			str += " au ";
			str += to_nation_name();
			str += ".";
		}
		else
		{
			str  = "Le ";
			str += from_nation_name();
			str += " ";
			str += requestStr;
			str += " votre derni�re version de ";
			str += tech_res[talk_para1]->tech_des();
			str += ".";
		}
	}
	else
	{
		if( viewing_nation_recno == from_nation_recno )
		{
			str  = "Le ";
			str += to_nation_name();

			if( reply_type == REPLY_ACCEPT )
				str += " accepte";
			else
				str += " refuse";

			str += " de vous livrer sa derni�re version de ";
			str += tech_res[talk_para1]->tech_des();
			str += ".";
		}
		else
		{
			if( reply_type == REPLY_ACCEPT )
				str = "Vous acceptez";
			else
				str = "Vous refusez";

			str += " de livrer votre derni�re version de ";
			str += tech_res[talk_para1]->tech_des();
			str += " au ";
			str += from_nation_name();
			str += ".";
		}
	}
}
//------- End of function TalkMsg::demand_tech ------//


//----- Begin of function TalkMsg::request_military_aid ------//
//
void TalkMsg::request_military_aid()
{
	//---------------------------------------------//
	//
	// Send:
	// <King>'s Kingdom requests immediate military aid from you.
	// You request immediate military aid from <King>'s Kingdom.
	//
	// Reply:
	// <King>'s Kingdom agrees to immediately send your requested
	// military aid.
	// <King>'s Kingdom denies you your requested military aid.
	//
	// You agree to immediately send military aid to <King>'s Kingdom.
	// You refuse to send military aid to <King>'s Kingdom.
	//
	//---------------------------------------------//
	//
	// Envoyer:
	// Le Royaume de <King> vous demande un soutien militaire imm�diat.
	// Vous demandez un soutien militaire imm�diat au Royaume de <King>.
	//
	// R�pondre:
	// Le Royaume de <King> vous apporte un soutien militaire imm�diat.
	// Le Royaume de <King> refuse de vous apporter un soutien militaire
	// imm�diat.
	//
	// Vous apportez un soutien militaire imm�diat au Royaume de <King>.
	// Vous refusez d'apporter un soutien militaire imm�diat au Royaume de 
	// <King>.
	//
	//---------------------------------------------//

	if( reply_type == REPLY_WAITING || !should_disp_reply )
	{
		if( viewing_nation_recno == from_nation_recno )
		{
			str  = "Vous demandez un soutien militaire imm�diat au ";
			str +=  to_nation_name();
			str += ".";
		}
		else
		{
			str = "Le ";
			str += from_nation_name();
			str += " vous demande un soutien militaire imm�diat.";
		}
	}
	else
	{
		if( viewing_nation_recno == from_nation_recno )
		{
			str = "Le ";
			str += to_nation_name();

			if( reply_type == REPLY_ACCEPT )
				str += " vous apporte un soutien militaire imm�diat.";
			else
				str += " refuse de vous apporter un soutien militaire imm�diat.";
		}
		else
		{
			if( reply_type == REPLY_ACCEPT )
				str = "Vous apportez un soutien militaire imm�diat au ";
			else
				str = "Vous refusez d'apporter un soutien militaire imm�diat au ";

			str += from_nation_name();
			str += ".";
		}
	}
}
//------- End of function TalkMsg::request_military_aid ------//


//----- Begin of function TalkMsg::request_trade_embargo ------//
//
// talk_para1 - the nation to have a trade embargo on.
//
void TalkMsg::request_trade_embargo()
{
	//---------------------------------------------//
	//
	// Send:
	// <King>'s Kingdom requests you to join an embargo on trade with
	// <King B>'s Kingdom.
	//
	// You request <King>'s Kingdom to join an embargo on trade with
	// <King B>'s Kingdom.
	//
	// Reply:
	// <King>'s Kingdom agrees/refuses to join an embargo on trade
	// with <King B>'s Kingdom.
	//
	// You agree/refuse to join an embargo on trade with <King B>'s Kingdom
	// as requested by <King>'s Kingdom.
	//
	//---------------------------------------------//
	//
	// Envoyer:
	// Le Royaume de <King> demande que vous preniez part � un embargo 
	// commercial contre le Royaume de <King B>.
	//
	// Vous demandez au Royaume de <King> de prendre part � un embargo
	// commercial contre le Royaume de <King B>.
	//
	// R�pondre:
	// Le Royaume de <King> accepte/refuse de prendre part � un embargo
	// commercial contre le Royaume de <King B>.
	//
	// Vous acceptez/refusez de prendre part � l'embargo commercial contre 
	// le Royaume de <King B>, organis� par le Royaume de <King>.
	//
	//---------------------------------------------//

	if( reply_type == REPLY_WAITING || !should_disp_reply )
	{
		if( viewing_nation_recno == from_nation_recno )
		{
			str  = "Vous demandez au ";
			str += to_nation_name();
			str += " de prendre part";
		}
		else
		{
			str  = "Le ";
			str += from_nation_name();
			str += " demande que vous preniez part";
		}

		str += " � un embargo commercial contre le ";
		if (nation_array.is_deleted(talk_para1))
		{
			str += "the destroyed ";
			str += nation_array.custom_nation_name_array[talk_para1];
		}
		else
		{
			str += nation_array.custom_nation_name_array[talk_para1];
				//nation_array[talk_para1]->nation_name();
			str += nation_color_code_str(talk_para1);
		}
		str += ".";
	}
	else
	{
		if( viewing_nation_recno == from_nation_recno )
		{
			str  = "Le ";
			str += to_nation_name();

			if( reply_type == REPLY_ACCEPT )
				str += " accepte";
			else
				str += " refuse";

			str += "  de prendre part � un embargo commercial contre le ";
			if (nation_array.is_deleted(talk_para1))
			{
				str += "the destroyed ";
				str += nation_array.custom_nation_name_array[talk_para1];
			}
			else
			{
				str += nation_array.custom_nation_name_array[talk_para1];
					//nation_array[talk_para1]->nation_name();
				str += nation_color_code_str(talk_para1);
			}
			str += ".";
		}
		else
		{
			if( reply_type == REPLY_ACCEPT )
				str = "Vous acceptez";
			else
				str = "Vous refusez";

			str += " de prendre part � l'embargo commercial contre le ";
			if (nation_array.is_deleted(talk_para1))
			{
				str += "the destroyed ";
				str += nation_array.custom_nation_name_array[talk_para1];
			}
			else
			{
				str += nation_array.custom_nation_name_array[talk_para1];
					//nation_array[talk_para1]->nation_name();
				str += nation_color_code_str(talk_para1);
			}
			str += ", organis� par le ";
			str += from_nation_name();
			str += ".";
		}
	}
}
//------- End of function TalkMsg::request_trade_embargo ------//


//----- Begin of function TalkMsg::request_surrender ------//
//
void TalkMsg::request_surrender()
{
	//---------------------------------------------//
	//
	// Send:
	//
	// To unite our two Kingdoms under his rule, King
	// <King name> offers <amount> for your throne.
	//
	// You offer <amount> for the throne of <King>'s
	// Kingdom.
	//
	// Reply:
	//
	// King <king name> refuses to dishonor himself by
	// selling his throne!
	//
	// King <king name> agrees to take your money in
	// exchange for his throne.
	//
	// You refuse to dishonor yourself by selling your
	// throne to <King>'s kingdom.
	//
	//---------------------------------------------//
	//
	// Envoyer:
	//
	// Afin d'unifier vos deux Royaumes sous son r�gne, le Roi <King name> 
	// vous offre <amount> pour votre tr�ne.
	//
	// Vous offrez <amount> pour r�gner sur le Royaume de <King>.
	//
	// R�pondre:
	//
	// Le Roi <king name> refuse de se d�shonorer en bradant son tr�ne !
	//
	// Le Roi <king name> accepte votre or en �change de son tr�ne.
	//
	// Vous refusez de vous d�shonorer en bradant votre tr�ne au Royaume de
	// <King>.
	//
	//---------------------------------------------//

	if( reply_type == REPLY_WAITING || !should_disp_reply )
	{
		if( viewing_nation_recno == from_nation_recno )
		{
			str  = "Vous offrez ";
			str += talk_para1*10;	// *10 to restore its original value.
			str += " pour r�gner sur le ";
			str += to_nation_name();
			str += ".";
		}
		else
		{
			str  = "Afin d'unifier vos deux Royaumes sous son r�gne, le Roi ";
			str += from_king_name();
			str += " vous offre ";
			str += talk_para1*10;	// *10 to restore its original value.
			str += " pour votre tr�ne.";
		}
	}
	else
	{
		if( viewing_nation_recno == from_nation_recno )
		{
			str  = "Le Roi ";
			str += to_king_name();

			if( reply_type == REPLY_ACCEPT )
				str += " accepte votre or en �change de son tr�ne.";
			else
				str += " refuse de se d�shonorer en bradant son tr�ne !";
		}
		else
		{
			if( reply_type == REPLY_ACCEPT )
			{
				str = "You agree to take the money in exchange for your throne.";
			}
			else
			{
				str  = "Vous refusez de vous d�shonorer en bradant votre tr�ne au ";
				str += from_nation_name();
				str += ".";
			}
		}
	}
}
//------- End of function TalkMsg::request_surrender ------//


//----- Begin of function TalkMsg::surrender ------//
//
void TalkMsg::surrender()
{
	//---------------------------------------------//
	//
	// Send:
	// <King>'s Kingdom offerrrendered to you.
	// You have surrendered to <King>'s Kingdom.
	//
	//---------------------------------------------//
	//
	// Envoyer:
	// Le Royaume de <King> s'est soumis � vous.
	// Vous abandonnez votre Royaume au Royaume de <King>.
	//
	//---------------------------------------------//


	if( viewing_nation_recno == from_nation_recno )
	{
		str  = "Vous abandonnez votre Royaume au ";
		str += to_nation_name();
		str += ".";
	}
	else
	{
		str  = "Le ";
		str += from_nation_name();
		str += " s'est soumis � vous.";
	}
}
//------- End of function TalkMsg::surrender ------//

#endif
