/* src/game.c
 * core game logic
 */

#include <string.h>

#include "raylib.h"

#include "game.h"
#include "ui.h"
#include "input.h"
#include "puzzle_loader.h"

void Game_Init(Game *g) {
	*g = (Game) { 0 };
	g->theme = Theme_Default();
	g->screen = SCREEN_MENU;
	g->selRow = 0;
	g->selCol = 0;
	g->inputMode = INPUT_MODE_INSERT;
	g->highlightConflicts = true;
	g->selectedColorIndex = 0;
	strcpy(g->puzzleTitle, "Sudoku");

	/* menu state */
	g->menuSelection = 0;
	g->difficultySelection = 0;
	g->loadPuzzleSelection = 0;
	g->puzzleListInitialized = false;
	g->puzzleList.count = 0;

	/* settings state */
	g->settingsSelection = 0;
	g->tempConfig = g_config;
	g->settingsNeedApply = false;

	Board_Clear(&g->board);
}

void Game_Update(Game *g) {
	/* handle input for play screen */
	if (g->screen == SCREEN_PLAY) {
		Input_Update(g);
		return;
	}

	/* all menu screens share the same rendering setup */
	ClearBackground(ColorFromUInt(g->theme.bg));
	UI_DrawTopBar(g);

	switch (g->screen) {
	case SCREEN_MENU:
		UI_DrawCenteredText("sudoku", 120);
		const char *items[] = { "play random", "load puzzle", "settings", "quit" };
		int hit = UI_Menu(items, 4, &g->menuSelection);
		if (hit >= 0) {
			const AppScreen screens[] = { SCREEN_DIFFICULTY,
				SCREEN_LOAD_PUZZLE,
				SCREEN_SETTINGS,
				SCREEN_QUIT };
			g->screen = screens[hit];

			/* initialize settings when entering settings screen */
			if (g->screen == SCREEN_SETTINGS) {
				g->tempConfig = g_config;
				g->settingsSelection = 0;
				g->settingsNeedApply = false;
			}
		}
		break;
	case SCREEN_DIFFICULTY:
		UI_DrawDifficultyMenu(g);
		break;
	case SCREEN_LOAD_PUZZLE:
		UI_DrawLoadPuzzleMenu(g);
		break;
	case SCREEN_SETTINGS:
		UI_DrawSettingsMenu(g);
		break;
	default:
		break;
	}
}

void Game_Draw(const Game *g) {
	BeginDrawing();

	switch (g->screen) {
	case SCREEN_PLAY:
		ClearBackground(ColorFromUInt(g->theme.bg));
		UI_DrawTopBar(g);
		UI_DrawBoard(g);
		break;
	case SCREEN_QUIT:
		ClearBackground(ColorFromUInt(g->theme.bg));
		UI_DrawTopBar(g);
		UI_DrawCenteredText("goodbye!", WINDOW_H / 2 - 18);
		break;
	default:
		break;
	}

	EndDrawing();
}

bool Game_LoadPuzzleFile(Game *g, const char *filepath) {
	Puzzle puzzle;
	if (!Puzzle_LoadFromFile(&puzzle, filepath)) return false;

	/* load board and puzzle */
	g->board = puzzle.board;
	strncpy(g->puzzleTitle, puzzle.meta.title, MAX_PUZZLE_TITLE_GAME - 1);
	g->puzzleTitle[MAX_PUZZLE_TITLE_GAME - 1] = '\0';

	/* reset game state */
	g->selRow = 0;
	g->selCol = 0;
	g->inputMode = INPUT_MODE_INSERT;
	g->screen = SCREEN_PLAY;

	return true;
}
