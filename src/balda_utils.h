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

#ifndef __BALDA_UTILS__H__
#define __BALDA_UTILS__H__

typedef unsigned char balda_bool;

typedef struct
{
	char buffer[33];
} balda_int_converter_t;

const char* balda_itoa(balda_int_converter_t*, int);

typedef struct
{
	int x, y;
} balda_point_t;

balda_point_t balda_make_point(int x, int y);

#define BALDA_SQR(x) ((x)*(x))
#define BALDA_MIN(x, y) (((x) < (y)) ? (x) : (y))
#define BALDA_MAX(x, y) (((x) > (y)) ? (x) : (y))

#define balda_points_equal(p1, p2) (((p1).x == (p2).x) && ((p1).y == (p2).y))

#endif
