/* include/board.h
 * board and cell functions
 */

#ifndef BOARD_H
#define BOARD_H

#include <stdbool.h>
#include <stdint.h>

#include "config.h"

typedef struct Cell {
	uint8_t value; /* 0 for empty, 1-9 for digit */
	bool given; /* true if part of puzzle */
	uint16_t notes; /* bitmask 1<<n for note n (1..9) */
} Cell;

typedef struct Board {
	Cell cells[BOARD_SIZE][BOARD_SIZE];
} Board;

void Board_Clear(Board *b);
void Board_FromString(Board *b, const char *digits81);
bool Board_IsValidMove(const Board *b, int r, int c, int v);
void Board_Set(Board *b, int r, int c, int v, bool markGiven);
bool Board_IsComplete(const Board *b);
void Board_ClearNotesAffectedBy(Board *b, int r, int c, int v);

void Board_GenerateRandom(Board *b, Difficulty difficulty);

#endif // BOARD_H
