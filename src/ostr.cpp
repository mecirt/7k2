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

//Filename    : OSTR.CPP
//Description : Object String

#include <omisc.h>
#include <ctype.h>
#include <ostr.h>


//------- Define static variable --------//

static char work_buf[MAX_STR_LEN+1];

//-------- Begin of constructor/destructor functions --------//

String::String()
{
   str_buf[0] = NULL;
}

String::String( const char *s )
{
   strncpy(str_buf, s, MAX_STR_LEN );
   str_buf[MAX_STR_LEN] = NULL;
}

String::String( String& s )
{
   memcpy(str_buf, s.str_buf, MAX_STR_LEN+1 );
}

//---------- End of constructor/destructor functions --------//


//------- Begin of function String::substr --------//
//
// <int> pos = the start position of the sub string you want
//             start from 0, 0-aligned
// [int] len = the length of the sub string
//             (default : till the end of the string
//
char* String::substr(int pos, int len)
{
  work_buf[0] = NULL;

  int strLen = strlen(str_buf);

  if(pos >= strLen)                     // check for boundary errors
     return work_buf;

  if( len <= 0 )
     len = strLen - pos;
  else
  {
     if(len > strLen - pos)
		  // ##### begin Gilbert 22/7 ########//
        // return work_buf;
		  return str_buf + pos;
		  // ##### end Gilbert 22/7 ########//
  }

  strncpy( work_buf, str_buf+pos, len );
  work_buf[len] = NULL;

  return work_buf;
}

//------- End of function String::substr ------------//


//------- Begin of function String::upper/lower -------//

char* String::upper()
{
   memcpy( work_buf, str_buf, len()+1 );
   for (int i = 0; i < strlen(work_buf); ++i)
     work_buf[i] = toupper(work_buf[i]);

   return work_buf;
}

char* String::lower(void)
{
   memcpy( work_buf, str_buf, len()+1 );
   for (int i = 0; i < strlen(work_buf); ++i)
     work_buf[i] = tolower(work_buf[i]);

   return work_buf;
}

//--------- End of function String::upper/lower --------//


//-------- Begin of function String::at ---------------//
//
// <char*> searchStr = the string you want to search for
//
// return : >= 0 the position of the search str in the string
//          = -1 if not found
//
int String::at(char* searchStr)
{
  int pos;
  char *tmp;

  if( (tmp = strstr(str_buf, searchStr)) != NULL)
    pos = (int)(tmp-str_buf);
  else
    pos = -1;

  return pos;
}
//---------- End of function String::at ---------------//


//-------- Begin of operator= functions -----------//

String& String::operator=(String& s)
{
   memcpy(str_buf, s.str_buf, MAX_STR_LEN+1 );

   return *this;
}

String& String::operator=(const char *s)
{
   strncpy(str_buf, s, MAX_STR_LEN );
   str_buf[MAX_STR_LEN] = NULL;

   return *this;
}

String& String::operator=(long value)
{
   strncpy(str_buf, m.format(value), MAX_STR_LEN );
   str_buf[MAX_STR_LEN] = NULL;

   return *this;
}
//---------- End of operator= functions -----------//


//-------- Begin of operator+= functions -----------//

String& String::operator+=(String& s)
{
	strncat( str_buf, s.str_buf, MAX_STR_LEN );
	str_buf[MAX_STR_LEN] = NULL;
	return *this;
}

String& String::operator+=(const char *s)
{
	strncat( str_buf, s, MAX_STR_LEN );
	str_buf[MAX_STR_LEN] = NULL;
   return *this;
}

String& String::operator+=(long value)
{
   strncat( str_buf, m.format(value), MAX_STR_LEN );
   str_buf[MAX_STR_LEN] = NULL;
   return *this;
}

//---------- End of operator+= functions -----------//


//-------- Begin of operator*= functions -----------//

String& String::operator*=(int n)
{
  memcpy( work_buf, str_buf, len()+1 );

  for(int i=1; i<n; i++)
     strncat(str_buf, work_buf, MAX_STR_LEN);

  str_buf[MAX_STR_LEN] = NULL;

  return *this;
}

//---------- End of operator*= functions -----------//


//-------- Begin of function String::add_char --------//

String& String::add_char(char inChar)
{
	char charBuf[2] = "A";

	charBuf[0] = inChar;

	strncat( str_buf, charBuf, MAX_STR_LEN );
	str_buf[MAX_STR_LEN] = NULL;
	return *this;
}
//---------- End of function String::add_char -------//


