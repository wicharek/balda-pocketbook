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
#define BALDA_MAX_WORDS 	10
#define BALDA_MAX_TURNS 	20

extern balda_dict_data_t balda_dict_data[];

typedef struct
{
	balda_char letter;
} balda_field_cell_t;

typedef struct
{
	balda_char words[BALDA_MAX_WORDS][BALDA_MAX_SEQUENCE+1];
	int length;
} balda_word_list_t;

typedef struct
{
	balda_sequence_t current_sequence;
	balda_sequence_t best_sequence;
	balda_field_cell_t field[BALDA_FIELD_WIDTH][BALDA_FIELD_HEIGHT];
} balda_ai_search_context_t;

struct balda_t_impl
{
	BALDA_STATE state;
	BALDA_GAME_TYPE game_type;
	
	balda_char initial_word[BALDA_FIELD_WIDTH+1];
	balda_field_cell_t field[BALDA_FIELD_WIDTH][BALDA_FIELD_HEIGHT];
	char player_names[2][BALDA_PLAYER_NAME_BUFFER_SIZE];
	int player_score[2];
	balda_word_list_t player_word_lists[2];
	
	int active_player;
	
	//balda_sequence_t sequence;
	balda_dict_t dict;
	int winner;
	int turns_left;
	
	balda_ai_search_context_t ai_search_context;
};

void balda_sequence_copy(balda_sequence_t* to, const balda_sequence_t* from)
{
	memcpy(to, from, sizeof(balda_sequence_t));
}

void balda_sequence_reset(balda_sequence_t* sequence)
{
	sequence->length = 0;
}

balda_bool balda_sequence_contains_point(balda_sequence_t* sequence, int x, int y)
{
	int i;
	for (i=0; i<sequence->length; ++i)
	{
		if (sequence->entries[i].pos.x == x && sequence->entries[i].pos.y == y)
			return balda_true;
	}
	
	return balda_false;
}

BALDA_SEQUENCE_NEXT_RESULT balda_sequence_next(balda_sequence_t* sequence, BALDA_DIRECTION direction,
	balda_field_cell_t field[BALDA_FIELD_WIDTH][BALDA_FIELD_HEIGHT])
{
	assert(sequence);
	
	int i;
	balda_point_t next_pos;
	
	printf("[balda_sequence_next] direction: %d\n", direction);
	
	assert(sequence->length > 0);
	
	next_pos = sequence->entries[sequence->length-1].pos;
	
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
	if (field[next_pos.x][next_pos.y].letter == BALDA_CHAR_NONE
		&& !balda_points_equal(sequence->insert_pos, next_pos))
	{
		printf(" BALDA_SEQUENCE_NEXT_RESULT_FAIL_EMPTY_CELL\n");
		return BALDA_SEQUENCE_NEXT_RESULT_FAIL_EMPTY_CELL;
	}
	
	// Check if cell was not already selected
	for (i=0; i<sequence->length; ++i)
	{
		if (balda_points_equal(next_pos, sequence->entries[i].pos))
		{
			printf(" BALDA_SEQUENCE_NEXT_RESULT_FAIL_ALREADY_SELECTED\n");
			return BALDA_SEQUENCE_NEXT_RESULT_FAIL_ALREADY_SELECTED;
		}
	}
	
	// Everything ok. Add to sequence.
	sequence->entries[sequence->length].pos = next_pos;
	++sequence->length;
	
	return BALDA_SEQUENCE_NEXT_RESULT_OK;
}

const balda_char* balda_sequence_get_word_impl(balda_sequence_t* sequence,
	balda_field_cell_t field[BALDA_FIELD_WIDTH][BALDA_FIELD_HEIGHT])
{
	int i;
	for (i=0; i<sequence->length; ++i)
	{
		if (balda_points_equal(sequence->entries[i].pos, sequence->insert_pos))
			sequence->word[i] = sequence->insert_char;
		else
			sequence->word[i] = field[sequence->entries[i].pos.x][sequence->entries[i].pos.y].letter;
	}
	sequence->word[i] = BALDA_CHAR_NONE;

	return sequence->word;
}

const balda_char* balda_sequence_get_word(balda_sequence_t* sequence, balda_t* balda)
{
	return balda_sequence_get_word_impl(sequence, balda->field);
}

void balda_sequence_rollback_last(balda_sequence_t* sequence)
{
	printf("balda_sequence_rollback_last\n");
	
	if (sequence->length > 0)
	{
		--sequence->length;
	}
}

balda_point_t balda_sequence_last_selected(balda_sequence_t* sequence)
{
	if (sequence->length > 0)
	{
		return sequence->entries[sequence->length-1].pos;
	}
	
	return balda_make_point(-1, -1);
}

balda_point_t balda_sequence_prelast_selected(balda_sequence_t* sequence)
{
	if (sequence->length > 1)
	{
		return sequence->entries[sequence->length-2].pos;
	}
	
	return balda_make_point(-1, -1);
}



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

BALDA_SEQUENCE_START_RESULT balda_turn_sequence_start(balda_t* balda,
	balda_point_t start_pos, balda_point_t insert_pos, balda_char insert_char,
	balda_sequence_t* sequence)
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
	
	sequence->length = 1;
	sequence->entries[0].pos = start_pos;
	sequence->insert_char = insert_char;
	sequence->insert_pos = insert_pos;
	
	return BALDA_SEQUENCE_START_RESULT_OK;
}

BALDA_SEQUENCE_NEXT_RESULT balda_turn_sequence_next(balda_t* balda, balda_sequence_t* sequence,
	BALDA_DIRECTION direction)
{
	return balda_sequence_next(sequence, direction, balda->field);
}

void balda_new_game(balda_t* balda, BALDA_GAME_TYPE type)
{
	int i;
	
	balda->state = BALDA_STATE_PLAYING;
	balda->game_type = type;
	balda->turns_left = BALDA_MAX_TURNS;
	
	balda->player_score[0] = 0;
	balda->player_score[1] = 0;
	
	balda->player_word_lists[0].length = 0;
	balda->player_word_lists[1].length = 0;
	
	balda->active_player = rand() % 2;
	//balda->active_player = 1; // TEST: ai
	
	// peek initial word
	balda_bool peeked = balda_dict_peek_word_5(&balda->dict, balda->initial_word);
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
		balda->field[i][2].letter = balda->initial_word[i];
		//balda->field[i][3].letter = initial_word[i];
	}
	//balda->field[2][3].letter = 0;
	
	//balda_current_sequence_reset(balda);
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
	return (balda->turns_left == BALDA_MAX_TURNS) ?
		(-1) : ((balda->active_player + 1) % 2);
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
		(y < BALDA_FIELD_HEIGHT-1 && balda->field[x][y+1].letter != BALDA_CHAR_NONE)))
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
	
	if (balda->turns_left == 0)
	{
		balda->state = BALDA_STATE_GAMEOVER;
		balda->winner = (balda->player_score[0] > balda->player_score[1]) ? 0 :
			((balda->player_score[0] == balda->player_score[1]) ? GAME_RESULT_DRAW : 1);
	}
	else
	{
		balda->active_player = (balda->active_player + 1) % 2;
	}
}

balda_bool balda_was_word_used(balda_t* balda, const balda_char* word)
{
	int i, pi;
	
	if (balda_char_strcmp(balda->initial_word, word) == 0)
		return balda_true;
	
	for (pi=0; pi<2; ++pi)
	{
		for (i=0; i<balda->player_word_lists[pi].length; ++i)
		{
			if (balda_char_strcmp(balda->player_word_lists[pi].words[i], word) == 0)
				return balda_true;
		}
	}
	
	return balda_false;
}

BALDA_TURN_RESULT balda_make_turn(balda_t* balda, balda_sequence_t* sequence)
{
	balda_bool word_found;
	
	if (sequence->length < 2)
		return BALDA_TURN_RESULT_TOO_SHORT;
	const balda_char* word = balda_sequence_get_word(sequence, balda);
	
	int i;
	balda_bool insert_selected = 0;
	for (i=0; i<sequence->length; ++i)
	{
		if (balda_points_equal(sequence->entries[i].pos, sequence->insert_pos))
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
	
	
	if (balda_was_word_used(balda, word))
		return BALDA_TURN_RESULT_WORD_ALREADY_USED;
		
	balda->field[sequence->insert_pos.x][sequence->insert_pos.y].letter = sequence->insert_char;
	balda_after_turn(balda, word);
	
	return BALDA_TURN_RESULT_OK;
}

#define balda_field_no_letter_at_impl(x, y, field) (((field)[(x)][(y)].letter == BALDA_CHAR_NONE))
#define balda_field_is_letter_at_impl(x, y, field) (((field)[(x)][(y)].letter != BALDA_CHAR_NONE))
#define balda_field_no_letter_at(x, y) (balda_field_no_letter_at_impl((x), (y), (balda->field)))
#define balda_field_is_letter_at(x, y) (balda_field_is_letter_at_impl((x), (y), (balda->field)))

void balda_ai_find_best_word_callback(balda_t* balda,
	balda_sequence_t* local_sequence, int x, int y, balda_dict_chunk_t* dict_chunk)
{
	balda_ai_search_context_t* context = &balda->ai_search_context;
	//debug_printf(("balda_ai_find_best_word_callback"));
	
	balda_char c = context->field[x][y].letter;
	balda_dict_chunk_entry_t e;
	
	// check if current chunk has an entry for this char, otherwise
	// there is no need to check the sequence any more
	if (balda_dict_chunk_get_entry(dict_chunk, c, &e))
	{
		// add cell to sequence
		local_sequence->entries[local_sequence->length].pos = balda_make_point(x, y);
		local_sequence->word[local_sequence->length] = c;
		local_sequence->length++;
		local_sequence->word[local_sequence->length] = BALDA_CHAR_NONE;
		
		// if current sequence forms a word
		if (e.ends_here)
		{
			// longer word was found
			if (local_sequence->length > context->current_sequence.length
				&& balda_sequence_contains_point(local_sequence, local_sequence->insert_pos.x, local_sequence->insert_pos.y)
				&& !balda_was_word_used(balda, local_sequence->word))
			{
				balda_sequence_copy(&context->current_sequence, local_sequence);
			}
		}
		
		if (balda_dict_chunk_is_valid(&e.next_chunk))
		{
			// continue traversing
			// up
			if (y > 0 && balda_field_is_letter_at_impl(x, y-1, context->field)
				&& !balda_sequence_contains_point(local_sequence, x, y-1))
			{
				balda_ai_find_best_word_callback(balda, local_sequence, x, y-1,
					&e.next_chunk);
			}
			
			// down
			if (y < BALDA_FIELD_HEIGHT-1 && balda_field_is_letter_at_impl(x, y+1, context->field)
				&& !balda_sequence_contains_point(local_sequence, x, y+1))
			{
				balda_ai_find_best_word_callback(balda, local_sequence, x, y+1,
					&e.next_chunk);
			}
			
			// left
			if (x > 0 && balda_field_is_letter_at_impl(x-1, y, context->field)
				&& !balda_sequence_contains_point(local_sequence, x-1, y))
			{
				balda_ai_find_best_word_callback(balda, local_sequence, x-1, y,
					&e.next_chunk);
			}
			
			// right
			if (x < BALDA_FIELD_WIDTH-1 && balda_field_is_letter_at_impl(x+1, y, context->field)
				&& !balda_sequence_contains_point(local_sequence, x+1, y))
			{
				balda_ai_find_best_word_callback(balda, local_sequence, x+1, y,
					&e.next_chunk);
			}
		}
		
		// rollback sequence
		local_sequence->length--;
	}
}

/**
 * Searches for best word in specified context.
 */
void balda_ai_find_best_word(balda_t* balda, balda_dict_t* dict)
{
	//debug_printf(("balda_ai_find_best_word"));
	balda_ai_search_context_t* context = &balda->ai_search_context;
	
	balda_sequence_t local_sequence;
	balda_dict_chunk_t dict_chunk;
	
	// reset sequences
	balda_sequence_reset(&context->current_sequence);
	// init local sequence
	local_sequence.insert_pos = context->current_sequence.insert_pos;
	local_sequence.insert_char = context->current_sequence.insert_char;
	
	// loop through field
	int x, y;
	for (x=0; x<BALDA_FIELD_WIDTH; ++x)
	{
		for (y=0; y<BALDA_FIELD_HEIGHT; ++y)
		{
			if (balda_field_is_letter_at_impl(x, y, context->field))
			{
				balda_sequence_reset(&local_sequence);
				
				balda_dict_get_root_chunk(dict, &dict_chunk);
				balda_ai_find_best_word_callback(balda, &local_sequence, x, y, &dict_chunk);
			}
		}
	}
}

balda_sequence_t* balda_get_last_ai_sequence(balda_t* balda)
{
	return &balda->ai_search_context.best_sequence;
}

void balda_make_ai_turn(balda_t* balda)
{
	assert(balda_is_active_player_ai(balda));
	debug_printf(("balda_make_ai_turn"));
	
	//balda_char word[BALDA_MAX_SEQUENCE];
	
	// make a copy of field for faster access (due to insert char)
	memcpy(&balda->ai_search_context.field, &balda->field, sizeof(balda->field));
	// reset best sequence
	balda_sequence_reset(&balda->ai_search_context.best_sequence);
	
	// loop through all cells suitable for inserting
	// random loop order
	int x, y, x0, xm, dx, y0, ym, dy;
	if (rand() % 2)
	{
		x0 = 0;
		xm = BALDA_FIELD_WIDTH-1;
		dx = 1;
	}
	else
	{
		x0 = BALDA_FIELD_WIDTH-1;
		xm = 0;
		dx = -1;
	}
	
	if (rand() % 2)
	{
		y0 = 0;
		ym = BALDA_FIELD_HEIGHT-1;
		dy = 1;
	}
	else
	{
		y0 = BALDA_FIELD_HEIGHT-1;
		ym = 0;
		dy = -1;
	}
	
	for (x=x0; x!=xm; x += dx)
	{
		for (y=y0; y!=ym; y += dy)
		{
			if (balda_field_no_letter_at(x, y) &&
				((x > 0 && balda_field_is_letter_at(x-1, y)) || 
				(x < (BALDA_FIELD_WIDTH-1) && balda_field_is_letter_at(x+1, y)) ||
				(y > 0 && balda_field_is_letter_at(x, y-1)) || 
				(y < (BALDA_FIELD_HEIGHT-1) && balda_field_is_letter_at(x, y+1))))
			{
				// can insert here
				debug_printf(("insert at: { %d, %d }", x, y));
				
				balda->ai_search_context.current_sequence.insert_pos = balda_make_point(x, y);
				for (balda->ai_search_context.current_sequence.insert_char=1;
					balda->ai_search_context.current_sequence.insert_char<=BALDA_CHAR_MAX;
					++balda->ai_search_context.current_sequence.insert_char)
				{
					//debug_printf((" char: { %d }", balda->ai_search_context.current_sequence.insert_char));
					
					balda->ai_search_context.field[x][y].letter = balda->ai_search_context.current_sequence.insert_char;
					
					// now loop through all possible words
					balda_ai_find_best_word(balda, &balda->dict);
					
					// longer word was found
					if (balda->ai_search_context.current_sequence.length > balda->ai_search_context.best_sequence.length)
					{
						debug_printf((" word found, length: %d", balda->ai_search_context.current_sequence.length));
						//debug_printf((" insert_char: %d", balda->ai_search_context.current_sequence.insert_char));
						
						balda_sequence_copy(&balda->ai_search_context.best_sequence,
							&balda->ai_search_context.current_sequence);
					}
					
					balda->ai_search_context.field[x][y].letter = BALDA_CHAR_NONE;
				}
			}
		}
	}
	
	if (balda->ai_search_context.best_sequence.length)
	{
		// word was found
		const balda_char* word = balda_sequence_get_word_impl(&balda->ai_search_context.best_sequence,
			balda->field);
		
		//debug_printf((" insert_char: %d", balda->ai_search_context.best_sequence.insert_char));
		
		balda->field[balda->ai_search_context.best_sequence.insert_pos.x]
			[balda->ai_search_context.best_sequence.insert_pos.y].letter = balda->ai_search_context.best_sequence.insert_char;
		balda_after_turn(balda, word);
	}
	else
	{
		// no word found, surrender
		balda_surrender(balda);
	}
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
