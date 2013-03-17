/*
 * Seven Kingdoms: Ancient Adversaries
 *
 * Copyright 2010 Jesse Allen
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


#ifndef _WIN32_COMPAT_H
#define _WIN32_COMPAT_H

#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

typedef uint8_t  BYTE;
typedef uint8_t  UCHAR;
typedef uint16_t WORD;
typedef int32_t  BOOL;
typedef uint32_t UINT;
typedef uint32_t DWORD;
typedef uint32_t ULONG;
typedef uint32_t WPARAM;

#define TRUE 1
#define FALSE 0

#define MAX_PATH 260

#ifndef WIN32

#ifndef __stdcall
# ifdef __i386__
#  ifdef __GNUC__
#   ifdef __APPLE__ /* Mac OS X uses a 16-byte aligned stack and not a 4-byte one */
#    define __stdcall __attribute__((__stdcall__)) __attribute__((__force_align_arg_pointer__))
#   else
#    define __stdcall __attribute__((__stdcall__))
#   endif
#  elif defined(_MSC_VER)
    /* Nothing needs to be done. __stdcall already exists */
#  else
#   error You need to define __stdcall for your compiler
#  endif
# elif defined(__x86_64__) && defined (__GNUC__)
#  define __stdcall __attribute__((ms_abi))
# else  /* __i386__ */
#  define __stdcall
# endif  /* __i386__ */
#endif /* __stdcall */


inline char *itoa(int num, char *str, int radix)
{
	sprintf(str,"%d",num);
	return str;
}

inline char *ltoa(long num, char *str, int radix)
{
	sprintf(str,"%ld",num);
	return str;
}

#define strcmpi(s1,s2) strcasecmp(s1,s2)
#define strnicmp(s1,s2,len) strncasecmp(s1,s2,len)

#define _rotr(value,shift) (value<<shift)|(value>>(32-shift))

inline char *strupr(char *str)
{
	while (*str)
	{
		*str = toupper(*str);
		str++;
	}
	return str;
}

inline char *strlwr(char *str)
{
	while (*str)
	{
		*str = tolower(*str);
		str++;
	}
	return str;
}

#endif

#ifdef WIN32
#include <io.h>
int mkdir (char *path, int) {
    return mkdir (path);
}

#endif

typedef struct GUID {
  unsigned long  Data1;
  unsigned short Data2;
  unsigned short Data3;
  unsigned char  Data4[ 8 ];
  bool operator==(const GUID &g2) {
    return (memcmp(this, &g2, sizeof(GUID) ) == 0);
  }
} GUID;

#include <platform.h>

#endif // _WIN32_COMPAT_H
