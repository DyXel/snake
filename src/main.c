/*
 * Copyright (c) 2022, Dylam De La Torre <dyxel04@gmail.com>
 *
 * SPDX-License-Identifier: Zlib
 */
#include <stdlib.h>
#include <time.h>

#ifdef __EMSCRIPTEN__
/* NOTE: Remember to compile with `-s USE_SDL=2` if using emscripten. */
#include <emscripten.h>
#include <SDL2/SDL.h>
#define SDL_EVENT_POLLING_FUNCTION(e) SDL_PollEvent((e))
#else
#include <SDL.h>
#define SDL_EVENT_POLLING_FUNCTION(e) SDL_WaitEvent((e))
#endif /* __EMSCRIPTEN__ */

#include "snake.h"

#define STEP_RATE_IN_MILLISECONDS 125
#define SNAKE_BLOCK_SIZE_IN_PIXELS 24
#define SDL_WINDOW_WIDTH (SNAKE_BLOCK_SIZE_IN_PIXELS * SNAKE_GAME_WIDTH)
#define SDL_WINDOW_HEIGHT (SNAKE_BLOCK_SIZE_IN_PIXELS * SNAKE_GAME_HEIGHT)

typedef struct
{
	SDL_Renderer* renderer;
	SnakeContext snake_ctx;
}MainLoopPayload;

static Uint32 sdl_timer_callback_(Uint32 interval, void* payload)
{
	SDL_Event e;
	SDL_UserEvent ue;
	/* NOTE: snake_step is not directly called here for
	 * multithreaded concerns.
	 */
	(void)payload;
	ue.type = SDL_USEREVENT;
	ue.code = 0;
	ue.data1 = NULL;
	ue.data2 = NULL;
	e.type = SDL_USEREVENT;
	e.user = ue;
	SDL_PushEvent(&e);
	return interval;
}

static int handle_key_event_(SnakeContext* ctx, SDL_Scancode key_code)
{
	switch(key_code)
	{
	/* Quit. */
	case SDL_SCANCODE_ESCAPE:
	case SDL_SCANCODE_Q:
#ifdef __EMSCRIPTEN__
		emscripten_cancel_main_loop();
#endif /* __EMSCRIPTEN__ */
		return 0;
	/* Restart the game as if the program was launched. */
	case SDL_SCANCODE_R: snake_initialize(ctx); break;
	/* Decide new direction of the snake. */
	case SDL_SCANCODE_RIGHT: snake_redir(ctx, SNAKE_DIR_RIGHT); break;
	case SDL_SCANCODE_UP: snake_redir(ctx, SNAKE_DIR_UP); break;
	case SDL_SCANCODE_LEFT: snake_redir(ctx, SNAKE_DIR_LEFT); break;
	case SDL_SCANCODE_DOWN: snake_redir(ctx, SNAKE_DIR_DOWN); break;
	default: break;
	}
	return 1;
}

static void set_rect_xy_(SDL_Rect* r, short x, short y)
{
	r->x = x * SNAKE_BLOCK_SIZE_IN_PIXELS;
	r->y = y * SNAKE_BLOCK_SIZE_IN_PIXELS;
}

static void draw_scene_(SDL_Renderer* renderer, SnakeContext* ctx)
{
	SDL_Rect r;
	unsigned i;
	unsigned j;
	int ct;
	r.w = r.h = SNAKE_BLOCK_SIZE_IN_PIXELS;
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);
	for(i = 0; i < SNAKE_GAME_WIDTH; i++)
	{
		for(j = 0; j < SNAKE_GAME_HEIGHT; j++)
		{
			set_rect_xy_(&r, i, j);
			ct = snake_cell_at(ctx, i, j);
			if(ct == SNAKE_CELL_NOTHING)
				continue;
			if(ct == SNAKE_CELL_FOOD)
				SDL_SetRenderDrawColor(renderer, 0, 0, 128, 255);
			else /* body */
				SDL_SetRenderDrawColor(renderer, 0, 128, 0, 255);
			SDL_RenderFillRect(renderer, &r);
		}
	}
	SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255); /*head*/
	set_rect_xy_(&r, ctx->head_xpos, ctx->head_ypos);
	SDL_RenderFillRect(renderer, &r);
	SDL_RenderPresent(renderer);
}

static int main_loop_(MainLoopPayload* payload)
{
	SDL_Event e;
	SnakeContext* ctx = &payload->snake_ctx;
	while(SDL_EVENT_POLLING_FUNCTION(&e))
	{
		switch(e.type)
		{
		case SDL_QUIT:
#ifdef __EMSCRIPTEN__
			emscripten_cancel_main_loop();
#endif /* __EMSCRIPTEN__ */
			return 0;
		case SDL_USEREVENT:
			snake_step(ctx);
			draw_scene_(payload->renderer, ctx);
			break;
		case SDL_KEYDOWN: return handle_key_event_(ctx, e.key.keysym.scancode);
		}
	}
	return 1;
}

#ifdef __EMSCRIPTEN__
void emscripten_main_loop_(void* payload)
{
	main_loop_((MainLoopPayload*)payload);
}
#endif /* __EMSCRIPTEN__ */

int main(int argc, char* argv[])
{
	int exit_value;
	SDL_Window* window;
	MainLoopPayload loop_payload;
	SDL_TimerID step_timer;
	(void)argc;
	(void)argv;
	exit_value = 0;
	window = NULL;
	loop_payload.renderer = NULL;
	step_timer = 0;
	srand(time(0));
	if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0)
	{
		exit_value = 1;
		goto quit;
	}
	SDL_SetHint("SDL_VIDEO_X11_NET_WM_BYPASS_COMPOSITOR", "0");
	window = SDL_CreateWindow(
		"Snake",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOW_WIDTH,
		SDL_WINDOW_HEIGHT,
		SDL_WINDOW_SHOWN);
	if(window == NULL)
	{
		exit_value = 2;
		goto quit;
	}
	loop_payload.renderer = SDL_CreateRenderer(
		window,
		-1,
		SDL_RENDERER_ACCELERATED);
	if(loop_payload.renderer == NULL)
	{
		exit_value = 3;
		goto quit;
	}
	snake_initialize(&loop_payload.snake_ctx);
	step_timer = SDL_AddTimer(
		STEP_RATE_IN_MILLISECONDS,
		sdl_timer_callback_,
		NULL);
	if(step_timer == 0)
	{
		exit_value = 4;
		goto quit;
	}
#ifdef __EMSCRIPTEN__
	emscripten_set_main_loop_arg(emscripten_main_loop_, &loop_payload, -1, 1);
#else
	while(main_loop_(&loop_payload)) {}
#endif /* __EMSCRIPTEN__ */
quit:
	if(exit_value > 0)
		fprintf(stderr, "%s", SDL_GetError());
	SDL_RemoveTimer(step_timer);
	SDL_DestroyRenderer(loop_payload.renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return exit_value;
}
