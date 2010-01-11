#ifndef __BALDA_CONTROLLER_H__
#define __BALDA_CONTROLLER_H__

#include "balda.h"

typedef enum
{
	BALDA_TURN_STAGE_SELECT_POS,
	BALDA_TURN_STAGE_SELECT_LETTER,
	BALDA_TURN_STAGE_SELECT_FIRST,
	BALDA_TURN_STAGE_DEFINE_WORD
} BALDA_TURN_STAGE;

void balda_controller_init(balda_t*);
int balda_controller_handler(int type, int par1, int par2);

#endif
