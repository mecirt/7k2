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

//Filename   : OC_EAS13.CPP
//Description: CampaignEastWest - STAGE_JOINT_FIGHT_BACK

#include <all.h>
#include <osite.h>
#include <otech.h>
#include <ogame.h>
#include <otechres.h>
#include <ostate.h>
#include <oraceres.h>
#include <onews.h>
#include <omonsres.h>
#include <onation2.h>
#include <oc_east.h>
#include <of_camp.h>

//----- define constant ----------//

enum { STAGE_13_EASTERN_NATION_RECNO = 2,
		 STAGE_13_MONSTER_NATION_RECNO = 3  };

//---- Begin of function CampaignEastWest::stage_13_create_game ----//
//
// Vars defined in event functions before calling this function:
//
// -cur_monster_nation_recno    - the target monster nation recno
// -attacker_state_recno	     - the player attacking state
// -attacker2_state_recno		  - the eastern empire attacking state
// -target_state_recno			  - the monster target state
//
int CampaignEastWest::stage_13_create_game()
{
	init_random_plot('A');

	plot_enemy_nation_recno = STAGE_13_MONSTER_NATION_RECNO;

	//---- create game objects and vars ----//

	if( !stage_13_create_nation() )
		return 0;

	//------- init relationship -------//

	stage_13_init_relation();		// must init relationship first so that allied units are visible on the map

	//-------- create lairs ---------//

	if( !stage_13_create_lair() )
		return 0;

	//---- create the player force ----//

	create_troop(PLAYER_NATION_RECNO, 55-campaign_difficulty*6 );

	//---- create the eastern empire force ----//

	create_troop(STAGE_13_EASTERN_NATION_RECNO, 55-campaign_difficulty*6 );

	//------ init stage 1 vars -------//

	stage_13_init_vars();

	//---- generate plants and other objects on the map -----//

	world.gen_rocks();		// generate mountains

	world.generate_map2();

	//------- create additional raw sites ----------//

	create_raw_site();

	// ----- create royal units ------//

	create_royal_units(CAMPAIGN_PLAYER_NATION_RECNO);

	//------ plot create game --------//

	(this->*plot_create_game_FP)();

	return 1;
}
//---- End of function CampaignEastWest::stage_13_create_game -----//


///---- Begin of function CampaignEastWest::stage_13_init_vars ----//

void CampaignEastWest::stage_13_init_vars()
{
	//----- set player nation vars -----//

	init_player_resource(40);

	init_nation_resource(STAGE_13_EASTERN_NATION_RECNO, 40);

	init_nation_resource(STAGE_13_MONSTER_NATION_RECNO, 50);

	//-------- add victory conditions -------//

	game.add_win_condition( E_DEFEAT_KINGDOM, 1, STAGE_13_MONSTER_NATION_RECNO );  	// the objective is to destroy the enemy kingdom
}
//---- End of function CampaignEastWest::stage_13_init_vars -----//


//---- Begin of function CampaignEastWest::stage_13_prelude ----//
//
// Pick a state to attack.
//
void CampaignEastWest::stage_13_prelude()
{
	play_speech("Pre-13.wav");
	disp_narrative( res_stage.read("13PRELUD") );
	stop_speech();

	attack_state( attacker_state_recno, target_state_recno, 0, 1, 2 );

	//------- set config settings -------//

	config.explore_whole_map = 0;
}
//---- End of function CampaignEastWest::stage_13_prelude -----//


//---- Begin of function CampaignEastWest::stage_13_create_nation ----//

int CampaignEastWest::stage_13_create_nation()
{
	//------- create player nation --------//

	state_array[attacker_state_recno]->add_game_nation();

	//------- create the Eastern nation --------//

	state_array[attacker2_state_recno]->add_game_nation();

	//-------- create the monster nation ------//

	state_array[target_state_recno]->add_game_nation();

	return 1;
}
//---- End of function CampaignEastWest::stage_13_create_nation ----//


//---- Begin of function CampaignEastWest::stage_13_create_lair ----//

int CampaignEastWest::stage_13_create_lair()
{
	int lairAddCount = 6 + m.random(3);						// add 6 to 7 Fryhtan Lairs
	int independentTownAddCount=0;
	int specialFirmCount=0;

	if( m.random(2)==0 )		// 50% chance having independent towns
		independentTownAddCount = 6 + m.random(3);		// 6 to 8 independent towns

	if( m.random(2)==0 )
		specialFirmCount = m.random(5);

	create_lair(cur_monster_nation_recno, STAGE_13_MONSTER_NATION_RECNO, lairAddCount, independentTownAddCount, specialFirmCount);

	return 1;
}
//---- End of function CampaignEastWest::stage_13_create_lair ----//


//------- Begin of function CampaignEastWest::stage_13_init_relation -------//
//
void CampaignEastWest::stage_13_init_relation()
{
	set_nation_hostile(PLAYER_NATION_RECNO, STAGE_13_MONSTER_NATION_RECNO);
	set_nation_allied(PLAYER_NATION_RECNO , STAGE_13_EASTERN_NATION_RECNO);
}
//------- End of function CampaignEastWest::stage_13_init_relation --------//


//=================================================================//


//------- Begin of function CampaignEastWest::stage_13_next_day -------//
//
void CampaignEastWest::stage_13_next_day()
{
	//------- call plot next day --------//

	(this->*plot_next_day_FP)();
}
//------- End of function CampaignEastWest::stage_13_next_day --------//


//============================================================//


//--- Begin of function CampaignEastWest::stage_13_process_game_result ---//
//
void CampaignEastWest::stage_13_process_game_result()
{
	//--- if the player won the game -----//

	if( game.result_win_condition_id >= 0 )
	{
		attack_state( attacker_state_recno, target_state_recno, 1, 2, 3, attacker2_state_recno );

		set_event( EVENT_RANDOM_POINT_2 );
	}

	//---- if the player lost the game ----//

	else
	{
		// must retry

		info.game_year = game_year;			// revert info.game_year back to campaign's game year so that the game's time will not advance

		m.set_random_seed( saved_random_seed );		// restore the random seed so that the game will replay in the same way
	}
}
//---- End of function CampaignEastWest::stage_13_process_game_result -----//


//------ Begin of function CampaignEastWest::stage_13_disp_end_game_msg ------//
//
void CampaignEastWest::stage_13_disp_end_game_msg()
{
	if( game.result_win_condition_id >= 0 )
	{
		disp_in_game_msg( res_stage.read("13WIN") );
	}
	else
	{
		disp_in_game_msg( res_stage.read("13LOSE") );
	}
}
//------- End of function CampaignEastWest::stage_13_disp_end_game_msg -------//

