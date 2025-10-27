/* src/puzzle_loader.c
 * load and parse the puzzle file format (ex. lol.txt)
 */

#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <dirent.h>
#include <sys/stat.h>

#include "puzzle_loader.h"

/* trim whitespace */
static void trim(char *str) {
	if (!str || !*str) return;

	/* leading */
	char *start = str;
	while (*start && isspace((unsigned char) *start))
		start++;

	/* trailing */
	char *end = start + strlen(start) - 1;
	while (end > start && isspace((unsigned char) *end))
		end--;
	*(end + 1) = '\0';

	if (start != str) memmove(str, start, strlen(start) + 1);
}

/* metadata line parser */
static bool parse_metadata_line(
	const char *line, const char *key, char *value, size_t value_size) {
	size_t key_len = strlen(key);

	/* check for line starting with key */
	if (strncmp(line, key, key_len) != 0) return false;

	/* skip key and colon */
	const char *val_start = line + key_len;
	if (*val_start == ':') val_start++;

	while (*val_start && isspace((unsigned char) *val_start))
		val_start++;

	/* ingest value */
	strncpy(value, val_start, value_size - 1);
	value[value_size - 1] = '\0';
	trim(value);

	return true;
}

bool Puzzle_LoadFromString(Puzzle *puzzle, const char *data) {
	if (!puzzle || !data) return false;

	/* init puzzle */
	memset(puzzle, 0, sizeof(Puzzle));
	strcpy(puzzle->meta.title, "untitled");
	strcpy(puzzle->meta.author, "unknown");

	char buffer[256];
	const char *line_start = data;
	const char *line_end;
	int board_row = 0;

	while (*line_start) {
		/* find end of line */
		line_end = line_start;
		while (*line_end && *line_end != '\n' && *line_end != '\r')
			line_end++;

		/* ingest line */
		size_t line_len = line_end - line_start;
		if (line_len >= sizeof(buffer)) line_len = sizeof(buffer) - 1;
		memcpy(buffer, line_start, line_len);
		buffer[line_len] = '\0';
		trim(buffer);

		/* skipping empty lines */
		if (buffer[0] != '\0') { /* metadata parsing */
			if (parse_metadata_line(
				    buffer, "title", puzzle->meta.title, MAX_PUZZLE_TITLE)) {
			} /* title parsed */
			else if (parse_metadata_line(buffer,
					 "author",
					 puzzle->meta.author,
					 MAX_PUZZLE_AUTHOR)) {
			} /* author parsed */
			else if (strlen(buffer) >= BOARD_SIZE) {
				if (board_row < BOARD_SIZE) {
					for (int c = 0; c < BOARD_SIZE; c++) {
						char ch = buffer[c];
						if (ch >= '1' && ch <= '9') {
							puzzle->board
								.cells[board_row][c]
								.value
								= (uint8_t) (ch - '0');
							puzzle->board
								.cells[board_row][c]
								.given
								= true;
						}
						else {
							puzzle->board
								.cells[board_row][c]
								.value
								= 0;
							puzzle->board
								.cells[board_row][c]
								.given
								= false;
						}
					}
					board_row++;
				}
			}
		}

		line_start = line_end;
		if (*line_start == '\r') line_start++;
		if (*line_start == '\n') line_start++;
	}

	if (board_row != BOARD_SIZE) return false;

	return true;
}

bool Puzzle_LoadFromFile(Puzzle *puzzle, const char *filepath) {
	if (!puzzle || !filepath) return false;

	FILE *f = fopen(filepath, "r");
	if (!f) return false;

	char buffer[4096];
	size_t total_read = 0;
	size_t bytes_read;

	while ((bytes_read = fread(
			buffer + total_read, 1, sizeof(buffer) - total_read - 1, f))
		> 0) {
		total_read += bytes_read;
		if (total_read >= sizeof(buffer) - 1) break;
	}
	buffer[total_read] = '\0';

	fclose(f);

	return Puzzle_LoadFromString(puzzle, buffer);
}

int PuzzleFileList_ScanDirectory(PuzzleFileList *list, const char *directory) {
	if (!list || !directory) return 0;

	list->count = 0;

	DIR *dir = opendir(directory);
	if (!dir) return 0;

	struct dirent *entry;
	while ((entry = readdir(dir)) != NULL && list->count < MAX_PUZZLE_FILES) {
		size_t len = strlen(entry->d_name);
		if (len < 4 || strcmp(entry->d_name + len - 4, ".txt") != 0) continue;

		char fullpath[MAX_FILEPATH_LEN];
		snprintf(fullpath, sizeof(fullpath), "%s/%s", directory, entry->d_name);
		struct stat st;
		if (stat(fullpath, &st) == 0 && S_ISDIR(st.st_mode)) continue;

		snprintf(list->filepaths[list->count],
			MAX_FILEPATH_LEN,
			"%s/%s",
			directory,
			entry->d_name);

		Puzzle puzzle;
		if (Puzzle_LoadFromFile(&puzzle, list->filepaths[list->count])) {
			snprintf(list->titles[list->count],
				MAX_PUZZLE_TITLE,
				"%s",
				puzzle.meta.title);
		}
		else {
			snprintf(list->titles[list->count],
				MAX_PUZZLE_TITLE,
				"%s",
				entry->d_name);
		}

		list->count++;
	}

	closedir(dir);
	return list->count;
}
