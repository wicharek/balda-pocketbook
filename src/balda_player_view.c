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

#include "balda_player_view.h"
#include "balda_strings.h"
#include <inkview.h>

const int BALDA_PLAYER_VIEW_MARGIN = 10;
const int BALDA_PLAYER_VIEW_NAME_Y = 120;
const int BALDA_PLAYER_VIEW_FRAME_Y = 142;
const int BALDA_PLAYER_VIEW_BASE_WIDTH = 160;
const int BALDA_PLAYER_VIEW_TURN_MARK_Y = 102;
const int BALDA_PLAYER_VIEW_TURN_MARK_MARGIN = 10;

const int BALDA_PLAYER_VIEW_SCORE_X = 170;
const int BALDA_PLAYER_VIEW_SCORE_Y = 142;
const int BALDA_PLAYER_VIEW_SCORE_W = 80;
const int BALDA_PLAYER_VIEW_SCORE_H = 50;

const int BALDA_PLAYER_VIEW_WORD_MARGIN_X = 12;
const int BALDA_PLAYER_VIEW_WORD_W = 160;
const int BALDA_PLAYER_VIEW_WORD_PADDING = 6;
const int BALDA_PLAYER_VIEW_WORD_ITEM_H = 24;
const int BALDA_PLAYER_VIEW_WORD_Y = 150;

struct balda_player_view_t_impl
{
	int index;
	balda_view_t* view;
	balda_t* balda;
	
	int player_name_w, player_name_h;
	
	ifont* font_player_word;
	ifont* font_player_name;
	ifont* font_score_panel;
	
	balda_int_converter_t int_conv;
};

extern const ibitmap img_turn_mark;

balda_player_view_t* balda_player_view_init(balda_view_t* view, balda_t* balda, int index)
{
	balda_player_view_t* pv = (balda_player_view_t*)malloc(sizeof(balda_player_view_t));
	pv->view = view;
	pv->index = index;
	pv->balda = balda;
	pv->player_name_w = 0;
	
	pv->font_player_name = OpenFont("LiberationSans", 18, 1);
	pv->font_score_panel = OpenFont("LiberationSans-Bold", 36, 0);
	pv->font_player_word = OpenFont("LiberationSans", 16, 1);
	
	pv->player_name_h = 18;
	
	return pv;
}

void balda_player_view_free(balda_player_view_t* pv)
{
	CloseFont(pv->font_player_name);
	CloseFont(pv->font_score_panel);
	CloseFont(pv->font_player_word);
	
	free(pv);
}

void balda_player_view_draw_name(balda_player_view_t* pv)
{
	int x, w = StringWidth((char*)balda_get_player_name(pv->balda, pv->index));
	
	if (pv->index == 0)
	{
		x = BALDA_PLAYER_VIEW_MARGIN;
	}
	else
	{
		x = ScreenWidth() - w
			- BALDA_PLAYER_VIEW_MARGIN;
	}
	
	balda_view_invalidate(pv->view, x, BALDA_PLAYER_VIEW_NAME_Y,
		pv->player_name_w, pv->player_name_h, 0);
	
	SetFont(pv->font_player_name, BLACK);
	DrawString(x, BALDA_PLAYER_VIEW_NAME_Y,
		balda_get_player_name(pv->balda, pv->index));
	
	pv->player_name_w = w;	
	balda_view_invalidate(pv->view, x, BALDA_PLAYER_VIEW_NAME_Y,
		pv->player_name_w, pv->player_name_h, 0);
}

void balda_player_view_draw_frame(balda_player_view_t* pv)
{
	int x;
	
	if (pv->index == 0)
	{
		x = BALDA_PLAYER_VIEW_MARGIN;
	}
	else
	{
		x = ScreenWidth() - BALDA_PLAYER_VIEW_BASE_WIDTH - BALDA_PLAYER_VIEW_MARGIN;
	}
	
	DrawLine(x, BALDA_PLAYER_VIEW_FRAME_Y, x + BALDA_PLAYER_VIEW_BASE_WIDTH, BALDA_PLAYER_VIEW_FRAME_Y,
		BLACK);
	balda_view_invalidate(pv->view, x, BALDA_PLAYER_VIEW_FRAME_Y,
		BALDA_PLAYER_VIEW_BASE_WIDTH, 1, 1);
}

int balda_player_view_turn_mark_x(balda_player_view_t* pv)
{
	if (pv->index == 0)
	{
		return BALDA_PLAYER_VIEW_TURN_MARK_MARGIN;
	}
	
	return ScreenWidth() - img_turn_mark.width - BALDA_PLAYER_VIEW_TURN_MARK_MARGIN;
}

void balda_player_view_clear_active(balda_player_view_t* pv)
{
	balda_view_fill_area(pv->view,
		balda_player_view_turn_mark_x(pv), BALDA_PLAYER_VIEW_TURN_MARK_Y,
		img_turn_mark.width, img_turn_mark.height,
		WHITE);
}

void balda_player_view_draw_active(balda_player_view_t* pv)
{
	balda_view_draw_bitmap(pv->view, balda_player_view_turn_mark_x(pv),
		BALDA_PLAYER_VIEW_TURN_MARK_Y, &img_turn_mark);
}

int balda_player_view_score_x(balda_player_view_t* pv)
{
	if (pv->index == 0)
	{
		return BALDA_PLAYER_VIEW_SCORE_X;
	}
	
	return ScreenWidth() - BALDA_PLAYER_VIEW_SCORE_X - BALDA_PLAYER_VIEW_SCORE_W;
}

void balda_player_view_draw_score(balda_player_view_t* pv)
{
	balda_view_fill_area(pv->view,
		balda_player_view_score_x(pv), BALDA_PLAYER_VIEW_SCORE_Y,
		BALDA_PLAYER_VIEW_SCORE_W, BALDA_PLAYER_VIEW_SCORE_H,
		BLACK);
	
	SetFont(pv->font_score_panel, WHITE);
	DrawTextRect(balda_player_view_score_x(pv), BALDA_PLAYER_VIEW_SCORE_Y,
		BALDA_PLAYER_VIEW_SCORE_W, BALDA_PLAYER_VIEW_SCORE_H,
		(char*)balda_itoa(&pv->int_conv, balda_get_score(pv->balda, pv->index)),
		VALIGN_MIDDLE | ALIGN_CENTER);
}

void balda_player_view_draw_word(balda_player_view_t* pv, int word_index)
{
	SetFont(pv->font_player_word, BLACK);
	
	int left = ((pv->index == 0) ? BALDA_PLAYER_VIEW_WORD_MARGIN_X :
		ScreenWidth() - BALDA_PLAYER_VIEW_WORD_MARGIN_X - BALDA_PLAYER_VIEW_WORD_W)
			+ BALDA_PLAYER_VIEW_WORD_PADDING;
	int top = BALDA_PLAYER_VIEW_WORD_Y
		+ word_index * BALDA_PLAYER_VIEW_WORD_ITEM_H + BALDA_PLAYER_VIEW_WORD_PADDING;
	
	char word_buf[64];
	const balda_char* word = balda_get_player_word(pv->balda, pv->index, word_index);
	balda_char_to_utf8_lowercase(word, word_buf, sizeof(word_buf));
	
	char utf8_buf[128];
	sprintf(utf8_buf, "%s (%d)", word_buf, balda_char_strlen(word));
	
	DrawTextRect(left, top,
		BALDA_PLAYER_VIEW_WORD_W - BALDA_PLAYER_VIEW_WORD_PADDING * 2,
		BALDA_PLAYER_VIEW_WORD_ITEM_H,
		utf8_buf, VALIGN_MIDDLE | (pv->index == 0 ? ALIGN_LEFT : ALIGN_RIGHT));
	balda_view_invalidate(pv->view, left, top,
		BALDA_PLAYER_VIEW_WORD_W - BALDA_PLAYER_VIEW_WORD_PADDING * 2,
		BALDA_PLAYER_VIEW_WORD_ITEM_H,
		1);
}

void balda_player_view_draw(balda_player_view_t* pv)
{
	balda_player_view_draw_name(pv);
	balda_player_view_draw_frame(pv);
}

void balda_player_view_update_name(balda_player_view_t* pv)
{
	// TODO
}

void balda_player_view_update_active(balda_player_view_t* pv)
{
	balda_player_view_clear_active(pv);
	if (balda_get_active_player(pv->balda) == pv->index)
		balda_player_view_draw_active(pv);
}

void balda_player_view_update_score(balda_player_view_t* pv)
{
	balda_player_view_draw_score(pv);
}

void balda_player_view_draw_last_word(balda_player_view_t* pv)
{
	int l = balda_get_word_list_length(pv->balda, pv->index);
	if (l > 0)
		balda_player_view_draw_word(pv, l-1);
}
