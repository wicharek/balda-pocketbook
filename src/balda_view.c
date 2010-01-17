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

#include "balda_view.h"
#include "balda_strings.h"
#include "balda_cancel_button.h"

#define BALDA_MAX_SEQUENCE (BALDA_FIELD_WIDTH*BALDA_FIELD_HEIGHT)

const int BALDA_VIEW_FIELD_Y = 230;
const int BALDA_VIEW_FIELD_CELL_W = 40;
const int BALDA_VIEW_FIELD_CELL_H = 40;
const int BALDA_VIEW_FIELD_CELL_PADDING = 2;

const int BALDA_VIEW_FIELD_SEQUENCE_FILL_COLOR = BLACK;
const int BALDA_VIEW_FIELD_SEQUENCE_STROKE_COLOR = WHITE;

const int BALDA_VIEW_KEYBOARD_Y = 620;
const int BALDA_VIEW_KEYBOARD_CELL_W = 48;
const int BALDA_VIEW_KEYBOARD_CELL_H = 48;
const int BALDA_VIEW_KEYBOARD_CELL_PADDING = 2;
const int BALDA_VIEW_KEYBOARD_W = 11;

const int BALDA_VIEW_PLAYER_NAME_MARGIN = 10;
const int BALDA_VIEW_PLAYER_NAME_Y = 120;

const int BALDA_VIEW_TURN_MARK_Y = 102;
const int BALDA_VIEW_TURN_MARK_MARGIN = 10;

const int BALDA_VIEW_SCORE_MARGIN = 10;
const int BALDA_VIEW_SCORE_Y = 140;
const int BALDA_VIEW_SCORE_WIDTH = 160;
const int BALDA_VIEW_SCORE_HEIGHT = 430;
const int BALDA_VIEW_SCORE_PANEL_PADDING = 8;
const int BALDA_VIEW_SCORE_PANEL_WIDTH = 75;
const int BALDA_VIEW_SCORE_PANEL_DIGIT_WIDTH = 100;
const int BALDA_VIEW_SCORE_PANEL_HEIGHT = 54;
const int BALDA_VIEW_SCORE_WORD_PADDING = 6;
const int BALDA_VIEW_SCORE_WORD_ITEM_H = 24;

extern const ibitmap img_logo;
extern const ibitmap img_turn_mark;
extern const ibitmap img_icon_back;

struct balda_view_t_impl
{
	balda_t* balda;
	
	ifont* font_version;
	ifont* font_player_name;
	ifont* font_score_panel;
	ifont* font_player_word;
	ifont* font_field;
	ifont* font_keyboard;
	ifont* font_game_winner;
	ifont* font_game_over_hint;
	
	const balda_char* keyboard;
	
	// precalculated
	int field_left;
	int keyboard_left;
	int keyboard_h;
	
	balda_point_t field_sel;
	balda_char field_inserting_char;
	balda_point_t field_inserting_point;
	int sequence_length;
	BALDA_DIRECTION sequence[BALDA_MAX_SEQUENCE];
	BALDA_VIEW_FIELD_SELECT_MODE field_mode;
	
	balda_bool keyboard_selected;
	balda_point_t keyboard_pos;
	
	balda_cancel_button_t btn_cancel;
	
	balda_int_converter_t int_conv;
};

typedef enum
{
	BALDA_VIEW_FIELD_ENTRY_NORMAL,
	BALDA_VIEW_FIELD_ENTRY_INSERTING,
	BALDA_VIEW_FIELD_ENTRY_SEQUENCE,
	BALDA_VIEW_FIELD_ENTRY_SEQUENCE_INSERTING
} BALDA_VIEW_FIELD_ENTRY_DRAW_STYLE;

#define BALDA_VIEW_FIELD_SEQUENCE_FLAG_NONE		0x00
#define BALDA_VIEW_FIELD_SEQUENCE_FLAG_LEFT		0x01
#define BALDA_VIEW_FIELD_SEQUENCE_FLAG_RIGHT	0x02
#define BALDA_VIEW_FIELD_SEQUENCE_FLAG_TOP		0x10
#define BALDA_VIEW_FIELD_SEQUENCE_FLAG_BOTTOM	0x20

balda_view_t* balda_view_init(balda_t* balda)
{
	balda_view_t* view = (balda_view_t*)malloc(sizeof(balda_view_t));
	
	view->balda = balda;
	
	view->font_version = OpenFont("LiberationSans", 11, 0);
	view->font_player_name = OpenFont("LiberationSans", 18, 0);
	view->font_score_panel = OpenFont("LiberationSans-Bold", 36, 1);
	view->font_player_word = OpenFont("LiberationSans", 16, 1);
	view->font_field = OpenFont("LiberationSans-Bold", 24, 0);
	view->font_keyboard = OpenFont("LiberationSans-Bold", 28, 0);
	view->font_game_winner = OpenFont("LiberationSans-Bold", 42, 0);
	view->font_game_over_hint = OpenFont("LiberationSans", 18, 0);
	
	view->keyboard = 0;
	
	view->field_left = (ScreenWidth() - BALDA_VIEW_FIELD_CELL_W * BALDA_FIELD_WIDTH
		- BALDA_VIEW_FIELD_CELL_PADDING * (BALDA_FIELD_WIDTH - 1)) / 2;
	view->keyboard_left = (ScreenWidth() - BALDA_VIEW_KEYBOARD_CELL_W * BALDA_VIEW_KEYBOARD_W
		- BALDA_VIEW_KEYBOARD_CELL_PADDING * (BALDA_VIEW_KEYBOARD_W - 1)) / 2;
	
	balda_cancel_button_init(&view->btn_cancel,
		balda_make_point((ScreenWidth() - BALDA_CANCEL_BUTTON_WIDTH) / 2, 438+12));
	
	return view;
}

void balda_view_reset(balda_view_t* view)
{
	view->field_mode = BALDA_VIEW_FIELD_SELECT_MODE_NONE;
	view->keyboard_selected = 0;
}

void balda_view_free(balda_view_t* view)
{
	assert(view);
	
	CloseFont(view->font_version);
	CloseFont(view->font_player_name);
	CloseFont(view->font_score_panel);
	CloseFont(view->font_field);
	CloseFont(view->font_keyboard);
	CloseFont(view->font_player_word);
	CloseFont(view->font_game_winner);
	CloseFont(view->font_game_over_hint);
	
	balda_cancel_button_free(&view->btn_cancel);
	
	free(view);
}

balda_char balda_view_get_letter_at(balda_view_t* view, int x, int y)
{
	if ((view->field_mode == BALDA_VIEW_FIELD_SELECT_MODE_INSERTING || view->field_mode == BALDA_VIEW_FIELD_SELECT_MODE_SEQUENCE)
		&& view->field_inserting_point.x == x && view->field_inserting_point.y == y)
	{
		return view->field_inserting_char;
	}
	
	return balda_get_letter_at(view->balda, x, y);
}

void balda_view_draw_char(balda_char c, int x, int y, int w, int h, ifont* font, int color)
{
	char utf8_buf[4];
	balda_single_char_to_utf8(c, utf8_buf, sizeof(utf8_buf));
	
	SetFont(font, color);
	DrawTextRect(x, y, w, h, utf8_buf, VALIGN_MIDDLE | ALIGN_CENTER);
}


// Field

#define balda_view_field_entry_x(view, cell_x) ((view)->field_left + (cell_x) * (BALDA_VIEW_FIELD_CELL_W + BALDA_VIEW_FIELD_CELL_PADDING))
#define balda_view_field_entry_y(view, cell_y) (BALDA_VIEW_FIELD_Y + (cell_y) * (BALDA_VIEW_FIELD_CELL_H + BALDA_VIEW_FIELD_CELL_PADDING))

void balda_view_draw_field_entry(balda_view_t* view, int x, int y, balda_char c, BALDA_VIEW_FIELD_ENTRY_DRAW_STYLE style,
	balda_bool update, int sequence_flag)
{
	int update_grow = 0;
	balda_bool sequence = (style == BALDA_VIEW_FIELD_ENTRY_SEQUENCE || style == BALDA_VIEW_FIELD_ENTRY_SEQUENCE_INSERTING);
	
	if (sequence)
	{
		if (sequence_flag & BALDA_VIEW_FIELD_SEQUENCE_FLAG_TOP) // opened from top
		{
			DrawLine(x, y,
				x, y-BALDA_VIEW_FIELD_CELL_PADDING/2, BALDA_VIEW_FIELD_SEQUENCE_STROKE_COLOR);
			DrawLine(x+BALDA_VIEW_FIELD_CELL_W-1, y,
				x+BALDA_VIEW_FIELD_CELL_W-1, y-BALDA_VIEW_FIELD_CELL_PADDING/2, BALDA_VIEW_FIELD_SEQUENCE_STROKE_COLOR);
		}
		else
		{
			DrawLine(x, y,
				x+BALDA_VIEW_FIELD_CELL_W-1, y, BALDA_VIEW_FIELD_SEQUENCE_STROKE_COLOR);
		}
		
		if (sequence_flag & BALDA_VIEW_FIELD_SEQUENCE_FLAG_BOTTOM) // opened from bottom
		{
			DrawLine(x, y+BALDA_VIEW_FIELD_CELL_H,
				x, y+BALDA_VIEW_FIELD_CELL_H-1+BALDA_VIEW_FIELD_CELL_PADDING/2, BALDA_VIEW_FIELD_SEQUENCE_STROKE_COLOR);
			DrawLine(x+BALDA_VIEW_FIELD_CELL_W-1, y+BALDA_VIEW_FIELD_CELL_H-1,
				x+BALDA_VIEW_FIELD_CELL_W-1, y+BALDA_VIEW_FIELD_CELL_H-1+BALDA_VIEW_FIELD_CELL_PADDING/2, BALDA_VIEW_FIELD_SEQUENCE_STROKE_COLOR);
		}
		else
		{
			DrawLine(x, y+BALDA_VIEW_FIELD_CELL_H-1,
				x+BALDA_VIEW_FIELD_CELL_W-1, y+BALDA_VIEW_FIELD_CELL_H-1, BALDA_VIEW_FIELD_SEQUENCE_STROKE_COLOR);
		}
		
		if (sequence_flag & BALDA_VIEW_FIELD_SEQUENCE_FLAG_LEFT) // opened from left
		{
			DrawLine(x, y,
				x-BALDA_VIEW_FIELD_CELL_PADDING/2, y, BALDA_VIEW_FIELD_SEQUENCE_STROKE_COLOR);
			DrawLine(x, y+BALDA_VIEW_FIELD_CELL_H-1,
				x-BALDA_VIEW_FIELD_CELL_PADDING/2, y+BALDA_VIEW_FIELD_CELL_H-1, BALDA_VIEW_FIELD_SEQUENCE_STROKE_COLOR);
		}
		else
		{
			DrawLine(x, y,
				x, y+BALDA_VIEW_FIELD_CELL_H-1, BALDA_VIEW_FIELD_SEQUENCE_STROKE_COLOR);
		}
		
		if (sequence_flag & BALDA_VIEW_FIELD_SEQUENCE_FLAG_RIGHT) // opened from right
		{
			DrawLine(x+BALDA_VIEW_FIELD_CELL_W-1, y,
				x+BALDA_VIEW_FIELD_CELL_W-1+BALDA_VIEW_FIELD_CELL_PADDING/2, y, BALDA_VIEW_FIELD_SEQUENCE_STROKE_COLOR);
			DrawLine(x+BALDA_VIEW_FIELD_CELL_W, y+BALDA_VIEW_FIELD_CELL_H-1,
				x+BALDA_VIEW_FIELD_CELL_W-1+BALDA_VIEW_FIELD_CELL_PADDING/2, y+BALDA_VIEW_FIELD_CELL_H-1, BALDA_VIEW_FIELD_SEQUENCE_STROKE_COLOR);
		}
		else
		{
			DrawLine(x+BALDA_VIEW_FIELD_CELL_W-1, y,
				x+BALDA_VIEW_FIELD_CELL_W-1, y+BALDA_VIEW_FIELD_CELL_H-1, BALDA_VIEW_FIELD_SEQUENCE_STROKE_COLOR);
		}
		
		update_grow = BALDA_VIEW_FIELD_CELL_PADDING / 2;
		
		if (style == BALDA_VIEW_FIELD_ENTRY_SEQUENCE_INSERTING)
		{
			DrawRect(x+3, y+3, BALDA_VIEW_FIELD_CELL_W-6, BALDA_VIEW_FIELD_CELL_H-6, BALDA_VIEW_FIELD_SEQUENCE_STROKE_COLOR);
		}
	}
	else
	{
		DrawRect(x, y, BALDA_VIEW_FIELD_CELL_W, BALDA_VIEW_FIELD_CELL_H, BLACK);
		if (style == BALDA_VIEW_FIELD_ENTRY_INSERTING)
		{
			DrawRect(x+1, y+1, BALDA_VIEW_FIELD_CELL_W-2, BALDA_VIEW_FIELD_CELL_H-2, BLACK);
		}
	}
	
	if (c)
	{
		balda_view_draw_char(c, x, y, BALDA_VIEW_FIELD_CELL_W, BALDA_VIEW_FIELD_CELL_H, view->font_field,
			sequence ? BALDA_VIEW_FIELD_SEQUENCE_STROKE_COLOR : BLACK);
	}
		
	if (update)
	{
		PartialUpdateBW(x-update_grow, y-update_grow,
			BALDA_VIEW_FIELD_CELL_W+update_grow*2, BALDA_VIEW_FIELD_CELL_H+update_grow*2);
	}
}

void balda_view_field_invert(balda_view_t* view, int x, int y)
{
	int rx = view->field_left + x * (BALDA_VIEW_FIELD_CELL_W + BALDA_VIEW_FIELD_CELL_PADDING) - BALDA_VIEW_FIELD_CELL_PADDING / 2;
	int ry = BALDA_VIEW_FIELD_Y + y * (BALDA_VIEW_FIELD_CELL_H + BALDA_VIEW_FIELD_CELL_PADDING) - BALDA_VIEW_FIELD_CELL_PADDING / 2;
	int w = BALDA_VIEW_FIELD_CELL_W + BALDA_VIEW_FIELD_CELL_PADDING;
	int h = BALDA_VIEW_FIELD_CELL_H + BALDA_VIEW_FIELD_CELL_PADDING;
	
	InvertAreaBW(rx, ry, w, h);
	PartialUpdateBW(rx, ry, w, h);
}

balda_bool balda_view_is_cancel_selected(balda_view_t* view)
{
	return view->btn_cancel.selected;
}

balda_point_t balda_view_field_get_selection(balda_view_t* view)
{
	return view->field_sel;
}

balda_point_t balda_view_field_get_insert_pos(balda_view_t* view)
{
	return view->field_inserting_point;
}

balda_char balda_view_field_get_insert_char(balda_view_t* view)
{
	return view->field_inserting_char;
}

void balda_view_field_clear_selection(balda_view_t* view)
{
	switch (view->field_mode)
	{
		case BALDA_VIEW_FIELD_SELECT_MODE_SINGLE:
		{
			if (view->field_sel.y == BALDA_FIELD_HEIGHT)
			{
				balda_cancel_button_set_selected(&view->btn_cancel, 0);
				balda_cancel_button_draw(&view->btn_cancel, 1, BALDA_CANCEL_BUTTON_FORCE_REDRAW_NONE);
			}
			else
			{
				balda_view_field_invert(view, view->field_sel.x, view->field_sel.y);
			}
			view->field_mode = BALDA_VIEW_FIELD_SELECT_MODE_NONE;
		}
		break;
		
		case BALDA_VIEW_FIELD_SELECT_MODE_INSERTING:
		{
			view->field_inserting_char = BALDA_CHAR_NONE;
			
			/*balda_view_draw_field_entry(view,
				balda_view_field_entry_x(view, view->field_sel.x), balda_view_field_entry_y(view, view->field_sel.y),
				balda_view_get_letter_at(view, view->field_sel.x, view->field_sel.y), BALDA_VIEW_FIELD_ENTRY_NORMAL,
				0, BALDA_VIEW_FIELD_SEQUENCE_FLAG_NONE);*/
			view->field_mode = BALDA_VIEW_FIELD_SELECT_MODE_NONE;
		}
		break;
		
		default:
			// nothing so far
		break;
	}
}

void balda_view_field_select_single(balda_view_t* view, balda_point_t point)
{
	if (view->field_mode == BALDA_VIEW_FIELD_SELECT_MODE_INSERTING)
	{
		FillArea(balda_view_field_entry_x(view, view->field_sel.x)-BALDA_VIEW_FIELD_CELL_PADDING/2,
			balda_view_field_entry_y(view, view->field_sel.y)-BALDA_VIEW_FIELD_CELL_PADDING/2,
			BALDA_VIEW_FIELD_CELL_W+BALDA_VIEW_FIELD_CELL_PADDING, BALDA_VIEW_FIELD_CELL_H+BALDA_VIEW_FIELD_CELL_PADDING,
			WHITE);
		balda_view_draw_field_entry(view,
			balda_view_field_entry_x(view, view->field_sel.x), balda_view_field_entry_y(view, view->field_sel.y),
			balda_view_get_letter_at(view, view->field_sel.x, view->field_sel.y), BALDA_VIEW_FIELD_ENTRY_NORMAL,
			0, BALDA_VIEW_FIELD_SEQUENCE_FLAG_NONE);
	}
	
	balda_view_field_clear_selection(view);
	
	view->field_mode = BALDA_VIEW_FIELD_SELECT_MODE_SINGLE;
	view->field_sel = point;
	
	if (view->field_sel.y != BALDA_FIELD_HEIGHT)
		balda_view_field_invert(view, view->field_sel.x, view->field_sel.y);
	
	balda_cancel_button_set_visible(&view->btn_cancel, 1);
	balda_cancel_button_set_mode(&view->btn_cancel, BALDA_CANCEL_BUTTON_MODE_SURRENDER);
	balda_cancel_button_set_selected(&view->btn_cancel, view->field_sel.y == BALDA_FIELD_HEIGHT);
	balda_cancel_button_draw(&view->btn_cancel, 1, BALDA_CANCEL_BUTTON_FORCE_REDRAW_NONE);
}

void balda_view_field_select_single_delta(balda_view_t* view, int dx, int dy)
{
	assert(view->field_mode == BALDA_VIEW_FIELD_SELECT_MODE_SINGLE);
	
	balda_point_t pos = balda_make_point((view->field_sel.x + dx) % BALDA_FIELD_WIDTH,
		(view->field_sel.y + dy) % (BALDA_FIELD_HEIGHT + 1));
		
	if (pos.x < 0)
		pos.x = BALDA_FIELD_WIDTH + pos.x;
	if (pos.y < 0)
		pos.y = (BALDA_FIELD_HEIGHT + 1) + pos.y;
	
	balda_view_field_select_single(view, pos);
}

void balda_view_field_select_inserting(balda_view_t* view, balda_point_t point, balda_char c)
{
	if (view->field_mode == BALDA_VIEW_FIELD_SELECT_MODE_INSERTING
		&& point.x == view->field_sel.x && point.y == view->field_sel.y)
	{
		FillArea(balda_view_field_entry_x(view, point.x), balda_view_field_entry_y(view, point.y),
			BALDA_VIEW_FIELD_CELL_W, BALDA_VIEW_FIELD_CELL_H,
			WHITE);
	}
	else
	{
		balda_view_field_clear_selection(view);
		view->field_mode = BALDA_VIEW_FIELD_SELECT_MODE_INSERTING;
	}
	view->field_sel = point;
	
	view->field_inserting_char = c;
	balda_view_draw_field_entry(view,
		balda_view_field_entry_x(view, point.x), balda_view_field_entry_y(view, point.y),
		view->field_inserting_char, BALDA_VIEW_FIELD_ENTRY_INSERTING, 1, BALDA_VIEW_FIELD_SEQUENCE_FLAG_NONE);
		
	balda_cancel_button_set_visible(&view->btn_cancel, 1);
	balda_cancel_button_set_mode(&view->btn_cancel, BALDA_CANCEL_BUTTON_MODE_SURRENDER);
	balda_cancel_button_set_selected(&view->btn_cancel, 0);
	balda_cancel_button_draw(&view->btn_cancel, 1, BALDA_CANCEL_BUTTON_FORCE_REDRAW_NONE);
}

void balda_view_field_select_sequence_first(balda_view_t* view, balda_point_t point, balda_point_t insert_point, balda_char insert_char)
{
	BALDA_VIEW_FIELD_SELECT_MODE prev_mode = view->field_mode;
	/*if (!balda_points_equal(view->field_sel, insert_point) || insert_char != view->field_inserting_char)
	{
		balda_view_field_select_inserting(view, insert_point, insert_char);
	}*/
	
	view->field_sel = point;
	view->field_inserting_point = insert_point;
	view->sequence_length = 0;
	
	if (prev_mode == BALDA_VIEW_FIELD_SELECT_MODE_INSERTING)
	{
		balda_view_field_invert(view, view->field_sel.x, view->field_sel.y);
	}
	else if (prev_mode == BALDA_VIEW_FIELD_SELECT_MODE_SEQUENCE)
	{
		if (balda_points_equal(view->field_inserting_point, point))
		{
			// Slightly change char
			int x = balda_view_field_entry_x(view, point.x);
			int y = balda_view_field_entry_y(view, point.y);
			DrawRect(x+3, y+3, BALDA_VIEW_FIELD_CELL_W-6, BALDA_VIEW_FIELD_CELL_H-6, BLACK);
			DrawRect(x+1, y+1, BALDA_VIEW_FIELD_CELL_W-2, BALDA_VIEW_FIELD_CELL_H-2, WHITE);
			PartialUpdateBW(x+3, y+3, BALDA_VIEW_FIELD_CELL_W-6, BALDA_VIEW_FIELD_CELL_H-6);
		}
		
		/*FillArea(balda_view_field_entry_x(view, point.x) - BALDA_VIEW_FIELD_CELL_PADDING/2,
			balda_view_field_entry_y(view, point.y) - BALDA_VIEW_FIELD_CELL_PADDING/2,
			BALDA_VIEW_FIELD_CELL_W + BALDA_VIEW_FIELD_CELL_PADDING,
			BALDA_VIEW_FIELD_CELL_H + BALDA_VIEW_FIELD_CELL_PADDING,
			BALDA_VIEW_FIELD_SEQUENCE_FILL_COLOR);
		
		balda_view_draw_field_entry(view,
			balda_view_field_entry_x(view, point.x), balda_view_field_entry_y(view, point.y),
			balda_view_get_letter_at(view, point.x, point.y),
			balda_points_equal(view->field_inserting_point, point) ?
				BALDA_VIEW_FIELD_ENTRY_INSERTING : BALDA_VIEW_FIELD_ENTRY_NORMAL,
			1, BALDA_VIEW_FIELD_SEQUENCE_FLAG_NONE);*/
	}
	
	view->field_mode = BALDA_VIEW_FIELD_SELECT_MODE_SEQUENCE;
	
	balda_cancel_button_set_visible(&view->btn_cancel, 1);
	balda_cancel_button_set_mode(&view->btn_cancel, BALDA_CANCEL_BUTTON_MODE_BACK);
	balda_cancel_button_set_selected(&view->btn_cancel, view->field_sel.y == BALDA_FIELD_HEIGHT);
	balda_cancel_button_draw(&view->btn_cancel, 1, BALDA_CANCEL_BUTTON_FORCE_REDRAW_NONE);
}

void balda_view_field_select_sequence_first_delta(balda_view_t* view, int dx, int dy)
{
	assert(view->field_mode == BALDA_VIEW_FIELD_SELECT_MODE_SEQUENCE);
	
	balda_point_t pos = balda_make_point((view->field_sel.x + dx) % BALDA_FIELD_WIDTH,
		(view->field_sel.y + dy) % (BALDA_FIELD_HEIGHT + 1));
		
	if (pos.x < 0)
		pos.x = BALDA_FIELD_WIDTH + pos.x;
	if (pos.y < 0)
		pos.y = BALDA_FIELD_HEIGHT + pos.y + 1;
	
	if (view->field_sel.y < BALDA_FIELD_HEIGHT)
		balda_view_field_invert(view, view->field_sel.x, view->field_sel.y);
	if (pos.y < BALDA_FIELD_HEIGHT)
		balda_view_field_invert(view, pos.x, pos.y);
	
	balda_cancel_button_set_selected(&view->btn_cancel, pos.y == BALDA_FIELD_HEIGHT);
	balda_cancel_button_draw(&view->btn_cancel, 1, BALDA_CANCEL_BUTTON_FORCE_REDRAW_NONE);
	
	view->field_sel = pos;
}

void balda_view_field_select_sequence_first_complete(balda_view_t* view)
{
	balda_point_t point = view->field_sel;
	
	if (balda_points_equal(view->field_inserting_point, point))
	{
		// If first selected is inserting char, redraw it
		FillArea(balda_view_field_entry_x(view, point.x) - BALDA_VIEW_FIELD_CELL_PADDING/2,
			balda_view_field_entry_y(view, point.y) - BALDA_VIEW_FIELD_CELL_PADDING/2,
			BALDA_VIEW_FIELD_CELL_W + BALDA_VIEW_FIELD_CELL_PADDING,
			BALDA_VIEW_FIELD_CELL_H + BALDA_VIEW_FIELD_CELL_PADDING,
			BALDA_VIEW_FIELD_SEQUENCE_FILL_COLOR);
		
		balda_view_draw_field_entry(view,
			balda_view_field_entry_x(view, point.x), balda_view_field_entry_y(view, point.y),
			balda_view_get_letter_at(view, point.x, point.y), BALDA_VIEW_FIELD_ENTRY_SEQUENCE_INSERTING,
			1, BALDA_VIEW_FIELD_SEQUENCE_FLAG_NONE);
	}
}

void balda_view_field_partial_update(balda_view_t* view, balda_point_t p1, balda_point_t p2)
{
	int ux, uy, uw, uh;
	int sq_dist = BALDA_SQR(p1.x - p2.x) + BALDA_SQR(p1.y - p2.y);
		
	if (sq_dist <= 0)
	{
		// cells are neibours
		// redraw via single update
		ux = view->field_left + BALDA_MIN(p1.x, p2.x)
			* (BALDA_VIEW_FIELD_CELL_W + BALDA_VIEW_FIELD_CELL_PADDING) - BALDA_VIEW_FIELD_CELL_PADDING / 2;
		uy = BALDA_VIEW_FIELD_Y + BALDA_MIN(p1.y, p2.y)
			* (BALDA_VIEW_FIELD_CELL_H + BALDA_VIEW_FIELD_CELL_PADDING) - BALDA_VIEW_FIELD_CELL_PADDING / 2;
		uw = 2 * (BALDA_VIEW_FIELD_CELL_W + BALDA_VIEW_FIELD_CELL_PADDING);
		uh = 2 * (BALDA_VIEW_FIELD_CELL_H + BALDA_VIEW_FIELD_CELL_PADDING);
		
		PartialUpdateBW(ux, uy, uw, uh);
	}
	else
	{
		// cells are too far
		// redraw with multiple updates
		uw = (BALDA_VIEW_FIELD_CELL_W + BALDA_VIEW_FIELD_CELL_PADDING);
		uh = (BALDA_VIEW_FIELD_CELL_H + BALDA_VIEW_FIELD_CELL_PADDING);
		
		ux = view->field_left + p1.x
			* (BALDA_VIEW_FIELD_CELL_W + BALDA_VIEW_FIELD_CELL_PADDING) - BALDA_VIEW_FIELD_CELL_PADDING / 2;
		uy = BALDA_VIEW_FIELD_Y + p1.y
			* (BALDA_VIEW_FIELD_CELL_H + BALDA_VIEW_FIELD_CELL_PADDING) - BALDA_VIEW_FIELD_CELL_PADDING / 2;
		PartialUpdateBW(ux, uy, uw, uh);
		
		ux = view->field_left + p2.x
			* (BALDA_VIEW_FIELD_CELL_W + BALDA_VIEW_FIELD_CELL_PADDING) - BALDA_VIEW_FIELD_CELL_PADDING / 2;
		uy = BALDA_VIEW_FIELD_Y + p2.y
			* (BALDA_VIEW_FIELD_CELL_H + BALDA_VIEW_FIELD_CELL_PADDING) - BALDA_VIEW_FIELD_CELL_PADDING / 2;
		PartialUpdateBW(ux, uy, uw, uh);
	}
}

int balda_view_flag_from_direction(BALDA_DIRECTION direction)
{
	switch (direction)
	{
		case BALDA_DIRECTION_UP: return BALDA_VIEW_FIELD_SEQUENCE_FLAG_BOTTOM;
		case BALDA_DIRECTION_DOWN: return BALDA_VIEW_FIELD_SEQUENCE_FLAG_TOP;
		case BALDA_DIRECTION_LEFT: return BALDA_VIEW_FIELD_SEQUENCE_FLAG_RIGHT;
		case BALDA_DIRECTION_RIGHT: return BALDA_VIEW_FIELD_SEQUENCE_FLAG_LEFT;
	}
	
	return BALDA_VIEW_FIELD_SEQUENCE_FLAG_NONE;
}

void balda_view_field_select_sequence_delta(balda_view_t* view, BALDA_DIRECTION direction)
{
	int prev_flags = BALDA_VIEW_FIELD_SEQUENCE_FLAG_NONE, next_flags = BALDA_VIEW_FIELD_SEQUENCE_FLAG_NONE;
	balda_point_t prev_pos = view->field_sel, next_pos = view->field_sel;
	balda_bool rollback = (view->sequence_length &&
		balda_are_directions_opposite(view->sequence[view->sequence_length-1], direction));
	
	assert(view->field_mode == BALDA_VIEW_FIELD_SELECT_MODE_SEQUENCE);
	
	if (!rollback)
	{
		if (view->sequence_length > 0)
		{
			prev_flags = balda_view_flag_from_direction(view->sequence[view->sequence_length-1]);
		}
	}
	else if (view->sequence_length > 1)
	{
		next_flags = balda_view_flag_from_direction(view->sequence[view->sequence_length-2]);
	}
	
	switch (direction)
	{
		case BALDA_DIRECTION_UP:
			--next_pos.y;
			
			if (!rollback)
			{
				prev_flags |= BALDA_VIEW_FIELD_SEQUENCE_FLAG_TOP;
				next_flags = BALDA_VIEW_FIELD_SEQUENCE_FLAG_BOTTOM;
			}
		break;
		
		case BALDA_DIRECTION_DOWN:
			++next_pos.y;
			
			if (!rollback)
			{
				prev_flags |= BALDA_VIEW_FIELD_SEQUENCE_FLAG_BOTTOM;
				next_flags = BALDA_VIEW_FIELD_SEQUENCE_FLAG_TOP;
			}
		break;
		
		case BALDA_DIRECTION_LEFT:
			--next_pos.x;
			
			if (!rollback)
			{
				prev_flags |= BALDA_VIEW_FIELD_SEQUENCE_FLAG_LEFT;
				next_flags = BALDA_VIEW_FIELD_SEQUENCE_FLAG_RIGHT;
			}
		break;
		
		case BALDA_DIRECTION_RIGHT:
			++next_pos.x;
			
			if (!rollback)
			{
				prev_flags |= BALDA_VIEW_FIELD_SEQUENCE_FLAG_RIGHT;
				next_flags = BALDA_VIEW_FIELD_SEQUENCE_FLAG_LEFT;
			}
		break;
	}
	
	if (rollback)
	{
		--view->sequence_length;
	}
	else
	{
		view->sequence[view->sequence_length] = direction;
		++view->sequence_length;
	}
	view->field_sel = next_pos;
	
	FillArea(balda_view_field_entry_x(view, prev_pos.x) - BALDA_VIEW_FIELD_CELL_PADDING/2,
		balda_view_field_entry_y(view, prev_pos.y) - BALDA_VIEW_FIELD_CELL_PADDING/2,
		BALDA_VIEW_FIELD_CELL_W + BALDA_VIEW_FIELD_CELL_PADDING,
		BALDA_VIEW_FIELD_CELL_H + BALDA_VIEW_FIELD_CELL_PADDING,
		rollback ? WHITE : BALDA_VIEW_FIELD_SEQUENCE_FILL_COLOR);
	
	balda_view_draw_field_entry(view,
		balda_view_field_entry_x(view, prev_pos.x), balda_view_field_entry_y(view, prev_pos.y),
		balda_view_get_letter_at(view, prev_pos.x, prev_pos.y), 
		balda_points_equal(view->field_inserting_point, prev_pos) ?
			(!rollback ? BALDA_VIEW_FIELD_ENTRY_SEQUENCE_INSERTING : BALDA_VIEW_FIELD_ENTRY_INSERTING)
			: (!rollback ? BALDA_VIEW_FIELD_ENTRY_SEQUENCE : BALDA_VIEW_FIELD_ENTRY_NORMAL),
		0, prev_flags);
	
	FillArea(balda_view_field_entry_x(view, next_pos.x) - BALDA_VIEW_FIELD_CELL_PADDING/2,
		balda_view_field_entry_y(view, next_pos.y) - BALDA_VIEW_FIELD_CELL_PADDING/2,
		BALDA_VIEW_FIELD_CELL_W + BALDA_VIEW_FIELD_CELL_PADDING,
		BALDA_VIEW_FIELD_CELL_H + BALDA_VIEW_FIELD_CELL_PADDING,
		BALDA_VIEW_FIELD_SEQUENCE_FILL_COLOR);
		
	balda_view_draw_field_entry(view,
		balda_view_field_entry_x(view, next_pos.x), balda_view_field_entry_y(view, next_pos.y),
		balda_view_get_letter_at(view, next_pos.x, next_pos.y), 
		balda_points_equal(view->field_inserting_point, next_pos) ?
			BALDA_VIEW_FIELD_ENTRY_SEQUENCE_INSERTING : BALDA_VIEW_FIELD_ENTRY_SEQUENCE,
		0, next_flags);
	
	//balda_view_field_invert(view, view->field_sel.x, view->field_sel.y);
	balda_view_field_partial_update(view, prev_pos, next_pos);
}


// Keyboard

void balda_view_keyboard_invert(balda_view_t* view, int x, int y, balda_bool update)
{
	int rx = view->keyboard_left + x * (BALDA_VIEW_KEYBOARD_CELL_W + BALDA_VIEW_KEYBOARD_CELL_PADDING) - BALDA_VIEW_KEYBOARD_CELL_PADDING / 2;
	int ry = BALDA_VIEW_KEYBOARD_Y + y * (BALDA_VIEW_KEYBOARD_CELL_H + BALDA_VIEW_KEYBOARD_CELL_PADDING) - BALDA_VIEW_KEYBOARD_CELL_PADDING / 2;
	int w = BALDA_VIEW_KEYBOARD_CELL_W + BALDA_VIEW_KEYBOARD_CELL_PADDING;
	int h = BALDA_VIEW_KEYBOARD_CELL_H + BALDA_VIEW_KEYBOARD_CELL_PADDING;
	
	InvertAreaBW(rx, ry, w, h);
	
	if (update)
		PartialUpdateBW(rx, ry, w, h);
}

void balda_view_keyboard_clear_selection(balda_view_t* view)
{
	if (view->keyboard_selected)
	{
		balda_view_keyboard_invert(view, view->keyboard_pos.x, view->keyboard_pos.y, 1);
		view->keyboard_selected = 0;
	}
}

void balda_view_keyboard_select(balda_view_t* view, int x, int y)
{
	balda_view_keyboard_clear_selection(view);
	
	view->keyboard_pos.x = x;
	view->keyboard_pos.y = y;
	
	balda_view_keyboard_invert(view, view->keyboard_pos.x, view->keyboard_pos.y, 1);
	view->keyboard_selected = 1;
}

void balda_view_keyboard_select_point(balda_view_t* view, balda_point_t pos)
{
	balda_view_keyboard_select(view, pos.x, pos.y);
}

void balda_view_keyboard_select_delta(balda_view_t* view, int dx, int dy)
{
	balda_point_t pos = balda_make_point((view->keyboard_pos.x + dx) % BALDA_VIEW_KEYBOARD_W,
		(view->keyboard_pos.y + dy) % view->keyboard_h);
	int ux, uy, uw, uh;
		
	if (pos.x < 0)
		pos.x = BALDA_VIEW_KEYBOARD_W + pos.x;
	if (pos.y < 0)
		pos.y = view->keyboard_h + pos.y;
	
	if (view->keyboard_selected)
	{
		int sq_dist = BALDA_SQR(pos.x - view->keyboard_pos.x) + BALDA_SQR(pos.y - view->keyboard_pos.y);
		
		if (sq_dist <= 0)
		{
			// cells are neibours
			// redraw via single update
			balda_view_keyboard_invert(view, view->keyboard_pos.x, view->keyboard_pos.y, 0);
			balda_view_keyboard_invert(view, pos.x, pos.y, 0);
			
			ux = view->keyboard_left + BALDA_MIN(view->keyboard_pos.x, pos.x)
				* (BALDA_VIEW_KEYBOARD_CELL_W + BALDA_VIEW_KEYBOARD_CELL_PADDING) - BALDA_VIEW_KEYBOARD_CELL_PADDING / 2;
			uy = BALDA_VIEW_KEYBOARD_Y + BALDA_MIN(view->keyboard_pos.y, pos.y)
				* (BALDA_VIEW_KEYBOARD_CELL_H + BALDA_VIEW_KEYBOARD_CELL_PADDING) - BALDA_VIEW_KEYBOARD_CELL_PADDING / 2;
			uw = 2 * (BALDA_VIEW_KEYBOARD_CELL_W + BALDA_VIEW_KEYBOARD_CELL_PADDING);
			uh = 2 * (BALDA_VIEW_KEYBOARD_CELL_H + BALDA_VIEW_KEYBOARD_CELL_PADDING);
			
			PartialUpdateBW(ux, uy, uw, uh);
		}
		else
		{
			// cells are too far
			// redraw with multiple updates
			balda_view_keyboard_invert(view, view->keyboard_pos.x, view->keyboard_pos.y, 1);
			balda_view_keyboard_invert(view, pos.x, pos.y, 1);
			view->keyboard_pos = pos;
		}
	}
}

balda_bool balda_view_keyboard_is_back_selected(balda_view_t* view)
{
	assert(view->keyboard);
	
	int char_index;
	
	if (view->keyboard_selected)
	{
		char_index = view->keyboard_pos.x + view->keyboard_pos.y * BALDA_VIEW_KEYBOARD_W;
		return view->keyboard[char_index] == 0;
	}
	
	return 0;
}

balda_char balda_view_keyboard_get_selected_char(balda_view_t* view)
{
	assert(view->keyboard);
	
	int char_index;
	
	if (view->keyboard_selected)
	{
		char_index = view->keyboard_pos.x + view->keyboard_pos.y * BALDA_VIEW_KEYBOARD_W;
		return view->keyboard[char_index];
	}
	
	return 0;
}

balda_point_t balda_view_keyboard_get_selection(balda_view_t* view)
{
	assert(view->keyboard);
	return view->keyboard_pos;
}


void balda_view_show_turn(balda_view_t* view)
{
	int player_index = balda_get_active_player(view->balda);
	int left[2] = { BALDA_VIEW_TURN_MARK_MARGIN, ScreenWidth() - BALDA_VIEW_TURN_MARK_MARGIN - img_turn_mark.width};
	int i;
	
	for (i=0; i<2; ++i)
	{
		if (player_index == i)
			DrawBitmap(left[i], BALDA_VIEW_TURN_MARK_Y, &img_turn_mark);
		else
			FillArea(left[i], BALDA_VIEW_TURN_MARK_Y, img_turn_mark.width, img_turn_mark.height, WHITE);
			
		PartialUpdateBW(left[i], BALDA_VIEW_TURN_MARK_Y, img_turn_mark.width, img_turn_mark.height);
	}
}

void balda_view_draw_logo(balda_view_t* view)
{
	// logo image
	DrawBitmap(10, 10, &img_logo);
	
	// version
	SetFont(view->font_version, BLACK);
	DrawString(10, 60, BALDA_APP_VERSION_STRING_FULL_RU);
	
}

void balda_view_draw_field(balda_view_t* view, balda_bool update)
{
	int left = view->field_left;
	int i, j, x, y = BALDA_VIEW_FIELD_Y;
	
	FillArea(view->field_left-BALDA_VIEW_FIELD_CELL_PADDING/2,
		BALDA_VIEW_FIELD_Y-BALDA_VIEW_FIELD_CELL_PADDING/2,
		(BALDA_VIEW_FIELD_CELL_W + BALDA_VIEW_FIELD_CELL_PADDING) * BALDA_FIELD_WIDTH,
		(BALDA_VIEW_FIELD_CELL_H + BALDA_VIEW_FIELD_CELL_PADDING) * BALDA_FIELD_HEIGHT,
		WHITE);
	
	for (j=0; j<BALDA_FIELD_HEIGHT; ++j, y += (BALDA_VIEW_FIELD_CELL_H + BALDA_VIEW_FIELD_CELL_PADDING))
	{
		x = left;
		for (i=0; i<BALDA_FIELD_WIDTH; ++i, x += (BALDA_VIEW_FIELD_CELL_W + BALDA_VIEW_FIELD_CELL_PADDING))
		{
			balda_view_draw_field_entry(view, x, y, balda_get_letter_at(view->balda, i, j),
				BALDA_VIEW_FIELD_ENTRY_NORMAL, 0, BALDA_VIEW_FIELD_SEQUENCE_FLAG_NONE);
		}
	}
	
	if (update)
	{	
		PartialUpdateBW(view->field_left-BALDA_VIEW_FIELD_CELL_PADDING/2,
			BALDA_VIEW_FIELD_Y-BALDA_VIEW_FIELD_CELL_PADDING/2,
			(BALDA_VIEW_FIELD_CELL_W + BALDA_VIEW_FIELD_CELL_PADDING) * BALDA_FIELD_WIDTH,
			(BALDA_VIEW_FIELD_CELL_H + BALDA_VIEW_FIELD_CELL_PADDING) * BALDA_FIELD_HEIGHT);
	}
}

void balda_view_draw_players(balda_view_t* view)
{
	const char* player_name_1;
	
	SetFont(view->font_player_name, BLACK);
	DrawString(BALDA_VIEW_PLAYER_NAME_MARGIN, BALDA_VIEW_PLAYER_NAME_Y,
		balda_get_player_name(view->balda, 0));
	
	// HACK: StringWidth -- why not const char* param?
	player_name_1 = balda_get_player_name(view->balda, 1);
	DrawString(ScreenWidth() - StringWidth((char*)player_name_1) - BALDA_VIEW_PLAYER_NAME_MARGIN,
		BALDA_VIEW_PLAYER_NAME_Y,
		player_name_1);
}

void balda_view_draw_score(balda_view_t* view, int player_index, balda_bool update)
{
	int left = (player_index == 0) ? BALDA_VIEW_SCORE_MARGIN :
		ScreenWidth() - BALDA_VIEW_SCORE_MARGIN - BALDA_VIEW_SCORE_WIDTH;
	int bg_color = LGRAY;
	int score_panel_x = (player_index == 0) ? (left + BALDA_VIEW_SCORE_WIDTH) :
		(left - BALDA_VIEW_SCORE_PANEL_WIDTH);
	int score_panel_digit_x = (player_index == 0) ? (score_panel_x - BALDA_VIEW_SCORE_PANEL_PADDING +
			BALDA_VIEW_SCORE_PANEL_WIDTH - BALDA_VIEW_SCORE_PANEL_DIGIT_WIDTH) :
		(score_panel_x + BALDA_VIEW_SCORE_PANEL_PADDING);
	int score_panel_digit_w = BALDA_VIEW_SCORE_PANEL_DIGIT_WIDTH;
	
	if (update)
	{
		FillArea(score_panel_digit_x, BALDA_VIEW_SCORE_Y, score_panel_digit_w, BALDA_VIEW_SCORE_PANEL_HEIGHT, bg_color);
	}
	
	SetFont(view->font_score_panel, BLACK);
	DrawTextRect(score_panel_digit_x, BALDA_VIEW_SCORE_Y, score_panel_digit_w, BALDA_VIEW_SCORE_PANEL_HEIGHT,
		(char*)balda_itoa(&view->int_conv, balda_get_score(view->balda, player_index)),
		VALIGN_MIDDLE | (player_index == 0 ? ALIGN_RIGHT : ALIGN_LEFT));
		
	if (update)
	{
		PartialUpdate(score_panel_digit_x, BALDA_VIEW_SCORE_Y,
			score_panel_digit_w, BALDA_VIEW_SCORE_PANEL_HEIGHT);
	}
}
void balda_view_draw_scoreboard(balda_view_t* view, int player_index)
{
	int left = (player_index == 0) ? BALDA_VIEW_SCORE_MARGIN :
		(ScreenWidth() - BALDA_VIEW_SCORE_MARGIN - BALDA_VIEW_SCORE_WIDTH);
	int bg_color = LGRAY;
	int score_panel_x = (player_index == 0) ? (left + BALDA_VIEW_SCORE_WIDTH) :
		(left - BALDA_VIEW_SCORE_PANEL_WIDTH);
		
	FillArea(left, BALDA_VIEW_SCORE_Y, BALDA_VIEW_SCORE_WIDTH, BALDA_VIEW_SCORE_HEIGHT, bg_color);
	FillArea(score_panel_x, BALDA_VIEW_SCORE_Y, BALDA_VIEW_SCORE_PANEL_WIDTH,
		BALDA_VIEW_SCORE_PANEL_HEIGHT, bg_color);
		
	balda_view_draw_score(view, player_index, 0);
}

void balda_view_draw_player_last_word(balda_view_t* view, int player_index, balda_bool update)
{
	int word_index = balda_get_word_list_length(view->balda, player_index)-1;
	
	if (word_index >= 0)
	{
		int left = ((player_index == 0) ? BALDA_VIEW_SCORE_MARGIN :
			ScreenWidth() - BALDA_VIEW_SCORE_MARGIN - BALDA_VIEW_SCORE_WIDTH) + BALDA_VIEW_SCORE_WORD_PADDING;
		int top = BALDA_VIEW_SCORE_Y + BALDA_VIEW_SCORE_PANEL_HEIGHT + word_index * BALDA_VIEW_SCORE_WORD_ITEM_H + BALDA_VIEW_SCORE_WORD_PADDING;
		
		//int bg_color = LGRAY;
		
		char word_buf[64];
		const balda_char* word = balda_get_player_word(view->balda, player_index, word_index);
		balda_char_to_utf8_lowercase(word, word_buf, sizeof(word_buf));
		
		char utf8_buf[128];
		sprintf(utf8_buf, "%s (%d)", word_buf, balda_char_strlen(word));
		
		SetFont(view->font_player_word, BLACK);
		DrawTextRect(left, top, BALDA_VIEW_SCORE_WIDTH - BALDA_VIEW_SCORE_WORD_PADDING*2, BALDA_VIEW_SCORE_WORD_ITEM_H,
			utf8_buf, VALIGN_MIDDLE | (player_index == 0 ? ALIGN_LEFT : ALIGN_RIGHT));
		
		if (update)
		{
			PartialUpdate(left, top, BALDA_VIEW_SCORE_WIDTH - BALDA_VIEW_SCORE_WORD_PADDING*2, BALDA_VIEW_SCORE_WORD_ITEM_H);
		}
	}
}

void balda_view_set_keyboard(balda_view_t* view, const balda_char* keyboard)
{
	view->keyboard = keyboard;
}

void balda_view_draw_keyboard(balda_view_t* view)
{
	int x, y = BALDA_VIEW_KEYBOARD_Y, i, done=0;
	const balda_char* c = view->keyboard;
	view->keyboard_h = 0;
	
	if (c)
	{
		view->keyboard_h = 1;
		while (!done)
		{
			x = view->keyboard_left;
			for (i=0; i<BALDA_VIEW_KEYBOARD_W; ++i, x += (BALDA_VIEW_KEYBOARD_CELL_W + BALDA_VIEW_KEYBOARD_CELL_PADDING), ++c)
			{
				DrawRect(x, y, BALDA_VIEW_KEYBOARD_CELL_W, BALDA_VIEW_KEYBOARD_CELL_H, BLACK);
				
				if (*c != 0)
				{
					balda_view_draw_char(*c, x, y, BALDA_VIEW_KEYBOARD_CELL_W, BALDA_VIEW_KEYBOARD_CELL_H, view->font_keyboard, BLACK);
				}
				else
				{
					DrawBitmap(x + (BALDA_VIEW_KEYBOARD_CELL_W - img_icon_back.width) / 2,
						y + (BALDA_VIEW_KEYBOARD_CELL_H - img_icon_back.height) / 2, &img_icon_back);
					done = 1;
					break;
				}
			}
			
			if (i >= BALDA_VIEW_KEYBOARD_W)
			{
				y += BALDA_VIEW_KEYBOARD_CELL_H + BALDA_VIEW_KEYBOARD_CELL_PADDING;
				++view->keyboard_h;
			}
		}
	}
}

void balda_view_draw_game_over(balda_view_t* view)
{
	int width = (BALDA_VIEW_KEYBOARD_CELL_W + BALDA_VIEW_KEYBOARD_CELL_PADDING) * BALDA_VIEW_KEYBOARD_W;
	int height = (BALDA_VIEW_KEYBOARD_CELL_W + BALDA_VIEW_KEYBOARD_CELL_PADDING) * view->keyboard_h;
	FillArea(view->keyboard_left, BALDA_VIEW_KEYBOARD_Y,
		width, height, WHITE);
	
	int winner = balda_get_winner(view->balda);
	assert(winner != GAME_RESULT_NONE);
	
	SetFont(view->font_game_winner, BLACK);
	
	if (winner == GAME_RESULT_DRAW)
	{
		DrawTextRect(view->keyboard_left, BALDA_VIEW_KEYBOARD_Y, width, height,
			(char*)balda_string(BALDA_STR_DRAW), VALIGN_MIDDLE | ALIGN_CENTER);
	}
	else
	{
		DrawTextRect(view->keyboard_left, BALDA_VIEW_KEYBOARD_Y, width, height,
			(char*)balda_get_player_name(view->balda, winner), VALIGN_MIDDLE | ALIGN_CENTER);
	}
	
	SetFont(view->font_game_over_hint, BLACK);
	if (winner != GAME_RESULT_DRAW)
	{
		DrawTextRect(view->keyboard_left, BALDA_VIEW_KEYBOARD_Y, width, height,
			(char*)balda_string(BALDA_STR_WINNER), VALIGN_TOP | ALIGN_CENTER);
	}
	
	DrawTextRect(view->keyboard_left, BALDA_VIEW_KEYBOARD_Y, width, height,
			(char*)balda_string(BALDA_STR_PRESS_TO_PLAY_AGAIN), VALIGN_BOTTOM | ALIGN_CENTER);
	
	
	PartialUpdateBW(view->keyboard_left, BALDA_VIEW_KEYBOARD_Y,
		width, height);
}

void balda_view_draw_all(balda_view_t* view)
{
	balda_view_draw_logo(view);
	balda_view_draw_field(view, 0);
	balda_cancel_button_draw(&view->btn_cancel, 0, BALDA_CANCEL_BUTTON_FORCE_REDRAW_FULL);
	balda_view_draw_players(view);
	balda_view_draw_scoreboard(view, 0);
	balda_view_draw_scoreboard(view, 1);
	balda_view_draw_keyboard(view);
}
