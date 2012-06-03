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

//Filename    : OREMOTE.CPP
//Description : Object Remote

#include <win32_compat.h>

#include <all.h>
#include <obox.h>
#include <ovga.h>
#include <ostr.h>
#include <ofont.h>
#include <osys.h>
#include <oworld.h>
#include <ogame.h>
#include <opower.h>
#include <onation.h>
#include <oremote.h>
#include <odplay.h>
#include <oerrctrl.h>

//---------- Define static variables ----------//

static int	connection_failed = 0;		// connect_game_disconnect_handler() will set connection_failed to 0

//---------- Define static functions ----------//

static void connect_game_disconnect_handler(DWORD playerId);

//--------- Begin of function Remote::Remote ----------//

Remote::Remote()
{
	//--------------------------------------------//

	packet_send_count    = 0;
	packet_receive_count = 0;

	//--------------------------------------------//

	common_msg_buf    = mem_add( COMMON_MSG_BUF_SIZE );

	mp_ptr				= NULL;
	connectivity_mode = 0;
	//	handle_vga_lock   = 1;
	handle_vga_lock   = 0;			// lock vga front in MulitPlayerDP
	process_queue_flag = 0;
	nation_processing = 0;
	// ###### patch begin Gilbert 22/1 #######//
	sync_test_level = 0;			// 0=disable, bit0= random seed, bit1=crc, bit7=error encountered
	// ###### patch end Gilbert 22/1 #######//
}
//--------- End of function Remote::Remote ----------//


//--------- Begin of function Remote::~Remote ----------//

Remote::~Remote()
{
	deinit();

	mem_del( common_msg_buf  );
}
//--------- End of function Remote::~Remote ----------//


//--------- Begin of function Remote::init ----------//
//
// <int> connectivityMode - connectivity mode can be:
//									 REMOTE_WSOCK_TCPIP, REMOTE_WSOCK_IPX,
//									 REMOTE_MODEM, REMOTE_NULL_MODEM
//
void Remote::init(MultiPlayerType *mp)
{
	if( connectivity_mode )
		deinit();

	connectivity_mode = 1;
	poll_msg_flag = 0;
	mp_ptr = mp;

	// ###### patch begin Gilbert 22/1 #######//
	sync_test_level = (m.is_file_exist("SYNC1.SYS") ? 1 : 0)
		| (m.is_file_exist("SYNC2.SYS") ? 2 : 0);
		// 0=disable, bit0= random seed, bit1=crc
	// ###### patch end Gilbert 22/1 #######//

	reset_process_frame_delay();

	set_alternating_send(1);		// send every frame
}
//--------- End of function Remote::init ----------//


//--------- Begin of function Remote::deinit ----------//

void Remote::deinit()
{
	if( connectivity_mode )
	{
		connectivity_mode = 0;
	}
	// ###### patch begin Gilbert 22/1 #######//
	sync_test_level = 0;			// 0=disable, bit0= random seed, bit1=crc
	// ###### patch end Gilbert 22/1 #######//
}
//--------- End of function Remote::deinit ----------//


//--------- Begin of function Remote::create_game ---------//
//
int Remote::create_game()
{
	//--------- initialize session parameters ---------//

	is_host = TRUE;

	return 1;
}
//--------- End of function Remote::create_game ---------//


//--------- Begin of function Remote::connect_game ---------//
//
int Remote::connect_game()
{
	is_host = FALSE;
	return 1;
}
//--------- End of function Remote::connect_game ---------//


//-------- Begin of function Remote::is_enable ---------//
//
int Remote::is_enable()
{
	return connectivity_mode;
}
//--------- End of function Remote::is_enable ----------//

/*
//-------- Begin of function Remote::can_start_game ---------//
//
int Remote::can_start_game()
{
	err_when(!connectivity_mode);

	return wsock_ptr->can_start_game();
}
//--------- End of function Remote::can_start_game ----------//
*/

//-------- Begin of function Remote::number_of_opponent ---------//
//
int Remote::number_of_opponent()
{
	err_when(!connectivity_mode);

	//return wsock_ptr->number_of_player;
	return mp_ptr->player_pool.size()-1;
}
//--------- End of function Remote::number_of_opponent ----------//


//-------- Begin of function Remote::self_player_id ---------//
//
DWORD Remote::self_player_id()
{
	err_when(!connectivity_mode);

	// return wsock_ptr->self_player_id;
	return mp_ptr->my_player_id;
}
//--------- End of function Remote::self_player_id ----------//

/*
//-------- Begin of function Remote::set_disconnect_handler ---------//
//
void Remote::set_disconnect_handler(DisconnectFP disconnectFP)
{
	err_when(!connectivity_mode);

	wsock_ptr->set_disconnect_handler(disconnectFP);
}
//--------- End of function Remote::set_disconnect_handler ----------//
*/


//-------- Begin of function Remote::start_game ---------//
//
void Remote::start_game()
{
	err_when(!connectivity_mode);

	// wsock_ptr->start_game();
}
//--------- End of function Remote::start_game ----------//


//----- Begin of function connect_game_disconnect_handler -----//
//
// Disconnection handler for Remote::connect_game()
//
static void connect_game_disconnect_handler(DWORD playerId)
{
	connection_failed = 1;
}
//------ End of function connect_game_disconnect_handler ------//



//-------- Begin of function Remote::reset_process_frame_delay ---------//
void Remote::reset_process_frame_delay()
{
	process_frame_delay = 1;
	err_when( process_frame_delay > MAX_PROCESS_FRAME_DELAY );
}
//-------- End of function Remote::reset_process_frame_delay ---------//


//-------- Begin of function Remote::get_process_frame_delay ---------//
int Remote::get_process_frame_delay()
{
	return process_frame_delay;
}
//-------- End of function Remote::get_process_frame_delay ---------//


//-------- Begin of function Remote::set_process_frame_delay ---------//
void Remote::set_process_frame_delay(int f)
{
	// must not be called after init_receive_queue()

	if( f < 1 )
		f = 1;
	if( f > MAX_PROCESS_FRAME_DELAY )
		f = MAX_PROCESS_FRAME_DELAY;
	process_frame_delay = f;
}
//-------- End of function Remote::set_process_frame_delay ---------//


//-------- Begin of function Remote::calc_process_frame_delay ---------//
int Remote::calc_process_frame_delay(int milliSecond)
{
	int f = 1 + (milliSecond+20)/100;
	if( f < 1 )
		f = 1;
	if( f > MAX_PROCESS_FRAME_DELAY )
		f = MAX_PROCESS_FRAME_DELAY;
	return f;
}
//-------- End of function Remote::calc_process_frame_delay ---------//


//-------- Begin of function Remote::set_alternating_send ---------//
void Remote::set_alternating_send(int rate)
{
	if( rate <= 0 || rate > 100 || (sync_test_level != 0 && rate != 1) )
	{
		err_here();
		// must not have any sync testing to enable alternating send
		// tell_random_seed and crc checking must send data every frame
	}
	else
		alternating_send_rate = rate;
}
//-------- End of function Remote::set_alternating_send ---------//


//-------- Begin of function Remote::set_alternating_send ---------//
int Remote::get_alternating_send()
{
	return alternating_send_rate;
}
//-------- End of function Remote::set_alternating_send ---------//


//-------- Begin of function Remote::has_send_frame ---------//
int Remote::has_send_frame(int nationRecno, DWORD frameCount)
{
	return ((frameCount + nationRecno) % alternating_send_rate) == 0;
}
//-------- End of function Remote::has_send_frame ---------//


//-------- Begin of function Remote::next_send_frame ---------//
// if has_send_frame is true, return frameCount
// otherwise return the next frame which has_send_frame return true
DWORD Remote::next_send_frame(int nationRecno, DWORD frameCount)
{
	DWORD remainder = ((frameCount + nationRecno) % alternating_send_rate);
	if(remainder == 0)
		return frameCount;
	else
		return frameCount + alternating_send_rate - remainder;
}
//-------- End of function Remote::next_send_frame ---------//


// --------- begin of function Remote::is_lobbied -------//
// 0 - not lobbied, 1 - create game, 2 - join game
int Remote::is_lobbied()
{
	if( mp_obj.init_flag )
		return mp_obj.is_lobbied();
	else
		return 0;
}
// --------- end of function Remote::is_lobbied -------//
