/* src/board.c
 * handle sudoku board logic
 */

#include <string.h>
#include <stdlib.h>

#include "board.h"
#include "generator.h"

void Board_Clear(Board *b) {
	for (int r = 0; r < BOARD_SIZE; r++)
		for (int c = 0; c < BOARD_SIZE; c++)
			b->cells[r][c] = (Cell) { 0 };
}

void Board_FromString(Board *b, const char *s) {
	Board_Clear(b);
	if (!s) return;
	for (int i = 0; i < BOARD_SIZE * BOARD_SIZE && s[i]; i++) {
		int r = i / BOARD_SIZE, c = i % BOARD_SIZE;
		char ch = s[i];
		if (ch >= '1' && ch <= '9') {
			b->cells[r][c].value = (uint8_t) (ch - '0');
			b->cells[r][c].given = true;
		}
	}
}

bool Board_IsValidMove(const Board *b, int r, int c, int v) {
	if (v < 1 || v > 9) return false;
	for (int i = 0; i < BOARD_SIZE; i++) {
		if (i != c && b->cells[r][i].value == v) return false;
		if (i != r && b->cells[i][c].value == v) return false;
	}
	int r0 = (r / SUBGRID) * SUBGRID, c0 = (c / SUBGRID) * SUBGRID;
	for (int rr = r0; rr < r0 + SUBGRID; rr++)
		for (int cc = c0; cc < c0 + SUBGRID; cc++)
			if (!(rr == r && cc == c) && b->cells[rr][cc].value == v)
				return false;
	return true;
}

void Board_Set(Board *b, int r, int c, int v, bool markGiven) {
	if (r < 0 || r >= BOARD_SIZE || c < 0 || c >= BOARD_SIZE) return;
	b->cells[r][c].value = (uint8_t) v;
	if (markGiven) b->cells[r][c].given = true;
}

bool Board_IsComplete(const Board *b) {
	for (int r = 0; r < BOARD_SIZE; r++)
		for (int c = 0; c < BOARD_SIZE; c++)
			if (b->cells[r][c].value == 0) return false;
	return true;
}

void Board_GenerateRandom(Board *b, Difficulty difficulty) {
	Generator_CreatePuzzle(b, difficulty, GEN_FLAG_UNIQUE | GEN_FLAG_FAST);
}

void Board_ClearNotesAffectedBy(Board *b, int r, int c, int v) {
	if (v < 1 || v > 9) return;

	unsigned int noteBit = 1u << v;

	/* clear notes in the same row */
	for (int col = 0; col < BOARD_SIZE; col++) {
		b->cells[r][col].notes &= ~noteBit;
	}

	/* clear notes in the same column */
	for (int row = 0; row < BOARD_SIZE; row++) {
		b->cells[row][c].notes &= ~noteBit;
	}

	/* clear notes in the same subgrid */
	int r0 = (r / SUBGRID) * SUBGRID;
	int c0 = (c / SUBGRID) * SUBGRID;
	for (int row = r0; row < r0 + SUBGRID; row++) {
		for (int col = c0; col < c0 + SUBGRID; col++) {
			b->cells[row][col].notes &= ~noteBit;
		}
	}
}
