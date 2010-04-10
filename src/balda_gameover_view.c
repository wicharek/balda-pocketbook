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

#include "balda_gameover_view.h"
#include "balda_strings.h"
#include <inkview.h>
#include <assert.h>

struct balda_gameover_view_t_impl
{
	balda_view_t* view;
	balda_t* balda;
	int x, y, w, h;
	
	ifont* font_game_winner;
	ifont* font_game_over_hint;
};

balda_gameover_view_t* balda_gameover_view_init(balda_view_t* view, balda_t* balda,
	int x, int y, int w, int h)
{
	balda_gameover_view_t* go = (balda_gameover_view_t*)malloc(sizeof(balda_gameover_view_t));
	go->view = view;
	go->balda = balda;
	
	go->font_game_winner = OpenFont("LiberationSans-Bold", 42, 0);
	go->font_game_over_hint = OpenFont("LiberationSans", 18, 0);
	
	go->x = x;
	go->y = y;
	go->w = w;
	go->h = h;
	
	return go;
}

void balda_gameover_view_free(balda_gameover_view_t* go)
{
	CloseFont(go->font_game_winner);
	CloseFont(go->font_game_over_hint);
	
	free(go);
}

void balda_gameover_view_show(balda_gameover_view_t* go)
{
	FillArea(go->x, go->y, go->w, go->h, WHITE);
	
	int winner = balda_get_winner(go->balda);
	assert(winner != GAME_RESULT_NONE);
	
	SetFont(go->font_game_winner, BLACK);
	
	if (winner == GAME_RESULT_DRAW)
	{
		DrawTextRect(go->x, go->y, go->w, go->h,
			(char*)balda_string(BALDA_STR_DRAW), VALIGN_MIDDLE | ALIGN_CENTER);
	}
	else
	{
		DrawTextRect(go->x, go->y, go->w, go->h,
			(char*)balda_get_player_name(go->balda, winner), VALIGN_MIDDLE | ALIGN_CENTER);
	}
	
	SetFont(go->font_game_over_hint, BLACK);
	if (winner != GAME_RESULT_DRAW)
	{
		DrawTextRect(go->x, go->y, go->w, go->h,
			(char*)balda_string(BALDA_STR_WINNER), VALIGN_TOP | ALIGN_CENTER);
	}
	
	DrawTextRect(go->x, go->y, go->w, go->h,
			(char*)balda_string(BALDA_STR_PRESS_TO_PLAY_AGAIN), VALIGN_BOTTOM | ALIGN_CENTER);
	
	balda_view_invalidate(go->view, go->x, go->y, go->w, go->h, 1);
}
