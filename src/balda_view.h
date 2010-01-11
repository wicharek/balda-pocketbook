#ifndef __BALDA_VIEW_H__
#define __BALDA_VIEW_H__

#include "balda.h"
#include "balda_utils.h"

struct balda_view_t_impl;
typedef struct balda_view_t_impl balda_view_t;

typedef enum
{
	BALDA_VIEW_FIELD_SELECT_MODE_NONE,
	BALDA_VIEW_FIELD_SELECT_MODE_SINGLE,
	BALDA_VIEW_FIELD_SELECT_MODE_INSERTING,
	BALDA_VIEW_FIELD_SELECT_MODE_SEQUENCE
} BALDA_VIEW_FIELD_SELECT_MODE;

balda_view_t* balda_view_init(balda_t* balda);
void balda_view_free(balda_view_t*);

void balda_view_show_turn(balda_view_t* view);
void balda_view_set_keyboard(balda_view_t* view, const balda_char* keyboard);

balda_point_t balda_view_field_get_selection(balda_view_t* view);
balda_point_t balda_view_field_get_insert_pos(balda_view_t* view);
balda_char balda_view_field_get_insert_char(balda_view_t* view);
void balda_view_field_clear_selection(balda_view_t* view);
void balda_view_field_select_single(balda_view_t* view, balda_point_t point);
void balda_view_field_select_single_delta(balda_view_t* view, int dx, int dy);
void balda_view_field_select_inserting(balda_view_t* view, balda_point_t point, balda_char);

void balda_view_field_select_sequence_first(balda_view_t* view, balda_point_t point,
	balda_point_t insert_point, balda_char insert_char);
void balda_view_field_select_sequence_first_delta(balda_view_t* view, int dx, int dy);
void balda_view_field_select_sequence_first_complete(balda_view_t* view);
void balda_view_field_select_sequence_delta(balda_view_t* view, BALDA_DIRECTION direction);
	
void balda_view_keyboard_clear_selection(balda_view_t* view);
void balda_view_keyboard_select(balda_view_t* view, int x, int y);
void balda_view_keyboard_select_delta(balda_view_t* view, int dx, int dy);
balda_char balda_view_keyboard_get_selected_char(balda_view_t* view);

void balda_view_reset(balda_view_t* view);
void balda_view_draw_all(balda_view_t* view);


#endif
