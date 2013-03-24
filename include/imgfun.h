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

// Filename    : IMGFUN.H
// Description : Declaration of pixel formats

#ifndef __IMGFUN_H
#define __IMGFUN_H

// ----- define enum type PixelFormatFlag16 -------//

enum PixelFormatFlag
{
	PIXFORM_RGB_555 = 0,	// red on low bits
	PIXFORM_RGB_565 = 1,
	PIXFORM_BGR_555 = 2,	// blue on low bits
	PIXFORM_BGR_565 = 3,
};



#endif
