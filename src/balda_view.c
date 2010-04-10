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

typedef struct balda_view_redraw_region_t_impl
{
	balda_bool valid;
	int left, top, right, bottom;
} balda_view_redraw_region_t;

struct balda_view_t_impl
{
	// black-white region
	balda_view_redraw_region_t bw_region;
	// grayscale region
	balda_view_redraw_region_t gs_region;
};

void balda_view_redraw_region_add(balda_view_redraw_region_t* region, int x, int y, int w, int h)
{
	if (region->valid)
	{
		if (x < region->left)
			region->left = x;
		if (y < region->top)
			region->top = y;
		if ((x + w) > region->right)
			region->right = x + w;
		if ((y + h) > region->bottom)
			region->bottom = y + h;
	}
	else
	{
		region->valid = balda_true;
		region->left = x;
		region->top = y;
		region->right = x + w;
		region->bottom = y + h;
	}
}

int balda_view_redraw_region_area(balda_view_redraw_region_t* region)
{
	if (region->valid)
		return (region->right - region->left) * (region->bottom - region->top);
	return 0;
}

void balda_view_redraw_region_union(const balda_view_redraw_region_t* region1,
	const balda_view_redraw_region_t* region2, balda_view_redraw_region_t* out_region)
{
	if ((out_region->valid = (region1->valid || region2->valid)))
	{
		if ((region1->valid && ((region1->left < region2->left) || !region2->valid)))
			out_region->left = region1->left;
		else if ((region2->valid && ((region2->left < region1->left) || !region1->valid)))
			out_region->left = region2->left;
		
		if ((region1->valid && ((region1->top < region2->top) || !region2->valid)))
			out_region->top = region1->top;
		else if ((region2->valid && ((region2->top < region1->top) || !region1->valid)))
			out_region->top = region2->top;
			
		if ((region1->valid && ((region1->right > region2->right) || !region2->valid)))
			out_region->right = region1->right;
		else if ((region2->valid && ((region2->right > region1->right) || !region1->valid)))
			out_region->right = region2->right;
			
		if ((region1->valid && ((region1->bottom > region2->bottom) || !region2->valid)))
			out_region->bottom = region1->bottom;
		else if ((region2->valid && ((region2->bottom > region1->bottom) || !region1->valid)))
			out_region->bottom = region2->bottom;
	}
}

balda_view_t* balda_view_init()
{
	balda_view_t* view = (balda_view_t*)malloc(sizeof(balda_view_t));
	view->bw_region.valid = balda_false;
	view->gs_region.valid = balda_false;
	
	return view;
}

void balda_view_free(balda_view_t* view)
{
	assert(view);
	free(view);
}

void balda_view_invalidate(balda_view_t* view, int x, int y, int w, int h, balda_bool bw)
{
	debug_printf(("balda_view_invalidate: %s(%d, %d, %d, %d)",
				bw ? "BW" : "GS", x, y, w, h));
	
	if (bw)
		balda_view_redraw_region_add(&view->bw_region, x, y, w, h);
	else
		balda_view_redraw_region_add(&view->gs_region, x, y, w, h);
}

void balda_view_update(balda_view_t* view)
{
	balda_view_redraw_region_t united;
	balda_view_redraw_region_union(&view->bw_region, &view->gs_region, &united);
	int united_area = balda_view_redraw_region_area(&united);
	
	debug_printf(("view->gs_region: (%d)(%d, %d, %d, %d)",
				view->gs_region.valid, view->gs_region.left, view->gs_region.top, view->gs_region.right, view->gs_region.bottom));
	debug_printf(("view->bw_region: (%d)(%d, %d, %d, %d)",
				view->bw_region.valid, view->bw_region.left, view->bw_region.top, view->bw_region.right, view->bw_region.bottom));
	debug_printf(("united: (%d)(%d, %d, %d, %d)",
				united.valid, united.left, united.top, united.right, united.bottom));
	
	if (united_area && united_area > (((ScreenWidth() * ScreenHeight()) * 95) / 100))
	{
		debug_printf(("balda_view_update: FULL"));
		FullUpdate();
		
		view->bw_region.valid = balda_false;
		view->gs_region.valid = balda_false;
	}
	else
	{
		if (view->bw_region.valid)
		{
			debug_printf(("balda_view_update: PARTIAL_BW(%d, %d, %d, %d)",
				view->bw_region.left, view->bw_region.top,
				view->bw_region.right - view->bw_region.left,
				view->bw_region.bottom - view->bw_region.top));
			
			PartialUpdateBW(view->bw_region.left, view->bw_region.top,
				view->bw_region.right - view->bw_region.left,
				view->bw_region.bottom - view->bw_region.top);
			
			view->bw_region.valid = balda_false;
		}
		
		if (view->gs_region.valid)
		{
			debug_printf(("balda_view_update: PARTIAL_GS(%d, %d, %d, %d)",
				view->gs_region.left, view->gs_region.top,
				view->gs_region.right-view->gs_region.left,
				view->gs_region.bottom-view->gs_region.top));
			
			PartialUpdate(view->gs_region.left, view->gs_region.top,
				view->gs_region.right-view->gs_region.left,
				view->gs_region.bottom-view->gs_region.top);
			
			view->gs_region.valid = balda_false;
		}
	}
}

void balda_view_draw_char(balda_view_t* view, int x, int y, int w, int h, ifont* font, int color, balda_char c)
{
	char utf8_buf[4];
	balda_single_char_to_utf8(c, utf8_buf, sizeof(utf8_buf));
	
	SetFont(font, color);
	DrawTextRect(x, y, w, h, utf8_buf, VALIGN_MIDDLE | ALIGN_CENTER);
	balda_view_invalidate(view, x, y, w, h, !font->aa);
}

void balda_view_reset(balda_view_t* view)
{
	view->gs_region.valid = balda_false;
	view->bw_region.valid = balda_false;
}

void balda_view_draw_bitmap(balda_view_t* view, int x, int y, const ibitmap* bmp)
{
	DrawBitmap(x, y, bmp);
	balda_view_invalidate(view, x, y, bmp->width, bmp->height, bmp->depth == 2);
}

void balda_view_draw_rect(balda_view_t* view, int x, int y, int w, int h, int color)
{
	DrawRect(x, y, w, h, color);
	balda_view_invalidate(view, x, y, w, h, color == BLACK || color == WHITE);
}

void balda_view_fill_area(balda_view_t* view, int x, int y, int w, int h, int color)
{
	FillArea(x, y, w, h, color);
	balda_view_invalidate(view, x, y, w, h, color == BLACK || color == WHITE);
}
