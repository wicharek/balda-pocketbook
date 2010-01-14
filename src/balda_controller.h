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

#ifndef __BALDA_CONTROLLER_H__
#define __BALDA_CONTROLLER_H__

#include "balda.h"

typedef enum
{
	BALDA_TURN_STAGE_SELECT_POS,
	BALDA_TURN_STAGE_SELECT_LETTER,
	BALDA_TURN_STAGE_SELECT_FIRST,
	BALDA_TURN_STAGE_DEFINE_WORD
} BALDA_TURN_STAGE;

void balda_controller_init(balda_t*);
int balda_controller_handler(int type, int par1, int par2);

#endif
