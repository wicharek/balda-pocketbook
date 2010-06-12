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
 
#include "balda_controller.h"
#include "balda_strings.h"

#include "balda_view.h"
#include "balda_field_view.h"
#include "balda_keyboard_view.h"
#include "balda_button.h"
#include "balda_player_view.h"
#include "balda_gameover_view.h"
#include "balda_title_view.h"

#include <inkview.h>
#include <stdlib.h>
#include <assert.h>

const char* balda_ytsuken_keyboard_ru = "ЙЦУКЕНГШЩЗХФЫВАПРОЛДЖЭЯЧСМИТЬБЮЪ";
const char* balda_abvgde_keyboard_ru = "АБВГДЕЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯ";

static imenu new_game_menu[] = {
	{ ITEM_ACTIVE, 111, BALDA_STR_MENU_ONE_PLAYER, NULL },
	{ ITEM_ACTIVE, 112, BALDA_STR_MENU_TWO_PLAYERS, NULL },
	
	{ 0, 0, NULL, NULL }
};

static imenu difficulty_menu[] = {
	{ ITEM_ACTIVE, 121, BALDA_STR_MENU_DIFFICULTY_EASY, NULL },
	{ ITEM_BULLET, 122, BALDA_STR_MENU_DIFFICULTY_NORMAL, NULL },
	{ ITEM_ACTIVE, 123, BALDA_STR_MENU_DIFFICULTY_HARD, NULL },
	
	{ 0, 0, NULL, NULL }
};

static imenu main_menu[] = {
	{ ITEM_HEADER, 0, BALDA_STR_MENU_HEADER, NULL },
	{ ITEM_ACTIVE, 101, BALDA_STR_MENU_RESUME, NULL },
	{ ITEM_SUBMENU, 110, BALDA_STR_MENU_NEW_GAME, new_game_menu },
	{ ITEM_SUBMENU, 120, BALDA_STR_MENU_DIFFICULTY, difficulty_menu },
	{ ITEM_ACTIVE, 140, BALDA_STR_MENU_ABOUT, NULL },
	{ ITEM_ACTIVE, 150, BALDA_STR_MENU_EXIT, NULL },
	
	{ 0, 0, NULL, NULL }
};

typedef struct balda_controller_t
{
	BALDA_UI_TYPE ui_type;
	
	balda_t* balda;
	balda_view_t* view;
	balda_keyboard_view_t* kb_view;
	balda_field_view_t* field_view;
	balda_button_t* btn_cancel;
	balda_button_t* btn_confirm;
	balda_player_view_t* player_view[2];
	balda_gameover_view_t* gameover_view;
	balda_title_view_t* title_view;
	
	balda_point_t field_pos;
	balda_point_t kb_pos;
	balda_sequence_t sequence;
	
	BALDA_TURN_STAGE turn_stage;
	
	balda_bool pointer_pressed;
	balda_point_t pointer_pressed_at;
	balda_button_t* pointer_down_button;
	
	balda_char ytsuken_keyboard_ru[33];
	balda_char abvgde_keyboard_ru[33];
	balda_bool is_initialized;
} balda_controller_t;

static balda_controller_t g_balda_controller;


void balda_controller_init(balda_t* balda)
{
	g_balda_controller.balda = balda;
	g_balda_controller.is_initialized = 0;
	g_balda_controller.pointer_pressed = 0;
	g_balda_controller.pointer_down_button = 0;
	
	balda_char_from_utf8(balda_ytsuken_keyboard_ru, g_balda_controller.ytsuken_keyboard_ru, 33);
	balda_char_from_utf8(balda_abvgde_keyboard_ru, g_balda_controller.abvgde_keyboard_ru, 33);
	
	debug_printf(("GetHardwareType: \"%s\"", GetHardwareType()));
	debug_printf(("GetDeviceModel: \"%s\"", GetDeviceModel()));
	
	// detect interface type
	if (strcmp(GetHardwareType(), "Emulator") == 0)
	{
		g_balda_controller.ui_type = BALDA_UI_TYPE_KEYS;
		//g_balda_controller.ui_type = BALDA_UI_TYPE_TOUCHSCREEN;
	}
	else
	{
		if (strcmp(GetDeviceModel(), "PocketBook 302") == 0)
			g_balda_controller.ui_type = BALDA_UI_TYPE_TOUCHSCREEN;
		else
			g_balda_controller.ui_type = BALDA_UI_TYPE_KEYS;
	}
}

void balda_controller_free()
{
	if (g_balda_controller.is_initialized)
	{
		balda_view_free(g_balda_controller.view);
		balda_keyboard_view_free(g_balda_controller.kb_view);
		
		balda_field_view_free(g_balda_controller.field_view );
		balda_button_free(g_balda_controller.btn_cancel);
		if (g_balda_controller.ui_type == BALDA_UI_TYPE_TOUCHSCREEN)
		{
			balda_button_free(g_balda_controller.btn_confirm);
		}	
		
		balda_player_view_free(g_balda_controller.player_view[0]);
		balda_player_view_free(g_balda_controller.player_view[1]);
		balda_title_view_free(g_balda_controller.title_view);
		balda_gameover_view_free(g_balda_controller.gameover_view);
	}
}

void balda_controller_on_game_started();
void balda_controller_on_game_over();
void balda_controller_new_game(BALDA_GAME_TYPE type)
{
	balda_new_game(g_balda_controller.balda, type);
	if (type == BALDA_GAME_TYPE_ONE_PLAYER)
		balda_set_player_name(g_balda_controller.balda, 1, balda_string(BALDA_STR_AI_PLAYER_NAME));
	else
		balda_set_player_name(g_balda_controller.balda, 1, balda_string(BALDA_STR_DEFAULT_PLAYER_NAME_1));
	balda_controller_on_game_started();
}

void balda_controller_redraw_after_turn(balda_bool first)
{
	int i, prev_i = balda_get_previous_active_player(g_balda_controller.balda);
	for (i=0; i<2; ++i)
	{
		balda_player_view_update_active(g_balda_controller.player_view[i]);
		
		if (i == prev_i || first)
		{
			balda_player_view_update_score(g_balda_controller.player_view[i]);
			balda_player_view_draw_last_word(g_balda_controller.player_view[i]);
		}
	}
}

void balda_controller_next_turn(balda_bool first)
{
	////int active_player = balda_get_active_player(g_balda_controller.balda);
	//balda_view_show_turn(g_balda_controller.view);
	
	if (g_balda_controller.turn_stage != BALDA_TURN_STAGE_AFTER_AI_TURN)
	{
		balda_controller_redraw_after_turn(first);
	}
	
	if (!first)
	{
		//balda_view_draw_score(g_balda_controller.view, balda_get_previous_active_player(g_balda_controller.balda), 1);
		//balda_view_draw_player_last_word(g_balda_controller.view, balda_get_previous_active_player(g_balda_controller.balda), 1);
	}
	
	if (balda_is_active_player_ai(g_balda_controller.balda))
	{
		ShowHourglass();
		balda_make_ai_turn(g_balda_controller.balda);
		HideHourglass();
		
		balda_sequence_t* ai_seq = balda_get_last_ai_sequence(g_balda_controller.balda);
		/*balda_field_view_set_insert_char(g_balda_controller.field_view, ai_seq->insert_pos,
			ai_seq->insert_char);*/
		balda_field_view_load(g_balda_controller.field_view, g_balda_controller.balda);
		balda_field_view_load_sequence(g_balda_controller.field_view, ai_seq);
		
		if (balda_is_game_over(g_balda_controller.balda))
		{
			int active_player = balda_get_active_player(g_balda_controller.balda);
			balda_player_view_update_score(g_balda_controller.player_view[active_player]);
			balda_player_view_draw_last_word(g_balda_controller.player_view[active_player]);
			
			balda_controller_on_game_over();
		}
		else
		{
			g_balda_controller.turn_stage = BALDA_TURN_STAGE_AFTER_AI_TURN;
			balda_controller_redraw_after_turn(first);
		}
	}
	else
	{
		g_balda_controller.turn_stage = BALDA_TURN_STAGE_SELECT_POS;
		
		if (!first && g_balda_controller.turn_stage != BALDA_TURN_STAGE_AFTER_AI_TURN)
			balda_field_view_load(g_balda_controller.field_view, g_balda_controller.balda);
		
		if (balda_is_game_over(g_balda_controller.balda))
		{
			int active_player = balda_get_active_player(g_balda_controller.balda);
			balda_player_view_update_score(g_balda_controller.player_view[active_player]);
			balda_player_view_draw_last_word(g_balda_controller.player_view[active_player]);
			
			balda_controller_on_game_over();
		}
		else
		{
			if (g_balda_controller.ui_type == BALDA_UI_TYPE_KEYS)
			{
				// initially select central point on field
				g_balda_controller.field_pos = balda_make_point(BALDA_FIELD_WIDTH / 2, BALDA_FIELD_HEIGHT / 2);
				balda_field_view_select(g_balda_controller.field_view, g_balda_controller.field_pos);
			}
			else if (g_balda_controller.ui_type == BALDA_UI_TYPE_TOUCHSCREEN)
			{
				// nothing is selected initially
				balda_field_view_deselect(g_balda_controller.field_view);
				balda_keyboard_view_clear_selection(g_balda_controller.kb_view);
			}
			
			balda_button_set_mode(g_balda_controller.btn_cancel, BALDA_BUTTON_MODE_SURRENDER);
			balda_button_set_selected(g_balda_controller.btn_cancel, 0);
			balda_button_set_visible(g_balda_controller.btn_cancel, 1);
			
			balda_button_redraw(g_balda_controller.btn_cancel, 1);
			
			if (g_balda_controller.ui_type == BALDA_UI_TYPE_TOUCHSCREEN)
			{
				balda_button_set_visible(g_balda_controller.btn_confirm, 0);
				balda_button_set_selected(g_balda_controller.btn_cancel, 0);
				balda_button_redraw(g_balda_controller.btn_confirm, 1);
			}
		}
	}
	
	balda_view_update(g_balda_controller.view);
}

void balda_controller_on_game_started()
{
	ClearScreen();
	FullUpdate();
	
	//balda_view_draw_all(g_balda_controller.view);
	//balda_view_reset(g_balda_controller.view);
	
	balda_field_view_reset(g_balda_controller.field_view);
	balda_field_view_load(g_balda_controller.field_view, g_balda_controller.balda);
	balda_keyboard_view_draw(g_balda_controller.kb_view);
	balda_player_view_draw(g_balda_controller.player_view[0]);
	balda_player_view_draw(g_balda_controller.player_view[1]);
	balda_title_view_show(g_balda_controller.title_view);
	
	//DrawString(0, 0, GetDeviceModel());
	
	FullUpdate();
	FineUpdate();
	
	balda_view_reset(g_balda_controller.view);
	balda_controller_next_turn(1);
}

void balda_controller_on_game_over()
{
	balda_gameover_view_show(g_balda_controller.gameover_view);
	//balda_view_update(g_balda_controller.view);
}

void balda_controller_on_evt_init()
{
	g_balda_controller.is_initialized = 1;
	
	/*int i;
	for (i=0; i<100; ++i)
	{
		debug_printf(("rand_normal: %f", rand_normal_with_interval(3.0)));
	}*/
	
	// initialize views
	g_balda_controller.view = balda_view_init(g_balda_controller.balda);
	
	g_balda_controller.kb_view = balda_keyboard_view_init(g_balda_controller.view,
		g_balda_controller.ui_type != BALDA_UI_TYPE_TOUCHSCREEN);
	balda_keyboard_view_set_keyboard(g_balda_controller.kb_view, g_balda_controller.ytsuken_keyboard_ru);
	
	g_balda_controller.field_view = balda_field_view_init(g_balda_controller.view);
	g_balda_controller.btn_cancel = balda_button_init(g_balda_controller.view,
		balda_make_point((ScreenWidth() - BALDA_BUTTON_WIDTH) / 2, 463+12
			+ (g_balda_controller.ui_type == BALDA_UI_TYPE_TOUCHSCREEN ? (BALDA_BUTTON_HEIGHT + 12) : 0)));
		
	if (g_balda_controller.ui_type == BALDA_UI_TYPE_TOUCHSCREEN)
	{
		g_balda_controller.btn_confirm = balda_button_init(g_balda_controller.view,
			balda_make_point((ScreenWidth() - BALDA_BUTTON_WIDTH) / 2, 463+12));
		balda_button_set_mode(g_balda_controller.btn_confirm, BALDA_BUTTON_MODE_CONFIRM);
	}
	
	g_balda_controller.player_view[0] = balda_player_view_init(g_balda_controller.view, g_balda_controller.balda, 0);
	g_balda_controller.player_view[1] = balda_player_view_init(g_balda_controller.view, g_balda_controller.balda, 1);
	g_balda_controller.title_view = balda_title_view_init(g_balda_controller.view);
	g_balda_controller.gameover_view = balda_gameover_view_init(g_balda_controller.view, g_balda_controller.balda,
		balda_keyboard_view_x(g_balda_controller.kb_view), balda_keyboard_view_y(g_balda_controller.kb_view),
		balda_keyboard_view_w(g_balda_controller.kb_view), balda_keyboard_view_h(g_balda_controller.kb_view));
	
	balda_set_player_name(g_balda_controller.balda, 0, balda_string(BALDA_STR_DEFAULT_PLAYER_NAME_0));
}

void balda_controller_on_evt_show()
{
	//ClearScreen();
	//FullUpdate();
	
	//balda_controller_new_game(BALDA_GAME_TYPE_TWO_PLAYERS);
	balda_controller_new_game(BALDA_GAME_TYPE_ONE_PLAYER);
}

void balda_controller_set_difficulty_bullet(int index)
{
	int i;
	for (i=0 ;i<3; ++i)
		difficulty_menu[i].type = (i == index) ? ITEM_BULLET : ITEM_ACTIVE;
}

void balda_controller_main_menu_handler(int index)
{
	switch (index)
	{
		case 101: // Resume
			// Do nothing
		break;
		
		case 111: // New game -> Single player
			balda_controller_new_game(BALDA_GAME_TYPE_ONE_PLAYER);
		break;
		
		case 112: // New game -> Two players
			balda_controller_new_game(BALDA_GAME_TYPE_TWO_PLAYERS);
		break;
		
		case 121: // Easy
			balda_set_game_difficulty(g_balda_controller.balda, BALDA_GAME_DIFFICULTY_EASY);
			balda_controller_set_difficulty_bullet(0);
		break;
		
		case 122: // Normal
			balda_set_game_difficulty(g_balda_controller.balda, BALDA_GAME_DIFFICULTY_NORMAL);
			balda_controller_set_difficulty_bullet(1);
		break;
		
		case 123: // Hard
			balda_set_game_difficulty(g_balda_controller.balda, BALDA_GAME_DIFFICULTY_HARD);
			balda_controller_set_difficulty_bullet(2);
		break;
		
		case 140: // About
			Message(ICON_INFORMATION, (char*)balda_string(BALDA_STR_ABOUT_TITLE),
				(char*)balda_string(BALDA_STR_ABOUT_MESSAGE), 10000);
		break;
		
		case 150: // Exit
			CloseApp();
		break;
	}
}

void balda_controller_on_cursor_key_pressed(int key)
{
	BALDA_DIRECTION direction = BALDA_DIRECTION_UP;
	int dx = 0, dy = 0;
	
	switch (key)
	{
		case KEY_UP:
			dy = -1;
			direction = BALDA_DIRECTION_UP;
		break;
		case KEY_DOWN:
			dy = 1;
			direction = BALDA_DIRECTION_DOWN;
		break;
		
		case KEY_LEFT:
			dx = -1;
			direction = BALDA_DIRECTION_LEFT;
		break;
		
		case KEY_RIGHT:
			dx = 1;
			direction = BALDA_DIRECTION_RIGHT;
		break;
	}
	
	switch (g_balda_controller.turn_stage)
	{
		case BALDA_TURN_STAGE_SELECT_POS:
		case BALDA_TURN_STAGE_SELECT_FIRST:
		{
			g_balda_controller.field_pos.x += dx;
			g_balda_controller.field_pos.y += dy;
			
			if (g_balda_controller.field_pos.x >= BALDA_FIELD_WIDTH)
				g_balda_controller.field_pos.x = 0;
			else if (g_balda_controller.field_pos.x < 0)
				g_balda_controller.field_pos.x = BALDA_FIELD_WIDTH-1;
				
			if (g_balda_controller.field_pos.y >= BALDA_FIELD_HEIGHT+1)
				g_balda_controller.field_pos.y = 0;
			else if (g_balda_controller.field_pos.y < 0)
				g_balda_controller.field_pos.y = BALDA_FIELD_HEIGHT;
			
			if (g_balda_controller.field_pos.y < BALDA_FIELD_HEIGHT)
			{
				balda_field_view_select(g_balda_controller.field_view,
					g_balda_controller.field_pos);
				
				balda_button_set_selected(g_balda_controller.btn_cancel, 0);
				balda_button_redraw(g_balda_controller.btn_cancel, 0);
			}
			else
			{
				balda_field_view_deselect(g_balda_controller.field_view);
				
				// cancel button selected
				balda_button_set_selected(g_balda_controller.btn_cancel, 1);
				balda_button_redraw(g_balda_controller.btn_cancel, 0);
			}
		}
		break;
		
		case BALDA_TURN_STAGE_SELECT_LETTER:
		{
			balda_keyboard_view_select_delta(g_balda_controller.kb_view, dx, dy);
			balda_field_view_set_insert_char(g_balda_controller.field_view,
				g_balda_controller.field_pos,
				balda_keyboard_view_get_selected_char(g_balda_controller.kb_view));
			
			//balda_view_field_select_inserting(g_balda_controller.view,
			//	balda_view_field_get_selection(g_balda_controller.view),
			//	balda_view_keyboard_get_selected_char(g_balda_controller.view));
		}
		break;
		
		case BALDA_TURN_STAGE_DEFINE_WORD:
		{
			BALDA_SEQUENCE_NEXT_RESULT result = balda_turn_sequence_next(g_balda_controller.balda,
				&g_balda_controller.sequence, direction);
			
			switch (result)
			{
				case BALDA_SEQUENCE_NEXT_RESULT_OK:
					balda_field_view_load_sequence(g_balda_controller.field_view,
						&g_balda_controller.sequence);
				break;
				
				case BALDA_SEQUENCE_NEXT_RESULT_FAIL_ALREADY_SELECTED:
				{
					// Check if this cell is previously selected
					balda_point_t prev_pos = balda_sequence_prelast_selected(&g_balda_controller.sequence);
					balda_point_t last_pos = balda_sequence_last_selected(&g_balda_controller.sequence);
					balda_point_t next_pos = balda_make_point(last_pos.x + dx, last_pos.y + dy);
					
					printf("prev_pos: { %d, %d } next_pos: { %d, %d }\n",
						prev_pos.x, prev_pos.y, next_pos.x, next_pos.y);
					
					if (balda_points_equal(prev_pos, next_pos))
					{
						// Rollback selection then
						balda_sequence_rollback_last(&g_balda_controller.sequence);
						balda_field_view_load_sequence(g_balda_controller.field_view,
							&g_balda_controller.sequence);
					}
				}
				break;
				
				case BALDA_SEQUENCE_NEXT_RESULT_FAIL_EMPTY_CELL:
				case BALDA_SEQUENCE_NEXT_RESULT_FAIL_FIELD_BORDER:
					// Just ignore
				break;
			}
		}
		break;
		
		case BALDA_TURN_STAGE_AFTER_AI_TURN:
		{
			// ignore
		}
		break;
	}
	
	balda_view_update(g_balda_controller.view);
}

balda_bool balda_controller_try_insert_char_at_pos(balda_point_t pos)
{
	switch (balda_can_add_letter_at(g_balda_controller.balda, pos.x, pos.y))
	{
		case BALDA_ADD_LETTER_RESULT_OK:
		{
			return 1;
		}
		break;
		
		case BALDA_ADD_LETTER_RESULT_FAIL_NOT_EMPTY_CELL:
		{
			Message(ICON_ERROR, (char*)balda_string(BALDA_STR_NOT_EMPTY_CELL_TITLE),
				(char*)balda_string(BALDA_STR_NOT_EMPTY_CELL_MESSAGE), 5000);
		}
		break;
		
		case BALDA_ADD_LETTER_RESULT_FAIL_NOT_NEAR:
		{
			Message(ICON_ERROR, (char*)balda_string(BALDA_STR_NOT_NEAR_TITLE),
				(char*)balda_string(BALDA_STR_NOT_NEAR_MESSAGE), 5000);
		}
		break;
	}
	
	return 0;
}

balda_bool balda_controller_sequence_start(balda_point_t pos)
{
	BALDA_SEQUENCE_START_RESULT result = balda_turn_sequence_start(g_balda_controller.balda,
		pos,
		balda_field_view_get_insert_char_pos(g_balda_controller.field_view),
		balda_field_view_get_insert_char(g_balda_controller.field_view),
		&g_balda_controller.sequence);
		
	switch (result)
	{
		case BALDA_SEQUENCE_START_RESULT_OK:
			// everything fine continue defining word sequence
			balda_field_view_load_sequence(g_balda_controller.field_view,
				&g_balda_controller.sequence);
			return 1;
		break;
		
		case BALDA_SEQUENCE_START_RESULT_FAIL_EMPTY_CELL:
			// can't start in empty cell
			Message(ICON_ERROR, (char*)balda_string(BALDA_STR_EMPTY_CELL_TITLE),
				(char*)balda_string(BALDA_STR_EMPTY_CELL_MESSAGE), 5000);
		break;
	}
	
	return 0;
}

balda_bool balda_controller_try_make_turn()
{
	BALDA_TURN_RESULT result = balda_make_turn(g_balda_controller.balda, &g_balda_controller.sequence);
							
	switch (result)
	{
		case BALDA_TURN_RESULT_OK:
		{
			balda_controller_next_turn(0);
			return 1;
		}
		break;
		
		case BALDA_TURN_RESULT_WORD_ALREADY_USED:
		{
			static char utf8_buffer[51];
			static char msg_buffer[512];
			balda_char_to_utf8(balda_sequence_get_word(&g_balda_controller.sequence, g_balda_controller.balda),
				utf8_buffer, sizeof(utf8_buffer));
			//printf("buf: %s\n", utf8_buffer);
			sprintf(msg_buffer, balda_string(BALDA_STR_WORD_ALREADY_USED_MESSAGE), utf8_buffer);
			
			Message(ICON_ERROR, (char*)balda_string(BALDA_STR_WORD_ALREADY_USED_TITLE),
				msg_buffer, 5000);
		}
		break;
		
		case BALDA_TURN_RESULT_INSERT_CHAR_NOT_SELECTED:
		{
			Message(ICON_ERROR, (char*)balda_string(BALDA_STR_INSERT_CHAR_NOT_SELECTED_TITLE),
				(char*)balda_string(BALDA_STR_INSERT_CHAR_NOT_SELECTED_MESSAGE), 5000);
		}
		break;
		
		case BALDA_TURN_RESULT_TOO_SHORT:
		{
			Message(ICON_ERROR, (char*)balda_string(BALDA_STR_TOO_SHORT_TITLE),
				(char*)balda_string(BALDA_STR_TOO_SHORT_MESSAGE), 5000);
		}
		break;
		
		case BALDA_TURN_RESULT_WORD_NOT_FOUND:
		{
			static char utf8_buffer[51];
			static char msg_buffer[256];
			balda_char_to_utf8(balda_sequence_get_word(&g_balda_controller.sequence, g_balda_controller.balda),
				utf8_buffer, sizeof(utf8_buffer));
			//printf("buf: %s\n", utf8_buffer);
			sprintf(msg_buffer, balda_string(BALDA_STR_UNKNOWN_WORD_MESSAGE), utf8_buffer);
			
			Message(ICON_ERROR, (char*)balda_string(BALDA_STR_UNKNOWN_WORD_TITLE),
				msg_buffer, 5000);
		}
		break;
	}
	
	return 0;
}

void balda_controller_back_from_letter_select()
{
	balda_field_view_select(g_balda_controller.field_view, g_balda_controller.field_pos);
	balda_field_view_remove_insert_char(g_balda_controller.field_view);
	balda_keyboard_view_clear_selection(g_balda_controller.kb_view);
	
	balda_button_set_mode(g_balda_controller.btn_cancel, BALDA_BUTTON_MODE_SURRENDER);
	balda_button_redraw(g_balda_controller.btn_cancel, 0);
	
	g_balda_controller.turn_stage = BALDA_TURN_STAGE_SELECT_POS;
}

void balda_controller_back_from_select_first()
{
	balda_keyboard_view_select(g_balda_controller.kb_view,
		g_balda_controller.kb_pos.x, g_balda_controller.kb_pos.y);
	
	balda_button_set_selected(g_balda_controller.btn_cancel, 0);
	balda_button_redraw(g_balda_controller.btn_cancel, 0);
	g_balda_controller.field_pos = balda_field_view_get_insert_char_pos(g_balda_controller.field_view);
	
	g_balda_controller.turn_stage = BALDA_TURN_STAGE_SELECT_LETTER;
}

void balda_controller_back_from_define_word()
{
	balda_sequence_reset(&g_balda_controller.sequence);
	balda_field_view_replace_sequence_with_select(g_balda_controller.field_view,
		g_balda_controller.field_pos);
	
	g_balda_controller.turn_stage = BALDA_TURN_STAGE_SELECT_FIRST;
}

void balda_controller_on_key_pressed(int key)
{
	if (key != KEY_MENU && balda_is_game_over(g_balda_controller.balda))
	{
		// restart game on button press
		balda_controller_new_game(balda_get_game_type(g_balda_controller.balda));
		return;
	}
	
	if (key != KEY_MENU && g_balda_controller.turn_stage == BALDA_TURN_STAGE_AFTER_AI_TURN)
	{
		balda_controller_next_turn(0);
		return;
	}
	
	if (g_balda_controller.ui_type == BALDA_UI_TYPE_KEYS)
	{
		switch (key)
		{
			case KEY_MENU:
				OpenMenu(main_menu, 0, 10, 10, balda_controller_main_menu_handler);
			break;
			
			case KEY_UP: case KEY_DOWN: case KEY_LEFT: case KEY_RIGHT:
				balda_controller_on_cursor_key_pressed(key);
			break;
			
			/*case KEY_BACK:
			{
				CloseApp();
			}
			break;*/
			
			case KEY_PREV:
			{
				switch (g_balda_controller.turn_stage)
				{
					case BALDA_TURN_STAGE_SELECT_POS:
						// do nothing
					break;
					
					case BALDA_TURN_STAGE_SELECT_LETTER:
					{
						// Go back to select pos
						balda_controller_back_from_letter_select();
					}
					break;
					
					case BALDA_TURN_STAGE_SELECT_FIRST:
					{
						balda_controller_back_from_select_first();
					}
					break;
					
					case BALDA_TURN_STAGE_DEFINE_WORD:
					{
						balda_controller_back_from_define_word();
					}
					break;
					
					case BALDA_TURN_STAGE_AFTER_AI_TURN:
					{
						// ignore
					}
					break;
				}
			}
			break;
			
			case KEY_OK:
			{
				switch (g_balda_controller.turn_stage)
				{
					case BALDA_TURN_STAGE_SELECT_POS:
					{
						// Has user selected a position to insert letter?
						if (g_balda_controller.field_pos.y == BALDA_FIELD_HEIGHT)
						{
							// Surrender selected, end game
							balda_surrender(g_balda_controller.balda);
							balda_controller_on_game_over();
						}
						else
						{
							// Letter insert place selected
							// Check if it is correct
							balda_point_t pos = balda_field_view_get_selection(g_balda_controller.field_view);
							
							if (balda_controller_try_insert_char_at_pos(pos))
							{
								g_balda_controller.turn_stage = BALDA_TURN_STAGE_SELECT_LETTER;
								balda_keyboard_view_select(g_balda_controller.kb_view, 5, 1); // keyboard center
								
								balda_field_view_deselect(g_balda_controller.field_view);
								balda_field_view_set_insert_char(g_balda_controller.field_view,
									g_balda_controller.field_pos,
									balda_keyboard_view_get_selected_char(g_balda_controller.kb_view));
								
								/*balda_view_field_select_inserting(g_balda_controller.view, pos,
									balda_view_keyboard_get_selected_char(g_balda_controller.view));*/
							}
						}
					}
					break;
					
					case BALDA_TURN_STAGE_SELECT_LETTER:
					{
						g_balda_controller.kb_pos = balda_keyboard_view_get_selection(g_balda_controller.kb_view);
						if (balda_keyboard_view_is_back_selected(g_balda_controller.kb_view))
						{
							// Go back to select pos
							balda_controller_back_from_letter_select();
						}
						else
						{
							// User has selected a character
							// Now he should select first letter in a word
							g_balda_controller.turn_stage = BALDA_TURN_STAGE_SELECT_FIRST;
							
							balda_field_view_select(g_balda_controller.field_view, g_balda_controller.field_pos);
							balda_keyboard_view_clear_selection(g_balda_controller.kb_view);
							
							balda_button_set_mode(g_balda_controller.btn_cancel, BALDA_BUTTON_MODE_BACK);
							balda_button_redraw(g_balda_controller.btn_cancel, 0);
						}
						
						balda_view_update(g_balda_controller.view);
					}
					break;
					
					case BALDA_TURN_STAGE_SELECT_FIRST:
					{
						// Has user selected a position to start sequnce?
						if (g_balda_controller.field_pos.y == BALDA_FIELD_HEIGHT)
						{
							// No, "Back" was pressed
							balda_controller_back_from_select_first();
						}
						else
						{
							// User has selected a first character in a sequence
							// Now he should define a word
							
							if (balda_controller_sequence_start(balda_field_view_get_selection(g_balda_controller.field_view)))
								g_balda_controller.turn_stage = BALDA_TURN_STAGE_DEFINE_WORD;
						}
					}
					break;
					
					case BALDA_TURN_STAGE_DEFINE_WORD:
					{
						if (balda_sequence_length(&g_balda_controller.sequence) > 1)
						{
							// Try make turn
							balda_controller_try_make_turn();
						}
						else
						{
							// Cancel selecting
							balda_controller_back_from_define_word();
						}
					}
					break;
					
					case BALDA_TURN_STAGE_AFTER_AI_TURN:
					{
						// ignore
					}
					break;
				}
				
				balda_view_update(g_balda_controller.view);
			}
			break;
		}
	}
}

void balda_controller_after_touchscreen_pos_selection()
{
	balda_field_view_set_insert_char(g_balda_controller.field_view,
		balda_field_view_get_selection(g_balda_controller.field_view),
		balda_keyboard_view_get_selected_char(g_balda_controller.kb_view));
		
	balda_button_set_mode(g_balda_controller.btn_confirm, BALDA_BUTTON_MODE_NEXT);
	balda_button_set_visible(g_balda_controller.btn_confirm, 1);
	balda_button_redraw(g_balda_controller.btn_confirm, 0);
}

void balda_controller_on_pointer_pressed(balda_point_t p)
{
	switch (g_balda_controller.turn_stage)
	{
		case BALDA_TURN_STAGE_SELECT_POS:
		case BALDA_TURN_STAGE_SELECT_LETTER:
		{
			balda_point_t pos;
			
			if (balda_field_view_get_cell_at_point(g_balda_controller.field_view, p, &pos))
			{
				// select field view cell
				if (balda_controller_try_insert_char_at_pos(pos))
				{
					balda_field_view_select(g_balda_controller.field_view, pos);
					
					if (balda_keyboard_view_is_selected(g_balda_controller.kb_view))
					{
						balda_controller_after_touchscreen_pos_selection();
					}
				}
			}
			else if (balda_keyboard_view_get_button_at_point(g_balda_controller.kb_view, p, &pos))
			{
				balda_keyboard_view_select_point(g_balda_controller.kb_view, pos);
				
				if (balda_field_view_is_selected(g_balda_controller.field_view))
				{
					balda_controller_after_touchscreen_pos_selection();
				}
			}
		}
		break;
		
		case BALDA_TURN_STAGE_SELECT_FIRST:
		{
			balda_point_t pos;
			
			if (balda_field_view_get_cell_at_point(g_balda_controller.field_view, p, &pos))
			{
				if (balda_controller_sequence_start(pos))
				{
					g_balda_controller.turn_stage = BALDA_TURN_STAGE_DEFINE_WORD;
				}
			}
		}
		break;
		
		case BALDA_TURN_STAGE_DEFINE_WORD:
		{
			balda_point_t pos;
			
			if (balda_field_view_get_cell_at_point(g_balda_controller.field_view, p, &pos))
			{
				debug_printf(("balda_field_view_get_cell_at_point (%d, %d)", pos.x, pos.y));
				if (balda_get_letter_at(g_balda_controller.balda, pos.x, pos.y) != BALDA_CHAR_NONE ||
					balda_points_equal(pos, g_balda_controller.sequence.insert_pos))
				{
					debug_printf(("OK"));
					
					int i, x, dx = 0, y, dy = 0;
					balda_sequence_entry_t e;
					balda_bool found = 0;
					balda_point_t p;
					
					for (i = 0; i<g_balda_controller.sequence.length; ++i)
					{
						if (balda_points_equal(g_balda_controller.sequence.entries[i].pos, pos))
						{
							g_balda_controller.sequence.length = i+1;
							balda_field_view_load_sequence(g_balda_controller.field_view,
								&g_balda_controller.sequence);
							
							found = 1;
							break;
						}
					}
					
					if (!found)
					{
						for (i = g_balda_controller.sequence.length-1; i>=0; --i)
						{
							e = g_balda_controller.sequence.entries[i];
							if (e.pos.x == pos.x || e.pos.y == pos.y)
							{
								found = 1;
								if (e.pos.y == pos.y)
								{
									dx = (pos.x > e.pos.x) ? 1 : -1;
									for (x = e.pos.x + dx; x != pos.x; x += dx)
									{
										debug_printf(("x: %d", x));
										debug_printf(("y: %d", pos.y));
										p = balda_make_point(x, pos.y);
										found = (balda_get_letter_at(g_balda_controller.balda, x, pos.y) != BALDA_CHAR_NONE ||
												balda_points_equal(p, g_balda_controller.sequence.insert_pos))
											&& !balda_sequence_contains_point(&g_balda_controller.sequence, x, pos.y);
											
										if (!found)
											break;
									}
								}
								else if (e.pos.x == pos.x)
								{
									dy = (pos.y > e.pos.y) ? 1 : -1;
									for (y = e.pos.y + dy; y != pos.y; y += dy)
									{
										debug_printf(("x: %d", pos.x));
										debug_printf(("y: %d", y));
										p = balda_make_point(pos.x, y);
										found = (balda_get_letter_at(g_balda_controller.balda, pos.x, y) != BALDA_CHAR_NONE ||
												balda_points_equal(p, g_balda_controller.sequence.insert_pos))
											&& !balda_sequence_contains_point(&g_balda_controller.sequence, pos.x, y);
										
										if (!found)
											break;
									}
								}
								
								if (found)
								{
									debug_printf(("found: (%d, %d)", e.pos.x, e.pos.y));
									
									// found
									g_balda_controller.sequence.length = i+1;
									
									if (e.pos.y == pos.y)
									{
										for (x = e.pos.x + dx; ; x += dx)
										{
											debug_printf((" (%d, %d)", x, pos.y));
											
											g_balda_controller.sequence.entries[g_balda_controller.sequence.length].pos = 
												balda_make_point(x, pos.y);
											++g_balda_controller.sequence.length;
											
											if (x == pos.x)
												break;
										}
									}
									else if (e.pos.x == pos.x)
									{
										for (y = e.pos.y + dy; ; y += dy)
										{
											g_balda_controller.sequence.entries[g_balda_controller.sequence.length].pos = 
												balda_make_point(pos.x, y);
											++g_balda_controller.sequence.length;
											
											if (y == pos.y)
												break;
										}
									}
									
									balda_field_view_load_sequence(g_balda_controller.field_view,
										&g_balda_controller.sequence);
									
									break;
								}
							}
						}
					}
					
					balda_button_set_mode(g_balda_controller.btn_confirm, BALDA_BUTTON_MODE_CONFIRM);
					balda_button_set_visible(g_balda_controller.btn_confirm, g_balda_controller.sequence.length > 1);
					balda_button_redraw(g_balda_controller.btn_confirm, 0);
				}
			}
		}
		break;
		
		case BALDA_TURN_STAGE_AFTER_AI_TURN:
		{
			// do nothing, handled earlier
		}
		break;
	}
	
	balda_view_update(g_balda_controller.view);
}

void balda_controller_on_button_pressed(balda_button_t* button)
{
	debug_printf(("balda_controller_on_button_pressed"));
	
	switch (g_balda_controller.turn_stage)
	{
		case BALDA_TURN_STAGE_SELECT_POS:
		case BALDA_TURN_STAGE_SELECT_LETTER:
		{
			if (button == g_balda_controller.btn_cancel)
			{
				balda_button_set_visible(g_balda_controller.btn_cancel, 0);
				balda_button_set_visible(g_balda_controller.btn_confirm, 0);
				balda_button_redraw(g_balda_controller.btn_cancel, 0);
				balda_button_redraw(g_balda_controller.btn_confirm, 0);
				
				// surrender
				balda_surrender(g_balda_controller.balda);
				balda_controller_on_game_over();
			}
			else if (button == g_balda_controller.btn_confirm)
			{
				g_balda_controller.turn_stage = BALDA_TURN_STAGE_SELECT_FIRST;
				
				balda_field_view_deselect(g_balda_controller.field_view);
				
				/*balda_button_set_mode(g_balda_controller.btn_confirm, BALDA_BUTTON_MODE_CONFIRM);
				balda_button_set_selected(g_balda_controller.btn_confirm, 0);
				balda_button_redraw(g_balda_controller.btn_confirm, 0);*/

				balda_button_set_selected(g_balda_controller.btn_confirm, 0);
				balda_button_set_visible(g_balda_controller.btn_confirm, 0);
				balda_button_redraw(g_balda_controller.btn_confirm, 0);
				
				balda_button_set_mode(g_balda_controller.btn_cancel, BALDA_BUTTON_MODE_BACK);
				balda_button_redraw(g_balda_controller.btn_cancel, 0);
			}
		}
		break;
		
		case BALDA_TURN_STAGE_SELECT_FIRST:
		case BALDA_TURN_STAGE_DEFINE_WORD:
		{
			if (button == g_balda_controller.btn_cancel)
			{
				g_balda_controller.turn_stage = BALDA_TURN_STAGE_SELECT_POS;
				
				balda_field_view_clear_sequence(g_balda_controller.field_view, 1);
				balda_field_view_select(g_balda_controller.field_view,
					balda_field_view_get_insert_char_pos(g_balda_controller.field_view));
				
				balda_button_set_mode(g_balda_controller.btn_confirm, BALDA_BUTTON_MODE_NEXT);
				balda_button_set_visible(g_balda_controller.btn_confirm, 1);
				balda_button_redraw(g_balda_controller.btn_confirm, 0);
				
				balda_button_set_mode(g_balda_controller.btn_cancel, BALDA_BUTTON_MODE_SURRENDER);
				balda_button_set_selected(g_balda_controller.btn_cancel, 0);
				balda_button_redraw(g_balda_controller.btn_cancel, 0);
			}
			else if (button == g_balda_controller.btn_confirm
				&& g_balda_controller.turn_stage == BALDA_TURN_STAGE_DEFINE_WORD)
			{
				balda_controller_try_make_turn();
				
				balda_button_set_selected(g_balda_controller.btn_confirm, 0);
				balda_button_redraw(g_balda_controller.btn_confirm, 0);
			}
		}
		break;
		
		case BALDA_TURN_STAGE_AFTER_AI_TURN:
		{
			// do nothing, handled earlier
		}
		break;
	}
	
	balda_view_update(g_balda_controller.view);
}

void balda_controller_on_pointer_event(int type, int x, int y)
{
	switch (type)
	{
		case EVT_POINTERDOWN:
		{
			if (balda_button_contains_point(g_balda_controller.btn_cancel, balda_make_point(x, y)))
			{
				g_balda_controller.pointer_down_button = g_balda_controller.btn_cancel;
			}
			else if (balda_button_contains_point(g_balda_controller.btn_confirm, balda_make_point(x, y)))
			{
				g_balda_controller.pointer_down_button = g_balda_controller.btn_confirm;
			}
			else
			{
				g_balda_controller.pointer_pressed = 1;
				g_balda_controller.pointer_pressed_at = balda_make_point(x, y);
			}
			
			if (g_balda_controller.pointer_down_button)
			{
				balda_button_set_selected(g_balda_controller.pointer_down_button, 1);
				balda_button_redraw(g_balda_controller.pointer_down_button, 0);
				
				balda_view_update(g_balda_controller.view);
			}
		}
		break;
		
		case EVT_POINTERMOVE:
		{
			if (g_balda_controller.pointer_pressed)
			{
				if (balda_point_distance_squared(x, y, g_balda_controller.pointer_pressed_at.x,
					g_balda_controller.pointer_pressed_at.y) > BALDA_POINTER_PRESSED_THRESHOLD)
				{
					// cancel pressing, moved pointer too far
					g_balda_controller.pointer_pressed = 0;
				}
			}
			else if (g_balda_controller.pointer_down_button)
			{
				balda_button_set_selected(g_balda_controller.pointer_down_button,
					balda_button_contains_point(g_balda_controller.pointer_down_button, balda_make_point(x, y)));
				balda_button_redraw(g_balda_controller.pointer_down_button, 0);
				
				balda_view_update(g_balda_controller.view);
			}
		}
		break;
		
		case EVT_POINTERUP:
		{
			if (balda_is_game_over(g_balda_controller.balda))
			{
				// restart game on button press
				balda_controller_new_game(balda_get_game_type(g_balda_controller.balda));
				return;
			}
			
			if (g_balda_controller.turn_stage == BALDA_TURN_STAGE_AFTER_AI_TURN)
			{
				balda_controller_next_turn(0);
				return;
			}
			
			if (g_balda_controller.pointer_pressed)
			{
				g_balda_controller.pointer_pressed = 0;
				balda_controller_on_pointer_pressed(g_balda_controller.pointer_pressed_at);
			}
			else if (g_balda_controller.pointer_down_button)
			{
				if (balda_button_contains_point(g_balda_controller.pointer_down_button, balda_make_point(x, y)))
				{
					balda_controller_on_button_pressed(g_balda_controller.pointer_down_button);
				}
				
				g_balda_controller.pointer_down_button = 0;
			}
		}
		break;
		
		case EVT_POINTERLONG:
		{
			if (g_balda_controller.pointer_down_button)
			{
				balda_button_set_selected(g_balda_controller.pointer_down_button, 0);
				balda_button_redraw(g_balda_controller.pointer_down_button, 0);
				g_balda_controller.pointer_down_button = 0;
			}
			g_balda_controller.pointer_pressed = 0;
			
			OpenMenu(main_menu, 0, x, y, balda_controller_main_menu_handler);
		}
		break;
		
		case EVT_POINTERHOLD:
		{
			// cancel pressing, too long pressure
			g_balda_controller.pointer_pressed = 0;
		}
		break;
	}
}

int balda_controller_handler(int type, int par1, int par2)
{
	switch (type)
	{
		case EVT_INIT:
			balda_controller_on_evt_init();
		break;
		
		case EVT_SHOW:
			balda_controller_on_evt_show();
		break;
		
		case EVT_POINTERUP: case EVT_POINTERDOWN: case EVT_POINTERMOVE:
		case EVT_POINTERLONG: case EVT_POINTERHOLD:
		{
			if (g_balda_controller.ui_type == BALDA_UI_TYPE_TOUCHSCREEN)
				balda_controller_on_pointer_event(type, par1, par2);
		}
		break;
		
		case EVT_KEYPRESS:
			if (par1 != KEY_OK)
			{
				balda_controller_on_key_pressed(par1);
			}
		break;
		
		case EVT_KEYREPEAT:
			if (par1 == KEY_OK)
			{
				balda_controller_on_key_pressed(KEY_MENU);
			}
		break;
		
		case EVT_KEYRELEASE:
			if (par1 == KEY_OK && par2 == 0)
			{
				balda_controller_on_key_pressed(KEY_OK);
			}
		break;
		
		default:
		break;
	}
	
	return 0;
}
