#include <inkview.h>
#include <stdlib.h>
#include <assert.h>

#include "balda_view.h"
#include "balda_utils.h"

const int BALDA_VIEW_FIELD_Y = 230;
const int BALDA_VIEW_FIELD_CELL_W = 40;
const int BALDA_VIEW_FIELD_CELL_H = 40;
const int BALDA_VIEW_FIELD_CELL_PADDING = 2;

const int BALDA_VIEW_PLAYER_NAME_MARGIN = 10;
const int BALDA_VIEW_PLAYER_NAME_Y = 120;

const int BALDA_VIEW_SCORE_MARGIN = 10;
const int BALDA_VIEW_SCORE_Y = 140;
const int BALDA_VIEW_SCORE_WIDTH = 160;
const int BALDA_VIEW_SCORE_HEIGHT = 430;
const int BALDA_VIEW_SCORE_PANEL_PADDING = 8;
const int BALDA_VIEW_SCORE_PANEL_WIDTH = 75;
const int BALDA_VIEW_SCORE_PANEL_HEIGHT = 54;

extern const ibitmap img_logo;

struct balda_view_t_impl
{
	balda_t* balda;
	
	ifont* font_version;
	ifont* font_player_name;
	ifont* font_score_panel;
	
	const balda_char* keyboard;
	
	balda_int_converter_t int_conv;
};

balda_view_t* balda_view_init(balda_t* balda)
{
	balda_view_t* view = (balda_view_t*)malloc(sizeof(balda_view_t));
	
	view->balda = balda;
	
	view->font_version = OpenFont("LiberationSans", 11, 0);
	view->font_player_name = OpenFont("LiberationSans", 14, 1);
	view->font_score_panel = OpenFont("LiberationSans-Bold", 36, 1);
	view->keyboard = 0;
	
	return view;
}

void balda_view_free(balda_view_t* view)
{
	assert(view);
	
	CloseFont(view->font_version);
	CloseFont(view->font_player_name);
	
	free(view);
}

void balda_view_set_keyboard(balda_view_t* view, const balda_char* keyboard)
{
	view->keyboard = keyboard;
}

void balda_view_draw_logo(balda_view_t* view)
{
	// logo image
	DrawBitmap(10, 10, &img_logo);
	
	// version
	SetFont(view->font_version, BLACK);
	DrawString(10, 60, BALDA_APP_VERSION_STRING_FULL_RU);
	
}

void balda_view_draw_field(balda_view_t* view)
{
	int screen_w = ScreenWidth();
	int left = (screen_w - BALDA_VIEW_FIELD_CELL_W * BALDA_FIELD_WIDTH
		- BALDA_VIEW_FIELD_CELL_PADDING * (BALDA_FIELD_WIDTH - 1)) / 2;
	int i, j, x, y = BALDA_VIEW_FIELD_Y;
	
	printf("%d\n", left);
	
	for (j=0; j<BALDA_FIELD_HEIGHT; ++j, y += (BALDA_VIEW_FIELD_CELL_H + BALDA_VIEW_FIELD_CELL_PADDING))
	{
		x = left;
		for (i=0; i<BALDA_FIELD_WIDTH; ++i, x += (BALDA_VIEW_FIELD_CELL_W + BALDA_VIEW_FIELD_CELL_PADDING))
		{
			DrawRect(x, y, BALDA_VIEW_FIELD_CELL_W, BALDA_VIEW_FIELD_CELL_H, BLACK);
		}
	}
}

void balda_view_draw_players(balda_view_t* view)
{
	const char* player_name_1;
	
	SetFont(view->font_player_name, BLACK);
	DrawString(BALDA_VIEW_PLAYER_NAME_MARGIN, BALDA_VIEW_PLAYER_NAME_Y,
		balda_get_player_name(view->balda, 0));
	
	// HACK: StringWidth -- why not const char* param?
	player_name_1 = balda_get_player_name(view->balda, 1);
	DrawString(ScreenWidth() - StringWidth((char*)player_name_1) - BALDA_VIEW_PLAYER_NAME_MARGIN,
		BALDA_VIEW_PLAYER_NAME_Y,
		player_name_1);
}

void balda_view_draw_scoreboard(balda_view_t* view, int player_index)
{
	int left = (player_index == 0) ? BALDA_VIEW_SCORE_MARGIN :
		ScreenWidth() - BALDA_VIEW_SCORE_MARGIN - BALDA_VIEW_SCORE_WIDTH;
	int bg_color = LGRAY;
	int score_panel_x = (player_index == 0) ? (left + BALDA_VIEW_SCORE_WIDTH) :
		(left - BALDA_VIEW_SCORE_PANEL_WIDTH);
	int score_panel_digit_x = (player_index == 0) ? 0 : (score_panel_x + BALDA_VIEW_SCORE_PANEL_PADDING);
	int score_panel_digit_w = (player_index == 0) ? (score_panel_x + BALDA_VIEW_SCORE_PANEL_WIDTH - BALDA_VIEW_SCORE_PANEL_PADDING) :
		(ScreenWidth() - score_panel_digit_x);
	
	FillArea(left, BALDA_VIEW_SCORE_Y, BALDA_VIEW_SCORE_WIDTH, BALDA_VIEW_SCORE_HEIGHT, bg_color);
	FillArea(score_panel_x, BALDA_VIEW_SCORE_Y, BALDA_VIEW_SCORE_PANEL_WIDTH,
		BALDA_VIEW_SCORE_PANEL_HEIGHT, bg_color);
	
	SetFont(view->font_score_panel, BLACK);
	DrawTextRect(score_panel_digit_x, BALDA_VIEW_SCORE_Y, score_panel_digit_w, BALDA_VIEW_SCORE_PANEL_HEIGHT,
		(char*)balda_itoa(&view->int_conv, balda_get_score(view->balda, player_index)),
		VALIGN_MIDDLE | (player_index == 0 ? ALIGN_RIGHT : ALIGN_LEFT));
	
}

void balda_view_draw_keyboard(balda_view_t* view)
{
	char utf8_buf[4];
	
	if (view->keyboard)
	{
		
	}
}

void balda_view_draw_all(balda_view_t* view)
{
	balda_view_draw_logo(view);
	balda_view_draw_field(view);
	balda_view_draw_players(view);
	balda_view_draw_scoreboard(view, 0);
	balda_view_draw_scoreboard(view, 1);
	balda_view_draw_keyboard(view);
}
