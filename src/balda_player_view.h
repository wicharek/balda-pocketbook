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

#ifndef __BALDA_PLAYER_VIEW_H__
#define __BALDA_PLAYER_VIEW_H__

#include "balda.h"
#include "balda_view.h"
#include "balda_utils.h"

struct balda_player_view_t_impl;
typedef struct balda_player_view_t_impl balda_player_view_t;

balda_player_view_t* balda_player_view_init(balda_view_t* view, balda_t* balda, int index);
void balda_player_view_free(balda_player_view_t* pv);
void balda_player_view_draw(balda_player_view_t* pv);

void balda_player_view_update_name(balda_player_view_t* pv);
void balda_player_view_update_active(balda_player_view_t* pv);
void balda_player_view_update_score(balda_player_view_t* pv);
void balda_player_view_draw_last_word(balda_player_view_t* pv);

#endif
