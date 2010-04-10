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

#include "balda_button.h"
#include "balda_strings.h"
#include <inkview.h>

extern const ibitmap img_icon_surrender;
extern const ibitmap img_icon_back;
extern const ibitmap img_icon_confirm;
extern const ibitmap img_icon_next;

typedef enum
{
	BALDA_BUTTON_REDRAW_NONE = 0,
	BALDA_BUTTON_REDRAW_SELECTED = 0x01,
	BALDA_BUTTON_REDRAW_VISIBLE = 0x02,
	BALDA_BUTTON_REDRAW_MODE = 0x04
} BALDA_BUTTON_REDRAW_FLAG;

typedef enum
{
	BALDA_BUTTON_FORCE_REDRAW_NONE,
	BALDA_BUTTON_FORCE_REDRAW_FULL,
	BALDA_BUTTON_FORCE_REDRAW_NO_CLEAR
} BALDA_BUTTON_FORCE_REDRAW;

struct balda_button_t_impl
{
	balda_point_t pos;
	BALDA_BUTTON_MODE mode;
	balda_bool selected;
	balda_bool visible;
	ifont* font_caption;
	int redraw_flags;
	balda_view_t* view;
};


balda_button_t* balda_button_init(balda_view_t* view, balda_point_t pos)
{
	balda_button_t* button = (balda_button_t*)malloc(sizeof(balda_button_t));
	
	button->view = view;
	button->pos = pos;
	button->selected = 0;
	button->visible = 0;
	button->mode = BALDA_BUTTON_MODE_BACK;
	button->redraw_flags = BALDA_BUTTON_REDRAW_MODE;
	button->font_caption = OpenFont("LiberationSans-Bold", 24, 0);
	
	return button;
}

void balda_button_free(balda_button_t* button)
{
	CloseFont(button->font_caption);
	free(button);
}

balda_bool balda_button_contains_point(balda_button_t* button, balda_point_t point)
{
	if (button->visible)
	{
		point.x -= button->pos.x;
		point.y -= button->pos.y;
		return point.x >= 0
			&& point.y >= 0
			&& point.x < BALDA_BUTTON_WIDTH
			&& point.y < BALDA_BUTTON_HEIGHT;
	}
	
	return 0;
}

void balda_button_update(balda_button_t* button)
{
	//PartialUpdateBW(button->pos.x-BALDA_BUTTON_MARGIN, button->pos.y-BALDA_BUTTON_MARGIN,
	//	BALDA_BUTTON_WIDTH+BALDA_BUTTON_MARGIN*2, BALDA_BUTTON_HEIGHT+BALDA_BUTTON_MARGIN*2);
	
	balda_view_invalidate(button->view,
		button->pos.x-BALDA_BUTTON_MARGIN, button->pos.y-BALDA_BUTTON_MARGIN,
		BALDA_BUTTON_WIDTH+BALDA_BUTTON_MARGIN*2, BALDA_BUTTON_HEIGHT+BALDA_BUTTON_MARGIN*2,
		1);
}

void balda_button_invert(balda_button_t* button)
{
	InvertAreaBW(button->pos.x-BALDA_BUTTON_MARGIN, button->pos.y-BALDA_BUTTON_MARGIN,
		BALDA_BUTTON_WIDTH+BALDA_BUTTON_MARGIN*2, BALDA_BUTTON_HEIGHT+BALDA_BUTTON_MARGIN*2);
}

void balda_button_clear(balda_button_t* button)
{
	FillArea(button->pos.x-BALDA_BUTTON_MARGIN, button->pos.y-BALDA_BUTTON_MARGIN,
		BALDA_BUTTON_WIDTH+BALDA_BUTTON_MARGIN*2, BALDA_BUTTON_HEIGHT+BALDA_BUTTON_MARGIN*2, WHITE);
}

void balda_button_draw_content(balda_button_t* button, const ibitmap* icon, const char* caption)
{
	DrawBitmap(button->pos.x + BALDA_BUTTON_ICON_PADDING,
		button->pos.y + (BALDA_BUTTON_HEIGHT - icon->height) / 2, icon);
	SetFont(button->font_caption, BLACK);
	DrawTextRect(button->pos.x + BALDA_BUTTON_ICON_PADDING + icon->width + BALDA_BUTTON_ICON_CAPTIONS_OFFSET,
		button->pos.y, BALDA_BUTTON_WIDTH-(BALDA_BUTTON_ICON_PADDING + icon->width + BALDA_BUTTON_ICON_CAPTIONS_OFFSET),
		BALDA_BUTTON_HEIGHT, (char*)caption, VALIGN_MIDDLE | ALIGN_LEFT);
}

void balda_button_draw(balda_button_t* button, BALDA_BUTTON_FORCE_REDRAW force)
{
	balda_bool full_redraw = (force == BALDA_BUTTON_FORCE_REDRAW_FULL) ||
		(button->redraw_flags & BALDA_BUTTON_REDRAW_MODE) ||
		(button->redraw_flags & BALDA_BUTTON_REDRAW_VISIBLE);
	
	if (!button->visible && !full_redraw)
	{
		return;
	}
	else
	{
		if (full_redraw)
		{
			balda_button_clear(button);
			if (button->visible)
			{
				DrawRect(button->pos.x, button->pos.y, BALDA_BUTTON_WIDTH, BALDA_BUTTON_HEIGHT, BLACK);
				
				switch (button->mode)
				{
					case BALDA_BUTTON_MODE_SURRENDER:
					{
						balda_button_draw_content(button, &img_icon_surrender, balda_string(BALDA_STR_CAPTION_SURRENDER));
					}
					break;
					
					case BALDA_BUTTON_MODE_BACK:
					{
						balda_button_draw_content(button, &img_icon_back, balda_string(BALDA_STR_CAPTION_BACK));
					}
					break;
					
					case BALDA_BUTTON_MODE_CONFIRM:
					{
						balda_button_draw_content(button, &img_icon_confirm, balda_string(BALDA_STR_CAPTION_CONFIRM));
					}
					break;
					
					case BALDA_BUTTON_MODE_NEXT:
					{
						balda_button_draw_content(button, &img_icon_next, balda_string(BALDA_STR_CAPTION_NEXT));
					}
					break;
				}
			}
		
			if (button->selected && button->visible)
			{
				balda_button_invert(button);
			}
		}
		else if (button->redraw_flags & BALDA_BUTTON_REDRAW_SELECTED)
		{
			balda_button_invert(button);
		}
	}
	
	button->redraw_flags = BALDA_BUTTON_REDRAW_NONE;
	
	balda_button_update(button);
}


void balda_button_set_mode(balda_button_t* button, BALDA_BUTTON_MODE mode)
{
	if (button->mode != mode)
	{
		button->mode = mode;
		button->redraw_flags |= BALDA_BUTTON_REDRAW_MODE;
	}
}

void balda_button_set_visible(balda_button_t* button, balda_bool visible)
{
	if (button->visible != visible)
	{
		button->visible = visible;
		button->redraw_flags ^= BALDA_BUTTON_REDRAW_VISIBLE;
	}
}

void balda_button_set_selected(balda_button_t* button, balda_bool selected)
{
	if (button->selected != selected)
	{
		button->selected = selected;
		button->redraw_flags ^= BALDA_BUTTON_REDRAW_SELECTED;
	}
}

void balda_button_redraw(balda_button_t* button, balda_bool force)
{
	balda_button_draw(button, force ? BALDA_BUTTON_FORCE_REDRAW_FULL :
		BALDA_BUTTON_FORCE_REDRAW_NONE);
}
