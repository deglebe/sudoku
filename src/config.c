/* src/config.c
 * Runtime configuration loading from Lua
 */

#define _GNU_SOURCE

#include "config.h"

#define LUA_IMPL
#include "minilua.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Config g_config;

/* helper to get integer from lua table */
#define LUA_GET_INT(L, table, field, dest)                                             \
	do {                                                                           \
		lua_getfield(L, -1, field);                                            \
		if (lua_isinteger(L, -1)) {                                            \
			dest = (int) lua_tointeger(L, -1);                             \
		}                                                                      \
		lua_pop(L, 1);                                                         \
	} while (0)

/* helper to get unsigned int from lua table */
#define LUA_GET_UINT(L, table, field, dest)                                            \
	do {                                                                           \
		lua_getfield(L, -1, field);                                            \
		if (lua_isinteger(L, -1)) {                                            \
			dest = (unsigned int) lua_tointeger(L, -1);                    \
		}                                                                      \
		lua_pop(L, 1);                                                         \
	} while (0)

/* helper to get string from lua table */
#define LUA_GET_STR(L, table, field, dest, maxlen)                                     \
	do {                                                                           \
		lua_getfield(L, -1, field);                                            \
		if (lua_isstring(L, -1)) {                                             \
			const char *str = lua_tostring(L, -1);                         \
			strncpy(dest, str, maxlen - 1);                                \
			dest[maxlen - 1] = '\0';                                       \
		}                                                                      \
		lua_pop(L, 1);                                                         \
	} while (0)

/* load configuration from lua file */
bool Config_Load(Config *cfg, const char *path) {
	lua_State *L = luaL_newstate();
	if (!L) {
		fprintf(stderr, "failed to create Lua state\n");
		return false;
	}

	/* load and run the config file */
	if (luaL_dofile(L, path) != LUA_OK) {
		fprintf(stderr,
			"failed to load config file '%s': %s\n",
			path,
			lua_tostring(L, -1));
		lua_close(L);
		return false;
	}

	/* get the global 'config' table */
	lua_getglobal(L, "config");
	if (!lua_istable(L, -1)) {
		fprintf(stderr, "config file must define 'config' table\n");
		lua_close(L);
		return false;
	}

	/* load app section */
	lua_getfield(L, -1, "app");
	if (lua_istable(L, -1)) {
		LUA_GET_STR(L, "app", "title", cfg->app_title, sizeof(cfg->app_title));
		LUA_GET_STR(
			L, "app", "version", cfg->app_version, sizeof(cfg->app_version));
	}
	lua_pop(L, 1);

	/* load grid section */
	lua_getfield(L, -1, "grid");
	if (lua_istable(L, -1)) {
		LUA_GET_INT(L, "grid", "board_size", cfg->board_size);
		LUA_GET_INT(L, "grid", "subgrid", cfg->subgrid);
	}
	lua_pop(L, 1);

	/* load window section */
	lua_getfield(L, -1, "window");
	if (lua_istable(L, -1)) {
		LUA_GET_INT(L, "window", "tile_pix", cfg->tile_pix);
		LUA_GET_INT(L, "window", "grid_line_thick", cfg->grid_line_thick);
		LUA_GET_INT(L, "window", "grid_line_thick_bold", cfg->grid_line_thick_bold);
		LUA_GET_INT(L, "window", "board_pad", cfg->board_pad);
		LUA_GET_INT(L, "window", "sidebar_w", cfg->sidebar_w);
		LUA_GET_INT(L, "window", "topbar_h", cfg->topbar_h);
	}
	lua_pop(L, 1);

	/* calculate window dimensions */
	cfg->window_w
		= cfg->board_pad * 2 + cfg->tile_pix * cfg->board_size + cfg->sidebar_w;
	cfg->window_h
		= cfg->board_pad * 2 + cfg->tile_pix * cfg->board_size + cfg->topbar_h;

	/* load ux section */
	lua_getfield(L, -1, "ux");
	if (lua_istable(L, -1)) {
		LUA_GET_INT(L, "ux", "repeat_delay_frames", cfg->repeat_delay_frames);
		LUA_GET_INT(L, "ux", "repeat_rate_frames", cfg->repeat_rate_frames);
	}
	lua_pop(L, 1);

	/* load ui section */
	lua_getfield(L, -1, "ui");
	if (lua_istable(L, -1)) {
		LUA_GET_INT(L, "ui", "menu_start_y", cfg->menu_start_y);
		LUA_GET_INT(L, "ui", "menu_item_spacing", cfg->menu_item_spacing);
		LUA_GET_INT(L, "ui", "menu_padding_x", cfg->menu_padding_x);
		LUA_GET_INT(L, "ui", "menu_padding_y", cfg->menu_padding_y);
		LUA_GET_INT(L, "ui", "topbar_padding", cfg->topbar_padding);
		LUA_GET_INT(L, "ui", "sidebar_margin", cfg->sidebar_margin);
		LUA_GET_INT(L, "ui", "controls_line_spacing", cfg->controls_line_spacing);
		LUA_GET_INT(
			L, "ui", "controls_section_spacing", cfg->controls_section_spacing);
		LUA_GET_INT(L, "ui", "color_keypad_size", cfg->color_keypad_size);
		LUA_GET_INT(L, "ui", "color_keypad_spacing", cfg->color_keypad_spacing);
		LUA_GET_INT(L, "ui", "color_keypad_cols", cfg->color_keypad_cols);
	}
	lua_pop(L, 1);

	/* load font section */
	lua_getfield(L, -1, "font");
	if (lua_istable(L, -1)) {
		LUA_GET_INT(L, "font", "size_title", cfg->font_size_title);
		LUA_GET_INT(L, "font", "size_digit", cfg->font_size_digit);
		LUA_GET_INT(L, "font", "size_menu", cfg->font_size_menu);
		LUA_GET_INT(L, "font", "size_heading", cfg->font_size_heading);
		LUA_GET_INT(L, "font", "size_large", cfg->font_size_large);
		LUA_GET_INT(L, "font", "size_topbar", cfg->font_size_topbar);
		LUA_GET_INT(L, "font", "size_normal", cfg->font_size_normal);
		LUA_GET_INT(L, "font", "size_small", cfg->font_size_small);
		LUA_GET_INT(L, "font", "size_note", cfg->font_size_note);
	}
	lua_pop(L, 1);

	/* load cell section */
	lua_getfield(L, -1, "cell");
	if (lua_istable(L, -1)) {
		LUA_GET_INT(L, "cell", "note_padding_x", cfg->note_padding_x);
		LUA_GET_INT(L, "cell", "note_padding_y", cfg->note_padding_y);
		LUA_GET_INT(L, "cell", "note_grid_size", cfg->note_grid_size);
	}
	lua_pop(L, 1);

	/* load theme section */
	lua_getfield(L, -1, "theme");
	if (lua_istable(L, -1)) {
		LUA_GET_UINT(L, "theme", "bg", cfg->theme.bg);
		LUA_GET_UINT(L, "theme", "grid", cfg->theme.grid);
		LUA_GET_UINT(L, "theme", "grid_bold", cfg->theme.gridBold);
		LUA_GET_UINT(L, "theme", "cell_bg", cfg->theme.cellBg);
		LUA_GET_UINT(L, "theme", "cell_sel", cfg->theme.cellSel);
		LUA_GET_UINT(L, "theme", "digit_given", cfg->theme.digitGiven);
		LUA_GET_UINT(L, "theme", "digit_user", cfg->theme.digitUser);
		LUA_GET_UINT(L, "theme", "accent", cfg->theme.accent);
		LUA_GET_UINT(L, "theme", "text", cfg->theme.text);
		LUA_GET_UINT(L, "theme", "bad", cfg->theme.bad);
		LUA_GET_UINT(L, "theme", "topbar_bg", cfg->theme.topbarBg);
		LUA_GET_UINT(L, "theme", "topbar_text", cfg->theme.topbarText);
		LUA_GET_UINT(L, "theme", "menu_text", cfg->theme.menuText);
		LUA_GET_UINT(L, "theme", "menu_sel", cfg->theme.menuSel);
		LUA_GET_UINT(L, "theme", "menu_sel_bg", cfg->theme.menuSelBg);
		LUA_GET_UINT(L, "theme", "highlight_row_col", cfg->theme.highlightRowCol);
		LUA_GET_UINT(L, "theme", "highlight_digit", cfg->theme.highlightDigit);

		/* load palette subsection */
		lua_getfield(L, -1, "palette");
		if (lua_istable(L, -1)) {
			LUA_GET_UINT(
				L, "palette", "red", cfg->theme.cellColors[CELL_COLOR_RED]);
			LUA_GET_UINT(L,
				"palette",
				"orange",
				cfg->theme.cellColors[CELL_COLOR_ORANGE]);
			LUA_GET_UINT(L,
				"palette",
				"yellow",
				cfg->theme.cellColors[CELL_COLOR_YELLOW]);
			LUA_GET_UINT(L,
				"palette",
				"green",
				cfg->theme.cellColors[CELL_COLOR_GREEN]);
			LUA_GET_UINT(L,
				"palette",
				"blue",
				cfg->theme.cellColors[CELL_COLOR_BLUE]);
			LUA_GET_UINT(L,
				"palette",
				"indigo",
				cfg->theme.cellColors[CELL_COLOR_INDIGO]);
			LUA_GET_UINT(L,
				"palette",
				"violet",
				cfg->theme.cellColors[CELL_COLOR_VIOLET]);
			LUA_GET_UINT(L,
				"palette",
				"light_gray",
				cfg->theme.cellColors[CELL_COLOR_LIGHT_GRAY]);
			LUA_GET_UINT(L,
				"palette",
				"white",
				cfg->theme.cellColors[CELL_COLOR_WHITE]);
		}
		lua_pop(L, 1); /* pop palette */
	}
	lua_pop(L, 1); /* pop theme */

	lua_pop(L, 1); /* pop config table */
	lua_close(L);

	return true;
}

/* initialize config with defaults and load from file */
void Config_Init(void) {
	/* set some sensible defaults first */
	g_config = (Config) { .app_title = "sudoku",
		.app_version = "0.1.0",
		.board_size = 9,
		.subgrid = 3,
		.tile_pix = 64,
		.grid_line_thick = 2,
		.grid_line_thick_bold = 4,
		.board_pad = 24,
		.sidebar_w = 280,
		.topbar_h = 56,
		.repeat_delay_frames = 22,
		.repeat_rate_frames = 5,
		.menu_start_y = 200,
		.menu_item_spacing = 56,
		.menu_padding_x = 16,
		.menu_padding_y = 8,
		.topbar_padding = 16,
		.sidebar_margin = 16,
		.controls_line_spacing = 22,
		.controls_section_spacing = 28,
		.color_keypad_size = 32,
		.color_keypad_spacing = 8,
		.color_keypad_cols = 3,
		.font_size_title = 48,
		.font_size_digit = 36,
		.font_size_menu = 32,
		.font_size_heading = 24,
		.font_size_large = 22,
		.font_size_topbar = 20,
		.font_size_normal = 18,
		.font_size_small = 16,
		.font_size_note = 14,
		.note_padding_x = 6,
		.note_padding_y = 4,
		.note_grid_size = 3,
		.theme = { .bg = 0xFFFFFFFF,
			.grid = 0x000000FF,
			.gridBold = 0x000000FF,
			.cellBg = 0xFFFFFFFF,
			.cellSel = 0x2B5CFF40,
			.digitGiven = 0x000000FF,
			.digitUser = 0x0066FFFF,
			.accent = 0x2B5CFFFF,
			.text = 0x000000FF,
			.bad = 0xFF4D4DFF,
			.topbarBg = 0xF0F0F0FF,
			.topbarText = 0x000000FF,
			.menuText = 0x333333FF,
			.menuSel = 0x2B5CFFFF,
			.menuSelBg = 0x2B5CFF30,
			.highlightRowCol = 0x2B5CFF15,
			.highlightDigit = 0xFFD70025,
			.cellColors = { [CELL_COLOR_RED] = 0xFF6B6B80,
				[CELL_COLOR_ORANGE] = 0xFFA50080,
				[CELL_COLOR_YELLOW] = 0xFFEB3B80,
				[CELL_COLOR_GREEN] = 0x4CAF5080,
				[CELL_COLOR_BLUE] = 0x2196F380,
				[CELL_COLOR_INDIGO] = 0x3F51B580,
				[CELL_COLOR_VIOLET] = 0x9C27B080,
				[CELL_COLOR_LIGHT_GRAY] = 0xBDBDBD80,
				[CELL_COLOR_WHITE] = 0xFFFFFF80 } } };

	/* calculate window dimensions with defaults */
	g_config.window_w = g_config.board_pad * 2
		+ g_config.tile_pix * g_config.board_size + g_config.sidebar_w;
	g_config.window_h = g_config.board_pad * 2
		+ g_config.tile_pix * g_config.board_size + g_config.topbar_h;

	/* try to load from file (fallback to defaults if it fails) */
	if (!Config_Load(&g_config, "config.lua")) {
		fprintf(stderr, "warning: failed to load config.lua, using defaults\n");
	}
}
