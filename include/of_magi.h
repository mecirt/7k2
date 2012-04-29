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

// Filename    : OF_MAGI.H
// Description : header file of magic Tor


#ifndef __OF_MAGI_H
#define __OF_MAGI_H

#include <ofirm.h>

// ------ define constant -------//

#define MAX_CAST_MAGIC_DELAY 100

// ------ define enum for the magic ------//

enum
{
	MAGIC_DESTROY_BUILDING = 1,
	MAGIC_FREEZE_UNIT,
	MAGIC_DAMAGE_AREA,
	MAGIC_TELEPORT,
	MAGIC_FREEZE_LASER,
	MAGIC_GOD_POWER,
	MAGIC_LASER,

	MAGIC_LAST,		// keep it last
	MAGIC_COUNT = MAGIC_LAST - 1
};


enum
{
	FIRM_MAGIC_MENU_MAIN,
	FIRM_MAGIC_MENU_CAST,
	FIRM_MAGIC_MENU_UNIT,
};


struct MagicInfo
{
	int magic_id;
	int tech_id;
	int min_live_point;					// cost if within cast_range
	int min_cost;							// cost if within cast_range
	int cast_delay;
	int effect;
	int cast_range;
	int bullet_speed;
	int cast_radius;
	int live_point_extra_range;		// if cast outside cast_range, cost on min_live_point per distance
	int cost_extra_range;				// if cast outside cast_range, cost on min_live_point per distance
};


class FirmMagic : public Firm
{
public:
	short	cast_delay;
	short cast_delay_max;
	int	casting_magic_id;		// for bullet function

	short	delayed_magic_count;
	short	delayed_magic_id;
	short	delayed_magic_x_loc;
	short	delayed_magic_y_loc;
	short delayed_magic_unit_recno;
	
	static int teleport_unit_recno;
	static MagicInfo magic_info_array[MAGIC_COUNT];
	
	int		animation_offset;
	short		base_firm_build_id;
	int		firm_attack_delay;

	int		magic_id_lower_limit;
	int		magic_id_upper_limit;
	short		delay_again_flag;
	
public:
	FirmMagic();
	~FirmMagic();

	void		init(int xLoc, int yLoc, int nationRecno, int firmId, char* buildCode, short builderRecno);
	void		deinit();
		
	//-------- function on auto attact -------//

	void		process_monster_firm();

	//------- process function --------//

	void		process();
	void		next_day();

	//------- AI functions -------//

	void		process_ai();

	//------- cast function ------//

	int	can_cast(int magicId);
	int	set_teleport_unit(int unitRecno);	// local player function
	int	cast_magic( int magicId, int targetXLoc, int targetYLoc, int targetUnitRecno, char remoteAction );
	int	process_delayed_magic(int magicId, int targetXLoc, int targetYLoc, int targetUnitRecno );
	int	calc_live_point_cost( int magicId, int targetXLoc, int targetYLoc, int targetUnitRecno );
	int	calc_cash_cost( int magicId, int targetXLoc, int targetYLoc, int targetUnitRecno );

	// ------ transfer energy point -------//

	int	transfer_energy( int targetFirmRecno, char remoteAction );
	float	calc_transfer_efficiency( int targetFirmRecno );

	// ------ interface function ------//

	void	put_info(int refreshFlag);
	void	detect_info();
	void	disp_edit_mode(int& refreshFlag, int& dispY1, int dispBG=1);
	int	detect_edit_mode();

	void	right_click(int xLoc, int yLoc);

	// ------ bullet function ------//

	float	bullet_damage();
	short	bullet_radius();
	char	bullet_fire();
	short	bullet_id();
	short	bullet_speed();
	short	bullet_init_z();

	//------- multiplayer checking codes -------//

	virtual	UCHAR crc8();
	virtual	void	clear_ptr();

	// ------- class casting function -------//

	virtual FirmMagic* cast_to_FirmMagic() { return this; }

	// ------- protected interface function ------//

	virtual void	disp_main_menu(int refreshFlag);
	virtual void	detect_main_menu();

	virtual void	disp_magic_menu(int refreshFlag);
	virtual void	detect_magic_menu();

protected:

	//-------- AI actions ---------//

	void		think_cast_magic();
	int		think_del();

	void 		think_destroy_building();
	void 		think_cast_to_area(int magicId);
	void		think_cast_to_top(int magicId);
};

#endif
