/* include/puzzle_loader.h
 * Puzzle file format loader
 */

#ifndef PUZZLE_LOADER_H
#define PUZZLE_LOADER_H

#include <stdbool.h>
#include "board.h"

#define MAX_PUZZLE_TITLE 128
#define MAX_PUZZLE_AUTHOR 128

typedef struct PuzzleMetadata {
	char title[MAX_PUZZLE_TITLE];
	char author[MAX_PUZZLE_AUTHOR];
} PuzzleMetadata;

typedef struct Puzzle {
	PuzzleMetadata meta;
	Board board;
} Puzzle;

/* load puzzle from file */
bool Puzzle_LoadFromFile(Puzzle *puzzle, const char *filepath);

/* load puzzle from string  */
bool Puzzle_LoadFromString(Puzzle *puzzle, const char *data);

/* puzzle file list management */
#define MAX_PUZZLE_FILES 100
#define MAX_FILEPATH_LEN 256

typedef struct PuzzleFileList {
	char filepaths[MAX_PUZZLE_FILES][MAX_FILEPATH_LEN];
	char titles[MAX_PUZZLE_FILES][MAX_PUZZLE_TITLE];
	int count;
} PuzzleFileList;

/* scan directory for .txt puzzle files */
int PuzzleFileList_ScanDirectory(PuzzleFileList *list, const char *directory);

#endif // PUZZLE_LOADER_H
