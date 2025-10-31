/* src/main.c
 * main game loop
 */

#include <stdlib.h>
#include <time.h>

#include "raylib.h"

#include "game.h"
#include "config.h"
#include "input.h"

int main(void) {
	/* seed rng */
	srand((unsigned int) time(NULL));

	/* initialize config from Lua */
	Config_Init();

	SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT);
	InitWindow(WINDOW_W, WINDOW_H, APP_TITLE);
	SetTargetFPS(60);

	/* disable default esc behaviour */
	SetExitKey(0);

	Game game;
	Game_Init(&game);

	while (true) {
		if (Input_EscapePressed()) {
			switch (game.screen) {
			case SCREEN_PLAY:
			case SCREEN_DIFFICULTY:
			case SCREEN_LOAD_PUZZLE:
			case SCREEN_SETTINGS:
				game.screen = SCREEN_MENU;
				break;
			case SCREEN_MENU:
				game.screen = SCREEN_QUIT;
				break;
			default:
				break;
			}
		}

		if (game.screen == SCREEN_QUIT || WindowShouldClose()) {
			break;
		}

		Game_Update(&game);
		Game_Draw(&game);
	}

	CloseWindow();
	return 0;
}
