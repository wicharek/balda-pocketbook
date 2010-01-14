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
#include "balda_view.h"
#include "balda_strings.h"

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

static imenu main_menu[] = {
	{ ITEM_HEADER, 0, BALDA_STR_MENU_HEADER, NULL },
	{ ITEM_ACTIVE, 101, BALDA_STR_MENU_RESUME, NULL },
	{ ITEM_SUBMENU, 110, BALDA_STR_MENU_NEW_GAME, new_game_menu },
	{ ITEM_ACTIVE, 150, BALDA_STR_MENU_EXIT, NULL },
	
	{ 0, 0, NULL, NULL }
};

typedef struct balda_controller_t
{
	balda_t* balda;
	balda_view_t* view;
	
	BALDA_TURN_STAGE turn_stage;
	
	balda_char ytsuken_keyboard_ru[33];
	balda_char abvgde_keyboard_ru[33];
} balda_controller_t;

static balda_controller_t g_balda_controller;


void balda_controller_init(balda_t* balda)
{
	g_balda_controller.balda = balda;
	
	utf2ucs(balda_ytsuken_keyboard_ru, g_balda_controller.ytsuken_keyboard_ru, 33);
	utf2ucs(balda_abvgde_keyboard_ru, g_balda_controller.abvgde_keyboard_ru, 33);
}

void balda_controller_next_turn()
{
	//int active_player = balda_get_active_player(g_balda_controller.balda);
	balda_view_show_turn(g_balda_controller.view);
	
	if (balda_is_active_player_ai(g_balda_controller.balda))
	{
		ShowHourglass();
		balda_make_ai_turn(g_balda_controller.balda);
		HideHourglass();
		
		// TODO: Redraw field
		
		balda_controller_next_turn();
	}
	else
	{
		g_balda_controller.turn_stage = BALDA_TURN_STAGE_SELECT_POS;
		balda_view_field_select_single(g_balda_controller.view,
			balda_make_point(BALDA_FIELD_WIDTH / 2, BALDA_FIELD_HEIGHT / 2));
	}
}

void balda_controller_on_game_started()
{
	balda_view_draw_all(g_balda_controller.view);
	balda_view_reset(g_balda_controller.view);
	FullUpdate();
	FineUpdate();
	
	balda_controller_next_turn();
}

void balda_controller_on_evt_init()
{
	g_balda_controller.view = balda_view_init(g_balda_controller.balda);
	
	balda_view_set_keyboard(g_balda_controller.view, g_balda_controller.ytsuken_keyboard_ru);
	
	balda_new_game(g_balda_controller.balda, BALDA_GAME_TYPE_ONE_PLAYER);
	balda_set_player_name(g_balda_controller.balda, 0, balda_string(BALDA_STR_DEFAULT_PLAYER_NAME_0));
	balda_set_player_name(g_balda_controller.balda, 1, balda_string(BALDA_STR_AI_PLAYER_NAME));
}

void balda_controller_on_evt_show()
{
	ClearScreen();
	FullUpdate();
	
	balda_controller_on_game_started(1);
}

void balda_controller_main_menu_handler(int index)
{
	switch (index)
	{
		case 101: // Resume
			// Do nothing
		break;
		
		case 111: // New game -> Single player
			// TODO
		break;
		
		case 112: // New game -> Two players
			// TODO
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
		{
			balda_view_field_select_single_delta(g_balda_controller.view, dx, dy);
		}
		break;
		
		case BALDA_TURN_STAGE_SELECT_LETTER:
		{
			balda_view_keyboard_select_delta(g_balda_controller.view, dx, dy);
			balda_view_field_select_inserting(g_balda_controller.view,
				balda_view_field_get_selection(g_balda_controller.view),
				balda_view_keyboard_get_selected_char(g_balda_controller.view));
		}
		break;
		
		case BALDA_TURN_STAGE_SELECT_FIRST:
		{
			balda_view_field_select_sequence_first_delta(g_balda_controller.view, dx, dy);
		}
		break;
		
		case BALDA_TURN_STAGE_DEFINE_WORD:
		{
			BALDA_SEQUENCE_NEXT_RESULT result = balda_sequence_next(g_balda_controller.balda, direction);
			
			switch (result)
			{
				case BALDA_SEQUENCE_NEXT_RESULT_OK:
					balda_view_field_select_sequence_delta(g_balda_controller.view, direction);
				break;
				
				case BALDA_SEQUENCE_NEXT_RESULT_FAIL_ALREADY_SELECTED:
				{
					// Check if this cell is previously selected
					balda_point_t prev_pos = balda_sequence_prelast_selected(g_balda_controller.balda);
					balda_point_t last_pos = balda_sequence_last_selected(g_balda_controller.balda);
					balda_point_t next_pos = balda_make_point(last_pos.x + dx, last_pos.y + dy);
					
					printf("prev_pos: { %d, %d } next_pos: { %d, %d }\n",
						prev_pos.x, prev_pos.y, next_pos.x, next_pos.y);
					
					if (balda_points_equal(prev_pos, next_pos))
					{
						// Rollback selection then
						balda_sequence_rollback_last(g_balda_controller.balda);
						balda_view_field_select_sequence_delta(g_balda_controller.view, direction);
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
	}
}

void balda_controller_on_key_pressed(int key)
{
	switch (key)
	{
		case KEY_MENU:
			OpenMenu(main_menu, 0, 10, 60, balda_controller_main_menu_handler);
		break;
		
		case KEY_UP: case KEY_DOWN: case KEY_LEFT: case KEY_RIGHT:
			balda_controller_on_cursor_key_pressed(key);
		break;
		
		case KEY_OK:
		{
			switch (g_balda_controller.turn_stage)
			{
				case BALDA_TURN_STAGE_SELECT_POS:
				{
					// User has selected a position to insert letter
					
					if (balda_view_is_cancel_selected(g_balda_controller.view))
					{
						// Surrender selected, end game
						balda_surrender(g_balda_controller.balda);
						
						// TODO
					}
					else
					{
						// Letter insert place selected
						// Check if it is correct
						balda_point_t pos = balda_view_field_get_selection(g_balda_controller.view);
						
						switch (balda_can_add_letter_at(g_balda_controller.balda, pos.x, pos.y))
						{
							case BALDA_ADD_LETTER_RESULT_OK:
							{
								g_balda_controller.turn_stage = BALDA_TURN_STAGE_SELECT_LETTER;
								balda_view_keyboard_select(g_balda_controller.view, 5, 1); // kayboard center
								balda_view_field_select_inserting(g_balda_controller.view, pos,
									balda_view_keyboard_get_selected_char(g_balda_controller.view));
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
					}
				}
				break;
				
				case BALDA_TURN_STAGE_SELECT_LETTER:
				{
					if (balda_view_keyboard_is_back_selected(g_balda_controller.view))
					{
						// Go back to pos selection
						balda_point_t sel_pos = balda_view_field_get_selection(g_balda_controller.view);
						balda_view_field_select_single(g_balda_controller.view, sel_pos);
						balda_view_keyboard_clear_selection(g_balda_controller.view);
						
						g_balda_controller.turn_stage = BALDA_TURN_STAGE_SELECT_POS;
					}
					else
					{
						// User has selected a character
						// Now he should select first letter in a word
						g_balda_controller.turn_stage = BALDA_TURN_STAGE_SELECT_FIRST;
						
						balda_char insert_char = balda_view_keyboard_get_selected_char(g_balda_controller.view);
						balda_point_t insert_point = balda_view_field_get_selection(g_balda_controller.view);
						
						balda_view_keyboard_clear_selection(g_balda_controller.view);
						balda_view_field_select_sequence_first(g_balda_controller.view, insert_point,
							insert_point, insert_char);
					}
				}
				break;
				
				case BALDA_TURN_STAGE_SELECT_FIRST:
				{
					// User has selected a first character in a sequence
					// Now he should define a word
					
					BALDA_SEQUENCE_START_RESULT result = balda_sequence_start(g_balda_controller.balda,
						balda_view_field_get_selection(g_balda_controller.view),
						balda_view_field_get_insert_pos(g_balda_controller.view),
						balda_view_field_get_insert_char(g_balda_controller.view));
						
					switch (result)
					{
						case BALDA_SEQUENCE_START_RESULT_OK:
							// everything fine continue defining word sequence
							g_balda_controller.turn_stage = BALDA_TURN_STAGE_DEFINE_WORD;
							balda_view_field_select_sequence_first_complete(g_balda_controller.view);
						break;
						
						case BALDA_SEQUENCE_START_RESULT_FAIL_EMPTY_CELL:
							// can't start in empty cell
							Message(ICON_ERROR, (char*)balda_string(BALDA_STR_EMPTY_CELL_TITLE),
								(char*)balda_string(BALDA_STR_EMPTY_CELL_MESSAGE), 5000);
						break;
					}
				}
				break;
			}
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
