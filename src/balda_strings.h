/***************************************************************************
 *   This file is a part of Balda game for PocketBook.                     *
 *   Copyright 2010 by Vitaliy Ivanov <wicharek@w2f2.com>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA .        *
 ***************************************************************************/

#ifndef __BALDA_STRINGS__H__
#define __BALDA_STRINGS__H__

typedef enum
{
	#define BALDA_STRING(name, value) name,
	#include "balda_strings.inl"
	#undef BALDA_STRING
} BALDA_STRING_ID;

const char* balda_string(BALDA_STRING_ID);

#define BALDA_STRING_DEFINE_MACROS
#define BALDA_STRING(name, value)
#include "balda_strings.inl"
#undef BALDA_STRING_DEFINE_MACROS
#undef BALDA_STRING

#endif
