/* include/config.h
 * main config file for sudoku
 */

#ifndef CONFIG_H
#define CONFIG_H

#include <stdbool.h>
#include <stdint.h>

#define APP_TITLE "sudoku"
#define APP_VERSION "0.0.1"

/* grid defs */
#define BOARD_SIZE 9
#define SUBGRID 3

/* window */
#define TILE_PIX 64
#define GRID_LINE_THICK 2
#define GRID_LINE_THICK_B 4
#define BOARD_PAD 24
#define SIDEBAR_W 280
#define TOPBAR_H 56
#define WINDOW_W (BOARD_PAD * 2 + TILE_PIX * BOARD_SIZE + SIDEBAR_W)
#define WINDOW_H (BOARD_PAD * 2 + TILE_PIX * BOARD_SIZE + TOPBAR_H)

/* ux */
#define REPEAT_DELAY_FRAMES 22
#define REPEAT_RATE_FRAMES 5

/* ui Layout constants */
#define MENU_START_Y 200
#define MENU_ITEM_SPACING 56
#define MENU_PADDING_X 16
#define MENU_PADDING_Y 8
#define TOPBAR_PADDING 16
#define SIDEBAR_MARGIN 16
#define CONTROLS_LINE_SPACING 22
#define CONTROLS_SECTION_SPACING 28

/* font sizes */
#define FONT_SIZE_TITLE 48
#define FONT_SIZE_DIGIT 36
#define FONT_SIZE_MENU 32
#define FONT_SIZE_HEADING 24
#define FONT_SIZE_LARGE 22
#define FONT_SIZE_TOPBAR 20
#define FONT_SIZE_NORMAL 18
#define FONT_SIZE_SMALL 16
#define FONT_SIZE_NOTE 14

/* cell layout */
#define NOTE_PADDING_X 6
#define NOTE_PADDING_Y 4
#define NOTE_GRID_SIZE 3

/* input mode */
typedef enum InputMode { INPUT_MODE_INSERT = 0, INPUT_MODE_NOTES = 1 } InputMode;

/* difficulty levels */
typedef enum Difficulty {
	DIFFICULTY_EASY = 0,
	DIFFICULTY_MEDIUM = 1,
	DIFFICULTY_HARD = 2,
	DIFFICULTY_MASTER = 3,
	DIFFICULTY_EXPERT = 4
} Difficulty;

/* theme colors (rgba format: 0xRRGGBBAA) */
#define COLOR_BG 0xFFFFFFFF
#define COLOR_GRID 0x000000FF
#define COLOR_GRID_BOLD 0x000000FF
#define COLOR_CELL_BG 0xFFFFFFFF
#define COLOR_CELL_SEL 0x2B5CFF40
#define COLOR_DIGIT_GIVEN 0x000000FF
#define COLOR_DIGIT_USER 0x0066FFFF
#define COLOR_ACCENT 0x2B5CFFFF
#define COLOR_TEXT 0x000000FF
#define COLOR_BAD 0xFF4D4DFF
#define COLOR_TOPBAR_BG 0xF0F0F0FF
#define COLOR_TOPBAR_TEXT 0x000000FF
#define COLOR_MENU_TEXT 0x333333FF
#define COLOR_MENU_SEL 0x2B5CFFFF
#define COLOR_MENU_SEL_BG 0x2B5CFF30
#define COLOR_HIGHLIGHT_ROW_COL 0x2B5CFF15
#define COLOR_HIGHLIGHT_DIGIT 0xFFD70025

/* forward decl for theme */
typedef struct Theme {
	unsigned int bg;
	unsigned int grid;
	unsigned int gridBold;
	unsigned int cellBg;
	unsigned int cellSel;
	unsigned int digitGiven;
	unsigned int digitUser;
	unsigned int accent;
	unsigned int text;
	unsigned int bad;
	unsigned int highlightRowCol;
	unsigned int highlightDigit;
} Theme;

#endif // CONFIG_H
