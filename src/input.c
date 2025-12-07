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

	/* convert to cell coordinates - already in valid range due to bounds check */
	*col = (mouseX - boardX) / TILE_PIX;
	*row = (mouseY - boardY) / TILE_PIX;

	return true;
}

static int MouseToColorIndex(int mouseX, int mouseY) {
	/* calculate color keypad position - must match UI_DrawSidebar exactly */
	int keypadX = BOARD_PAD + TILE_PIX * BOARD_SIZE + SIDEBAR_MARGIN;
	/* Start: TOPBAR_H + BOARD_PAD
	 * + CONTROLS_SECTION_SPACING (after "controls:")
	 * + CONTROLS_LINE_SPACING * 5 (5 control lines)
	 * + CONTROLS_SECTION_SPACING + 8 (after last control)
	 * + CONTROLS_SECTION_SPACING (after "color palette:")
	 */
	int keypadY = TOPBAR_H + BOARD_PAD + CONTROLS_SECTION_SPACING * 3
		+ CONTROLS_LINE_SPACING * 5 + 8;

	/* check each color button */
	for (int i = 1; i < CELL_COLOR_COUNT; i++) {
		int row = (i - 1) / COLOR_KEYPAD_COLS;
		int col = (i - 1) % COLOR_KEYPAD_COLS;
		int px = keypadX + col * (COLOR_KEYPAD_SIZE + COLOR_KEYPAD_SPACING);
		int py = keypadY + row * (COLOR_KEYPAD_SIZE + COLOR_KEYPAD_SPACING);

		if (mouseX >= px && mouseX < px + COLOR_KEYPAD_SIZE && mouseY >= py
			&& mouseY < py + COLOR_KEYPAD_SIZE) {
			return i;
		}
	}

	return 0; /* no color selected */
}

void Input_Update(Game *g) {
	/* mouse input for cell selection and color keypad */
	if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
		Vector2 mousePos = GetMousePosition();
		int row, col;
		if (MouseToBoardCoords((int) mousePos.x, (int) mousePos.y, &row, &col)) {
			g->selRow = row;
			g->selCol = col;
		}
		else {
			/* check if clicking on color keypad */
			int colorIdx
				= MouseToColorIndex((int) mousePos.x, (int) mousePos.y);
			if (colorIdx > 0) {
				Cell *cell = &g->board.cells[g->selRow][g->selCol];
				/* toggle color: if same color is already on cell, remove it */
				if (cell->color == colorIdx) {
					cell->color = 0;
					g->selectedColorIndex = 0;
				}
				else {
					cell->color = (uint8_t) colorIdx;
					g->selectedColorIndex = colorIdx;
				}
			}
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

	/* pause/play toggle */
	if (IsKeyPressed(KEY_SPACE)) g->paused = !g->paused;
}
