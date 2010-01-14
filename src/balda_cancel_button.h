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

#ifndef __BALDA_CANCEL_BUTTON_H__
#define __BALDA_CANCEL_BUTTON_H__

#include "balda_utils.h"
#include <inkview.h>


static const int BALDA_CANCEL_BUTTON_WIDTH = 208;
static const int BALDA_CANCEL_BUTTON_HEIGHT = 40;
static const int BALDA_CANCEL_BUTTON_MARGIN = 1;
static const int BALDA_CANCEL_BUTTON_ICON_PADDING = 16;
static const int BALDA_CANCEL_BUTTON_ICON_CAPTIONS_OFFSET = 12;

typedef enum
{
	BALDA_CANCEL_BUTTON_MODE_SURRENDER,
	BALDA_CANCEL_BUTTON_MODE_BACK
} BALDA_CANCEL_BUTTON_MODE;

typedef enum
{
	BALDA_CANCEL_BUTTON_REDRAW_NONE = 0,
	BALDA_CANCEL_BUTTON_REDRAW_SELECTED = 0x01,
	BALDA_CANCEL_BUTTON_REDRAW_VISIBLE = 0x02,
	BALDA_CANCEL_BUTTON_REDRAW_MODE = 0x04
} BALDA_CANCEL_BUTTON_REDRAW_FLAG;

typedef enum
{
	BALDA_CANCEL_BUTTON_FORCE_REDRAW_NONE,
	BALDA_CANCEL_BUTTON_FORCE_REDRAW_FULL,
	BALDA_CANCEL_BUTTON_FORCE_REDRAW_NO_CLEAR
} BALDA_CANCEL_BUTTON_FORCE_REDRAW;

typedef struct
{
	balda_point_t pos;
	BALDA_CANCEL_BUTTON_MODE mode;
	balda_bool selected;
	balda_bool visible;
	ifont* font_caption;
	BALDA_CANCEL_BUTTON_REDRAW_FLAG redraw_flags;
} balda_cancel_button_t;

void balda_cancel_button_init(balda_cancel_button_t* button, balda_point_t pos);
void balda_cancel_button_free(balda_cancel_button_t* button);

// These 3 methods. Make the neccesary drawing for button if draw parameter is true.
// They asume that button is currently draw with the previos state.
void balda_cancel_button_set_mode(balda_cancel_button_t* button, BALDA_CANCEL_BUTTON_MODE mode);
void balda_cancel_button_set_visible(balda_cancel_button_t* button, balda_bool visible);
void balda_cancel_button_set_selected(balda_cancel_button_t* button, balda_bool selected);

void balda_cancel_button_clear(balda_cancel_button_t* button);
void balda_cancel_button_draw(balda_cancel_button_t* button, balda_bool update, BALDA_CANCEL_BUTTON_FORCE_REDRAW force);

#endif
