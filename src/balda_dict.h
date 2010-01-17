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

#ifndef __BALDA_DICT_H__
#define __BALDA_DICT_H__

#include "balda_utils.h"
#include "balda_char.h"

typedef unsigned int balda_dict_data_t;

#define BALDA_DICT_HEADER_SINGLE_CHAR_FLAG  0x20000000
#define BALDA_DICT_HEADER_LAST_CHUNK_FLAG   0x20000000
#define BALDA_DICT_HEADER_ENDS_HERE_FLAG    0x40000000
#define BALDA_DICT_HEADER_PLAIN_FLAG        0x80000000
#define BALDA_DICT_HEADER_COUNT_MASK        0x1F000000
#define BALDA_DICT_HEADER_CHAR_MASK         0x1F000000
#define BALDA_DICT_HEADER_NEXT_CHUNK_MASK   0x00FFFFFF
#define BALDA_DICT_ENTRY_ENDS_HERE_FLAG     0x80000000
#define BALDA_DICT_ENTRY_CHAR_MASK          0x1F000000
#define BALDA_DICT_ENTRY_NEXT_CHUNK_MASK    0x00FFFFFF

#define BALDA_DICT_DATA_T_BIT_SIZE          (sizeof(balda_dict_data_t) * 8)
#define BALDA_DICT_CHAR_BIT_SIZE            5
#define BALDA_DICT_CHAR_MASK                0x0000001F


typedef struct
{
	balda_dict_data_t* mem_data;
} balda_dict_t;

typedef struct
{
	balda_dict_t* dict;
	balda_dict_data_t* p;
	balda_dict_data_t* plain_p;
	short plain_off;
	short plain_index;
} balda_dict_chunk_t;

typedef struct
{
	balda_dict_chunk_t next_chunk;
	balda_bool ends_here;
} balda_dict_chunk_entry_t;

void balda_dict_init(balda_dict_t* dict, balda_dict_data_t* data);
void balda_dict_free(balda_dict_t* dict);

balda_bool balda_dict_contains_word(balda_dict_t* dict, const balda_char* word);
balda_bool balda_dict_peek_word_5(balda_dict_t* dict, balda_char* out_word);

void balda_dict_get_root_chunk(balda_dict_t* dict, balda_dict_chunk_t* out_chunk);
balda_bool balda_dict_chunk_get_entry(balda_dict_chunk_t* chunk, balda_char c, balda_dict_chunk_entry_t* out_entry);
balda_char balda_dict_chunk_get_entry_by_index(balda_dict_chunk_t* chunk, int index, balda_dict_chunk_entry_t* out_entry);

#define balda_dict_chunk_is_valid(chunk) ((chunk)->p)

#endif
