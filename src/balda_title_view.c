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

#include "balda_title_view.h"
#include "balda_strings.h"
#include <inkview.h>
#include <assert.h>

struct balda_title_view_t_impl
{
	balda_view_t* view;
	ifont* font_version;
};

extern const ibitmap img_logo;

balda_title_view_t* balda_title_view_init(balda_view_t* view)
{
	balda_title_view_t* tv = (balda_title_view_t*)malloc(sizeof(balda_title_view_t));
	tv->view = view;
	
	tv->font_version = OpenFont("LiberationSans", 11, 1);
	
	return tv;
}

void balda_title_view_free(balda_title_view_t* tv)
{
	CloseFont(tv->font_version);

	free(tv);
}

void balda_title_view_show(balda_title_view_t* tv)
{
	// logo image
	DrawBitmap((ScreenWidth() - img_logo.width) / 2, 16, &img_logo);
	
	// version
	SetFont(tv->font_version, BLACK);
	DrawString(ScreenWidth() - StringWidth(BALDA_APP_VERSION_STRING_FULL_RU) - 10, 10,
		BALDA_APP_VERSION_STRING_FULL_RU);
}
