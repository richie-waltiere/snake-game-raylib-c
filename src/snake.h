// snake.h : Include file for standard system include files,
// or project specific include files.

#include "stdbool.h"

#pragma once

typedef enum {
	RIGHT,
	LEFT,
	UP,
	DOWN
} HeadDirection;

typedef struct {
	Rectangle wb_top;
	Rectangle wb_bottom;
	Rectangle wb_left;
	Rectangle wb_right;
} WorldBoundary;

typedef struct {
	HeadDirection direction;
	Vector2 position;
	Vector2 prev_position;
	Rectangle segment;
} SnakeHead;

typedef struct {
	Vector2 position;
	Rectangle segment;
} SnakeBody;

typedef struct {
	int snake_length;
	SnakeHead head;
	SnakeBody body[14];
} Snake;

typedef struct {
	Rectangle food_rect;
} Food;


typedef struct {
	Snake snake;
	Food food;
	WorldBoundary world_boundary;
	int target_FPS;
	float move_timer;
	float move_delay;
	bool grow_longer;
	bool is_level_complete;
	bool is_game_over;
} GameState;

GameState *GameInit(void);
GameState *GameUpdate(GameState *gs);
void GameDraw(GameState *gs);
void GameClose(void);