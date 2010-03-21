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

#ifndef __BALDA_CHAR_H__
#define __BALDA_CHAR_H__

typedef unsigned char balda_char;
static const balda_char BALDA_CHAR_NONE=0;
#define BALDA_CHAR_NONE_IS_0
#define BALDA_CHAR_MAX	32

// Converts single balda char to utf-8 string
void balda_single_char_to_utf8(balda_char bc, char* utf8_buffer, int utf8_buffer_size);
// Converts balda char string to utf-8 string
void balda_char_to_utf8(const balda_char* bc, char* utf8_buffer, int utf8_buffer_size);
// Converts balda char string to utf-8 string lowercase
void balda_char_to_utf8_lowercase(const balda_char* bc, char* utf8_buffer, int utf8_buffer_size);
// Converts utf-8 char string to balda char string
void balda_char_from_utf8(const char* utf8, balda_char* bc_buffer, int bc_buffer_size);

int balda_char_strlen(const balda_char* bc);
int balda_char_strcmp(const balda_char* bc1, const balda_char* bc2);
void balda_char_strcpy(const balda_char* source, balda_char* dest);

#endif
