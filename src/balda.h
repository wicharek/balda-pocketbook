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

#ifndef __BALDA_H__
#define __BALDA_H__

#include "balda_config.h"
#include "balda_utils.h"
#include "balda_char.h"

#define BALDA_FIELD_WIDTH 5
#define BALDA_FIELD_HEIGHT 5

#define BALDA_MAX_SEQUENCE 	(BALDA_FIELD_WIDTH*BALDA_FIELD_HEIGHT)

struct balda_t_impl;
typedef struct balda_t_impl balda_t;

typedef struct
{
	balda_point_t pos;
} balda_sequence_entry_t;

typedef struct
{
	balda_sequence_entry_t entries[BALDA_MAX_SEQUENCE];
	int length;
	balda_char insert_char;
	balda_point_t insert_pos;
	balda_char word[BALDA_MAX_SEQUENCE+1];
} balda_sequence_t;

static const int GAME_RESULT_NONE = -1;
static const int GAME_RESULT_DRAW = 2;

typedef enum
{
	BALDA_STATE_NONE,
	BALDA_STATE_PLAYING,
	BALDA_STATE_GAMEOVER
} BALDA_STATE;

typedef enum
{
	BALDA_GAME_TYPE_ONE_PLAYER,
	BALDA_GAME_TYPE_TWO_PLAYERS
} BALDA_GAME_TYPE;

typedef enum
{
	BALDA_GAME_DIFFICULTY_EASY,
	BALDA_GAME_DIFFICULTY_NORMAL,
	BALDA_GAME_DIFFICULTY_HARD
} BALDA_GAME_DIFFICULTY;

typedef enum
{
	BALDA_ADD_LETTER_RESULT_OK,
	BALDA_ADD_LETTER_RESULT_FAIL_NOT_EMPTY_CELL,
	BALDA_ADD_LETTER_RESULT_FAIL_NOT_NEAR
} BALDA_ADD_LETTER_RESULT;

typedef enum
{
	BALDA_SEQUENCE_START_RESULT_OK,
	BALDA_SEQUENCE_START_RESULT_FAIL_EMPTY_CELL
} BALDA_SEQUENCE_START_RESULT;

typedef enum
{
	BALDA_SEQUENCE_NEXT_RESULT_OK,
	BALDA_SEQUENCE_NEXT_RESULT_FAIL_EMPTY_CELL,
	BALDA_SEQUENCE_NEXT_RESULT_FAIL_FIELD_BORDER,
	BALDA_SEQUENCE_NEXT_RESULT_FAIL_ALREADY_SELECTED
} BALDA_SEQUENCE_NEXT_RESULT;

typedef enum
{
	BALDA_TURN_RESULT_OK,
	BALDA_TURN_RESULT_TOO_SHORT,
	BALDA_TURN_RESULT_INSERT_CHAR_NOT_SELECTED,
	BALDA_TURN_RESULT_WORD_ALREADY_USED,
	BALDA_TURN_RESULT_WORD_NOT_FOUND
} BALDA_TURN_RESULT;

typedef enum
{
	BALDA_DIRECTION_UP,
	BALDA_DIRECTION_RIGHT,
	BALDA_DIRECTION_DOWN,
	BALDA_DIRECTION_LEFT
} BALDA_DIRECTION;

#define balda_are_directions_opposite(dir1, dir2) \
	(((dir1) == BALDA_DIRECTION_UP && ((dir2) == BALDA_DIRECTION_DOWN)) || \
	((dir1) == BALDA_DIRECTION_DOWN && ((dir2) == BALDA_DIRECTION_UP)) || \
	((dir1) == BALDA_DIRECTION_LEFT && ((dir2) == BALDA_DIRECTION_RIGHT)) || \
	((dir1) == BALDA_DIRECTION_RIGHT && ((dir2) == BALDA_DIRECTION_LEFT)))

balda_t* balda_init(void);
void balda_free(balda_t*);

BALDA_STATE balda_get_state(balda_t* balda);
BALDA_GAME_TYPE balda_get_game_type(balda_t* balda);
void balda_set_game_difficulty(balda_t* balda, BALDA_GAME_DIFFICULTY game_difficulty);

void balda_new_game(balda_t* balda, BALDA_GAME_TYPE type);
void balda_surrender(balda_t* balda);
balda_bool balda_is_game_over(balda_t* balda);
int balda_get_winner(balda_t* balda);

void balda_set_player_name(balda_t* balda, int player_index, const char* name);
const char* balda_get_player_name(balda_t* balda, int player_index);

int balda_get_score(balda_t* balda, int player_index);
int balda_get_word_list_length(balda_t* balda, int player_index);
const balda_char* balda_get_player_word(balda_t* balda, int player_index, int word_index);
int balda_get_active_player(balda_t* balda);
int balda_get_previous_active_player(balda_t* balda);
balda_bool balda_is_active_player_ai(balda_t* balda);

balda_char balda_get_letter_at(balda_t* balda, int x, int y);
BALDA_ADD_LETTER_RESULT balda_can_add_letter_at(balda_t* balda, int x, int y);

BALDA_SEQUENCE_START_RESULT balda_turn_sequence_start(balda_t* balda,
	balda_point_t start_pos, balda_point_t insert_pos, balda_char insert_char,
	balda_sequence_t* sequence);
BALDA_SEQUENCE_NEXT_RESULT balda_turn_sequence_next(balda_t* balda, balda_sequence_t* sequence,
	BALDA_DIRECTION direction);
BALDA_TURN_RESULT balda_make_turn(balda_t* balda, balda_sequence_t* sequence);

void balda_make_ai_turn(balda_t* balda);
balda_sequence_t* balda_get_last_ai_sequence(balda_t* balda);	


void balda_sequence_reset(balda_sequence_t* sequence);
void balda_sequence_rollback_last(balda_sequence_t* sequence);
balda_point_t balda_sequence_last_selected(balda_sequence_t* sequence);
balda_point_t balda_sequence_prelast_selected(balda_sequence_t* sequence);
const balda_char* balda_sequence_get_word(balda_sequence_t* sequence, balda_t* balda);
balda_bool balda_sequence_contains_point(balda_sequence_t* sequence, int x, int y);

#define balda_sequence_length(sequence) ((sequence)->length)

#endif
