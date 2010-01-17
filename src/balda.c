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

#include <stdlib.h>
#include <assert.h>
#include <time.h>

// for text encoding functions
#include <inkview.h>

#include "balda.h"
#include "balda_dict.h"

#define BALDA_PLAYER_NAME_BUFFER_SIZE 128
#define BALDA_MAX_SEQUENCE 	(BALDA_FIELD_WIDTH*BALDA_FIELD_HEIGHT)
#define BALDA_MAX_WORDS 	10

extern balda_dict_data_t balda_dict_data[];

typedef struct
{
	balda_char letter;
} balda_field_cell_t;

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

typedef struct
{
	balda_char words[BALDA_MAX_WORDS][BALDA_MAX_SEQUENCE+1];
	int length;
} word_list_t;

struct balda_t_impl
{
	BALDA_STATE state;
	BALDA_GAME_TYPE game_type;
	
	balda_field_cell_t field[BALDA_FIELD_WIDTH][BALDA_FIELD_HEIGHT];
	char player_names[2][BALDA_PLAYER_NAME_BUFFER_SIZE];
	int player_score[2];
	word_list_t player_word_lists[2];
	
	int active_player;
	
	balda_sequence_t sequence;
	balda_dict_t dict;
	int winner;
	int turns_left;
};

balda_t* balda_init(void)
{
	balda_t* balda = (balda_t*)malloc(sizeof(balda_t));
	
	balda->state = BALDA_STATE_NONE;
	
	// reset player names
	balda->player_names[0][0] = 0;
	balda->player_names[1][0] = 0;
	
	balda_dict_init(&balda->dict, balda_dict_data);
	
	srand(time(0));
	
	return balda;
}

void balda_free(balda_t* balda)
{
	assert(balda);
	
	balda_dict_free(&balda->dict);
	free(balda);
}

BALDA_SEQUENCE_START_RESULT balda_sequence_start(balda_t* balda, balda_point_t start_pos, balda_point_t insert_pos, balda_char insert_char)
{
	assert(insert_char != BALDA_CHAR_NONE);
	assert(start_pos.x >= 0 && start_pos.x < BALDA_FIELD_WIDTH
		&& start_pos.y >= 0 && start_pos.y < BALDA_FIELD_HEIGHT);
	
	printf("[balda_sequence_start] start_pos: { %d, %d }, insert_pos: { %d, %d }\n",
		start_pos.x, start_pos.y, insert_pos.x, insert_pos.y);
	
	if (balda->field[start_pos.x][start_pos.y].letter == BALDA_CHAR_NONE
		&& !balda_points_equal(insert_pos, start_pos))
	{
		return BALDA_SEQUENCE_START_RESULT_FAIL_EMPTY_CELL;
	}
	
	balda->sequence.length = 1;
	balda->sequence.entries[0].pos = start_pos;
	balda->sequence.insert_char = insert_char;
	balda->sequence.insert_pos = insert_pos;
	
	return BALDA_SEQUENCE_START_RESULT_OK;
}

void balda_sequence_reset(balda_t* balda)
{
	balda->sequence.length = 0;
}

BALDA_SEQUENCE_NEXT_RESULT balda_sequence_next(balda_t* balda, BALDA_DIRECTION direction)
{
	int i;
	balda_point_t next_pos;
	
	printf("[balda_sequence_next] direction: %d\n", direction);
	
	assert(balda->sequence.length > 0);
	
	next_pos = balda->sequence.entries[balda->sequence.length-1].pos;
	
	switch (direction)
	{
		case BALDA_DIRECTION_UP:
			--next_pos.y;
		break;
		
		case BALDA_DIRECTION_DOWN:
			++next_pos.y;
		break;
		
		case BALDA_DIRECTION_LEFT:
			--next_pos.x;
		break;
		
		case BALDA_DIRECTION_RIGHT:
			++next_pos.x;
		break;
	}
	
	// Check if over the border
	if (next_pos.x < 0 || next_pos.y < 0 || next_pos.x >= BALDA_FIELD_WIDTH || next_pos.y >= BALDA_FIELD_HEIGHT)
	{
		printf(" BALDA_SEQUENCE_NEXT_RESULT_FAIL_FIELD_BORDER\n");
		return BALDA_SEQUENCE_NEXT_RESULT_FAIL_FIELD_BORDER;
	}
	
	// Check if cell not empty
	if (balda->field[next_pos.x][next_pos.y].letter == BALDA_CHAR_NONE
		&& !balda_points_equal(balda->sequence.insert_pos, next_pos))
	{
		printf(" BALDA_SEQUENCE_NEXT_RESULT_FAIL_EMPTY_CELL\n");
		return BALDA_SEQUENCE_NEXT_RESULT_FAIL_EMPTY_CELL;
	}
	
	// Check if cell was not already selected
	for (i=0; i<balda->sequence.length; ++i)
	{
		if (balda_points_equal(next_pos, balda->sequence.entries[i].pos))
		{
			printf(" BALDA_SEQUENCE_NEXT_RESULT_FAIL_ALREADY_SELECTED\n");
			return BALDA_SEQUENCE_NEXT_RESULT_FAIL_ALREADY_SELECTED;
		}
	}
	
	// Everything ok. Add to sequence.
	balda->sequence.entries[balda->sequence.length].pos = next_pos;
	++balda->sequence.length;
	
	return BALDA_SEQUENCE_NEXT_RESULT_OK;
}

const balda_char* balda_sequence_get_word(balda_t* balda)
{
	int i;
	for (i=0; i<balda->sequence.length; ++i)
	{
		if (balda_points_equal(balda->sequence.entries[i].pos, balda->sequence.insert_pos))
			balda->sequence.word[i] = balda->sequence.insert_char;
		else
			balda->sequence.word[i] = balda->field[balda->sequence.entries[i].pos.x][balda->sequence.entries[i].pos.y].letter;
	}
	balda->sequence.word[i] = BALDA_CHAR_NONE;

	return balda->sequence.word;
}

void balda_sequence_rollback_last(balda_t* balda)
{
	printf("balda_sequence_rollback_last\n");
	
	if (balda->sequence.length > 0)
	{
		--balda->sequence.length;
	}
}

int balda_sequence_length(balda_t* balda)
{
	return balda->sequence.length;
}

balda_point_t balda_sequence_last_selected(balda_t* balda)
{
	if (balda->sequence.length > 0)
	{
		return balda->sequence.entries[balda->sequence.length-1].pos;
	}
	
	return balda_make_point(-1, -1);
}

balda_point_t balda_sequence_prelast_selected(balda_t* balda)
{
	if (balda->sequence.length > 1)
	{
		return balda->sequence.entries[balda->sequence.length-2].pos;
	}
	
	return balda_make_point(-1, -1);
}

void balda_new_game(balda_t* balda, BALDA_GAME_TYPE type)
{
	int i;
	balda_char initial_word[6];
	
	balda->state = BALDA_STATE_PLAYING;
	balda->game_type = type;
	balda->turns_left = 20;
	
	balda->player_score[0] = 0;
	balda->player_score[1] = 0;
	
	balda->player_word_lists[0].length = 0;
	balda->player_word_lists[1].length = 0;
	
	balda->active_player = rand() % 2;
	
	// peek initial word
	balda_bool peeked = balda_dict_peek_word_5(&balda->dict, initial_word);
	assert(peeked);
	//balda_char_from_utf8("БАЛДА", initial_word, sizeof(initial_word) / sizeof(balda_char));
	
	#ifdef BALDA_CHAR_NONE_IS_0
		memset(balda->field, 0, sizeof(balda->field));
	#else
		int j;
		for (i=0; i<BALDA_FIELD_WIDTH; ++i)
		{
			for (j=0; j<BALDA_FIELD_HEIGHT; ++j)
			{
				balda->field[i][j].letter = BALDA_CHAR_NONE;
			}
		}
	#endif
	
	for (i=0; i<BALDA_FIELD_WIDTH; ++i)
	{
		balda->field[i][2].letter = initial_word[i];
		//balda->field[i][3].letter = initial_word[i];
	}
	//balda->field[2][3].letter = 0;
	
	balda_sequence_reset(balda);
}

void balda_surrender(balda_t* balda)
{
	balda->state = BALDA_STATE_GAMEOVER;
	balda->winner = (balda->active_player + 1) % 2;
}

balda_bool balda_is_game_over(balda_t* balda)
{
	return balda->state == BALDA_STATE_GAMEOVER;
}

int balda_get_winner(balda_t* balda)
{
	if (balda->state == BALDA_STATE_GAMEOVER)
		return balda->winner;
	return GAME_RESULT_NONE;
}

int balda_get_active_player(balda_t* balda)
{
	return balda->active_player;
}

int balda_get_previous_active_player(balda_t* balda)
{
	return (balda->active_player + 1) % 2;
}

balda_bool balda_is_active_player_ai(balda_t* balda)
{
	return balda->state == BALDA_STATE_PLAYING && balda->game_type == BALDA_GAME_TYPE_ONE_PLAYER &&
		balda->active_player == 1; // AI player has always index 1
}

balda_char balda_get_letter_at(balda_t* balda, int x, int y)
{
	return balda->field[x][y].letter;
}

BALDA_ADD_LETTER_RESULT balda_can_add_letter_at(balda_t* balda, int x, int y)
{
	if (balda->field[x][y].letter != BALDA_CHAR_NONE)
		return BALDA_ADD_LETTER_RESULT_FAIL_NOT_EMPTY_CELL;
	
	if (!((x > 0 && balda->field[x-1][y].letter != BALDA_CHAR_NONE) ||
		(y > 0 && balda->field[x][y-1].letter != BALDA_CHAR_NONE) ||
		(x < BALDA_FIELD_WIDTH-1 && balda->field[x+1][y].letter != BALDA_CHAR_NONE) ||
		(y < BALDA_FIELD_HEIGHT && balda->field[x][y+1].letter != BALDA_CHAR_NONE)))
	{
		return BALDA_ADD_LETTER_RESULT_FAIL_NOT_NEAR;
	}
	
	return BALDA_ADD_LETTER_RESULT_OK;
}

BALDA_STATE balda_get_state(balda_t* balda)
{
	return balda->state;
}

BALDA_GAME_TYPE balda_get_game_type(balda_t* balda)
{
	return balda->game_type;
}

int balda_get_word_list_length(balda_t* balda, int player_index)
{
	return balda->player_word_lists[player_index].length;
}

const balda_char* balda_get_player_word(balda_t* balda, int player_index, int word_index)
{
	assert(word_index < balda->player_word_lists[player_index].length);
	return balda->player_word_lists[player_index].words[word_index];
}

void balda_after_turn(balda_t* balda, const balda_char* word)
{
	--balda->turns_left;
	balda_char_strcpy(word,
		balda->player_word_lists[balda->active_player].words[balda->player_word_lists[balda->active_player].length]);
	++balda->player_word_lists[balda->active_player].length;
	
	balda->player_score[balda->active_player] += balda_char_strlen(word);
	balda->active_player = (balda->active_player + 1) % 2;
	
	if (balda->turns_left == 0)
	{
		balda->state = BALDA_STATE_GAMEOVER;
		balda->winner = (balda->player_score[0] > balda->player_score[1]) ? 0 :
			((balda->player_score[0] == balda->player_score[1]) ? GAME_RESULT_DRAW : 1);
	}
}

BALDA_TURN_RESULT balda_sequence_make_turn(balda_t* balda)
{
	balda_bool word_found;
	
	if (balda->sequence.length < 2)
		return BALDA_TURN_RESULT_TOO_SHORT;
	const balda_char* word = balda_sequence_get_word(balda);
	
	int i, pi;
	balda_bool insert_selected = 0;
	for (i=0; i<balda->sequence.length; ++i)
	{
		if (balda_points_equal(balda->sequence.entries[i].pos, balda->sequence.insert_pos))
		{
			insert_selected = 1;
			break;
		}
	}
	
	if (!insert_selected)
	{
		return BALDA_TURN_RESULT_INSERT_CHAR_NOT_SELECTED;
	}
	
	word_found = balda_dict_contains_word(&balda->dict, word);
	if (!word_found)
	{
		return BALDA_TURN_RESULT_WORD_NOT_FOUND;
	}
	
	
	for (pi=0; pi<2; ++pi)
	{
		for (i=0; i<balda->player_word_lists[pi].length; ++i)
		{
			if (balda_char_strcmp(balda->player_word_lists[pi].words[i], word) == 0)
				return BALDA_TURN_RESULT_WORD_ALREADY_USED;
		}
	}
		
	
	balda_sequence_reset(balda);
	balda->field[balda->sequence.insert_pos.x][balda->sequence.insert_pos.y].letter = balda->sequence.insert_char;
	balda_after_turn(balda, word);
	
	return BALDA_TURN_RESULT_OK;
}

void balda_make_ai_turn(balda_t* balda)
{
	assert(balda_is_active_player_ai(balda));
	balda_char word[] = { 0 };
	
	balda_after_turn(balda, word);
}

void balda_set_player_name(balda_t* balda, int player_index, const char* name)
{
	assert(player_index == 0 || player_index == 1);
	strcpy(balda->player_names[player_index], name);
}

const char* balda_get_player_name(balda_t* balda, int player_index)
{
	return balda->player_names[player_index];
}

int balda_get_score(balda_t* balda, int player_index)
{
	return balda->player_score[player_index];
}
