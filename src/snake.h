/*
 * Copyright (c) 2023, Dylam De La Torre <dyxel04@gmail.com>
 *
 * SPDX-License-Identifier: Zlib
 */
#ifndef SNAKE_H
#define SNAKE_H
#define SNAKE_GAME_WIDTH 24U
#define SNAKE_GAME_HEIGHT 18U
#define SNAKE_MATRIX_SIZE (SNAKE_GAME_WIDTH * SNAKE_GAME_HEIGHT)

typedef enum
{
	SNAKE_CELL_NOTHING = 0U,
	SNAKE_CELL_SRIGHT = 1U,
	SNAKE_CELL_SUP = 2U,
	SNAKE_CELL_SLEFT = 3U,
	SNAKE_CELL_SDOWN = 4U,
	SNAKE_CELL_FOOD = 5U
}SnakeCell;

#define SNAKE_CELL_MAX_BITS 3U /* floor(log2(SNAKE_CELL_FOOD)) + 1 */

typedef enum
{
	SNAKE_DIR_RIGHT,
	SNAKE_DIR_UP,
	SNAKE_DIR_LEFT,
	SNAKE_DIR_DOWN
}SnakeDirection;

typedef struct
{
	unsigned char cells[(SNAKE_MATRIX_SIZE * SNAKE_CELL_MAX_BITS) / 8U];
	char head_xpos;
	char head_ypos;
	char tail_xpos;
	char tail_ypos;
	char next_dir;
	char inhibit_tail_step;
	unsigned occupied_cells;
}SnakeContext;

void snake_initialize(SnakeContext* ctx);
void snake_redir(SnakeContext* ctx, SnakeDirection dir);
void snake_step(SnakeContext* ctx);
SnakeCell snake_cell_at(const SnakeContext* ctx, char x, char y);

#endif /* SNAKE_H */
