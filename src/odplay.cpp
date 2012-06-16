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

// Filename    : ODPLAY.CPP
// Description : MultiPlayerDP, multiplayer class using directPlay
// Onwer       : Gilbert

// STUB, all dplay stuff is non-functional!!!

#ifndef IMAGICMP

#include <odynarrb.h>
#include <odplay.h>
#include <all.h>
#include <string.h>
#include <ovgalock.h>
#include <oblob.h>
#include <omouse.h>
#include <osys.h>
#include <ovga.h>
#include <ot_gmenu.h>

// Define constant

// 7k1 guid
//GUID GAME_GUID = 
// { 0x12f70d44, 0x68be, 0x11d0, { 0xaa, 0xb6, 0x0, 0x0, 0xe9, 0xf9, 0xd, 0x5d } };

// 7k2 guid
#if( !defined(DEBUG))
	GUID GAME_GUID = 
	{ 0x13f71e45, 0x68be, 0x12d1, { 0xaa, 0xb6, 0x0, 0x0, 0xe9, 0xf9, 0x1e, 0x47 } };
#else
	GUID GAME_GUID = 
	{ 0x12f71e45, 0x68be, 0x12d1, { 0xaa, 0xb6, 0x0, 0x0, 0xe9, 0xf9, 0x1e, 0x47 } };
#endif

const DWORD session_desc_flags = 
//	DPSESSION_KEEPALIVE | DPSESSION_NODATAMESSAGES | DPSESSION_MIGRATEHOST
//	| DPSESSION_DIRECTPLAYPROTOCOL;			// new in dx6
0;

// To enable a lobby to launch a DirectPlay application, the application must add 
// the following entries to the system registry. 

//[HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\DirectPlay\Applications\SevenKingdoms]
//"Guid"="{12F70D44-68BE-11D0-AAB6-0000E9F90D5D}"
//"File"="7k.exe"
//"CommandLine"="-!lobby!"
//"Path"="C:\Seven Kingdoms"
//"CurrentDirectory"="C:\Seven Kingdoms"

// note that [HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\DirectPlay\Applications] 
// folder may not exist after installed dx5

HANDLE PLAYER_MESSAGE_HANDLE = NULL;	// ???

int broadcastPID()
{
  return -1; //DWORD_ALLPLAYERS;
}

struct DPSessionDesc
{
  GUID guid;
  int dwFlags;
  int dwMaxPlayers;
  int dwSize;
  char *lpszSessionNameA;
  GUID guidApplication;
  GUID guidInstance;
	char session_name[MP_SESSION_NAME_LEN+1];
	char pass_word[MP_SESSION_NAME_LEN+1];

	DPSessionDesc();
	DPSessionDesc(const DPSessionDesc &);
	DPSessionDesc& operator= (const DPSessionDesc &);
	void after_copy();
	DPSessionDesc *before_use();

	char *name_str() { return session_name; };
	GUID session_id() { return guid; }
};


DPSessionDesc::DPSessionDesc()
{
}

DPSessionDesc::DPSessionDesc(const DPSessionDesc &dpSessionDesc)
{
	after_copy();
}

DPSessionDesc& DPSessionDesc::operator= (const DPSessionDesc &src)
{
	memcpy(this, &src, sizeof( DPSessionDesc ) );
	after_copy();
	return *this;
}

void DPSessionDesc::after_copy()
{
}

DPSessionDesc *DPSessionDesc::before_use()
{
	return this;
}


// Stub classes to get the code below to complete. They do not do anything.
typedef struct tagDPLCONNECTION
{
    DWORD               dwSize;
    DWORD               dwFlags;
    DPSessionDesc *     lpSessionDesc;  /* Ptr to session desc to use for connect */
    char *              lpPlayerName;   /* Ptr to player name structure */
    GUID                guidSP;         /* GUID of Service Provider to use */
    LPVOID              lpAddress;      /* Ptr to Address of Service Provider to use */
    DWORD               dwAddressSize;  /* Size of address data */
} DPLCONNECTION, *LPDPLCONNECTION;

struct DPNAME {
  char *lpszShortNameA;
  char *lpszLongNameA;
  int dwSize;
};

#define DPLCONNECTION_CREATESESSION 1
#define DPLCONNECTION_JOINSESSION 2

#define DPSESSION_JOINDISABLED 1
#define DPSESSION_NEWPLAYERSDISABLED 2

#define DPOPEN_CREATE 1
#define DPOPEN_JOIN 2

#define PLAYER_MESSAGE_HANDLE 1
#define DPSET_GUARANTEED 1
#define DPSET_REMOTE 2
#define DPSET_LOCAL 4
#define DPSEND_GUARANTEED 1
#define DPSEND_OPENSTREAM 2
#define DPSEND_CLOSESTREAM 4
#define DPRECEIVE_ALL 1
#define DWORD_SYSMSG 0xffffffff
struct IDirectPlay4 {
  bool InitializeConnection(char *connection_ptr, int params) {
    return false;
  }
  bool GetSessionDesc( DPSessionDesc *joined_session, unsigned int *bufferSize) {
    return false;
  }
  bool Open(DPSessionDesc *sess, int flags) {
    return false;
  }
  void Close() {}
  void SetSessionDesc( DPSessionDesc *sess, int flags) {}
  bool CreatePlayer(unsigned int*, DPNAME*, int, void*, int, int) {
    return false;
  }
  void DestroyPlayer(unsigned int) {}
  bool SetPlayerData(unsigned int playerId, void *lpData, unsigned int dataSize, int flags) {
    return false;  
  }
  bool GetPlayerData(unsigned int playerId, void *lpData, unsigned int *dataSize, int flags) {
    return false;  
  }
  bool Send(unsigned int my_player_id, unsigned int toId, int, void *data, int dataSize) {
    return false;
  }
  bool GetMessageCount(unsigned int my_player_id, unsigned int *count) {
    return false;
  }
  int Receive(unsigned int *fromId, unsigned int *toId, int flags, void *recv_buffer, unsigned int *dataSize) {
    return false;
  }


};

struct IDirectPlayLobby3 {
  bool GetConnectionSettings(int, void *, unsigned int *size) {
    *size = 0;
    return false;
  }
  bool SetConnectionSettings(int,int, DPLCONNECTION *) {
    return false;
  }
#define IID_IDirectPlay4A 0
  bool ConnectEx( int, int, void**, void*) {
    return false;
  }
  bool SendLobbyMessage(int, int, void *lpData, unsigned int dataSize) {
    return false;
  }
  bool ReceiveLobbyMessage(int,int, unsigned int *msgFlag, void *recv_buffer, unsigned int *dataSize) {
    return false;
  }

};

typedef struct IDirectPlay4 *LPDIRECTPLAY4A;
typedef struct IDirectPlayLobby3 *LPDIRECTPLAYLOBBY3A;




// to start a multiplayer game, first check if it is called from a
// lobbied (MultiPlayerDP::is_lobbied)

// if it is a lobbied, call init_lobbied before create_player

// if not, call poll_service_provider; display them and let
// user to select, call init and pass the guid of the selected
// service; create_session or poll_sessions+join_session;
// finally create_player.

struct MultiPlayerDP::Private {
  DPSessionDesc joined_session;
  LPDIRECTPLAY4A direct_play4;
  LPDIRECTPLAYLOBBY3A direct_play_lobby;
  DPLCONNECTION *connection_string;         // only when lobbied

  void deleteDP4() {
    if (!direct_play4) return;
    VgaFrontLock vgaLock;
    // direct_play4->Release();
    delete direct_play4;
    direct_play4 = NULL;
  }
};

// ------- begin of function MultiPlayerDP::MultiPlayerDP -------//
MultiPlayerDP::MultiPlayerDP() : service_providers(sizeof(DPServiceProvider), 10 ),
	current_sessions(sizeof(DPSessionDesc), 10 ), player_pool(sizeof(DPPlayer), 8 ),
	recv_buffer(new char[MP_RECV_BUFFER_SIZE])
{
  d = new Private;

	init_flag = 0;
	co_inited = 0;
	d->direct_play_lobby = NULL;
	d->direct_play4 = NULL;
	recv_buffer_size = MP_RECV_BUFFER_SIZE;
	host_flag = 0;
	lobbied_flag = 0;
	d->connection_string = NULL;
}
// ------- end of function MultiPlayerDP::MultiPlayerDP -------//


// ------- begin of function MultiPlayerDP::~MultiPlayerDP -------//
MultiPlayerDP::~MultiPlayerDP()
{
	deinit();

	// ----- clear outstanding co_inited ---------//

	while( co_inited > 0 )	
	{
		// CoUninitialize();
		--co_inited;
	}

	delete[] recv_buffer;
  delete d;
}
// ------- end of function MultiPlayerDP::~MultiPlayerDP -------//


// ------- begin of function MultiPlayerDP::pre_init -------//
void MultiPlayerDP::pre_init()
{
	// nothing, for compatibilities with MultiPlayerIM
	lobbied_flag = 0;

	// CoInitialize(NULL);
	++co_inited;

  d->direct_play4 = new IDirectPlay4;  // STUB CODE
//	CoCreateInstance( CLSID_DirectPlay, NULL, CLSCTX_INPROC_SERVER, 
//		IID_IDirectPlay4A, (LPVOID*)&d->direct_play4);
	// no need to call direct_play4->Initialize
}
// ------- begin of function MultiPlayerDP::pre_init -------//

// ------- begin of function MultiPlayerDP::init -------//
void MultiPlayerDP::init(GUID serviceProviderGUID)
{
	VgaFrontLock vlock;
	DPServiceProvider *sp = NULL;
	int i;
	for( i = 1; i <= service_providers.size(); ++i )
	{
		sp = (DPServiceProvider *) service_providers.get(i);
		if( memcmp(&sp->guid, &serviceProviderGUID, sizeof(GUID) ) == 0 )
			break;
	}

	if( !sp || i > service_providers.size() )
		return;

	if (!d->direct_play4->InitializeConnection( sp->connection_ptr, 0 ))
		return;

	init_flag = 1;
	host_flag = 0;
	my_player_id = broadcastPID();		// no player
}
// ------- end of function MultiPlayerDP::init -------//

void MultiPlayerDP::init( DPServiceProvider *provider)
{
  init (provider->guid);
}

// ------- begin of function MultiPlayerDP::deinit -------//
void MultiPlayerDP::deinit()
{
	host_flag = 0;
	lobbied_flag = 0;
	if(init_flag)
	{
		// ######## patch begin Gilbert 24/11 ######//
		if( my_player_id != broadcastPID())
		{
			destroy_player( my_player_id );
			sys.sleep(2000);		// 2 seconds
		}
		// ######## patch end Gilbert 24/11 ######//

		// ######## patch begin Gilbert 24/11 ######//
		my_player_id = broadcastPID();		// mark player deleted
		// ######## patch end Gilbert 24/11 ######//
		init_flag = 0;
	}

	if( d->direct_play_lobby )
	{
		VgaFrontLock vgaLock;
		delete d->direct_play_lobby;
		d->direct_play_lobby = NULL;
	}
	d->deleteDP4();

	if( co_inited > 0 )
	{
		// CoUninitialize();
		--co_inited;
	}

	if( d->connection_string )
	{
		mem_del(d->connection_string);
		d->connection_string = NULL;
	}

}
// ------- end of function MultiPlayerDP::deinit -------//


// ----- begin of function MultiPlayerDP::init_lobbied ------//
void MultiPlayerDP::init_lobbied(int maxPlayers, char *)
{
	VgaFrontLock vlock;

	// ------- create DirectPlayLobby, if necessary ------//
	if( d->direct_play_lobby == NULL)
	{
          d->direct_play_lobby = new IDirectPlayLobby3;
		/*hr = CoCreateInstance( CLSID_DirectPlayLobby, NULL, CLSCTX_INPROC_SERVER, 
			IID_IDirectPlayLobby3A, (LPVOID*)&d->direct_play_lobby);
		if( hr != DP_OK )
			return;*/
	}

	// ------ get connection setting ---------//
	DWORD bufferSize = 0;
	if (d->direct_play_lobby->GetConnectionSettings(0, NULL, &bufferSize))
	{
		bufferSize += 0x80;		// give them a little more
		d->connection_string = (DPLCONNECTION *)mem_resize(d->connection_string, bufferSize);
		memset( d->connection_string, 0, sizeof(DPLCONNECTION) );
		d->connection_string->dwSize = sizeof(DPLCONNECTION);
		if (d->direct_play_lobby->GetConnectionSettings(0, d->connection_string, &bufferSize))
		{
			d->connection_string->lpSessionDesc->dwFlags = session_desc_flags;
			d->connection_string->lpSessionDesc->dwMaxPlayers = maxPlayers;
			if (!d->direct_play_lobby->SetConnectionSettings(0,0, d->connection_string))
				return;

			d->joined_session = *d->connection_string->lpSessionDesc;

			// drop the current d->direct_play4
			d->deleteDP4();

			// ------ obtain direct play interface --------//
			if (!d->direct_play_lobby->ConnectEx( 0, IID_IDirectPlay4A, (void**)&d->direct_play4, NULL))
			{
				err.msg( text_game_menu.str_lobby_error_not_found() );
				return;
			}

			init_flag = 1;
			if( d->connection_string->dwFlags & DPLCONNECTION_CREATESESSION )
				lobbied_flag = 1;		// auto create
			else if( d->connection_string->dwFlags & DPLCONNECTION_JOINSESSION )
				lobbied_flag = 2;		// auto join
			else
				lobbied_flag = 4;		// user selectable

			bufferSize = sizeof(DPSessionDesc);
			d->direct_play4->GetSessionDesc( &d->joined_session, &bufferSize);
			d->joined_session.after_copy();			
		}
		else
		{
			err.run( text_game_menu.str_lobby_error_no_connect_str() ); // "Cannot get connection string from lobby");
		}
	}
	else
	{
		err.run( text_game_menu.str_lobby_error_no_connect_str() ); // "Cannot get connection string from lobby");
	}
}
// ----- end of function MultiPlayerDP::init_lobbied ------//



// ----- begin of function MultiPlayerDP::is_lobbied -----//
// return 0=not lobbied, 1=auto create, 2=auto join, 4=selectable
int MultiPlayerDP::is_lobbied()
{
	return lobbied_flag;
}
// ----- end of function MultiPlayerDP::is_lobbied -----//


// ----- begin of function MultiPlayerDP::get_lobbied_name -----//
char *MultiPlayerDP::get_lobbied_name()
{
	err_when(!is_lobbied());

	// ------ get connection setting ---------//
	if( d->connection_string )
		return d->connection_string->lpPlayerName;
	else
		return NULL;
}
// ----- end of function MultiPlayerDP::get_lobbied_name -----//


// ----- begin of function MultiPlayerDP::poll_service_providers -----//
//
// store all possible service provider (TCPIP, IPX, modem, null modem ... )
// into service_provider_array
//
// need not call init before get_service_provider
//

/*
static bool enumConnectionsCallBack( LPCGUID lpguidSP,
	LPVOID lpConnection,  DWORD dwConnectionSize,
	LPCDPNAME lpName,  DWORD dwFlags,  LPVOID lpContext  )
{
	DPServiceProvider sp;
	MultiPlayerDP *mpdpPtr = (MultiPlayerDP *)lpContext;

	sp.guid = *lpguidSP;

	// ----- set valuable for the InitConnection -------//

	if( dwConnectionSize <= sizeof(sp.connection_ptr) )
		memcpy( sp.connection_ptr, lpConnection, dwConnectionSize);
	else
		return TRUE;		// skip this protocol

	// --------- set service provider name --------//

	if( lpName->lpszShortNameA )
	{
		strncpy(sp.description, lpName->lpszShortNameA, MP_SERVICE_PROVIDER_NAME_LEN);
		sp.description[MP_SERVICE_PROVIDER_NAME_LEN] = '\0';
	}
	else
	{
		sp.description[0] = '\0';
	}
	if( lpName->lpszLongNameA )
	{
		strncpy(sp.long_description, lpName->lpszLongNameA, MP_SERVICE_PROVIDER_NAME_LEN);
		sp.long_description[MP_SERVICE_PROVIDER_NAME_LEN] = '\0';
	}
	else
	{
		sp.long_description[0] = '\0';

		// recognize by GUID
		const char *longDescPtr = NULL;
		if( memcmp( lpguidSP, &DPSPGUID_IPX, sizeof(GUID) ) == 0 )
			longDescPtr = text_game_menu.str_mp_service_provider_long(1);
		else if( memcmp( lpguidSP, &DPSPGUID_TCPIP, sizeof(GUID) ) == 0 )
			longDescPtr = text_game_menu.str_mp_service_provider_long(2);
		else if( memcmp( lpguidSP, &DPSPGUID_MODEM, sizeof(GUID) ) == 0 )
			longDescPtr = text_game_menu.str_mp_service_provider_long(3);
		else if( memcmp( lpguidSP, &DPSPGUID_SERIAL, sizeof(GUID) ) == 0 )
			longDescPtr = text_game_menu.str_mp_service_provider_long(4);

		if( longDescPtr )
		{
			strncpy(sp.long_description, longDescPtr, MP_SERVICE_PROVIDER_NAME_LEN);
			sp.long_description[MP_SERVICE_PROVIDER_NAME_LEN] = '\0';
		}

	}

	// -------- add to the service provider list -------//

	mpdpPtr->service_providers.linkin(&sp);
	return TRUE;
}
*/

void MultiPlayerDP::poll_service_providers()
{
	service_providers.zap();

	// ------ allocate spaces for service_provider_array -------//
	VgaFrontLock vgaLock;
	err_when( !d->direct_play4 );

//	d->direct_play4->EnumConnections( &GAME_GUID, enumConnectionsCallBack, this, 
//		DPCONNECTION_DIRECTPLAY ); //| DPCONNECTION_DIRECTPLAYLOBBY );
}
// ----- end of function MultiPlayerDP::poll_service_providers -----//


// ----- begin of function MultiPlayerDP::get_service_provder -----//
// return a service provider
//
// <int> i			i-th service provider (i start from 1)
// return pointer to a DPServiceProvider, NULL if no more
DPServiceProvider* MultiPlayerDP::get_service_provider(int i)
{
	if( i <= 0 || i > service_providers.size() )
		return NULL;
	return (DPServiceProvider *) service_providers.get(i);
}
// ----- end of function MultiPlayerDP::get_service_provder -----//



// ----- begin of function MultiPlayerDP::poll_sessions ------//
//
// store all available sessions (TCPIP, IPX, modem, null modem ... )
// into current_sessions
//
/*
static bool EnumSessionsCallback( DPSessionDesc *lpSessionDesc,
	DWORD *timeOut, DWORD flags, LPVOID mpPtr)
{
	if( flags & DPESC_TIMEDOUT )
		return FALSE;

	if( memcmp(&lpSessionDesc->guidApplication, &GAME_GUID, sizeof(GUID)) == 0 )
	{
		MultiPlayerDP *mpdpPtr = (MultiPlayerDP *)mpPtr;
		DPSessionDesc sessionDesc(*lpSessionDesc);
		mpdpPtr->current_sessions.linkin(&sessionDesc);
	}
	return TRUE;
}
*/
int MultiPlayerDP::poll_sessions()
{
	err_when(!init_flag);
	current_sessions.zap();
	DPSessionDesc sessionDesc;
	memset(&sessionDesc, 0, sizeof( sessionDesc ) );
	sessionDesc.dwSize = sizeof( sessionDesc );
	sessionDesc.guidApplication = GAME_GUID;

	MouseDispCount showMouse;
//	return DP_OK == d->direct_play4->EnumSessions(&sessionDesc , 0, EnumSessionsCallback, this, 
//		DPENUMSESSIONS_AVAILABLE | DPENUMSESSIONS_ASYNC);
  return 0;
}
// ----- end of function MultiPlayerDP::poll_sessions ------//


// ----- begin of function MultiPlayerDP::get_session ------//
// return a session description
//
// <int> i			i-th session (i start from 1)
// return pointer to a session, NULL if no more
DPSessionDesc *MultiPlayerDP::get_session(int i)
{
	if( i <= 0 || i > current_sessions.size() )
		return NULL;
	return ((DPSessionDesc *) current_sessions.get(i))->before_use();
}
// ----- end of function MultiPlayerDP::get_session ------//

GUID MultiPlayerDP::get_session_id(int i)
{
  DPSessionDesc *sess = get_session(i);
  return sess ? sess->session_id() : GUID();
}

char *MultiPlayerDP::get_session_name(int i)
{
  DPSessionDesc *sess = get_session(i);
  return sess ? sess->name_str() : 0;
}


// ----- begin of function MultiPlayerDP::create_session ----//
//
// create a new session
//
// <char *> sessionName      arbitary name to identify a session, input from user
// <int>    maxPlayers       maximum no. of players in a session
//
// return TRUE if success
int MultiPlayerDP::create_session(char *sessionName, int maxPlayers)
{
	if(!init_flag || maxPlayers < 1)
		return FALSE;

	if( is_lobbied() == 1 )
	{
		host_flag = 1;
		return TRUE;
	}

	memset(&d->joined_session, 0, sizeof( d->joined_session) );
	d->joined_session.dwSize = sizeof( DPSessionDesc );
	// DPSESSION_NODATAMESSAGES disable unital data
	// remove DPSESSION_MIGRATEHOST 
	d->joined_session.dwFlags = session_desc_flags;
	d->joined_session.guidApplication = GAME_GUID;
	d->joined_session.dwMaxPlayers = maxPlayers;
	strncpy(d->joined_session.session_name, sessionName, MP_SESSION_NAME_LEN );
	d->joined_session.session_name[MP_SESSION_NAME_LEN]= '\0';
	d->joined_session.lpszSessionNameA = d->joined_session.session_name;

	MouseDispCount showMouse;
	if( !d->direct_play4->Open(&d->joined_session, DPOPEN_CREATE) )
	{
		host_flag = 1;
		return TRUE;
	}
	return FALSE;

}
// ----- end of function MultiPlayerDP::create_session ----//


// ------ begin of function MultiPlayerDP::join_session ------//
// join a session, by passing the DPSessionDesc pointer
//
// <DPSessionDesc *>  pointer to a DPSessionDesc
//
// return TRUE if success
int MultiPlayerDP::join_session(DPSessionDesc* sessionDesc)
{
	if( !init_flag)
		return FALSE;

	if( is_lobbied() == 2 )
	{
		host_flag = 0;
		return TRUE;
	}

	d->joined_session = *sessionDesc;
	VgaFrontLock vgaLock;
	if(!d->direct_play4->Open(&d->joined_session, DPOPEN_JOIN))
	{
		host_flag = 0;
		return TRUE;
	}
	return FALSE;
}


// join a session, by passing the index passed into get_session()
// note : do not call poll_sessions between get_session and join_session
//
// <int> currentSessionIndex       the index passed into get_session()
//
// currentSessionIndex start from 1
int MultiPlayerDP::join_session(int currentSessionIndex)
{
	if( !init_flag)
		return FALSE;

	if( is_lobbied() == 2 )
	{
		host_flag = 0;
		return TRUE;
	}

	VgaFrontLock vgaLock;
	d->joined_session = *get_session(currentSessionIndex);
	if(!d->direct_play4->Open(&d->joined_session, DPOPEN_JOIN))
	{
		host_flag = 0;
		return TRUE;
	}
	return FALSE;
}
// ------ end of function MultiPlayerDP::join_session ------//

// ------ begin of function MultiPlayerDP::close_session ------//
void MultiPlayerDP::close_session()
{
	VgaFrontLock vgaLock;
	if( init_flag)
		d->direct_play4->Close();
	host_flag = 0;
}
// ------ end of function MultiPlayerDP::close_session ------//

// ------ begin of function MultiPlayerDP::disable_join_session ------//
void MultiPlayerDP::disable_join_session()
{
	// called by host only!

	err_when( !host_flag );
	if( init_flag && host_flag )
	{
		d->joined_session.dwFlags |= DPSESSION_JOINDISABLED | DPSESSION_NEWPLAYERSDISABLED;
		VgaFrontLock vgaLock;
		d->direct_play4->SetSessionDesc( &d->joined_session, 0 );
	}
}
// ------ end of function MultiPlayerDP::disable_join_session ------//

// ------ begin of function MultiPlayerDP::create_player ------//
// create a local player
//
// <char *> friendlyName          short name of the player, best to be one word only
// [char *] formalName            long name of the player, take friendlyName if NULL (default: NULL)
// [void *] lpData, [DWORD] dataSize    pointer and size of any data sent the remote (default: NULL, 0)
// [DWORD] flags                  not use reserved (default:0)
//
// return TRUE if success
//
int MultiPlayerDP::create_player(char *friendlyName, char *formalName, 
	LPVOID lpData, DWORD dataSize, DWORD flags)
{
	if(!init_flag)
		return FALSE;

	DPNAME dpName;
	memset(&dpName, 0, sizeof(dpName) );
	dpName.dwSize = sizeof(dpName);
	dpName.lpszShortNameA = friendlyName;
	dpName.lpszLongNameA = formalName ? formalName : friendlyName;
	VgaFrontLock vgaLock;
	return !d->direct_play4->CreatePlayer(&my_player_id, &dpName, PLAYER_MESSAGE_HANDLE,
		lpData, dataSize, flags);
}
// ------ end of function MultiPlayerDP::create_player -----//


// ------ begin of function MultiPlayerDP::destroy_player ------//
// destroy player, (for remove from joining a session, before playing)
//
void MultiPlayerDP::destroy_player( DWORD playerId )
{
	VgaFrontLock vgaLock;
	d->direct_play4->DestroyPlayer(playerId);
}
// ------ end of function MultiPlayerDP::destroy_player ------//


// -------- begin of function MultiPlayerDP::poll_players ------//
// collect all players in the session into player_pool
// get each player by calling get_player
//
/*
static BOOL FAR PASCAL EnumPlayerCallback(DWORD dpId, DWORD dwPlayerType,
	LPCDPNAME lpName, DWORD dwFlags, LPVOID mpPtr)
{
	MultiPlayerDP *mpdpPtr = (MultiPlayerDP *)mpPtr;
	DPPlayer dpPlayer;
	dpPlayer.player_id = dpId;
	strncpy(dpPlayer.friendly_name, lpName->lpszShortNameA, MP_FRIENDLY_NAME_LEN );
	dpPlayer.friendly_name[MP_FRIENDLY_NAME_LEN] = '\0';
	strncpy(dpPlayer.formal_name, lpName->lpszLongNameA, MP_FORMAL_NAME_LEN );
	dpPlayer.formal_name[MP_FORMAL_NAME_LEN] = '\0';
	dpPlayer.connecting = 1;

	mpdpPtr->player_pool.linkin(&dpPlayer);
	return TRUE;
}
*/
void MultiPlayerDP::poll_players()
{
	player_pool.zap();
	VgaFrontLock vgaLock;
//	if(init_flag)
//		d->direct_play4->EnumPlayers(NULL, EnumPlayerCallback, this, 0 );
}
// -------- end of function MultiPlayerDP::poll_players ------//


// -------- begin of function MultiPlayerDP::get_player -----//
//
// return the i-th player in the player_pool
//
DPPlayer *MultiPlayerDP::get_player(int i)
{
	if( i <= 0 || i > player_pool.size() )
		return NULL;
	return (DPPlayer *)player_pool.get(i);
}
// -------- end of function MultiPlayerDP::get_player -----//


// -------- begin of function MultiPlayerDP::search_player -----//
//
// search player by playerID
//
DPPlayer *MultiPlayerDP::search_player(DWORD playerId)
{
	DPPlayer *player;
	int i = 0;
	while( (player = get_player(++i)) != NULL )
		if( player->player_id == playerId )
			return player;
	return NULL;
}

//
// search player by formal name, case insensitive
//
DPPlayer *MultiPlayerDP::search_player(char *name)
{
	DPPlayer *player;
	int i = 0;
	while( (player = get_player(++i)) != NULL )
		if( strncasecmp(player->formal_name, name, MP_FORMAL_NAME_LEN)== 0)
			return player;
	return NULL;
}
// -------- end of function MultiPlayerDP::get_player -----//


// ------- begin of function MultiPlayerDP::is_host --------//
int MultiPlayerDP::is_host(DWORD playerID)
{
	err_here();		// not supported
	return 0;
}
// ------- end of function MultiPlayerDP::is_host --------//


// ------- begin of function MultiPlayerDP::am_I_host --------//
int MultiPlayerDP::am_I_host()
{
	return host_flag;
}
// ------- end of function MultiPlayerDP::am_I_host --------//


// ----- begin of function MultiPlayerDP::is_player_connecting ----//
//
// determine whether a player is lost
//
// MultiPlayerDP::received must be called (or remote.poll_msg) , 
// so if a player is really lost, the system message from 
// directPlay is received
//
int MultiPlayerDP::is_player_connecting(DWORD playerId)
{
	for( int p = 1; p <= player_pool.size(); ++p)
	{
		DPPlayer *dpPlayer = (DPPlayer *) player_pool.get(p);
		if( dpPlayer->player_id == playerId )
		{
			return dpPlayer->connecting;
		}
	}
	return 0;
}
// ----- end of function MultiPlayerDP::is_player_connecting ----//


// ----- begin of function MultiPlayerDP::update_public_data ----//
// update a player's public data
//
// return TRUE on success
//
int MultiPlayerDP::update_public_data(DWORD playerId, LPVOID lpData, DWORD dataSize)
{
	VgaFrontLock vgaLock;
	return !d->direct_play4->SetPlayerData(playerId, lpData, dataSize, DPSET_REMOTE | DPSET_GUARANTEED);
}
// ----- end of function MultiPlayerDP::update_public_data ----//


// ----- begin of function MultiPlayerDP::retrieve_public_data ----//
// retrieve a player's public data
// prepare an allocated memory and pass its address as lpData,
// store its size in a DWORD, pass the pointer of the DWORD as lpDataSize
//
// if lpData is NULL, the function can get the size of the data
//
// return TRUE on success, *lpDataSize is updated to the size of the data
//
int MultiPlayerDP::retrieve_public_data(DWORD playerId, LPVOID lpData, DWORD *lpDataSize)
{
	VgaFrontLock vgaLock;
	return !d->direct_play4->GetPlayerData(playerId, lpData, lpDataSize, DPSET_REMOTE | DPSET_GUARANTEED);
}
// ----- end of function MultiPlayerDP::retrieve_public_data ----//


// ----- begin of function MultiPlayerDP::update_private_data ----//
// update a player's private data
//
// return TRUE on success
//
int MultiPlayerDP::update_private_data(DWORD playerId, LPVOID lpData, DWORD dataSize)
{
	VgaFrontLock vgaLock;
	return !d->direct_play4->SetPlayerData(playerId, lpData, dataSize, DPSET_LOCAL | DPSET_GUARANTEED);
}
// ----- end of function MultiPlayerDP::update_private_data ----//


// ----- begin of function MultiPlayerDP::retrieve_private_data ----//
// retrieve a player's private data
// prepare an allocated memory and pass its address as lpData,
// store its size in a DWORD, pass the pointer of the DWORD as lpDataSize
//
// if lpData is NULL, the function can get the size of the data
//
// return TRUE on success, *lpDataSize is updated to the size of the data
//
int MultiPlayerDP::retrieve_private_data(DWORD playerId, LPVOID lpData, DWORD *lpDataSize)
{
	VgaFrontLock vgaLock;
	return !d->direct_play4->GetPlayerData(playerId, lpData, lpDataSize, DPSET_LOCAL | DPSET_GUARANTEED);
}
// ----- end of function MultiPlayerDP::retrieve_private_data ----//

void MultiPlayerDP::playerLost (DWORD id)
{
  for( int p = 1; p <= player_pool.size(); ++p)
  {
    DPPlayer *dpPlayer = (DPPlayer *)player_pool.get(p);
    if( dpPlayer->player_id == id )
    {
      dpPlayer->connecting = 0;
    }
  }

}

// --------- begin of function MultiPlayerDP::send ---------//
// send message
//
// must not call it between IDirectDrawSurface2::Lock and IDirectDrawSurface2::Unlock,
// or between IDirectDrawSurface2::GetDC and IDirectDrawSurface2::ReleaseDC
// pass broadcastPID() as toId to all players
//
// return TRUE on success
//
int MultiPlayerDP::send(DWORD toId, LPVOID lpData, DWORD dataSize)
{
	err_when(!init_flag);
	VgaFrontLock vgaLock;
	return d->direct_play4->Send(my_player_id, toId, 0, lpData, dataSize);
}
// --------- end of function MultiPlayerDP::send ---------//


// ------- begin of function MultiPlayerDP::begin_stream -----//
// signal start of a lot of guaranteed messages being sent to this player
//
// note : call end_stream to finish begin_stream
//
void MultiPlayerDP::begin_stream(DWORD toId)
{
	err_when(!init_flag);
	VgaFrontLock vgaLock;
	d->direct_play4->Send(my_player_id, toId, DPSEND_GUARANTEED | DPSEND_OPENSTREAM, NULL,0);
}
// ------- end of function MultiPlayerDP::begin_stream -----//


// --------- begin of function MultiPlayerDP::send_stream ---------//
// send message
//
// must not call it between IDirectDrawSurface2::Lock and IDirectDrawSurface2::Unlock,
// or between IDirectDrawSurface2::GetDC and IDirectDrawSurface2::ReleaseDC
// pass broadcastPID() as toId to all players
//
// return TRUE on success
//
int MultiPlayerDP::send_stream(DWORD toId, LPVOID lpData, DWORD dataSize)
{
	err_when(!init_flag);
	VgaFrontLock vgaLock;
	return d->direct_play4->Send(my_player_id, toId, DPSEND_GUARANTEED, lpData, dataSize);
}
// --------- end of function MultiPlayerDP::send_stream ---------//


// ------- begin of function MultiPlayerDP::end_stream -----//
// signal end of a lot of guaranteed messages being sent to this player
//
void MultiPlayerDP::end_stream(DWORD toId)
{
	err_when(!init_flag);
	VgaFrontLock vgaLock;
	d->direct_play4->Send(my_player_id, toId, DPSEND_GUARANTEED | DPSEND_CLOSESTREAM, NULL,0);
}
// ------- end of function MultiPlayerDP::end_stream -----//


// ------- begin of function MultiPlayerDP::get_msg_count ------//
//
// get the number of outstanding message to receive
//
// return -1 if fail
//
int MultiPlayerDP::get_msg_count()
{
	err_when(!init_flag);
	DWORD count = 0;
	VgaFrontLock vgaLock;
	if(d->direct_play4->GetMessageCount(my_player_id, &count))
		return -1;
	return (int)count;
}
// ------- end of function MultiPlayerDP::get_msg_count ------//


// ------- begin of function MultiPlayerDP::receive ------//
// return NULL if fails
// sysMsgCount records how many system messages have been handled
// notice : *sysMsgCount may be != 0, but return NULL
//
char *MultiPlayerDP::receive(DWORD * from, DWORD * to, DWORD *dSize, int *sysMsgCount)
{
	err_when(!init_flag);
	DWORD fromId, toId;
	DWORD dataSize;
	int retryFlag;
	int hr;

	VgaFrontLock vgaLock;
	if( sysMsgCount )
		*sysMsgCount = 0;
	do
	{
		retryFlag = 0;
		dataSize = recv_buffer_size;
		hr=d->direct_play4->Receive(&fromId, &toId, DPRECEIVE_ALL, recv_buffer, &dataSize);
		switch(hr)
		{
		case 0:
			if(fromId == DWORD_SYSMSG)
			{
				handle_system_msg(recv_buffer, dataSize);
				if( sysMsgCount )
					(*sysMsgCount)++;
				retryFlag = 1;
			}
			else
			{
				*from = fromId;
				*to = toId;
				*dSize = dataSize;
			}
			break;
		default:
			return NULL;
		}
		
	} while (retryFlag);
	return recv_buffer;
}
// ------- end of function MultiPlayerDP::receive ------//


// ------- begin of function MultiPlayerDP::handle_system_msg ------//
void MultiPlayerDP::handle_system_msg(LPVOID lpData, DWORD dSize)
{
  /*
	switch( ((DPMSG_GENERIC *)lpData)->dwType )
	{
	case DPSYS_DESTROYPLAYERORGROUP:
		{
			DPMSG_DESTROYPLAYERORGROUP *dpmsg = (DPMSG_DESTROYPLAYERORGROUP *)lpData;
			if( dpmsg->dwPlayerType == DPPLAYERTYPE_PLAYER)
				playerLost(dpmsg->dpId);
		}
		break;
	case DPSYS_SETSESSIONDESC:
		{
			DPMSG_SETSESSIONDESC *dpmsg = (DPMSG_SETSESSIONDESC *)lpData;
			d->joined_session = dpmsg->dpDesc;
		}
		break;
	}
  */
}
// ------- end of function MultiPlayerDP::handle_system_msg ------//


// --------- begin of function MultiPlayerDP::send_lobby ---------//
// send message
//
// must not call it between IDirectDrawSurface2::Lock and IDirectDrawSurface2::Unlock,
// or between IDirectDrawSurface2::GetDC and IDirectDrawSurface2::ReleaseDC
//
// return TRUE on success
//
int MultiPlayerDP::send_lobby(LPVOID lpData, DWORD dataSize)
{
	err_when(!init_flag);
	VgaFrontLock vgaLock;
	return !d->direct_play_lobby->SendLobbyMessage(0, 0, lpData, dataSize);
}
// --------- end of function MultiPlayerDP::send_lobby ---------//


// ------- begin of function MultiPlayerDP::receive_lobby ------//
// return NULL if fails
char *MultiPlayerDP::receive_lobby(DWORD *dSize)
{
	err_when(!init_flag);
	DWORD dataSize, msgFlag;
	int retryFlag;
	int hr;

	VgaFrontLock vgaLock;
	do
	{
		retryFlag = 0;
		dataSize = recv_buffer_size;
		hr=d->direct_play_lobby->ReceiveLobbyMessage(0,0, &msgFlag, recv_buffer, &dataSize);
		switch(hr)
		{
		case 0:
			*dSize = dataSize;
			break;
		default:
			return NULL;
		}
		
	} while (retryFlag);
	return recv_buffer;
}
// ------- end of function MultiPlayerDP::receive_lobby ------//


// ------- begin of function MultiPlayerDP::handle_lobby_system_msg ------//
void MultiPlayerDP::handle_lobby_system_msg(LPVOID lpData, DWORD dSize)
{
}
// ------- end of function MultiPlayerDP::handle_lobby_system_msg ------//


// ------ Begin of function MultiPlayerDP::sort_sessions -------//

static int sort_session_id(const void *a, const void *b)
{
	return memcmp( &((DPSessionDesc *)a)->guidInstance, &((DPSessionDesc *)b)->guidInstance,
		sizeof(GUID) );
}


static int sort_session_name(const void *a, const void *b)
{
	return strcmp( ((DPSessionDesc *)a)->name_str(), ((DPSessionDesc *)b)->name_str() );
}

// sort current_sessions
// <int> sortType, 1=sort by GUID, 2=sort by session name
void MultiPlayerDP::sort_sessions(int sortType )
{
	// BUGHERE : quick_sort is a DynArray function but current_sessions is DynArrayB
	switch(sortType)
	{
	case 1:
		current_sessions.quick_sort(sort_session_id);
		break;
	case 2:
		current_sessions.quick_sort(sort_session_name);
		break;
	default:
		err_here();
	}

	for( int s = 1; get_session(s); ++s)
	{
		get_session(s)->before_use();		// let lpszSessionNameA point to its own session_name
	}
}
// ------ End of function MultiPlayerDP::sort_sessions -------//


#endif
