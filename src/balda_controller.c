#include "balda_controller.h"
#include "balda_view.h"
#include "balda_strings.h"

#include <inkview.h>
#include <stdlib.h>
#include <assert.h>

const balda_char* ytsuken_keyboard_ru = L"ЙЦУКЕНГШЩЗХФЫВАПРОЛДЖЭЯЧСМИТЬБЮЪ";
const balda_char* abvgde_keyboard_ru = L"АБВГДЕЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯ";

typedef struct balda_controller_t
{
	balda_t* balda;
	balda_view_t* view;
	
} balda_controller_t;

balda_controller_t g_balda_controller;


void balda_controller_init(balda_t* balda)
{
	g_balda_controller.balda = balda;
}

void balda_controller_on_evt_init()
{
	g_balda_controller.view = balda_view_init(g_balda_controller.balda);
	
	balda_new_game(g_balda_controller.balda, BALDA_GAME_TYPE_ONE_PLAYER);
	balda_set_player_name(g_balda_controller.balda, 0, balda_string(BALDA_STR_DEFAULT_PLAYER_NAME_0));
	balda_set_player_name(g_balda_controller.balda, 1, balda_string(BALDA_STR_DEFAULT_AI_PLAYER_NAME));
}

int balda_controller_handler(int type, int par1, int par2)
{
	switch (type)
	{
		case EVT_INIT:
			balda_controller_on_evt_init();
		break;
		
		case EVT_SHOW:
			ClearScreen();
			FullUpdate();
			
			balda_view_draw_all(g_balda_controller.view);
			
			FullUpdate();
			FineUpdate();
		break;
		
		default:
		break;
	}
	
	/*if (type == EVT_KEYPRESS)
	{
		CloseApp();
	}*/

	return 0;
}
