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

BALDA_STRING(BALDA_STR_DEFAULT_PLAYER_NAME_0, "Игрок 1")
BALDA_STRING(BALDA_STR_DEFAULT_PLAYER_NAME_1, "Игрок 2")
BALDA_STRING(BALDA_STR_AI_PLAYER_NAME, "Балда")

BALDA_STRING(BALDA_STR_NOT_EMPTY_CELL_TITLE, "Ячейка уже занята")
BALDA_STRING(BALDA_STR_NOT_EMPTY_CELL_MESSAGE, "Вы должны добавить букву в свободную ячейку. Попробуйте ещё раз.")
BALDA_STRING(BALDA_STR_NOT_NEAR_TITLE, "Неправильная ячейка")
BALDA_STRING(BALDA_STR_NOT_NEAR_MESSAGE, "Вы должны добавить букву в ячейку, прилегающую к уже занятым. Попробуйте ещё раз.")
BALDA_STRING(BALDA_STR_EMPTY_CELL_TITLE, "Пустая ячейка")
BALDA_STRING(BALDA_STR_EMPTY_CELL_MESSAGE, "Начинайте слово в ячейке, где есть буква.")
BALDA_STRING(BALDA_STR_TOO_SHORT_TITLE, "Слишком короткое слово") // Not used so far
BALDA_STRING(BALDA_STR_TOO_SHORT_MESSAGE, "Слово должно содержать минимум две буквы.") // Not used so far
BALDA_STRING(BALDA_STR_UNKNOWN_WORD_TITLE, "Неизвестное слово")
BALDA_STRING(BALDA_STR_UNKNOWN_WORD_MESSAGE, "Слово \"%s\" не найдено в словаре.")
BALDA_STRING(BALDA_STR_INSERT_CHAR_NOT_SELECTED_TITLE, "Не использована дополнительная буква")
BALDA_STRING(BALDA_STR_INSERT_CHAR_NOT_SELECTED_MESSAGE, "Выбранное слово должно содержать добавленную вами букву.")
BALDA_STRING(BALDA_STR_WORD_ALREADY_USED_TITLE, "Слово уже использовано")
BALDA_STRING(BALDA_STR_WORD_ALREADY_USED_MESSAGE, "Слово \"%s\" уже было использовано в текущей игре. "
	"Слово можно использовать только один раз за игру.")
BALDA_STRING(BALDA_STR_ABOUT_TITLE, "Об игре")
BALDA_STRING(BALDA_STR_ABOUT_MESSAGE, "Автор: Виталий Иванов\nПочта: wicharek@w2f2.com\nСайт: http://balda.w2f2.com")
BALDA_STRING(BALDA_STR_DRAW, "Ничья!")
BALDA_STRING(BALDA_STR_WINNER, "Победил:")
BALDA_STRING(BALDA_STR_PRESS_TO_PLAY_AGAIN, "Нажмите любую кнопку, чтобы сыграть ещё раз")


BALDA_STRING(BALDA_STR_CAPTION_SURRENDER, "Сдаться")
BALDA_STRING(BALDA_STR_CAPTION_BACK, "Отменить")
BALDA_STRING(BALDA_STR_CAPTION_CONFIRM, "Готово")
BALDA_STRING(BALDA_STR_CAPTION_NEXT, "Дальше")

#ifdef BALDA_STRING_DEFINE_MACROS
#define BALDA_STR_MENU_HEADER "Меню"
#define BALDA_STR_MENU_RESUME "Продолжить"
#define BALDA_STR_MENU_NEW_GAME "Новая игра"
#define BALDA_STR_MENU_ONE_PLAYER "Против компьютера"
#define BALDA_STR_MENU_TWO_PLAYERS "Два игрока"
#define BALDA_STR_MENU_ABOUT "Об игре"
#define BALDA_STR_MENU_EXIT "Выход"
#endif
