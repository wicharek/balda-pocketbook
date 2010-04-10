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

#include "balda_utils.h"
#include "balda_char.h"

struct balda_view_t_impl;
typedef struct balda_view_t_impl balda_view_t;

typedef struct ifont_s ifont;
typedef struct ibitmap_s ibitmap;

balda_view_t* balda_view_init();
void balda_view_free(balda_view_t*);

void balda_view_invalidate(balda_view_t* view, int x, int y, int w, int h, balda_bool bw);

void balda_view_reset(balda_view_t* view);
void balda_view_update(balda_view_t* view);
void balda_view_draw_char(balda_view_t* view, int x, int y, int w, int h, ifont* font, int color, balda_char c);
void balda_view_draw_bitmap(balda_view_t* view, int x, int y, const ibitmap* bmp);
void balda_view_draw_rect(balda_view_t* view, int x, int y, int w, int h, int color);
void balda_view_fill_area(balda_view_t* view, int x, int y, int w, int h, int color);

#endif
