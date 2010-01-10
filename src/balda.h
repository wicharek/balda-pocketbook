#ifndef __BALDA_H__
#define __BALDA_H__

#include "balda_config.h"

#define BALDA_FIELD_WIDTH 5
#define BALDA_FIELD_HEIGHT 5

typedef unsigned short balda_char;
typedef unsigned char balda_bool;

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

balda_t* balda_init(void);
void balda_free(balda_t*);

BALDA_STATE balda_get_state(balda_t* balda);

void balda_new_game(balda_t* balda, BALDA_GAME_TYPE type);

void balda_set_player_name(balda_t* balda, int player_index, const char* name);
const char* balda_get_player_name(balda_t* balda, int player_index);

int balda_get_score(balda_t* balda, int player_index);

#endif
