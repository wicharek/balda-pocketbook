#ifndef __BALDA_VIEW_H__
#define __BALDA_VIEW_H__

#include "balda.h"

struct balda_view_t_impl;
typedef struct balda_view_t_impl balda_view_t;

balda_view_t* balda_view_init(balda_t* balda);
void balda_view_free(balda_view_t*);

void balda_view_set_keyboard(balda_view_t* view, const balda_char* keyboard);

void balda_view_draw_all(balda_view_t* view);

#endif
