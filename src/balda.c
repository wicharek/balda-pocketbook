#include <stdlib.h>
#include <assert.h>

// for text encoding functions
#include <inkview.h>

#include "balda.h"

#define BALDA_PLAYER_NAME_BUFFER_SIZE 128

typedef struct
{
	balda_char letter;
	balda_bool filled;
} balda_field_cell_t;

struct balda_t_impl
{
	BALDA_STATE state;
	BALDA_GAME_TYPE game_type;
	
	balda_field_cell_t field[BALDA_FIELD_WIDTH][BALDA_FIELD_HEIGHT];
	char player_names[2][BALDA_PLAYER_NAME_BUFFER_SIZE];
	int player_score[2];
};

balda_t* balda_init(void)
{
	balda_t* balda = (balda_t*)malloc(sizeof(balda_t));
	
	balda->state = BALDA_STATE_NONE;
	
	// reset player names
	balda->player_names[0][0] = 0;
	balda->player_names[1][0] = 0;
	
	return balda;
}

void balda_free(balda_t* balda)
{
	assert(balda);
	free(balda);
}

void balda_new_game(balda_t* balda, BALDA_GAME_TYPE type)
{
	balda->state = BALDA_STATE_PLAYING;
	balda->game_type = type;
	
	balda->player_score[0] = 13;
	balda->player_score[1] = 254;
	
	// pick initial word
	// TODO
}

BALDA_STATE balda_get_state(balda_t* balda)
{
	return balda->state;
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
