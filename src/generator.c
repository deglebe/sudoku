/* src/generator.c
 * puzzle generation
 * uses knuth's algorithm x and dlx for generating valid solved boards
 */

#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "generator.h"
#include "config.h"

static inline int get_box_id(int row, int col) {
	return (row / SUBGRID) * SUBGRID + (col / SUBGRID);
}

void Generator_Seed(unsigned int seed) {
	srand(seed ? seed : (unsigned int) time(NULL));
}

/* dlx */
typedef struct DLXNode {
	struct DLXNode *left, *right, *up, *down;
	struct DLXColumn *column;
	int row_id;
} DLXNode;

typedef struct DLXColumn {
	DLXNode header;
	int size, id;
} DLXColumn;

typedef struct DLXRoot {
	DLXColumn root, *columns;
	DLXNode *nodes;
	int num_columns, node_idx;
} DLXRoot;

#define DLX_COLUMNS 324
#define DLX_MAX_NODES (BOARD_SIZE * BOARD_SIZE * BOARD_SIZE * 4)

static void DLX_Cover(DLXColumn *col) {
	col->header.right->left = col->header.left;
	col->header.left->right = col->header.right;
	for (DLXNode *row = col->header.down; row != &col->header; row = row->down)
		for (DLXNode *node = row->right; node != row; node = node->right) {
			node->down->up = node->up;
			node->up->down = node->down;
			node->column->size--;
		}
}

static void DLX_Uncover(DLXColumn *col) {
	for (DLXNode *row = col->header.up; row != &col->header; row = row->up)
		for (DLXNode *node = row->left; node != row; node = node->left) {
			node->column->size++;
			node->down->up = node;
			node->up->down = node;
		}
	col->header.right->left = &col->header;
	col->header.left->right = &col->header;
}

static DLXColumn *DLX_ChooseColumnMRV(DLXRoot *dlx) {
	DLXColumn *best = NULL;
	int min_size = BOARD_SIZE + 1;
	for (DLXNode *node = dlx->columns[0].header.right; node != &dlx->columns[0].header;
		node = node->right) {
		DLXColumn *col = node->column;
		if (col->size < min_size) {
			min_size = col->size;
			best = col;
			if (min_size == 1) break;
		}
	}
	return best;
}

/* encode sudoku constraints: cell, row, col, box */
static void encode_constraints(int row, int col, int val, int c[4]) {
	c[0] = row * BOARD_SIZE + col;
	c[1] = 81 + row * BOARD_SIZE + (val - 1);
	c[2] = 162 + col * BOARD_SIZE + (val - 1);
	c[3] = 243 + get_box_id(row, col) * BOARD_SIZE + (val - 1);
}

static void DLX_Init(DLXRoot *dlx) {
	dlx->num_columns = DLX_COLUMNS;
	dlx->node_idx = 0;
	dlx->columns = malloc((DLX_COLUMNS + 1) * sizeof(DLXColumn));
	dlx->nodes = malloc(DLX_MAX_NODES * sizeof(DLXNode));

	DLXColumn *root = &dlx->columns[0];
	root->header.left = root->header.right = &root->header;
	root->header.up = root->header.down = NULL;
	root->header.column = NULL;
	root->header.row_id = root->size = root->id = -1;

	for (int i = 1; i <= DLX_COLUMNS; i++) {
		DLXColumn *col = &dlx->columns[i];
		col->size = 0;
		col->id = i - 1;
		col->header.column = col;
		col->header.row_id = -1;
		col->header.up = col->header.down = &col->header;
		col->header.left = root->header.left;
		col->header.right = &root->header;
		root->header.left->right = &col->header;
		root->header.left = &col->header;
	}
}

static void DLX_AddRow(DLXRoot *dlx, int row_id, int constraints[4]) {
	DLXNode *first = NULL, *prev = NULL;
	for (int i = 0; i < 4; i++) {
		DLXColumn *col = &dlx->columns[constraints[i] + 1];
		DLXNode *node = &dlx->nodes[dlx->node_idx++];
		node->row_id = row_id;
		node->column = col;
		node->up = col->header.up;
		node->down = &col->header;
		col->header.up->down = node;
		col->header.up = node;
		col->size++;
		if (!first) {
			first = node;
			node->left = node->right = node;
		}
		else {
			node->left = prev;
			node->right = first;
			prev->right = node;
			first->left = node;
		}
		prev = node;
	}
}

static void DLX_BuildFromBoard(DLXRoot *dlx, const Board *b) {
	DLX_Init(dlx);
	for (int r = 0; r < BOARD_SIZE; r++)
		for (int c = 0; c < BOARD_SIZE; c++) {
			uint8_t fixed = b->cells[r][c].value;
			for (int v = (fixed ? fixed : 1); v <= (fixed ? fixed : 9); v++) {
				int constraints[4];
				encode_constraints(r, c, v, constraints);
				DLX_AddRow(dlx, r * 81 + c * 9 + v - 1, constraints);
			}
		}
}

static bool DLX_SolveRandom(DLXRoot *dlx, int sol[BOARD_SIZE * BOARD_SIZE * BOARD_SIZE]) {
	if (dlx->columns[0].header.right == &dlx->columns[0].header) return true;
	DLXColumn *col = DLX_ChooseColumnMRV(dlx);
	if (!col || col->size == 0) return false;
	DLX_Cover(col);

	DLXNode *rows[9];
	int cnt = 0;
	for (DLXNode *row = col->header.down; row != &col->header; row = row->down)
		rows[cnt++] = row;

	/* fisher-yates shuffle (thanks jonas for loaning me TAoCP!) */
	for (int i = cnt - 1; i > 0; i--) {
		int j = rand() % (i + 1);
		DLXNode *tmp = rows[i];
		rows[i] = rows[j];
		rows[j] = tmp;
	}

	bool found = false;
	for (int i = 0; i < cnt && !found; i++) {
		DLXNode *row = rows[i];
		for (DLXNode *n = row->right; n != row; n = n->right)
			DLX_Cover(n->column);
		sol[row->row_id] = 1;
		if (DLX_SolveRandom(dlx, sol))
			found = true;
		else
			sol[row->row_id] = 0;
		for (DLXNode *n = row->left; n != row; n = n->left)
			DLX_Uncover(n->column);
	}
	DLX_Uncover(col);
	return found;
}

bool Generator_FillGrid(Board *b) {
	Board_Clear(b);

	DLXRoot dlx;
	DLX_BuildFromBoard(&dlx, b);
	int solution[729] = { 0 };
	bool success = DLX_SolveRandom(&dlx, solution);

	if (success)
		for (int i = 0; i < 729; i++)
			if (solution[i]) {
				b->cells[i / 81][(i / 9) % 9].value = i % 9 + 1;
				b->cells[i / 81][(i / 9) % 9].given = true;
			}

	free(dlx.columns);
	free(dlx.nodes);
	return success;
}

/* fast backtracking solver via bitmasking */
typedef struct {
	uint8_t grid[9][9];
	uint16_t row_mask[9], col_mask[9], box_mask[9];
	int solution_count, max_solutions;
} FastSolver;

static void FastSolver_Init(FastSolver *fs, const Board *b) {
	memset(fs->row_mask, 0, sizeof(fs->row_mask));
	memset(fs->col_mask, 0, sizeof(fs->col_mask));
	memset(fs->box_mask, 0, sizeof(fs->box_mask));
	fs->solution_count = 0;
	for (int r = 0; r < 9; r++)
		for (int c = 0; c < 9; c++) {
			int v = b->cells[r][c].value;
			fs->grid[r][c] = v;
			if (v > 0) {
				int bit = 1 << (v - 1);
				fs->row_mask[r] |= bit;
				fs->col_mask[c] |= bit;
				fs->box_mask[get_box_id(r, c)] |= bit;
			}
		}
}

static inline uint16_t get_candidates(FastSolver *fs, int r, int c) {
	return fs->grid[r][c]
		? 0
		: ~(fs->row_mask[r] | fs->col_mask[c] | fs->box_mask[get_box_id(r, c)])
			& 0x1FF;
}

static bool find_mrv(FastSolver *fs, int *r, int *c, uint16_t *cand) {
	int min_cnt = 10, br = -1, bc = -1;
	uint16_t bcand = 0;
	for (int rr = 0; rr < 9; rr++)
		for (int cc = 0; cc < 9; cc++)
			if (!fs->grid[rr][cc]) {
				uint16_t candidates = get_candidates(fs, rr, cc);
				int cnt = __builtin_popcount(candidates);
				if (cnt == 0) return false;
				if (cnt < min_cnt) {
					min_cnt = cnt;
					br = rr;
					bc = cc;
					bcand = candidates;
					if (cnt == 1) {
						*r = br;
						*c = bc;
						*cand = bcand;
						return true;
					}
				}
			}
	if (br == -1) return false;
	*r = br;
	*c = bc;
	*cand = bcand;
	return true;
}

static void solve_fast(FastSolver *fs) {
	if (fs->solution_count >= fs->max_solutions) return;
	int r = -1, c = -1;
	uint16_t cand;
	if (!find_mrv(fs, &r, &c, &cand)) {
		if (r == -1) fs->solution_count++;
		return;
	}
	int box = get_box_id(r, c);
	for (int v = 1; v <= 9; v++)
		if (cand & (1 << (v - 1))) {
			int bit = 1 << (v - 1);
			fs->grid[r][c] = v;
			fs->row_mask[r] |= bit;
			fs->col_mask[c] |= bit;
			fs->box_mask[box] |= bit;
			solve_fast(fs);
			fs->grid[r][c] = 0;
			fs->row_mask[r] &= ~bit;
			fs->col_mask[c] &= ~bit;
			fs->box_mask[box] &= ~bit;
			if (fs->solution_count >= fs->max_solutions) return;
		}
}

int Generator_CountSolutions(const Board *b, int max_solutions) {
	FastSolver fs;
	FastSolver_Init(&fs, b);
	fs.max_solutions = max_solutions;
	solve_fast(&fs);
	return fs.solution_count;
}

bool Generator_HasUniqueSolution(const Board *b) {
	return Generator_CountSolutions(b, 2) == 1;
}

/* digging out values from previously generated solved puzzle */
GeneratorResult Generator_CreatePuzzle(Board *b, Difficulty diff, GeneratorFlags flags) {
	GeneratorResult result = { 0 };
	if (!Generator_FillGrid(b)) return result;

	bool check = (flags & GEN_FLAG_UNIQUE) != 0;
	static const int base[] = { 40, 32, 28, 24, 20 }, range[] = { 6, 7, 5, 5, 5 };
	int target = (diff <= DIFFICULTY_EXPERT) ? base[diff] + rand() % range[diff] : 35;
	int clues = 81,
	    check_freq
		= (diff <= DIFFICULTY_MEDIUM) ? (diff == DIFFICULTY_EASY ? 5 : 3) : 1;

	int pos[81];
	for (int i = 0; i < 81; i++)
		pos[i] = i;
	for (int i = 80; i > 0; i--) {
		int j = rand() % (i + 1);
		int tmp = pos[i];
		pos[i] = pos[j];
		pos[j] = tmp;
	}

	/* two phase removal: course then fine */
	int idx = 0, thresh = target + (diff <= DIFFICULTY_MEDIUM ? 5 : 8);
	int since_check = 0, agg_freq = (diff <= DIFFICULTY_MEDIUM) ? 3 : 1;

	/* phase 1: course */
	while (clues > thresh && idx < 81) {
		int r = pos[idx] / 9, c = pos[idx] % 9;
		idx++;
		if (!b->cells[r][c].value) continue;
		uint8_t saved = b->cells[r][c].value;
		b->cells[r][c].value = 0;
		b->cells[r][c].given = false;
		clues--;
		result.attempts++;
		since_check++;
		if (check && since_check >= agg_freq) {
			if (!Generator_HasUniqueSolution(b)) {
				b->cells[r][c].value = saved;
				b->cells[r][c].given = true;
				clues++;
				result.attempts--;
				break;
			}
			since_check = 0;
		}
	}

	/* phase 2: fine */
	int fails = 0, careful_check = 0;
	for (int i = idx; i < 81 && clues > target; i++) {
		int r = pos[i] / 9, c = pos[i] % 9;
		if (!b->cells[r][c].value) continue;
		result.attempts++;
		careful_check++;
		uint8_t saved = b->cells[r][c].value;
		b->cells[r][c].value = 0;
		b->cells[r][c].given = false;
		bool ok = true;
		if (check && careful_check >= check_freq) {
			ok = Generator_HasUniqueSolution(b);
			careful_check = 0;
		}
		if (ok) {
			clues--;
			fails = 0;
		}
		else {
			b->cells[r][c].value = saved;
			b->cells[r][c].given = true;
			if (++fails >= 10) break;
		}
	}

	result.success = true;
	result.clues = clues;
	result.unique = check && Generator_HasUniqueSolution(b);
	return result;
}
