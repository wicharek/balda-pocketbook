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

#ifndef __BALDA_KEYBOARD_VIEW_H__
#define __BALDA_KEYBOARD_VIEW_H__

#include "balda_view.h"
#include "balda_utils.h"

struct balda_keyboard_view_t_impl;
typedef struct balda_keyboard_view_t_impl balda_keyboard_view_t;

balda_keyboard_view_t* balda_keyboard_view_init(balda_view_t* view, balda_bool has_cancel_button);
void balda_keyboard_view_free(balda_keyboard_view_t* kb);
void balda_keyboard_view_draw(balda_keyboard_view_t* kb);

int balda_keyboard_view_x(balda_keyboard_view_t* kb);
int balda_keyboard_view_y(balda_keyboard_view_t* kb);
int balda_keyboard_view_w(balda_keyboard_view_t* kb);
int balda_keyboard_view_h(balda_keyboard_view_t* kb);

balda_bool balda_keyboard_view_get_button_at_point(balda_keyboard_view_t* kb, balda_point_t point,
	balda_point_t* out_button_pos);

void balda_keyboard_view_set_keyboard(balda_keyboard_view_t* kb, const balda_char* keyboard);
void balda_keyboard_view_clear_selection(balda_keyboard_view_t* kb);
void balda_keyboard_view_select(balda_keyboard_view_t* kb, int button_x, int button_y);
void balda_keyboard_view_select_point(balda_keyboard_view_t* kb, balda_point_t pos);
void balda_keyboard_view_select_delta(balda_keyboard_view_t* kb, int dx, int dy);
balda_bool balda_keyboard_view_is_selected(balda_keyboard_view_t* kb);
balda_char balda_keyboard_view_get_selected_char(balda_keyboard_view_t* kb);
balda_point_t balda_keyboard_view_get_selection(balda_keyboard_view_t* kb);
balda_bool balda_keyboard_view_is_back_selected(balda_keyboard_view_t* kb);

#endif
