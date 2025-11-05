/* include/ui.h
 * user interface drawing functions
 */

#ifndef UI_H
#define UI_H

#include <stdbool.h>

#include "game.h"
#include "raylib.h"

/* helper function to safely convert unsigned int color to Color struct */
Color ColorFromUInt(unsigned int c);

/* forward declare internal type */
typedef struct ThemeColors ThemeColors;

/* drawing helpers */
void UI_DrawTopBar(const Game *g);
void UI_DrawBoard(const Game *g);
void UI_DrawSidebar(const Game *g, const ThemeColors *colors);
void UI_DrawDifficultyMenu(Game *g);
void UI_DrawLoadPuzzleMenu(Game *g);
void UI_DrawSettingsMenu(Game *g);

/* menu helpers */
int UI_Menu(const char *const *items, int count, int *selected);
void UI_DrawCenteredText(const char *text, int y);

/* theme helpers */
Theme Theme_Default(void);

#endif // UI_H
