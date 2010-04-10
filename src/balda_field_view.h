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

#ifndef __BALDA_FIELD_VIEW_H__
#define __BALDA_FIELD_VIEW_H__

#include "balda.h"
#include "balda_view.h"
#include "balda_utils.h"

struct balda_field_view_t_impl;
typedef struct balda_field_view_t_impl balda_field_view_t;

balda_field_view_t* balda_field_view_init(balda_view_t* view);
void balda_field_view_free(balda_field_view_t* field);
void balda_field_view_reset(balda_field_view_t* field);
void balda_field_view_redraw(balda_field_view_t* field);

void balda_field_view_load(balda_field_view_t* field, balda_t* balda);
void balda_field_view_load_insert_select(balda_field_view_t* field, balda_t* balda,
	balda_bool selected, balda_point_t selected_pos,
	balda_char insert_char, balda_point_t insert_pos);
	
balda_bool balda_field_view_get_cell_at_point(balda_field_view_t* field, balda_point_t point,
	balda_point_t* out_cell_pos);

void balda_field_view_select(balda_field_view_t* field, balda_point_t pos);
void balda_field_view_deselect(balda_field_view_t* field);
balda_bool balda_field_view_is_selected(balda_field_view_t* field);
balda_point_t balda_field_view_get_selection(balda_field_view_t* field);

void balda_field_view_set_insert_char(balda_field_view_t* field, balda_point_t pos, balda_char c);
void balda_field_view_remove_insert_char(balda_field_view_t* field);
balda_bool balda_field_view_has_insert_char(balda_field_view_t* field);
balda_char balda_field_view_get_insert_char(balda_field_view_t* field);
balda_point_t balda_field_view_get_insert_char_pos(balda_field_view_t* field);

void balda_field_view_load_sequence(balda_field_view_t* field, balda_sequence_t* sequence);
void balda_field_view_replace_sequence_with_select(balda_field_view_t* field, balda_point_t pos);
void balda_field_view_clear_sequence(balda_field_view_t* field, balda_bool keep_insert_char);

#endif
