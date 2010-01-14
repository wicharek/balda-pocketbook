#ifndef __BALDA_H__
#define __BALDA_H__

#include "balda_config.h"
#include "balda_utils.h"

#define BALDA_FIELD_WIDTH 5
#define BALDA_FIELD_HEIGHT 5

typedef unsigned short balda_char;
static const balda_char BALDA_CHAR_NONE=0;

#define BALDA_CHAR_NONE_IS_0

struct balda_t_impl;
typedef struct balda_t_impl balda_t;

typedef enum
{
	BALDA_STATE_NONE,
	BALDA_STATE_PLAYING,
} BALDA_STATE;

typedef enum
{
	BALDA_GAME_TYPE_ONE_PLAYER,
	BALDA_GAME_TYPE_TWO_PLAYERS
} BALDA_GAME_TYPE;

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

void balda_new_game(balda_t* balda, BALDA_GAME_TYPE type);
void balda_surrender(balda_t* balda);

void balda_set_player_name(balda_t* balda, int player_index, const char* name);
const char* balda_get_player_name(balda_t* balda, int player_index);

int balda_get_score(balda_t* balda, int player_index);
int balda_get_active_player(balda_t* balda);
balda_bool balda_is_active_player_ai(balda_t* balda);

balda_char balda_get_letter_at(balda_t* balda, int x, int y);
BALDA_ADD_LETTER_RESULT balda_can_add_letter_at(balda_t* balda, int x, int y);

BALDA_SEQUENCE_START_RESULT balda_sequence_start(balda_t* balda, balda_point_t start_pos, balda_point_t insert_pos, balda_char insert_char);
void balda_sequence_reset(balda_t* balda);
BALDA_SEQUENCE_NEXT_RESULT balda_sequence_next(balda_t* balda, BALDA_DIRECTION direction);
BALDA_TURN_RESULT balda_sequence_make_turn(balda_t* balda);
void balda_sequence_rollback_last(balda_t* balda);
balda_point_t balda_sequence_last_selected(balda_t* balda);
balda_point_t balda_sequence_prelast_selected(balda_t* balda);

void balda_make_ai_turn(balda_t* balda);

#endif
