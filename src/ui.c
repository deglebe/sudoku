/* src/ui.c
 * ui handlers
 */

#include <stdio.h>
#include <string.h>

#include "raylib.h"

#include "ui.h"
#include "puzzle_loader.h"

/* caching color values */
typedef struct ThemeColors {
	Color bg;
	Color grid;
	Color gridBold;
	Color cellBg;
	Color cellSel;
	Color digitGiven;
	Color digitUser;
	Color accent;
	Color text;
	Color bad;
	Color highlightRowCol;
	Color highlightDigit;
	Color topbarBg;
	Color topbarText;
	Color menuText;
	Color menuSel;
	Color menuSelBg;
	Color cellColors[CELL_COLOR_COUNT];
} ThemeColors;

/* a bit forward thinking: conversion of a theme to cached color
 * TODO: themeing as a user customization :)
 */
static ThemeColors CacheThemeColors(const Theme *theme) {
	ThemeColors colors = { .bg = ColorFromUInt(theme->bg),
		.grid = ColorFromUInt(theme->grid),
		.gridBold = ColorFromUInt(theme->gridBold),
		.cellBg = ColorFromUInt(theme->cellBg),
		.cellSel = ColorFromUInt(theme->cellSel),
		.digitGiven = ColorFromUInt(theme->digitGiven),
		.digitUser = ColorFromUInt(theme->digitUser),
		.accent = ColorFromUInt(theme->accent),
		.text = ColorFromUInt(theme->text),
		.bad = ColorFromUInt(theme->bad),
		.highlightRowCol = ColorFromUInt(theme->highlightRowCol),
		.highlightDigit = ColorFromUInt(theme->highlightDigit),
		.topbarBg = ColorFromUInt(theme->topbarBg),
		.topbarText = ColorFromUInt(theme->topbarText),
		.menuText = ColorFromUInt(theme->menuText),
		.menuSel = ColorFromUInt(theme->menuSel),
		.menuSelBg = ColorFromUInt(theme->menuSelBg) };
	for (int i = 0; i < CELL_COLOR_COUNT; i++) {
		colors.cellColors[i] = ColorFromUInt(theme->cellColors[i]);
	}
	return colors;
}

/* get board pos and size */
static Rectangle BoardRect(void) {
	Rectangle rect = {
		BOARD_PAD, TOPBAR_H + BOARD_PAD, TILE_PIX * BOARD_SIZE, TILE_PIX * BOARD_SIZE
	};
	return rect;
}

/* uint to raylib color conversion */
Color ColorFromUInt(unsigned int c) {
	return (Color) { .r = (c >> 24) & 0xFF,
		.g = (c >> 16) & 0xFF,
		.b = (c >> 8) & 0xFF,
		.a = c & 0xFF };
}

/* default theme */
Theme Theme_Default(void) {
	Theme t = { .bg = COLOR_BG,
		.grid = COLOR_GRID,
		.gridBold = COLOR_GRID_BOLD,
		.cellBg = COLOR_CELL_BG,
		.cellSel = COLOR_CELL_SEL,
		.digitGiven = COLOR_DIGIT_GIVEN,
		.digitUser = COLOR_DIGIT_USER,
		.accent = COLOR_ACCENT,
		.text = COLOR_TEXT,
		.bad = COLOR_BAD,
		.highlightRowCol = COLOR_HIGHLIGHT_ROW_COL,
		.highlightDigit = COLOR_HIGHLIGHT_DIGIT,
		.cellColors = { 0, // CELL_COLOR_NONE
			COLOR_PALETTE_RED,
			COLOR_PALETTE_ORANGE,
			COLOR_PALETTE_YELLOW,
			COLOR_PALETTE_GREEN,
			COLOR_PALETTE_BLUE,
			COLOR_PALETTE_INDIGO,
			COLOR_PALETTE_VIOLET,
			COLOR_PALETTE_LIGHT_GRAY,
			COLOR_PALETTE_WHITE } };
	return t;
}

/* grid line consistency */
static void DrawGridLines(Rectangle boardRect, const ThemeColors *colors) {
	for (int i = 0; i <= BOARD_SIZE; i++) {
		int x = boardRect.x + i * TILE_PIX;
		int y = boardRect.y + i * TILE_PIX;

		/* 3x3 subgrids use bold lines and bold color */
		bool isBold = (i % SUBGRID == 0);
		int thickness = isBold ? GRID_LINE_THICK_B : GRID_LINE_THICK;
		Color lineColor = isBold ? colors->gridBold : colors->grid;

		/* vert line */
		DrawRectangle(
			x - thickness / 2, boardRect.y, thickness, boardRect.height, lineColor);
		/* horiz line */
		DrawRectangle(
			boardRect.x, y - thickness / 2, boardRect.width, thickness, lineColor);
	}
}

/* draw notes in a cell */
static void DrawCellNotes(Rectangle cell,
	unsigned int notes,
	int row,
	int col,
	const Game *g,
	const ThemeColors *colors) {
	/* pre-compute note cell size */
	const int noteCellSize = TILE_PIX / NOTE_GRID_SIZE;

	for (int noteNum = 1; noteNum <= 9; noteNum++) {
		/* check if note is set in the bitmask */
		if (notes & (1u << noteNum)) {
			/* calculate 3x3 grid position for this note */
			int noteRow = (noteNum - 1) / NOTE_GRID_SIZE;
			int noteCol = (noteNum - 1) % NOTE_GRID_SIZE;

			int x = cell.x + NOTE_PADDING_X + noteCol * noteCellSize;
			int y = cell.y + NOTE_PADDING_Y + noteRow * noteCellSize;

			/* check note validity */
			Color noteColor = Board_IsValidMove(&g->board, row, col, noteNum)
				? colors->text
				: colors->bad;

			char noteText[2] = { '0' + noteNum, '\0' };
			DrawText(noteText, x, y, FONT_SIZE_NOTE, noteColor);
		}
	}
}

/* draw contents in a cell */
static void DrawCellContent(Rectangle cell,
	const Cell *cellData,
	int row,
	int col,
	const Game *g,
	const ThemeColors *colors) {
	if (cellData->value) {
		/* draw the digit */
		char digitText[2] = { '0' + cellData->value, '\0' };
		int textWidth = MeasureText(digitText, FONT_SIZE_DIGIT);
		Color digitColor = cellData->given ? colors->digitGiven : colors->digitUser;

		/* highlight conflicts if enabled */
		if (g->highlightConflicts
			&& !Board_IsValidMove(&g->board, row, col, cellData->value)) {
			digitColor = colors->bad;
		}

		/* center the digit in the cell */
		int x = cell.x + (TILE_PIX - textWidth) / 2;
		int y = cell.y + (TILE_PIX - FONT_SIZE_DIGIT) / 2;
		DrawText(digitText, x, y, FONT_SIZE_DIGIT, digitColor);
	}
	else if (cellData->notes) {
		/* draw notes in the cell */
		DrawCellNotes(cell, cellData->notes, row, col, g, colors);
	}
}

/* draw the top bar with title and mode indicator */
void UI_DrawTopBar(const Game *g) {
	DrawRectangle(0, 0, WINDOW_W, TOPBAR_H, ColorFromUInt(COLOR_TOPBAR_BG));

	static char defaultTitle[128];
	const char *title;
	if (g->screen == SCREEN_PLAY) {
		title = g->puzzleTitle;
	}
	else {
		snprintf(defaultTitle, sizeof(defaultTitle), "%s  %s", APP_TITLE, APP_VERSION);
		title = defaultTitle;
	}
	DrawText(title,
		TOPBAR_PADDING,
		TOPBAR_PADDING,
		FONT_SIZE_TOPBAR,
		ColorFromUInt(COLOR_TOPBAR_TEXT));

	if (g->screen == SCREEN_PLAY) {
		const char *modeText = g->inputMode == INPUT_MODE_INSERT
			? "mode: INSERT"
			: "mode: NOTES";
		int textWidth = MeasureText(modeText, FONT_SIZE_TOPBAR);

		Color modeColor = g->inputMode == INPUT_MODE_INSERT
			? ColorFromUInt(COLOR_TOPBAR_TEXT)
			: ColorFromUInt(COLOR_ACCENT);
		DrawText(modeText,
			WINDOW_W - textWidth - TOPBAR_PADDING,
			TOPBAR_PADDING,
			FONT_SIZE_TOPBAR,
			modeColor);
	}
}

/* draw the sudoku board with grid, cells, digits, and notes */
void UI_DrawBoard(const Game *g) {
	Rectangle boardRect = BoardRect();

	ThemeColors colors = CacheThemeColors(&g->theme);

	DrawRectangleRec(boardRect, colors.cellBg);

	DrawGridLines(boardRect, &colors);

	/* get the value of the selected cell for digit highlighting */
	const Cell *selectedCell = &g->board.cells[g->selRow][g->selCol];
	int selectedDigit = selectedCell->value;

	/* draw cells, selection, and content */
	for (int row = 0; row < BOARD_SIZE; row++) {
		for (int col = 0; col < BOARD_SIZE; col++) {
			Rectangle cell = { boardRect.x + col * TILE_PIX,
				boardRect.y + row * TILE_PIX,
				TILE_PIX,
				TILE_PIX };

			const Cell *cellData = &g->board.cells[row][col];

			/* draw cell color if set */
			if (cellData->color > 0 && cellData->color < CELL_COLOR_COUNT) {
				DrawRectangleRec(cell, colors.cellColors[cellData->color]);
			}

			/*  highlight row and column of selected cell */
			if (g->selRow == row || g->selCol == col) {
				DrawRectangleRec(cell, colors.highlightRowCol);
			}

			/*  highlight cells with matching digit */
			if (selectedDigit != 0 && cellData->value == selectedDigit) {
				DrawRectangleRec(cell, colors.highlightDigit);
			}

			/* highlight selected cell */
			if (g->selRow == row && g->selCol == col) {
				DrawRectangleRec(cell, colors.cellSel);
			}

			DrawCellContent(cell, cellData, row, col, g, &colors);
		}
	}

	/* draw sidebar (shares color cache) */
	UI_DrawSidebar(g, &colors);
}

/* draw the sidebar with controls and status */
void UI_DrawSidebar(const Game *g, const ThemeColors *colors) {
	int x = BOARD_PAD + TILE_PIX * BOARD_SIZE + SIDEBAR_MARGIN;
	int y = TOPBAR_H + BOARD_PAD;

	DrawText("controls:", x, y, FONT_SIZE_LARGE, colors->text);
	y += CONTROLS_SECTION_SPACING;

	/*  Draw control instructions */
	DrawText("arrows: move", x, y, FONT_SIZE_NORMAL, colors->text);
	y += CONTROLS_LINE_SPACING;

	const char *digitAction = g->inputMode == INPUT_MODE_INSERT
		? "1-9: set digit"
		: "1-9: toggle note";
	DrawText(digitAction, x, y, FONT_SIZE_NORMAL, colors->text);
	y += CONTROLS_LINE_SPACING;

	DrawText("0 / backspace: clear", x, y, FONT_SIZE_NORMAL, colors->text);
	y += CONTROLS_LINE_SPACING;

	DrawText("n: toggle mode", x, y, FONT_SIZE_NORMAL, colors->text);
	y += CONTROLS_LINE_SPACING;

	DrawText("h: highlight conflicts", x, y, FONT_SIZE_NORMAL, colors->text);
	y += CONTROLS_LINE_SPACING;

	DrawText("esc: menu", x, y, FONT_SIZE_NORMAL, colors->text);
	y += CONTROLS_SECTION_SPACING + 8;

	/* Draw color keypad */
	DrawText("color palette:", x, y, FONT_SIZE_LARGE, colors->text);
	y += CONTROLS_SECTION_SPACING;

	for (int i = 1; i < CELL_COLOR_COUNT; i++) {
		int row = (i - 1) / COLOR_KEYPAD_COLS;
		int col = (i - 1) % COLOR_KEYPAD_COLS;
		int px = x + col * (COLOR_KEYPAD_SIZE + COLOR_KEYPAD_SPACING);
		int py = y + row * (COLOR_KEYPAD_SIZE + COLOR_KEYPAD_SPACING);

		Rectangle colorRect = { px, py, COLOR_KEYPAD_SIZE, COLOR_KEYPAD_SIZE };
		DrawRectangleRec(colorRect, colors->cellColors[i]);
		DrawRectangleLinesEx(colorRect, 2, colors->grid);

		/* highlight selected color */
		if (g->selectedColorIndex == i) {
			DrawRectangleLinesEx(colorRect, 3, colors->accent);
		}
	}

	y += ((CELL_COLOR_COUNT - 2) / COLOR_KEYPAD_COLS + 1)
		* (COLOR_KEYPAD_SIZE + COLOR_KEYPAD_SPACING);
	y += CONTROLS_SECTION_SPACING;

	/* reserve space for second keypad */
	DrawText("numbers: (wip)", x, y, FONT_SIZE_NORMAL, ColorAlpha(colors->text, 0.5f));

	if (Board_IsComplete(&g->board)) {
		y += CONTROLS_SECTION_SPACING;
		DrawText("solved!", x, y, FONT_SIZE_HEADING, colors->accent);
	}
}

void UI_DrawCenteredText(const char *text, int y) {
	int textWidth = MeasureText(text, FONT_SIZE_TITLE);
	DrawText(text, (WINDOW_W - textWidth) / 2, y, FONT_SIZE_TITLE, ColorFromUInt(COLOR_TEXT));
}

int UI_Menu(const char *const *items, int count, int *selected) {
	int sel = *selected;

	for (int i = 0; i < count; i++) {
		int textWidth = MeasureText(items[i], FONT_SIZE_MENU);
		int y = MENU_START_Y + i * MENU_ITEM_SPACING;
		int x = (WINDOW_W - textWidth) / 2;

		Color textColor = (i == sel) ? ColorFromUInt(COLOR_MENU_SEL)
					     : ColorFromUInt(COLOR_MENU_TEXT);

		if (i == sel) {
			DrawRectangle(x - MENU_PADDING_X,
				y - MENU_PADDING_Y,
				textWidth + 2 * MENU_PADDING_X,
				FONT_SIZE_MENU + 2 * MENU_PADDING_Y,
				ColorFromUInt(COLOR_MENU_SEL_BG));
		}

		DrawText(items[i], x, y, FONT_SIZE_MENU, textColor);
	}

	if (IsKeyPressed(KEY_UP)) sel = (sel - 1 < 0) ? count - 1 : sel - 1;
	if (IsKeyPressed(KEY_DOWN)) sel = (sel + 1 >= count) ? 0 : sel + 1;

	*selected = sel;

	if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) return sel;

	return -1;
}

void UI_DrawDifficultyMenu(Game *g) {
	UI_DrawCenteredText("Select Difficulty", 120);

	const char *difficulties[] = { "easy", "medium", "hard", "master", "expert" };

	int hit = UI_Menu(difficulties, 5, &g->difficultySelection);

	if (hit >= 0) {
		g->selectedDifficulty = (Difficulty) hit;
		Board_GenerateRandom(&g->board, g->selectedDifficulty);

		snprintf(g->puzzleTitle,
			MAX_PUZZLE_TITLE_GAME,
			"random puzzle - %s",
			difficulties[hit]);

		g->selRow = 0;
		g->selCol = 0;
		g->inputMode = INPUT_MODE_INSERT;
		g->screen = SCREEN_PLAY;
	}
}

void UI_DrawLoadPuzzleMenu(Game *g) {
	if (!g->puzzleListInitialized) {
		PuzzleFileList_ScanDirectory(&g->puzzleList, ".");
		g->puzzleListInitialized = true;
	}

	UI_DrawCenteredText("load puzzle", 120);

	if (g->puzzleList.count == 0) {
		DrawText("no puzzle files found in the directory",
			WINDOW_W / 2 - 200,
			250,
			FONT_SIZE_TOPBAR,
			ColorFromUInt(COLOR_TEXT));
		DrawText("create some! example puzzle is distributed with the source",
			WINDOW_W / 2 - 180,
			280,
			FONT_SIZE_SMALL,
			ColorFromUInt(COLOR_TEXT));

		/* override: esc goes to menu*/
		if (IsKeyPressed(KEY_ESCAPE)) {
			g->puzzleListInitialized = false;
			g->screen = SCREEN_MENU;
		}
		return;
	}

	/* menu items from puzzle title field */
	const char *items[MAX_PUZZLE_FILES];
	for (int i = 0; i < g->puzzleList.count; i++) {
		items[i] = g->puzzleList.titles[i];
	}

	int hit = UI_Menu(items, g->puzzleList.count, &g->loadPuzzleSelection);

	if (hit >= 0) {
		if (Game_LoadPuzzleFile(g, g->puzzleList.filepaths[hit])) {
			g->puzzleListInitialized = false;
		}
	}

	if (IsKeyPressed(KEY_ESCAPE)) {
		g->puzzleListInitialized = false;
		g->screen = SCREEN_MENU;
	}
}

typedef struct SettingsItem {
	const char *label;
	int *value;
	int min;
	int max;
	int step;
} SettingsItem;

void UI_DrawSettingsMenu(Game *g) {
	ThemeColors colors = CacheThemeColors(&g->theme);

	UI_DrawCenteredText("settings", 80);

	SettingsItem items[] = {
		{ "tile size", &g->tempConfig.tile_pix, 32, 96, 4 },
		{ "board padding", &g->tempConfig.board_pad, 8, 48, 4 },
		{ "sidebar width", &g->tempConfig.sidebar_w, 200, 400, 20 },
		{ "topbar height", &g->tempConfig.topbar_h, 40, 80, 4 },
		{ "grid line (thin)", &g->tempConfig.grid_line_thick, 1, 4, 1 },
		{ "grid line (bold)", &g->tempConfig.grid_line_thick_bold, 2, 8, 1 },
	};
	const int itemCount = sizeof(items) / sizeof(items[0]);

	const int totalItems = itemCount + 2;

	/* handle input */
	if (IsKeyPressed(KEY_UP)) {
		g->settingsSelection = (g->settingsSelection - 1 < 0)
			? totalItems - 1
			: g->settingsSelection - 1;
	}
	if (IsKeyPressed(KEY_DOWN)) {
		g->settingsSelection = (g->settingsSelection + 1 >= totalItems)
			? 0
			: g->settingsSelection + 1;
	}

	/* adjust values with left/right */
	if (g->settingsSelection < itemCount) {
		SettingsItem *item = &items[g->settingsSelection];
		if (IsKeyPressed(KEY_LEFT) && *item->value > item->min) {
			*item->value -= item->step;
			if (*item->value < item->min) *item->value = item->min;
			g->settingsNeedApply = true;
		}
		if (IsKeyPressed(KEY_RIGHT) && *item->value < item->max) {
			*item->value += item->step;
			if (*item->value > item->max) *item->value = item->max;
			g->settingsNeedApply = true;
		}
	}

	int startY = 160;
	int lineHeight = 36;

	for (int i = 0; i < itemCount; i++) {
		int y = startY + i * lineHeight;
		bool isSelected = (g->settingsSelection == i);

		Color labelColor = isSelected ? colors.accent : colors.text;
		/* keep value visible with high contrast - use digitUser for unselected, text for selected */
		Color valueColor = isSelected ? colors.text : colors.digitUser;

		/* draw label on the left */
		char label[128];
		snprintf(label, sizeof(label), "%s:", items[i].label);
		DrawText(label, WINDOW_W / 2 - 180, y, FONT_SIZE_NORMAL, labelColor);

		/* draw arrows on either side of the value if selected */
		if (isSelected) {
			DrawText("<", WINDOW_W / 2 + 30, y, FONT_SIZE_NORMAL, colors.accent);
			DrawText(">", WINDOW_W / 2 + 150, y, FONT_SIZE_NORMAL, colors.accent);
		}

		char value[32];
		snprintf(value, sizeof(value), "%d", *items[i].value);
		int valueWidth = MeasureText(value, FONT_SIZE_NORMAL);
		int valueX = WINDOW_W / 2 + 90 - valueWidth / 2; /* center between arrows */
		DrawText(value, valueX, y, FONT_SIZE_NORMAL, valueColor);
	}

	/* draw save/cancel buttons using same style as main menu */
	int buttonY = startY + itemCount * lineHeight + 40;

	/* save button */
	bool saveSelected = (g->settingsSelection == itemCount);
	int saveTextWidth = MeasureText("save & apply", FONT_SIZE_MENU);
	int saveX = WINDOW_W / 2 - saveTextWidth / 2;

	Color saveTextColor = saveSelected ? ColorFromUInt(COLOR_MENU_SEL)
					   : ColorFromUInt(COLOR_MENU_TEXT);

	if (saveSelected) {
		DrawRectangle(saveX - MENU_PADDING_X,
			buttonY - MENU_PADDING_Y,
			saveTextWidth + 2 * MENU_PADDING_X,
			FONT_SIZE_MENU + 2 * MENU_PADDING_Y,
			ColorFromUInt(COLOR_MENU_SEL_BG));
	}

	DrawText("save & apply", saveX, buttonY, FONT_SIZE_MENU, saveTextColor);

	buttonY += 48;
	bool cancelSelected = (g->settingsSelection == itemCount + 1);
	/* TODO: do this more efficiently */
	int cancelTextWidth = MeasureText("cancel", FONT_SIZE_MENU);
	int cancelX = WINDOW_W / 2 - cancelTextWidth / 2;

	Color cancelTextColor = cancelSelected ? ColorFromUInt(COLOR_MENU_SEL)
					       : ColorFromUInt(COLOR_MENU_TEXT);

	if (cancelSelected) {
		DrawRectangle(cancelX - MENU_PADDING_X,
			buttonY - MENU_PADDING_Y,
			cancelTextWidth + 2 * MENU_PADDING_X,
			FONT_SIZE_MENU + 2 * MENU_PADDING_Y,
			ColorFromUInt(COLOR_MENU_SEL_BG));
	}

	DrawText("cancel", cancelX, buttonY, FONT_SIZE_MENU, cancelTextColor);

	/* draw instructions */
	DrawText("use arrow keys to adjust values",
		WINDOW_W / 2 - 150,
		WINDOW_H - 80,
		FONT_SIZE_SMALL,
		ColorAlpha(colors.text, 0.7f));
	DrawText("press enter to select",
		WINDOW_W / 2 - 100,
		WINDOW_H - 60,
		FONT_SIZE_SMALL,
		ColorAlpha(colors.text, 0.7f));

	if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
		if (g->settingsSelection == itemCount) {
			const char *keys[] = { "tile_pix",
				"board_pad",
				"sidebar_w",
				"topbar_h",
				"grid_line_thick",
				"grid_line_thick_bold" };
			int values[] = { g->tempConfig.tile_pix,
				g->tempConfig.board_pad,
				g->tempConfig.sidebar_w,
				g->tempConfig.topbar_h,
				g->tempConfig.grid_line_thick,
				g->tempConfig.grid_line_thick_bold };

			if (Config_UpdateValues(
				    "config.lua", "window", keys, values, itemCount)) {
				printf("config updated successfully!\n");

				if (g->settingsNeedApply) {
					SetWindowSize(WINDOW_W, WINDOW_H);
					g->settingsNeedApply = false;
				}
			}
			else {
				fprintf(stderr, "failed to update config\n");
			}

			g->screen = SCREEN_MENU;
		}
		else if (g->settingsSelection == itemCount + 1) {
			g->settingsNeedApply = false;
			g->screen = SCREEN_MENU;
		}
	}
}
