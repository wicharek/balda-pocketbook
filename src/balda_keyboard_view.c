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

#include <inkview.h>
#include <stdlib.h>
#include <assert.h>

#include "balda_keyboard_view.h"
#include "balda_strings.h"

const int BALDA_VIEW_KEYBOARD_Y = 620;
const int BALDA_VIEW_KEYBOARD_CELL_W = 48;
const int BALDA_VIEW_KEYBOARD_CELL_H = 48;
const int BALDA_VIEW_KEYBOARD_CELL_PADDING = 2;
const int BALDA_VIEW_KEYBOARD_W = 11;

extern const ibitmap img_icon_back;

struct balda_keyboard_view_t_impl
{
	balda_view_t* view;
	ifont* font;
	const balda_char* keyboard;
	balda_bool has_cancel_button;

	balda_bool selected;
	balda_point_t selected_pos;
	
	// precalculated coords
	int l, h;
	int kb_size;
};

balda_keyboard_view_t* balda_keyboard_view_init(balda_view_t* view, balda_bool has_cancel_button)
{
	balda_keyboard_view_t* kb = (balda_keyboard_view_t*)malloc(sizeof(balda_keyboard_view_t));
	
	kb->view = view;
	kb->keyboard = 0;
	kb->selected = 0;
	kb->font = OpenFont("LiberationSans-Bold", 28, 0);
	kb->has_cancel_button = has_cancel_button;
	
	kb->l = (ScreenWidth() - BALDA_VIEW_KEYBOARD_CELL_W * BALDA_VIEW_KEYBOARD_W
		- BALDA_VIEW_KEYBOARD_CELL_PADDING * (BALDA_VIEW_KEYBOARD_W - 1)) / 2;
	
	return kb;
}

void balda_keyboard_view_free(balda_keyboard_view_t* kb)
{
	assert(kb);
	CloseFont(kb->font);
	free(kb);
}

int balda_keyboard_view_x(balda_keyboard_view_t* kb)
{
	return kb->l;
}

int balda_keyboard_view_y(balda_keyboard_view_t* kb)
{
	return BALDA_VIEW_KEYBOARD_Y;
}

int balda_keyboard_view_w(balda_keyboard_view_t* kb)
{
	return (BALDA_VIEW_KEYBOARD_CELL_W + BALDA_VIEW_KEYBOARD_CELL_PADDING) * BALDA_VIEW_KEYBOARD_W;
}

int balda_keyboard_view_h(balda_keyboard_view_t* kb)
{
	return (BALDA_VIEW_KEYBOARD_CELL_W + BALDA_VIEW_KEYBOARD_CELL_PADDING) *
		((kb->kb_size / BALDA_VIEW_KEYBOARD_W) + ((kb->kb_size % BALDA_VIEW_KEYBOARD_W) ? 1 : 0));
}

void balda_keyboard_view_draw(balda_keyboard_view_t* kb)
{
	int x, y = BALDA_VIEW_KEYBOARD_Y, i, done=0;
	const balda_char* c = kb->keyboard;
	kb->h = 0;
	
	if (c)
	{
		kb->h = 1;
		while (!done)
		{
			x = kb->l;
			for (i=0; i<BALDA_VIEW_KEYBOARD_W; ++i, x += (BALDA_VIEW_KEYBOARD_CELL_W + BALDA_VIEW_KEYBOARD_CELL_PADDING), ++c)
			{
				if (*c != 0 || kb->has_cancel_button)
				{
					balda_view_draw_rect(kb->view, x, y, BALDA_VIEW_KEYBOARD_CELL_W, BALDA_VIEW_KEYBOARD_CELL_H, BLACK);
					
					if (*c != 0)
					{
						balda_view_draw_char(kb->view,
							x, y, BALDA_VIEW_KEYBOARD_CELL_W, BALDA_VIEW_KEYBOARD_CELL_H,
							kb->font, BLACK, *c);
					}
					else
					{
						balda_view_draw_bitmap(kb->view,
							x + (BALDA_VIEW_KEYBOARD_CELL_W - img_icon_back.width) / 2,
							y + (BALDA_VIEW_KEYBOARD_CELL_H - img_icon_back.height) / 2,
							&img_icon_back);
							
						done = 1;
						break;
					}
				}
				else
				{
					done = 1;
					break;
				}
			}
			
			if (i >= BALDA_VIEW_KEYBOARD_W)
			{
				y += BALDA_VIEW_KEYBOARD_CELL_H + BALDA_VIEW_KEYBOARD_CELL_PADDING;
				++kb->h;
			}
		}
	}
}

balda_bool balda_keyboard_view_get_button_at_point(balda_keyboard_view_t* kb, balda_point_t point,
	balda_point_t* out_button_pos)
{
	point.x -= balda_keyboard_view_x(kb);
	point.y -= balda_keyboard_view_y(kb);
	
	if (point.x >=0 && point.x < balda_keyboard_view_w(kb) &&
		point.y >=0 && point.y < balda_keyboard_view_h(kb))
	{
		out_button_pos->x = point.x / (BALDA_VIEW_KEYBOARD_CELL_W + BALDA_VIEW_KEYBOARD_CELL_PADDING);
		out_button_pos->y = point.y / (BALDA_VIEW_KEYBOARD_CELL_H + BALDA_VIEW_KEYBOARD_CELL_PADDING);
		
		debug_printf(("out_button_pos: (%d, %d)", out_button_pos->x, out_button_pos->y));
		
		return ((out_button_pos->x + out_button_pos->y * BALDA_VIEW_KEYBOARD_W) < kb->kb_size);
	}
	
	return 0;
}

void balda_keyboard_view_set_keyboard(balda_keyboard_view_t* kb, const balda_char* keyboard)
{
	kb->keyboard = keyboard;
	kb->kb_size = balda_char_strlen(kb->keyboard) + (kb->has_cancel_button ? 1 : 0);
}

void balda_keyboard_view_invert(balda_keyboard_view_t* kb, int x, int y)
{
	int rx = kb->l + x * (BALDA_VIEW_KEYBOARD_CELL_W + BALDA_VIEW_KEYBOARD_CELL_PADDING)
		- BALDA_VIEW_KEYBOARD_CELL_PADDING / 2;
	int ry = BALDA_VIEW_KEYBOARD_Y + y * (BALDA_VIEW_KEYBOARD_CELL_H + BALDA_VIEW_KEYBOARD_CELL_PADDING)
		- BALDA_VIEW_KEYBOARD_CELL_PADDING / 2;
	int w = BALDA_VIEW_KEYBOARD_CELL_W + BALDA_VIEW_KEYBOARD_CELL_PADDING;
	int h = BALDA_VIEW_KEYBOARD_CELL_H + BALDA_VIEW_KEYBOARD_CELL_PADDING;
	
	InvertAreaBW(rx, ry, w, h);
	balda_view_invalidate(kb->view, rx, ry, w, h, balda_true);
}

void balda_keyboard_view_clear_selection(balda_keyboard_view_t* kb)
{
	if (kb->selected)
	{
		balda_keyboard_view_invert(kb, kb->selected_pos.x, kb->selected_pos.y);
		kb->selected = 0;
	}
}

balda_bool balda_keyboard_view_is_selected(balda_keyboard_view_t* kb)
{
	return kb->selected;
}

void balda_keyboard_view_select(balda_keyboard_view_t* kb, int button_x, int button_y)
{
	balda_keyboard_view_clear_selection(kb);
	
	kb->selected_pos.x = button_x;
	kb->selected_pos.y = button_y;
	
	balda_keyboard_view_invert(kb, kb->selected_pos.x, kb->selected_pos.y);
	kb->selected = 1;
}

void balda_keyboard_view_select_point(balda_keyboard_view_t* kb, balda_point_t pos)
{
	balda_keyboard_view_select(kb, pos.x, pos.y);
}

void balda_keyboard_view_select_delta(balda_keyboard_view_t* kb, int dx, int dy)
{
	balda_point_t pos = balda_make_point((kb->selected_pos.x + dx) % BALDA_VIEW_KEYBOARD_W,
		(kb->selected_pos.y + dy) % kb->h);
	
	if (pos.x < 0)
		pos.x = BALDA_VIEW_KEYBOARD_W + pos.x;
	if (pos.y < 0)
		pos.y = kb->h + pos.y;
	
	balda_keyboard_view_select(kb, pos.x, pos.y);
}

balda_char balda_keyboard_view_get_selected_char(balda_keyboard_view_t* kb)
{
	assert(kb->keyboard);
	
	int char_index;
	
	if (kb->selected)
	{
		char_index = kb->selected_pos.x + kb->selected_pos.y * BALDA_VIEW_KEYBOARD_W;
		return kb->keyboard[char_index];
	}
	
	return BALDA_CHAR_NONE;
}

balda_point_t balda_keyboard_view_get_selection(balda_keyboard_view_t* kb)
{
	assert(kb->keyboard);
	return kb->selected_pos;
}

balda_bool balda_keyboard_view_is_back_selected(balda_keyboard_view_t* kb)
{
	assert(kb->keyboard);
	
	int char_index;
	
	if (kb->selected)
	{
		char_index =  kb->selected_pos.x +  kb->selected_pos.y * BALDA_VIEW_KEYBOARD_W;
		return kb->keyboard[char_index] == 0;
	}
	
	return 0;
}
