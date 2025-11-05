/* include/config.h
 * main config file for sudoku
 */

#ifndef CONFIG_H
#define CONFIG_H

#include <stdbool.h>
#include <stdint.h>

/* compile-time constants needed for array sizes */
#define BOARD_SIZE_MAX 9
#define SUBGRID_MAX 3

/* cell color palette */
typedef enum CellColor {
	CELL_COLOR_NONE = 0,
	CELL_COLOR_RED = 1,
	CELL_COLOR_ORANGE = 2,
	CELL_COLOR_YELLOW = 3,
	CELL_COLOR_GREEN = 4,
	CELL_COLOR_BLUE = 5,
	CELL_COLOR_INDIGO = 6,
	CELL_COLOR_VIOLET = 7,
	CELL_COLOR_LIGHT_GRAY = 8,
	CELL_COLOR_WHITE = 9,
	CELL_COLOR_COUNT = 10
} CellColor;

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

/* theme structure */
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
	unsigned int topbarBg;
	unsigned int topbarText;
	unsigned int menuText;
	unsigned int menuSel;
	unsigned int menuSelBg;
	unsigned int cellColors[CELL_COLOR_COUNT];
} Theme;

/* runtime configuration structure */
typedef struct Config {
	/* application metadata */
	char app_title[64];
	char app_version[16];

	/* grid definitions */
	int board_size;
	int subgrid;

	/* window settings */
	int tile_pix;
	int grid_line_thick;
	int grid_line_thick_bold;
	int board_pad;
	int sidebar_w;
	int topbar_h;
	int window_w; /* calculated */
	int window_h; /* calculated */

	/* ux settings */
	int repeat_delay_frames;
	int repeat_rate_frames;

	/* ui layout constants */
	int menu_start_y;
	int menu_item_spacing;
	int menu_padding_x;
	int menu_padding_y;
	int topbar_padding;
	int sidebar_margin;
	int controls_line_spacing;
	int controls_section_spacing;
	int color_keypad_size;
	int color_keypad_spacing;
	int color_keypad_cols;

	/* font sizes */
	int font_size_title;
	int font_size_digit;
	int font_size_menu;
	int font_size_heading;
	int font_size_large;
	int font_size_topbar;
	int font_size_normal;
	int font_size_small;
	int font_size_note;

	/* cell layout */
	int note_padding_x;
	int note_padding_y;
	int note_grid_size;

	/* theme */
	Theme theme;
} Config;

extern Config g_config;

/* config api */
void Config_Init(void);
bool Config_Load(Config *cfg, const char *path);
bool Config_UpdateValues(
	const char *path, const char *section, const char **keys, const int *values, int count);

/* backward compatibility macros for existing code */
#define APP_TITLE (g_config.app_title)
#define APP_VERSION (g_config.app_version)
#define BOARD_SIZE (g_config.board_size)
#define SUBGRID (g_config.subgrid)
#define TILE_PIX (g_config.tile_pix)
#define GRID_LINE_THICK (g_config.grid_line_thick)
#define GRID_LINE_THICK_B (g_config.grid_line_thick_bold)
#define BOARD_PAD (g_config.board_pad)
#define SIDEBAR_W (g_config.sidebar_w)
#define TOPBAR_H (g_config.topbar_h)
#define WINDOW_W (g_config.window_w)
#define WINDOW_H (g_config.window_h)
#define REPEAT_DELAY_FRAMES (g_config.repeat_delay_frames)
#define REPEAT_RATE_FRAMES (g_config.repeat_rate_frames)
#define MENU_START_Y (g_config.menu_start_y)
#define MENU_ITEM_SPACING (g_config.menu_item_spacing)
#define MENU_PADDING_X (g_config.menu_padding_x)
#define MENU_PADDING_Y (g_config.menu_padding_y)
#define TOPBAR_PADDING (g_config.topbar_padding)
#define SIDEBAR_MARGIN (g_config.sidebar_margin)
#define CONTROLS_LINE_SPACING (g_config.controls_line_spacing)
#define CONTROLS_SECTION_SPACING (g_config.controls_section_spacing)
#define COLOR_KEYPAD_SIZE (g_config.color_keypad_size)
#define COLOR_KEYPAD_SPACING (g_config.color_keypad_spacing)
#define COLOR_KEYPAD_COLS (g_config.color_keypad_cols)
#define FONT_SIZE_TITLE (g_config.font_size_title)
#define FONT_SIZE_DIGIT (g_config.font_size_digit)
#define FONT_SIZE_MENU (g_config.font_size_menu)
#define FONT_SIZE_HEADING (g_config.font_size_heading)
#define FONT_SIZE_LARGE (g_config.font_size_large)
#define FONT_SIZE_TOPBAR (g_config.font_size_topbar)
#define FONT_SIZE_NORMAL (g_config.font_size_normal)
#define FONT_SIZE_SMALL (g_config.font_size_small)
#define FONT_SIZE_NOTE (g_config.font_size_note)
#define NOTE_PADDING_X (g_config.note_padding_x)
#define NOTE_PADDING_Y (g_config.note_padding_y)
#define NOTE_GRID_SIZE (g_config.note_grid_size)

/* theme color macros */
#define COLOR_BG (g_config.theme.bg)
#define COLOR_GRID (g_config.theme.grid)
#define COLOR_GRID_BOLD (g_config.theme.gridBold)
#define COLOR_CELL_BG (g_config.theme.cellBg)
#define COLOR_CELL_SEL (g_config.theme.cellSel)
#define COLOR_DIGIT_GIVEN (g_config.theme.digitGiven)
#define COLOR_DIGIT_USER (g_config.theme.digitUser)
#define COLOR_ACCENT (g_config.theme.accent)
#define COLOR_TEXT (g_config.theme.text)
#define COLOR_BAD (g_config.theme.bad)
#define COLOR_HIGHLIGHT_ROW_COL (g_config.theme.highlightRowCol)
#define COLOR_HIGHLIGHT_DIGIT (g_config.theme.highlightDigit)
#define COLOR_TOPBAR_BG (g_config.theme.topbarBg)
#define COLOR_TOPBAR_TEXT (g_config.theme.topbarText)
#define COLOR_MENU_TEXT (g_config.theme.menuText)
#define COLOR_MENU_SEL (g_config.theme.menuSel)
#define COLOR_MENU_SEL_BG (g_config.theme.menuSelBg)

/* cell color palette macros */
#define COLOR_PALETTE_RED (g_config.theme.cellColors[CELL_COLOR_RED])
#define COLOR_PALETTE_ORANGE (g_config.theme.cellColors[CELL_COLOR_ORANGE])
#define COLOR_PALETTE_YELLOW (g_config.theme.cellColors[CELL_COLOR_YELLOW])
#define COLOR_PALETTE_GREEN (g_config.theme.cellColors[CELL_COLOR_GREEN])
#define COLOR_PALETTE_BLUE (g_config.theme.cellColors[CELL_COLOR_BLUE])
#define COLOR_PALETTE_INDIGO (g_config.theme.cellColors[CELL_COLOR_INDIGO])
#define COLOR_PALETTE_VIOLET (g_config.theme.cellColors[CELL_COLOR_VIOLET])
#define COLOR_PALETTE_LIGHT_GRAY (g_config.theme.cellColors[CELL_COLOR_LIGHT_GRAY])
#define COLOR_PALETTE_WHITE (g_config.theme.cellColors[CELL_COLOR_WHITE])

#endif // CONFIG_H
