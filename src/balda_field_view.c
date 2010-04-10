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

#include "balda_field_view.h"
#include "balda_strings.h"

const int BALDA_VIEW_FIELD_Y = 230;
const int BALDA_VIEW_FIELD_CELL_W = 45;
const int BALDA_VIEW_FIELD_CELL_H = 45;
const int BALDA_VIEW_FIELD_CELL_PADDING = 2;

const int BALDA_VIEW_FIELD_SEQUENCE_FILL_COLOR = BLACK;
const int BALDA_VIEW_FIELD_SEQUENCE_STROKE_COLOR = WHITE;

typedef enum
{
	BALDA_FIELD_VIEW_CELL_SELECTION_NONE = 0,
	BALDA_FIELD_VIEW_CELL_SELECTION_SINGLE = 1,
	BALDA_FIELD_VIEW_CELL_SELECTION_SEQUENCE = 2
}
BALDA_FIELD_VIEW_CELL_SELECTION;

typedef struct
{
	// Type of the cell
	BALDA_FIELD_VIEW_CELL_SELECTION selection:2;
	// Is character in this cell currently being inserted
	balda_bool insert:1;
	// Character in cell
	balda_char ch:6;
	
	// "Opened" borders in select sequence cell
	balda_bool sequence_left:1;
	balda_bool sequence_top:1;
	balda_bool sequence_right:1;
	balda_bool sequence_bottom:1;
}
balda_field_view_cell_t;

struct balda_field_view_t_impl
{
	balda_view_t* view;
	
	ifont* font;
	
	balda_bool drawn;
	balda_field_view_cell_t cells[BALDA_FIELD_WIDTH][BALDA_FIELD_HEIGHT];
	
	balda_bool selected;
	balda_point_t selected_pos;
	
	balda_bool inserted;
	balda_point_t insert_pos;
	balda_char insert_char;
	
	int left;
};

#define balda_field_view_cell_reset(cell) memset((cell), 0, sizeof(balda_field_view_cell_t))
#define balda_field_view_cells_equal(cell1, cell2) \
	( \
		((cell1).selection == (cell2).selection) && ((cell1).ch == (cell2).ch) && ((cell1).insert == (cell2).insert) \
			&& (((cell1).selection != BALDA_FIELD_VIEW_CELL_SELECTION_SEQUENCE) \
				|| ((cell1).sequence_left == (cell2).sequence_left && (cell1).sequence_top == (cell2).sequence_top \
					&& (cell1).sequence_right == (cell2).sequence_right && (cell1).sequence_bottom == (cell2).sequence_bottom)) \
	)


balda_field_view_t* balda_field_view_init(balda_view_t* view)
{
	balda_field_view_t* field = (balda_field_view_t*)malloc(sizeof(balda_field_view_t));
	field->view = view;
	field->drawn = 0;
	
	field->left = (ScreenWidth() - BALDA_VIEW_FIELD_CELL_W * BALDA_FIELD_WIDTH
		- BALDA_VIEW_FIELD_CELL_PADDING * (BALDA_FIELD_WIDTH - 1)) / 2;
	
	field->font = OpenFont("LiberationSans-Bold", 26, 0);
	
	memset(field->cells, 0, sizeof(field->cells));
	
	return field;
}

void balda_field_view_free(balda_field_view_t* field)
{
	assert(field);
	CloseFont(field->font);
	free(field);
}

#define balda_field_view_cell_x(field, cell_x) ((field)->left + (cell_x) * (BALDA_VIEW_FIELD_CELL_W + BALDA_VIEW_FIELD_CELL_PADDING))
#define balda_field_view_cell_y(field, cell_y) (BALDA_VIEW_FIELD_Y + (cell_y) * (BALDA_VIEW_FIELD_CELL_H + BALDA_VIEW_FIELD_CELL_PADDING))

void balda_field_view_draw_cell(balda_field_view_t* field, int x, int y, balda_field_view_cell_t* old_value)
{
	balda_field_view_cell_t cell = field->cells[x][y];
	int update_grow = 0;
	
	x = balda_field_view_cell_x(field, x);
	y = balda_field_view_cell_y(field, y);
	
	if (cell.selection == BALDA_FIELD_VIEW_CELL_SELECTION_SEQUENCE)
	{	
		FillArea(x - BALDA_VIEW_FIELD_CELL_PADDING/2,
			y - BALDA_VIEW_FIELD_CELL_PADDING/2,
			BALDA_VIEW_FIELD_CELL_W + BALDA_VIEW_FIELD_CELL_PADDING,
			BALDA_VIEW_FIELD_CELL_H + BALDA_VIEW_FIELD_CELL_PADDING,
			BALDA_VIEW_FIELD_SEQUENCE_FILL_COLOR);
		
		if (cell.sequence_top) // opened from top
		{
			DrawLine(x, y,
				x, y-BALDA_VIEW_FIELD_CELL_PADDING/2, BALDA_VIEW_FIELD_SEQUENCE_STROKE_COLOR);
			DrawLine(x+BALDA_VIEW_FIELD_CELL_W-1, y,
				x+BALDA_VIEW_FIELD_CELL_W-1, y-BALDA_VIEW_FIELD_CELL_PADDING/2, BALDA_VIEW_FIELD_SEQUENCE_STROKE_COLOR);
		}
		else
		{
			DrawLine(x, y,
				x+BALDA_VIEW_FIELD_CELL_W-1, y, BALDA_VIEW_FIELD_SEQUENCE_STROKE_COLOR);
		}
		
		if (cell.sequence_bottom) // opened from bottom
		{
			DrawLine(x, y+BALDA_VIEW_FIELD_CELL_H,
				x, y+BALDA_VIEW_FIELD_CELL_H-1+BALDA_VIEW_FIELD_CELL_PADDING/2, BALDA_VIEW_FIELD_SEQUENCE_STROKE_COLOR);
			DrawLine(x+BALDA_VIEW_FIELD_CELL_W-1, y+BALDA_VIEW_FIELD_CELL_H-1,
				x+BALDA_VIEW_FIELD_CELL_W-1, y+BALDA_VIEW_FIELD_CELL_H-1+BALDA_VIEW_FIELD_CELL_PADDING/2, BALDA_VIEW_FIELD_SEQUENCE_STROKE_COLOR);
		}
		else
		{
			DrawLine(x, y+BALDA_VIEW_FIELD_CELL_H-1,
				x+BALDA_VIEW_FIELD_CELL_W-1, y+BALDA_VIEW_FIELD_CELL_H-1, BALDA_VIEW_FIELD_SEQUENCE_STROKE_COLOR);
		}
		
		if (cell.sequence_left) // opened from left
		{
			DrawLine(x, y,
				x-BALDA_VIEW_FIELD_CELL_PADDING/2, y, BALDA_VIEW_FIELD_SEQUENCE_STROKE_COLOR);
			DrawLine(x, y+BALDA_VIEW_FIELD_CELL_H-1,
				x-BALDA_VIEW_FIELD_CELL_PADDING/2, y+BALDA_VIEW_FIELD_CELL_H-1, BALDA_VIEW_FIELD_SEQUENCE_STROKE_COLOR);
		}
		else
		{
			DrawLine(x, y,
				x, y+BALDA_VIEW_FIELD_CELL_H-1, BALDA_VIEW_FIELD_SEQUENCE_STROKE_COLOR);
		}
		
		if (cell.sequence_right) // opened from right
		{
			DrawLine(x+BALDA_VIEW_FIELD_CELL_W-1, y,
				x+BALDA_VIEW_FIELD_CELL_W-1+BALDA_VIEW_FIELD_CELL_PADDING/2, y, BALDA_VIEW_FIELD_SEQUENCE_STROKE_COLOR);
			DrawLine(x+BALDA_VIEW_FIELD_CELL_W, y+BALDA_VIEW_FIELD_CELL_H-1,
				x+BALDA_VIEW_FIELD_CELL_W-1+BALDA_VIEW_FIELD_CELL_PADDING/2, y+BALDA_VIEW_FIELD_CELL_H-1, BALDA_VIEW_FIELD_SEQUENCE_STROKE_COLOR);
		}
		else
		{
			DrawLine(x+BALDA_VIEW_FIELD_CELL_W-1, y,
				x+BALDA_VIEW_FIELD_CELL_W-1, y+BALDA_VIEW_FIELD_CELL_H-1, BALDA_VIEW_FIELD_SEQUENCE_STROKE_COLOR);
		}
		
		update_grow = BALDA_VIEW_FIELD_CELL_PADDING / 2;
		
		if (cell.insert)
		{
			DrawRect(x+3, y+3, BALDA_VIEW_FIELD_CELL_W-6, BALDA_VIEW_FIELD_CELL_H-6, BALDA_VIEW_FIELD_SEQUENCE_STROKE_COLOR);
		}
	}
	else
	{
		FillArea(x - BALDA_VIEW_FIELD_CELL_PADDING/2,
			y - BALDA_VIEW_FIELD_CELL_PADDING/2,
			BALDA_VIEW_FIELD_CELL_W + BALDA_VIEW_FIELD_CELL_PADDING,
			BALDA_VIEW_FIELD_CELL_H + BALDA_VIEW_FIELD_CELL_PADDING,
			WHITE);
			
		DrawRect(x, y, BALDA_VIEW_FIELD_CELL_W, BALDA_VIEW_FIELD_CELL_H, BLACK);
		if (cell.insert)
		{
			DrawRect(x+1, y+1, BALDA_VIEW_FIELD_CELL_W-2, BALDA_VIEW_FIELD_CELL_H-2, BLACK);
		}
	}
	
	if (cell.ch)
	{
		balda_view_draw_char(field->view,
			x, y, BALDA_VIEW_FIELD_CELL_W, BALDA_VIEW_FIELD_CELL_H,
			field->font,
			cell.selection == BALDA_FIELD_VIEW_CELL_SELECTION_SEQUENCE ? BALDA_VIEW_FIELD_SEQUENCE_STROKE_COLOR : BLACK,
			cell.ch);
	}
	
	if (cell.selection == BALDA_FIELD_VIEW_CELL_SELECTION_SINGLE)
	{
		update_grow = BALDA_VIEW_FIELD_CELL_PADDING / 2;
		InvertAreaBW(x-update_grow, y-update_grow,
			BALDA_VIEW_FIELD_CELL_W + update_grow*2, BALDA_VIEW_FIELD_CELL_H + update_grow*2);
	}
		
	balda_view_invalidate(field->view, x-BALDA_VIEW_FIELD_CELL_PADDING / 2, y-BALDA_VIEW_FIELD_CELL_PADDING / 2,
		BALDA_VIEW_FIELD_CELL_W + BALDA_VIEW_FIELD_CELL_PADDING, BALDA_VIEW_FIELD_CELL_H + BALDA_VIEW_FIELD_CELL_PADDING, 1);
}

void balda_field_view_reset(balda_field_view_t* field)
{
	field->drawn = 0;
}

void balda_field_view_redraw(balda_field_view_t* field)
{
	int x, y;
	for (x=0; x<BALDA_FIELD_WIDTH; ++x)
	{
		for (y=0; y<BALDA_FIELD_HEIGHT; ++y)
		{
			balda_field_view_draw_cell(field, x, y, 0);
		}
	}
	
	field->drawn = 1;
}

void balda_field_view_set_cell(balda_field_view_t* field, int x, int y, balda_field_view_cell_t cell)
{
	if (field->drawn)
	{
		if (!balda_field_view_cells_equal(field->cells[x][y], cell))
		{
			balda_field_view_cell_t old_value = field->cells[x][y];
			field->cells[x][y] = cell;
			balda_field_view_draw_cell(field, x, y, &old_value);
		}
	}
	else
	{
		field->cells[x][y] = cell;
		balda_field_view_draw_cell(field, x, y, 0);
	}
}

void balda_field_view_load(balda_field_view_t* field, balda_t* balda)
{
	int x, y;
	for (x=0; x<BALDA_FIELD_WIDTH; ++x)
	{
		for (y=0; y<BALDA_FIELD_HEIGHT; ++y)
		{
			balda_field_view_cell_t cell;
			balda_field_view_cell_reset(&cell);
			cell.ch = balda_get_letter_at(balda, x, y);
			balda_field_view_set_cell(field, x, y, cell);
		}
	}
	
	field->selected = 0;
	field->inserted = 0;
	
	field->drawn = 1;
}

void balda_field_view_load_insert_select(balda_field_view_t* field, balda_t* balda,
	balda_bool selected, balda_point_t selected_pos,
	balda_char insert_char, balda_point_t insert_pos)
{
	int x, y;
	for (x=0; x<BALDA_FIELD_WIDTH; ++x)
	{
		for (y=0; y<BALDA_FIELD_HEIGHT; ++y)
		{
			balda_field_view_cell_t cell;
			balda_field_view_cell_reset(&cell);
			if (x == selected_pos.x && y == selected_pos.y)
			{
				cell.selection = BALDA_FIELD_VIEW_CELL_SELECTION_SINGLE;
				
				field->selected = 1;
				field->selected_pos = selected_pos;
			}
			if (x == insert_pos.x && y == insert_pos.y)
			{
				cell.insert = 1;
				cell.ch = insert_char;
				
				field->inserted = 1;
				field->insert_pos = insert_pos;
				field->insert_char = insert_char;
			}
			else
			{
				cell.ch = balda_get_letter_at(balda, x, y);
			}
			
			balda_field_view_set_cell(field, x, y, cell);
		}
	}
	
	field->drawn = 1;
}

void balda_field_view_select(balda_field_view_t* field, balda_point_t pos)
{
	debug_printf(("balda_field_view_select: (%d, %d)", pos.x, pos.y));
	
	balda_field_view_cell_t cell;
	
	if (field->selected)
	{
		cell = field->cells[field->selected_pos.x][field->selected_pos.y];
		cell.selection = BALDA_FIELD_VIEW_CELL_SELECTION_NONE;
		balda_field_view_set_cell(field, field->selected_pos.x, field->selected_pos.y, cell);
	}
	
	field->selected = 1;
	field->selected_pos = pos;
	
	cell = field->cells[field->selected_pos.x][field->selected_pos.y];
	cell.selection = BALDA_FIELD_VIEW_CELL_SELECTION_SINGLE;
	balda_field_view_set_cell(field, field->selected_pos.x, field->selected_pos.y, cell);
}

void balda_field_view_deselect(balda_field_view_t* field)
{
	if (field->selected)
	{
		balda_field_view_cell_t cell = field->cells[field->selected_pos.x][field->selected_pos.y];
		cell.selection = BALDA_FIELD_VIEW_CELL_SELECTION_NONE;
		balda_field_view_set_cell(field, field->selected_pos.x, field->selected_pos.y, cell);
		field->selected = 0;
	}
}

balda_bool balda_field_view_is_selected(balda_field_view_t* field)
{
	return field->selected;
}

balda_point_t balda_field_view_get_selection(balda_field_view_t* field)
{
	return field->selected_pos;
}

void balda_field_view_set_insert_char(balda_field_view_t* field, balda_point_t pos, balda_char c)
{
	balda_field_view_cell_t cell;
	
	if (!(field->inserted && c == field->insert_char && balda_points_equal(pos, field->insert_pos)))
	{
		if (field->inserted)
		{
			cell = field->cells[field->insert_pos.x][field->insert_pos.y];
			cell.ch = BALDA_CHAR_NONE;
			cell.insert = 0;
			balda_field_view_set_cell(field, field->insert_pos.x, field->insert_pos.y, cell);
		}
		
		field->inserted = 1;
		field->insert_pos = pos;
		field->insert_char = c;
		
		cell = field->cells[field->insert_pos.x][field->insert_pos.y];
		cell.ch = c;
		cell.insert = 1;
		balda_field_view_set_cell(field, field->insert_pos.x, field->insert_pos.y, cell);
	}
}

void balda_field_view_remove_insert_char(balda_field_view_t* field)
{
	if (field->inserted)
	{
		balda_field_view_cell_t cell = field->cells[field->insert_pos.x][field->insert_pos.y];
		cell.ch = BALDA_CHAR_NONE;
		cell.insert = 0;
		balda_field_view_set_cell(field, field->insert_pos.x, field->insert_pos.y, cell);
		field->inserted = 0;
	}
}

balda_bool balda_field_view_has_insert_char(balda_field_view_t* field)
{
	return field->inserted;
}

balda_char balda_field_view_get_insert_char(balda_field_view_t* field)
{
	return field->insert_char;
}

balda_point_t balda_field_view_get_insert_char_pos(balda_field_view_t* field)
{
	return field->insert_pos;
}

void balda_field_view_clear_any_selection(balda_field_view_t* field)
{
	// clear any selection
	int x, y;
	balda_field_view_cell_t cell;
	for (x=0; x<BALDA_FIELD_WIDTH; ++x)
	{
		for (y=0; y<BALDA_FIELD_HEIGHT; ++y)
		{
			cell = field->cells[x][y];
			cell.selection = BALDA_FIELD_VIEW_CELL_SELECTION_NONE;
			balda_field_view_set_cell(field, x, y, cell);
		}
	}
}

void balda_field_view_load_sequence(balda_field_view_t* field, balda_sequence_t* sequence)
{
	balda_field_view_set_insert_char(field, sequence->insert_pos, sequence->insert_char);
	balda_field_view_clear_any_selection(field);
	
	balda_sequence_entry_t* e, *prev_e = 0;
	balda_field_view_cell_t cell, prev_cell;
	int i;
	for (i=0; i<sequence->length; ++i)
	{
		e = &sequence->entries[i];
		
		cell = field->cells[e->pos.x][e->pos.y];
		cell.selection = BALDA_FIELD_VIEW_CELL_SELECTION_SEQUENCE;
		cell.sequence_left = 0;
		cell.sequence_top = 0;
		cell.sequence_right = 0;
		cell.sequence_bottom = 0;
		
		if (prev_e)
		{
			prev_cell = field->cells[prev_e->pos.x][prev_e->pos.y];
			
			if (e->pos.x == (prev_e->pos.x + 1)) // | prev_e |  |   e    |
			{
				prev_cell.sequence_right = 1;
				cell.sequence_left = 1;
			}
			else if (e->pos.x == (prev_e->pos.x - 1)) // |   e    |  | prev_e |
			{
				prev_cell.sequence_left = 1;
				cell.sequence_right = 1;
			}
			else if (e->pos.y == (prev_e->pos.y + 1)) // | prev_e | 
													  // |   e    |
			{
				prev_cell.sequence_bottom = 1;
				cell.sequence_top = 1;
			}
			else if (e->pos.y == (prev_e->pos.y - 1)) // |   e    |
													  // | prev_e | 
			{
				prev_cell.sequence_top = 1;
				cell.sequence_bottom = 1;
			}
			
			balda_field_view_set_cell(field, prev_e->pos.x, prev_e->pos.y, prev_cell);
		}
		
		balda_field_view_set_cell(field, e->pos.x, e->pos.y, cell);
		prev_e = e;
	}
}

void balda_field_view_replace_sequence_with_select(balda_field_view_t* field, balda_point_t pos)
{
	int x, y;
	balda_field_view_cell_t cell;
	for (x=0; x<BALDA_FIELD_WIDTH; ++x)
	{
		for (y=0; y<BALDA_FIELD_HEIGHT; ++y)
		{
			cell = field->cells[x][y];
			if (pos.x == x && pos.y == y)
				cell.selection = BALDA_FIELD_VIEW_CELL_SELECTION_SINGLE;
			else
				cell.selection = BALDA_FIELD_VIEW_CELL_SELECTION_NONE;
			balda_field_view_set_cell(field, x, y, cell);
		}
	}
	
	field->selected = 1;
	field->selected_pos = pos;
}

void balda_field_view_clear_sequence(balda_field_view_t* field, balda_bool keep_insert_char)
{
	balda_field_view_clear_any_selection(field);
	
	if (!keep_insert_char)
		balda_field_view_remove_insert_char(field);
}

balda_bool balda_field_view_get_cell_at_point(balda_field_view_t* field, balda_point_t point,
	balda_point_t* out_cell_pos)
{
	point.x -= field->left;
	point.y -= BALDA_VIEW_FIELD_Y;
	
	if (point.x >=0 && point.x < ((BALDA_VIEW_FIELD_CELL_W + BALDA_VIEW_FIELD_CELL_PADDING) * BALDA_FIELD_WIDTH) &&
		point.y >=0 && point.y < ((BALDA_VIEW_FIELD_CELL_H + BALDA_VIEW_FIELD_CELL_PADDING) * BALDA_FIELD_HEIGHT))
	{
		out_cell_pos->x = point.x / (BALDA_VIEW_FIELD_CELL_W + BALDA_VIEW_FIELD_CELL_PADDING);
		out_cell_pos->y = point.y / (BALDA_VIEW_FIELD_CELL_H + BALDA_VIEW_FIELD_CELL_PADDING);
		return 1;
	}
	
	return 0;
}
