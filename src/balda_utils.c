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

#include "balda_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

const char* balda_itoa(balda_int_converter_t* converter, int value)
{
	sprintf(converter->buffer, "%d", value);
	return converter->buffer;
}

balda_point_t balda_make_point(int x, int y)
{
	balda_point_t p;
	p.x = x;
	p.y = y;
	return p;
}

int balda_point_distance_squared(int x1, int y1, int x2, int y2)
{
	return BALDA_SQR(x2 - x1) + BALDA_SQR(y2 - y1);
}
/*
double rand_uniform()
{
	return (double)(rand()) / (double)(RAND_MAX);
}

double rand_normal()
{
	static double stored = -1;
	
	if (stored < 0)
	{
		double u = rand_uniform();
		double v = rand_uniform();
		
		double x = sqrt(-2 * log(u)) * cos(2 * M_PI * v);
		stored = sqrt(-2 * log(u)) * sin(2 * M_PI * v);
		
		return x;
	}
	else
	{
		double x = stored;
		stored = -1;
		return x;
	}
}

double rand_normal_with_interval(double interval)
{
	double n = rand_normal();
	if (n < -interval)
		n = -interval;
	else if (n > interval)
		n = interval;
	
	return (n + interval) / (interval + interval);
}
*/
