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

#include "balda_cancel_button.h"
#include "balda_strings.h"

extern const ibitmap img_icon_surrender;
extern const ibitmap img_icon_back;

void balda_cancel_button_init(balda_cancel_button_t* button, balda_point_t pos)
{
	button->pos = pos;
	button->selected = 0;
	button->visible = 0;
	button->mode = BALDA_CANCEL_BUTTON_MODE_BACK;
	button->redraw_flags = BALDA_CANCEL_BUTTON_REDRAW_MODE;
	button->font_caption = OpenFont("LiberationSans-Bold", 24, 0);
}

void balda_cancel_button_free(balda_cancel_button_t* button)
{
	CloseFont(button->font_caption);
}

void balda_cancel_button_set_mode(balda_cancel_button_t* button, BALDA_CANCEL_BUTTON_MODE mode)
{
	if (button->mode != mode)
	{
		button->mode = mode;
		button->redraw_flags ^= BALDA_CANCEL_BUTTON_REDRAW_MODE;
	}
}

void balda_cancel_button_set_visible(balda_cancel_button_t* button, balda_bool visible)
{
	if (button->visible != visible)
	{
		button->visible = visible;
		button->redraw_flags ^= BALDA_CANCEL_BUTTON_REDRAW_VISIBLE;
	}
}

void balda_cancel_button_invert(balda_cancel_button_t* button)
{
	InvertAreaBW(button->pos.x-BALDA_CANCEL_BUTTON_MARGIN, button->pos.y-BALDA_CANCEL_BUTTON_MARGIN,
		BALDA_CANCEL_BUTTON_WIDTH+BALDA_CANCEL_BUTTON_MARGIN*2, BALDA_CANCEL_BUTTON_HEIGHT+BALDA_CANCEL_BUTTON_MARGIN*2);
}

void balda_cancel_button_update(balda_cancel_button_t* button)
{
	PartialUpdateBW(button->pos.x-BALDA_CANCEL_BUTTON_MARGIN, button->pos.y-BALDA_CANCEL_BUTTON_MARGIN,
		BALDA_CANCEL_BUTTON_WIDTH+BALDA_CANCEL_BUTTON_MARGIN*2, BALDA_CANCEL_BUTTON_HEIGHT+BALDA_CANCEL_BUTTON_MARGIN*2);
}

void balda_cancel_button_set_selected(balda_cancel_button_t* button, balda_bool selected)
{
	if (button->selected != selected)
	{
		button->selected = selected;
		button->redraw_flags ^= BALDA_CANCEL_BUTTON_REDRAW_SELECTED;
	}
}

void balda_cancel_button_clear(balda_cancel_button_t* button)
{
	FillArea(button->pos.x-BALDA_CANCEL_BUTTON_MARGIN, button->pos.y-BALDA_CANCEL_BUTTON_MARGIN,
		BALDA_CANCEL_BUTTON_WIDTH+BALDA_CANCEL_BUTTON_MARGIN*2, BALDA_CANCEL_BUTTON_HEIGHT+BALDA_CANCEL_BUTTON_MARGIN*2, WHITE);
}

void balda_cancel_button_draw_content(balda_cancel_button_t* button, const ibitmap* icon, const char* caption)
{
	DrawBitmap(button->pos.x + BALDA_CANCEL_BUTTON_ICON_PADDING,
		button->pos.y + (BALDA_CANCEL_BUTTON_HEIGHT - icon->height) / 2, icon);
	SetFont(button->font_caption, BLACK);
	DrawTextRect(button->pos.x + BALDA_CANCEL_BUTTON_ICON_PADDING + icon->width + BALDA_CANCEL_BUTTON_ICON_CAPTIONS_OFFSET,
		button->pos.y, BALDA_CANCEL_BUTTON_WIDTH-(BALDA_CANCEL_BUTTON_ICON_PADDING + icon->width + BALDA_CANCEL_BUTTON_ICON_CAPTIONS_OFFSET),
		BALDA_CANCEL_BUTTON_HEIGHT, (char*)caption, VALIGN_MIDDLE | ALIGN_LEFT);
}

void balda_cancel_button_draw(balda_cancel_button_t* button, balda_bool update, BALDA_CANCEL_BUTTON_FORCE_REDRAW force)
{
	balda_bool full_redraw = (force == BALDA_CANCEL_BUTTON_FORCE_REDRAW_FULL) ||
		(button->redraw_flags & BALDA_CANCEL_BUTTON_REDRAW_MODE) ||
		(button->redraw_flags & BALDA_CANCEL_BUTTON_REDRAW_VISIBLE);
	
	if (!button->visible && (button->redraw_flags & BALDA_CANCEL_BUTTON_FORCE_REDRAW_NO_CLEAR))
	{
		return;
	}
	else
	{
		if (full_redraw)
		{
			balda_cancel_button_clear(button);
			if (button->visible)
			{
				DrawRect(button->pos.x, button->pos.y, BALDA_CANCEL_BUTTON_WIDTH, BALDA_CANCEL_BUTTON_HEIGHT, BLACK);
				
				switch (button->mode)
				{
					case BALDA_CANCEL_BUTTON_MODE_SURRENDER:
					{
						balda_cancel_button_draw_content(button, &img_icon_surrender, balda_string(BALDA_STR_CAPTION_SURRENDER));
					}
					break;
					
					case BALDA_CANCEL_BUTTON_MODE_BACK:
					{
						balda_cancel_button_draw_content(button, &img_icon_back, balda_string(BALDA_STR_CAPTION_BACK));
					}
					break;
				}
			}
		
			if (button->selected)
			{
				balda_cancel_button_invert(button);
			}
		}
		else if (button->redraw_flags & BALDA_CANCEL_BUTTON_REDRAW_SELECTED)
		{
			balda_cancel_button_invert(button);
		}
	}
	
	button->redraw_flags = BALDA_CANCEL_BUTTON_REDRAW_NONE;
	
	if (update)
	{
		balda_cancel_button_update(button);
	}
}
