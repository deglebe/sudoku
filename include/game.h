/* include/game.h
 * main game state and screen management
 */

#ifndef GAME_H
#define GAME_H

#include <stdbool.h>

#include "config.h"
#include "board.h"
#include "puzzle_loader.h"

#define MAX_PUZZLE_TITLE_GAME 128

typedef enum AppScreen {
	SCREEN_MENU,
	SCREEN_DIFFICULTY,
	SCREEN_LOAD_PUZZLE,
	SCREEN_PLAY,
	SCREEN_SETTINGS,
	SCREEN_QUIT
} AppScreen;

typedef struct Game {
	Board board;
	int selRow, selCol;
	AppScreen screen;
	Difficulty selectedDifficulty;
	Theme theme;
	InputMode inputMode;
	bool highlightConflicts;
	char puzzleTitle[MAX_PUZZLE_TITLE_GAME];
	int selectedColorIndex; /* 0 = none, 1-9 = color palette */

	int menuSelection;
	int difficultySelection;
	int loadPuzzleSelection;
	bool puzzleListInitialized;
	PuzzleFileList puzzleList;

	/* settings state */
	int settingsSelection;
	Config tempConfig; /* temporary config while editing */
	bool settingsNeedApply; /* track if window resize is needed */
} Game;

void Game_Init(Game *g);
void Game_Update(Game *g);
void Game_Draw(const Game *g);
bool Game_LoadPuzzleFile(Game *g, const char *filepath);

#endif // GAME_H
