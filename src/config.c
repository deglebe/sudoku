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
static inline void lua_get_int(lua_State *L, const char *field, int *dest) {
	lua_getfield(L, -1, field);
	if (lua_isinteger(L, -1)) {
		*dest = (int) lua_tointeger(L, -1);
	}
	lua_pop(L, 1);
}

/* helper to get unsigned int from lua table */
static inline void lua_get_uint(lua_State *L, const char *field, unsigned int *dest) {
	lua_getfield(L, -1, field);
	if (lua_isinteger(L, -1)) {
		*dest = (unsigned int) lua_tointeger(L, -1);
	}
	lua_pop(L, 1);
}

/* helper to get string from lua table */
static inline void lua_get_str(lua_State *L, const char *field, char *dest, size_t maxlen) {
	lua_getfield(L, -1, field);
	if (lua_isstring(L, -1)) {
		const char *str = lua_tostring(L, -1);
		strncpy(dest, str, maxlen - 1);
		dest[maxlen - 1] = '\0';
	}
	lua_pop(L, 1);
}

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
		lua_get_str(L, "title", cfg->app_title, sizeof(cfg->app_title));
		lua_get_str(L, "version", cfg->app_version, sizeof(cfg->app_version));
	}
	lua_pop(L, 1);

	/* load grid section */
	lua_getfield(L, -1, "grid");
	if (lua_istable(L, -1)) {
		lua_get_int(L, "board_size", &cfg->board_size);
		lua_get_int(L, "subgrid", &cfg->subgrid);
	}
	lua_pop(L, 1);

	/* load window section */
	lua_getfield(L, -1, "window");
	if (lua_istable(L, -1)) {
		lua_get_int(L, "tile_pix", &cfg->tile_pix);
		lua_get_int(L, "grid_line_thick", &cfg->grid_line_thick);
		lua_get_int(L, "grid_line_thick_bold", &cfg->grid_line_thick_bold);
		lua_get_int(L, "board_pad", &cfg->board_pad);
		lua_get_int(L, "sidebar_w", &cfg->sidebar_w);
		lua_get_int(L, "topbar_h", &cfg->topbar_h);
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
		lua_get_int(L, "repeat_delay_frames", &cfg->repeat_delay_frames);
		lua_get_int(L, "repeat_rate_frames", &cfg->repeat_rate_frames);
	}
	lua_pop(L, 1);

	/* load ui section */
	lua_getfield(L, -1, "ui");
	if (lua_istable(L, -1)) {
		lua_get_int(L, "menu_start_y", &cfg->menu_start_y);
		lua_get_int(L, "menu_item_spacing", &cfg->menu_item_spacing);
		lua_get_int(L, "menu_padding_x", &cfg->menu_padding_x);
		lua_get_int(L, "menu_padding_y", &cfg->menu_padding_y);
		lua_get_int(L, "topbar_padding", &cfg->topbar_padding);
		lua_get_int(L, "sidebar_margin", &cfg->sidebar_margin);
		lua_get_int(L, "controls_line_spacing", &cfg->controls_line_spacing);
		lua_get_int(L, "controls_section_spacing", &cfg->controls_section_spacing);
		lua_get_int(L, "color_keypad_size", &cfg->color_keypad_size);
		lua_get_int(L, "color_keypad_spacing", &cfg->color_keypad_spacing);
		lua_get_int(L, "color_keypad_cols", &cfg->color_keypad_cols);
	}
	lua_pop(L, 1);

	/* load font section */
	lua_getfield(L, -1, "font");
	if (lua_istable(L, -1)) {
		lua_get_int(L, "size_title", &cfg->font_size_title);
		lua_get_int(L, "size_digit", &cfg->font_size_digit);
		lua_get_int(L, "size_menu", &cfg->font_size_menu);
		lua_get_int(L, "size_heading", &cfg->font_size_heading);
		lua_get_int(L, "size_large", &cfg->font_size_large);
		lua_get_int(L, "size_topbar", &cfg->font_size_topbar);
		lua_get_int(L, "size_normal", &cfg->font_size_normal);
		lua_get_int(L, "size_small", &cfg->font_size_small);
		lua_get_int(L, "size_note", &cfg->font_size_note);
	}
	lua_pop(L, 1);

	/* load cell section */
	lua_getfield(L, -1, "cell");
	if (lua_istable(L, -1)) {
		lua_get_int(L, "note_padding_x", &cfg->note_padding_x);
		lua_get_int(L, "note_padding_y", &cfg->note_padding_y);
		lua_get_int(L, "note_grid_size", &cfg->note_grid_size);
	}
	lua_pop(L, 1);

	/* load theme section */
	lua_getfield(L, -1, "theme");
	if (lua_istable(L, -1)) {
		lua_get_uint(L, "bg", &cfg->theme.bg);
		lua_get_uint(L, "grid", &cfg->theme.grid);
		lua_get_uint(L, "grid_bold", &cfg->theme.gridBold);
		lua_get_uint(L, "cell_bg", &cfg->theme.cellBg);
		lua_get_uint(L, "cell_sel", &cfg->theme.cellSel);
		lua_get_uint(L, "digit_given", &cfg->theme.digitGiven);
		lua_get_uint(L, "digit_user", &cfg->theme.digitUser);
		lua_get_uint(L, "accent", &cfg->theme.accent);
		lua_get_uint(L, "text", &cfg->theme.text);
		lua_get_uint(L, "bad", &cfg->theme.bad);
		lua_get_uint(L, "topbar_bg", &cfg->theme.topbarBg);
		lua_get_uint(L, "topbar_text", &cfg->theme.topbarText);
		lua_get_uint(L, "menu_text", &cfg->theme.menuText);
		lua_get_uint(L, "menu_sel", &cfg->theme.menuSel);
		lua_get_uint(L, "menu_sel_bg", &cfg->theme.menuSelBg);
		lua_get_uint(L, "highlight_row_col", &cfg->theme.highlightRowCol);
		lua_get_uint(L, "highlight_digit", &cfg->theme.highlightDigit);

		/* load palette subsection */
		lua_getfield(L, -1, "palette");
		if (lua_istable(L, -1)) {
			lua_get_uint(L, "red", &cfg->theme.cellColors[CELL_COLOR_RED]);
			lua_get_uint(
				L, "orange", &cfg->theme.cellColors[CELL_COLOR_ORANGE]);
			lua_get_uint(
				L, "yellow", &cfg->theme.cellColors[CELL_COLOR_YELLOW]);
			lua_get_uint(L, "green", &cfg->theme.cellColors[CELL_COLOR_GREEN]);
			lua_get_uint(L, "blue", &cfg->theme.cellColors[CELL_COLOR_BLUE]);
			lua_get_uint(
				L, "indigo", &cfg->theme.cellColors[CELL_COLOR_INDIGO]);
			lua_get_uint(
				L, "violet", &cfg->theme.cellColors[CELL_COLOR_VIOLET]);
			lua_get_uint(L,
				"light_gray",
				&cfg->theme.cellColors[CELL_COLOR_LIGHT_GRAY]);
			lua_get_uint(L, "white", &cfg->theme.cellColors[CELL_COLOR_WHITE]);
		}
		lua_pop(L, 1); /* pop palette */
	}
	lua_pop(L, 1); /* pop theme */

	lua_pop(L, 1); /* pop config table */
	lua_close(L);

	return true;
}

/* update specific config values via text replacement (preserves comments and formatting) */
bool Config_UpdateValues(
	const char *path, const char *section, const char **keys, const int *values, int count) {
	(void) section; /* reserved for future section-aware updates */

	/* read entire file into memory */
	FILE *f = fopen(path, "r");
	if (!f) {
		fprintf(stderr, "failed to open config file '%s' for reading\n", path);
		return false;
	}

	fseek(f, 0, SEEK_END);
	long fileSize = ftell(f);
	fseek(f, 0, SEEK_SET);

	char *content = malloc(fileSize + 1);
	if (!content) {
		fclose(f);
		return false;
	}

	size_t bytesRead = fread(content, 1, fileSize, f);
	content[bytesRead] = '\0';
	fclose(f);

	/* create a buffer for the new content */
	char *newContent = malloc(fileSize * 2);
	if (!newContent) {
		free(content);
		return false;
	}

	char *src = content;
	char *dst = newContent;

	/* process line by line */
	while (*src) {
		/* find line boundaries */
		char *lineStart = src;
		char *lineEnd = strchr(src, '\n');
		size_t lineLen = lineEnd ? (size_t) (lineEnd - src) : strlen(src);

		/* check if this line contains any of our keys */
		bool replaced = false;
		for (int i = 0; i < count && !replaced; i++) {
			char pattern[128];
			snprintf(pattern, sizeof(pattern), "%s = ", keys[i]);

			char *match = strstr(lineStart, pattern);
			/* ensure match is within current line */
			if (match && match < lineStart + lineLen) {
				size_t prefixLen = match - lineStart + strlen(pattern);
				memcpy(dst, lineStart, prefixLen);
				dst += prefixLen;

				/* write the new value */
				dst += sprintf(dst, "%d", values[i]);

				char *valueStart = match + strlen(pattern);
				while (*valueStart == ' ' || *valueStart == '\t')
					valueStart++;
				char *valueEnd = valueStart;
				while (valueEnd < lineStart + lineLen && *valueEnd != ','
					&& *valueEnd != '\n' && *valueEnd != '\r') {
					valueEnd++;
				}

				size_t remainLen = (lineStart + lineLen) - valueEnd;
				memcpy(dst, valueEnd, remainLen);
				dst += remainLen;

				replaced = true;
			}
		}

		if (!replaced) {
			memcpy(dst, lineStart, lineLen);
			dst += lineLen;
		}

		if (lineEnd) {
			*dst++ = '\n';
			src = lineEnd + 1;
		}
		else {
			break;
		}
	}

	*dst = '\0';

	/* write back to file */
	f = fopen(path, "w");
	if (!f) {
		fprintf(stderr, "failed to open config file '%s' for writing\n", path);
		free(content);
		free(newContent);
		return false;
	}

	fputs(newContent, f);
	fclose(f);

	free(content);
	free(newContent);

	/* reload config to apply changes */
	return Config_Load(&g_config, path);
}

/* initialize config with defaults and load from file */
void Config_Init(void) {
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

	if (!Config_Load(&g_config, "config.lua")) {
		fprintf(stderr, "warning: failed to load config.lua, using defaults\n");
	}
}
