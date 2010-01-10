#include "balda_strings.h"

const char* balda_default_player_name_0_ru = "Игрок 1";
const char* balda_default_player_name_1_ru = "Игрок 2";
const char* balda_ai_player_name_ru = "Балда";

const char* balda_empty_string = "";

const char* balda_string(BALDA_STRING_ID id)
{
	switch (id)
	{
		case BALDA_STR_DEFAULT_PLAYER_NAME_0:
			return balda_default_player_name_0_ru;
	
		case BALDA_STR_DEFAULT_PLAYER_NAME_1:
			return balda_default_player_name_1_ru;
	
		case BALDA_STR_DEFAULT_AI_PLAYER_NAME:
			return balda_ai_player_name_ru;
	
		default:
			return balda_empty_string;
	}
}
