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

#ifndef __BALDA_VIEW_H__
#define __BALDA_VIEW_H__

#include "balda.h"
#include "balda_utils.h"

struct balda_view_t_impl;
typedef struct balda_view_t_impl balda_view_t;

typedef enum
{
	BALDA_VIEW_FIELD_SELECT_MODE_NONE,
	BALDA_VIEW_FIELD_SELECT_MODE_SINGLE,
	BALDA_VIEW_FIELD_SELECT_MODE_INSERTING,
	BALDA_VIEW_FIELD_SELECT_MODE_SEQUENCE
} BALDA_VIEW_FIELD_SELECT_MODE;

balda_view_t* balda_view_init(balda_t* balda);
void balda_view_free(balda_view_t*);

void balda_view_show_turn(balda_view_t* view);
void balda_view_set_keyboard(balda_view_t* view, const balda_char* keyboard);

balda_bool balda_view_is_cancel_selected(balda_view_t* view);
balda_point_t balda_view_field_get_selection(balda_view_t* view);
balda_point_t balda_view_field_get_insert_pos(balda_view_t* view);
balda_char balda_view_field_get_insert_char(balda_view_t* view);
void balda_view_field_clear_selection(balda_view_t* view);
void balda_view_field_select_single(balda_view_t* view, balda_point_t point);
void balda_view_field_select_single_delta(balda_view_t* view, int dx, int dy);
void balda_view_field_select_inserting(balda_view_t* view, balda_point_t point, balda_char);

void balda_view_field_select_sequence_first(balda_view_t* view, balda_point_t point,
	balda_point_t insert_point, balda_char insert_char);
void balda_view_field_select_sequence_first_delta(balda_view_t* view, int dx, int dy);
void balda_view_field_select_sequence_first_complete(balda_view_t* view);
void balda_view_field_select_sequence_delta(balda_view_t* view, BALDA_DIRECTION direction);
	
void balda_view_keyboard_clear_selection(balda_view_t* view);
void balda_view_keyboard_select(balda_view_t* view, int x, int y);
void balda_view_keyboard_select_point(balda_view_t* view, balda_point_t pos);
void balda_view_keyboard_select_delta(balda_view_t* view, int dx, int dy);
balda_char balda_view_keyboard_get_selected_char(balda_view_t* view);
balda_point_t balda_view_keyboard_get_selection(balda_view_t* view);
balda_bool balda_view_keyboard_is_back_selected(balda_view_t* view);

void balda_view_reset(balda_view_t* view);

void balda_view_draw_game_over(balda_view_t* view);
void balda_view_draw_score(balda_view_t* view, int player_index, balda_bool update);
void balda_view_draw_player_last_word(balda_view_t* view, int player_index, balda_bool update);
void balda_view_draw_field(balda_view_t* view, balda_bool update);
void balda_view_draw_all(balda_view_t* view);


#endif
