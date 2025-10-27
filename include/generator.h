/* include/generator.h
 *
 * implements:
 * - knuth's algorithm x with dancing links for complete grid generation
 * - backtracking solver with solution counting for uniqueness verification
 * - clue removal with difficulty control
 */

#ifndef GENERATOR_H
#define GENERATOR_H

#include <stdbool.h>
#include "board.h"

/* generator configuration flags */
typedef enum GeneratorFlags {
	GEN_FLAG_UNIQUE = 1 << 0, // ensure puzzle has unique solution
	GEN_FLAG_FAST = 1 << 1 // skip most checks, verify only at end
} GeneratorFlags;

/* result information from generator */
typedef struct GeneratorResult {
	bool success;
	bool unique;
	int clues;
	int attempts;
} GeneratorResult;

/* generate a complete valid Sudoku grid using dlx */
bool Generator_FillGrid(Board *b);

/* generate a puzzle with specified difficulty and flags */
GeneratorResult Generator_CreatePuzzle(
	Board *b, Difficulty difficulty, GeneratorFlags flags);

/* count the number of solutions a puzzle has */
int Generator_CountSolutions(const Board *b, int max_solutions);

/* verify if a puzzle has a unique solution */
bool Generator_HasUniqueSolution(const Board *b);

/* seed the random number generator for reproducible puzzles
 * pass 0 to use time-based seeding
 */
void Generator_Seed(unsigned int seed);

#endif // GENERATOR_H
