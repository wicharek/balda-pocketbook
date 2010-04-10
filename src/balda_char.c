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

#include "balda_char.h"
#include <stdio.h>
#include <assert.h>

void balda_single_char_to_utf8(balda_char bc, char* utf8_buffer, int utf8_buffer_size)
{
	char* utf8_buffer_e = (utf8_buffer + utf8_buffer_size);
	assert(utf8_buffer);
	
	if (utf8_buffer < utf8_buffer_e)
	{
		*utf8_buffer = '\xD0';
		++utf8_buffer;
	}
	else
	{
		return;
	}
	
	if (utf8_buffer < utf8_buffer_e)
	{
		*utf8_buffer = (bc + 0x90 - 1);
		++utf8_buffer;
	}
	else
	{
		return;
	}
	
	if (utf8_buffer < utf8_buffer_e)
	{
		*utf8_buffer = '\x00';
	}
}

void balda_char_to_utf8(const balda_char* bc, char* utf8_buffer, int utf8_buffer_size)
{
	char* utf8_buffer_e = (utf8_buffer + utf8_buffer_size);
	assert(bc);
	assert(utf8_buffer);
	
	while (*bc != BALDA_CHAR_NONE)
	{
		if (utf8_buffer < utf8_buffer_e)
		{
			*utf8_buffer = '\xD0';
			++utf8_buffer;
		}
		else
		{
			return;
		}
		
		if (utf8_buffer < utf8_buffer_e)
		{
			*utf8_buffer = ((*bc) + 0x90 - 1);
			++utf8_buffer;
			++bc;
		}
		else
		{
			return;
		}
	}
	
	if (utf8_buffer < utf8_buffer_e)
	{
		*utf8_buffer = '\x00';
	}
}

void balda_char_to_utf8_lowercase(const balda_char* bc, char* utf8_buffer, int utf8_buffer_size)
{
	char* utf8_buffer_e = (utf8_buffer + utf8_buffer_size);
	assert(bc);
	assert(utf8_buffer);
	
	while (*bc != BALDA_CHAR_NONE)
	{
		if (utf8_buffer < utf8_buffer_e)
		{
			if ((*bc-1) < 16)
			{
				*utf8_buffer = '\xD0';
				++utf8_buffer;
			}
			else
			{
				*utf8_buffer = '\xD1';
				++utf8_buffer;
			}
		}
		else
		{
			return;
		}
		
		if (utf8_buffer < utf8_buffer_e)
		{
			if ((*bc-1) < 16)
			{
				*utf8_buffer = ((*bc-1) + 0xB0);
				++utf8_buffer;
			}
			else
			{
				*utf8_buffer = ((*bc-17) + 0x80);
				++utf8_buffer;
			}
			++bc;
		}
		else
		{
			return;
		}
	}
	
	if (utf8_buffer < utf8_buffer_e)
	{
		*utf8_buffer = '\x00';
	}
}

void balda_char_from_utf8(const char* utf8, balda_char* bc_buffer, int bc_buffer_size)
{
	balda_char* bc_buffer_e = (bc_buffer + bc_buffer_size);
	
	assert(utf8);
	assert(bc_buffer);
	
	while (*utf8)
	{
		if (*utf8 == '\xD0')
		{
			++utf8;
			
			if (((*utf8) >= '\x90') && ((*utf8) <= '\xAF'))
			{
				*bc_buffer = (*utf8 - 0x90 + 1);
			}
			else if (((*utf8) >= '\xB0') && ((*utf8) <= '\xBF'))
			{
				*bc_buffer = (*utf8 - 0xB0 + 1);
			}
			else
			{
				fprintf(stderr, "Unable to convert utf8 string to balda_char. "
					"Unexpected char 0x%X.\n", (unsigned int)(*utf8) & 0xFF);
				return;
			}
			
			++bc_buffer;
			if (bc_buffer > bc_buffer_e)
				return;
			++utf8;
		}
		else if (*utf8 == '\xD1')
		{
			++utf8;
			
			if ((((unsigned int)(*utf8)) >= 0x80) && (((unsigned int)(*utf8)) <= 0x8F))
			{
				*bc_buffer = (*utf8 - 0x80 + 1 + 16);
			}
			else
			{
				fprintf(stderr, "Unable to convert utf8 string to balda_char. "
					"Unexpected char 0x%X.\n", (unsigned int)(*utf8) & 0xFF);
				return;
			}
			
			++bc_buffer;
			if (bc_buffer > bc_buffer_e)
				return;
			++utf8;
		}
		else
		{
			fprintf(stderr, "Unable to convert utf8 string to balda_char. "
					"Unexpected char 0x%X.\n", (unsigned int)(*utf8) & 0xFF);
			return;
		}
	}
	
	if (bc_buffer < bc_buffer_e)
	{
		*bc_buffer = '\x00';
	}
}

int balda_char_strlen(const balda_char* bc)
{
	int l = 0;
	while (*bc != BALDA_CHAR_NONE)
	{
		++l;
		++bc;
	}
	
	return l;
}

void balda_char_strcpy(const balda_char* source, balda_char* dest)
{
	do
	{
		*dest = *source;
		++source;
		++dest;
	}
	while (*source != BALDA_CHAR_NONE);
	*dest = BALDA_CHAR_NONE;
}

int balda_char_strcmp(const balda_char* bc1, const balda_char* bc2)
{
	while (*bc1 && *bc1 == *bc2)
	{
		++bc1;
		++bc2;
		//printf("balda_char_strcmp: %d = %d\n", *bc1, *bc2);
	}
	
	return (*bc1 - *bc2);
}
