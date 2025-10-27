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
	strcpy(g->puzzleTitle, "Sudoku");

	/* menu state */
	g->menuSelection = 0;
	g->difficultySelection = 0;
	g->loadPuzzleSelection = 0;
	g->puzzleListInitialized = false;
	g->puzzleList.count = 0;

	Board_Clear(&g->board);
}

void Game_Update(Game *g) {
	switch (g->screen) {
	case SCREEN_MENU:
		ClearBackground(ColorFromUInt(g->theme.bg));
		UI_DrawTopBar(g);
		UI_DrawCenteredText("sudoku", 120);
		const char *items[] = { "play random", "load puzzle", "settings", "quit" };
		int hit = UI_Menu(items, 4, &g->menuSelection);
		if (hit >= 0) {
			const AppScreen screens[] = { SCREEN_DIFFICULTY,
				SCREEN_LOAD_PUZZLE,
				SCREEN_SETTINGS,
				SCREEN_QUIT };
			g->screen = screens[hit];
		}
		break;
	case SCREEN_DIFFICULTY:
		ClearBackground(ColorFromUInt(g->theme.bg));
		UI_DrawTopBar(g);
		UI_DrawDifficultyMenu(g);
		break;
	case SCREEN_LOAD_PUZZLE:
		ClearBackground(ColorFromUInt(g->theme.bg));
		UI_DrawTopBar(g);
		UI_DrawLoadPuzzleMenu(g);
		break;
	case SCREEN_PLAY:
		Input_Update(g);
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
		UI_DrawSidebar(g);
		break;
	case SCREEN_SETTINGS:
		ClearBackground(ColorFromUInt(g->theme.bg));
		UI_DrawTopBar(g);
		UI_DrawCenteredText("settings (wip)", WINDOW_H / 2 - 18);
		DrawText("TODO: themes, input config, generator toggles",
			120,
			WINDOW_H / 2 + 20,
			20,
			ColorFromUInt(g->theme.text));
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
