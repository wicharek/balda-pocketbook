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

#include "balda_strings.h"

#define BALDA_STRING(name, value) const char* name##_ru = value;
#include "balda_strings.inl"
#undef BALDA_STRING

const char* balda_empty_string = "";

const char* balda_string(BALDA_STRING_ID id)
{
	switch (id)
	{
		#define BALDA_STRING(name, value) \
			case name: \
			return name##_ru;
		#include "balda_strings.inl"
		#undef BALDA_STRING
	
		default:
			return balda_empty_string;
	}
}
