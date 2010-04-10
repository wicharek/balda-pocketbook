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

#ifndef __BALDA_BUTTON_H__
#define __BALDA_BUTTON_H__

#include "balda_utils.h"
#include "balda_view.h"

static const int BALDA_BUTTON_WIDTH = 208;
static const int BALDA_BUTTON_HEIGHT = 40;
static const int BALDA_BUTTON_MARGIN = 1;
static const int BALDA_BUTTON_ICON_PADDING = 16;
static const int BALDA_BUTTON_ICON_CAPTIONS_OFFSET = 12;

struct balda_button_t_impl;
typedef struct balda_button_t_impl balda_button_t;

typedef enum
{
	BALDA_BUTTON_MODE_SURRENDER,
	BALDA_BUTTON_MODE_BACK,
	BALDA_BUTTON_MODE_CONFIRM,
	BALDA_BUTTON_MODE_NEXT
} BALDA_BUTTON_MODE;

balda_button_t* balda_button_init(balda_view_t* view, balda_point_t pos);
void balda_button_free(balda_button_t* button);

balda_bool balda_button_contains_point(balda_button_t* button, balda_point_t point);

void balda_button_set_mode(balda_button_t* button, BALDA_BUTTON_MODE mode);
void balda_button_set_visible(balda_button_t* button, balda_bool visible);
void balda_button_set_selected(balda_button_t* button, balda_bool selected);

void balda_button_redraw(balda_button_t* button, balda_bool force);

#endif
