/* src/input.c
 * input handler
 */

#include "raylib.h"

#include "input.h"
#include "board.h"

bool Input_EscapePressed(void) {
	return IsKeyPressed(KEY_ESCAPE);
}

static bool MouseToBoardCoords(int mouseX, int mouseY, int *row, int *col) {
	/* calculate board bounds */
	int boardX = BOARD_PAD;
	int boardY = TOPBAR_H + BOARD_PAD;
	int boardSize = TILE_PIX * BOARD_SIZE;

	/* check if mouse is within board bounds */
	if (mouseX < boardX || mouseX >= boardX + boardSize || mouseY < boardY
		|| mouseY >= boardY + boardSize) {
		*row = -1;
		*col = -1;
		return false;
	}

	/* convert to cell coordinates */
	*col = (mouseX - boardX) / TILE_PIX;
	*row = (mouseY - boardY) / TILE_PIX;

	/* clamp to valid range */
	if (*row < 0) *row = 0;
	if (*row >= BOARD_SIZE) *row = BOARD_SIZE - 1;
	if (*col < 0) *col = 0;
	if (*col >= BOARD_SIZE) *col = BOARD_SIZE - 1;

	return true;
}

void Input_Update(Game *g) {
	/* mouse input for cell selection */
	if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
		Vector2 mousePos = GetMousePosition();
		int row, col;
		if (MouseToBoardCoords((int) mousePos.x, (int) mousePos.y, &row, &col)) {
			g->selRow = row;
			g->selCol = col;
		}
	}

	/* keyboard selection movement */
	if (IsKeyPressed(KEY_RIGHT)) g->selCol = (g->selCol + 1) % BOARD_SIZE;
	if (IsKeyPressed(KEY_LEFT))
		g->selCol = (g->selCol - 1 + BOARD_SIZE) % BOARD_SIZE;
	if (IsKeyPressed(KEY_DOWN)) g->selRow = (g->selRow + 1) % BOARD_SIZE;
	if (IsKeyPressed(KEY_UP)) g->selRow = (g->selRow - 1 + BOARD_SIZE) % BOARD_SIZE;

	/* digit input */
	Cell *ce = &g->board.cells[g->selRow][g->selCol];
	if (!ce->given) {
		/* check if clear key (0, bcksp, del) */
		if (IsKeyPressed(KEY_ZERO) || IsKeyPressed(KEY_KP_0)
			|| IsKeyPressed(KEY_BACKSPACE) || IsKeyPressed(KEY_DELETE)) {
			ce->value = 0;
			ce->notes = 0;
		}
		else if (g->inputMode == INPUT_MODE_INSERT) {
			/* insert mode */
			for (int v = 1; v <= 9; v++) {
				if (IsKeyPressed(KEY_ZERO + v)
					|| IsKeyPressed(KEY_KP_0 + v)) {
					ce->value = (uint8_t) v;
					ce->notes = 0; /* clear notes */
					/* auto-remove notes from affected cells */
					Board_ClearNotesAffectedBy(
						&g->board, g->selRow, g->selCol, v);
					break;
				}
			}
		}
		else {
			/* notes mode: toggle candidates */
			for (int v = 1; v <= 9; v++) {
				if (IsKeyPressed(KEY_ZERO + v)
					|| IsKeyPressed(KEY_KP_0 + v)) {
					/* toggle the bit for this number */
					ce->notes ^= (1u << v);
					break;
				}
			}
		}
	}

	/* mode toggle */
	if (IsKeyPressed(KEY_N))
		g->inputMode = (g->inputMode == INPUT_MODE_INSERT) ? INPUT_MODE_NOTES
								   : INPUT_MODE_INSERT;

	/* conflict highlighting (default on)*/
	if (IsKeyPressed(KEY_H)) g->highlightConflicts = !g->highlightConflicts;
}
