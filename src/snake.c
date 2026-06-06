#include "raylib.h"
#include "snake.h"
#include "stdlib.h"
#include "stdbool.h"
#include "time.h"
#include "math.h"
#define TARGET_FPS 60
#define TIME_DELAY 0.1f
#define RECT_WIDTH 20
#define RECT_HEIGHT 20
#define GRID_HEIGHT 20
#define GRID_WIDTH 20
#define SCREEN_WIDTH (GRID_WIDTH * 50)
#define SCREEN_HEIGHT (GRID_HEIGHT * 25)
#define WB_OFFSET (GRID_WIDTH * 2)    // WB = World Boundary
#define WB_WEIGHT GRID_WIDTH
#define WB_WIDTH (SCREEN_WIDTH - 2 * WB_OFFSET + WB_WEIGHT) 
#define WB_HEIGHT (SCREEN_HEIGHT - 2 * WB_OFFSET + WB_WEIGHT)
#define PLAY_AREA_WIDTH (WB_WIDTH - 2 * WB_WEIGHT)
#define PLAY_AREA_HEIGHT (WB_HEIGHT - 2 * WB_WEIGHT)
#define NUM_HORIZONTAL_GRIDS (int)(PLAY_AREA_WIDTH / GRID_WIDTH)
#define NUM_VERTICAL_GRIDS (int)(PLAY_AREA_HEIGHT / GRID_HEIGHT)


void UpdateHeadLocation(Snake *s) {
	s->head.prev_position = s->head.position;
	if (s->head.direction == RIGHT) {
		s->head.position.x += GRID_WIDTH;
	}
	else if (s->head.direction == LEFT) {
		s->head.position.x -= GRID_WIDTH;
	}
	else if (s->head.direction == DOWN) {
		s->head.position.y += GRID_HEIGHT;
	}
	else if (s->head.direction == UP) {
		s->head.position.y -= GRID_HEIGHT;
	}
	else {
		printf("Unknown Error.");
	}
}

// This feels like bad design. Refactor later.
void UpdateBodyLocation(Snake *s) {
	Vector2 temp;
	Vector2 next_segment_position = s->head.prev_position;
	for (int i = 0; i < s->snake_length - 1; i++) {
		temp = s->body[i].position;
		s->body[i].position = next_segment_position;
		next_segment_position = temp;
	}
}

// Need to test this new function!
// Might need to add initial position.
// Update snake length to be the desired length after adding new segment.
void AddNewSegment(Snake *s) {
	int tail_idx = s->snake_length - 2;  // If length = 2. First segment is head. Then body at idx 0.
	s->body[tail_idx].segment.width = RECT_WIDTH;
	s->body[tail_idx].segment.height = RECT_HEIGHT;
}

void RepositionFood(Food* food) {
	int num_grid_units_w = (rand() % NUM_HORIZONTAL_GRIDS);
	int num_grid_units_h = (rand() % NUM_VERTICAL_GRIDS);
	food->food_rect.x = (WB_OFFSET + WB_WEIGHT) + num_grid_units_w * (GRID_WIDTH);
	food->food_rect.y = (WB_OFFSET + WB_WEIGHT) + num_grid_units_h * (GRID_HEIGHT);
}

void MoveSnake(Snake* s) {
	// Move head.
	s->head.segment.x = s->head.position.x;
	s->head.segment.y = s->head.position.y;

	// Move body.
	for (int i = 0; i < s->snake_length; i++) {
		s->body[i].segment.x = s->body[i].position.x;
		s->body[i].segment.y = s->body[i].position.y;
	}
}

GameState *GameInit(void) {
	GameState *game_state = (GameState *)malloc(sizeof(GameState));
	if (!game_state) exit(EXIT_FAILURE);
	RepositionFood(&game_state->food);
	//int num_grid_units_w = rand() % (WB_WIDTH / (GRID_WIDTH - 1));
	//int num_grid_units_h = rand() % (WB_HEIGHT / (GRID_HEIGHT - 1));
	//Food f = {
	//	RECT_WIDTH,
	//	RECT_HEIGHT,
	//};
	game_state->food.food_rect.width = RECT_WIDTH;
	game_state->food.food_rect.height = RECT_HEIGHT;
	WorldBoundary wb = {
		{WB_OFFSET, WB_OFFSET, WB_WIDTH, WB_WEIGHT},
		{WB_OFFSET, SCREEN_HEIGHT - WB_OFFSET, WB_WIDTH, WB_WEIGHT},
		{WB_OFFSET, WB_OFFSET, WB_WEIGHT, WB_HEIGHT},
		{SCREEN_WIDTH - WB_OFFSET, WB_OFFSET, WB_WEIGHT, WB_HEIGHT}
	};
	game_state->world_boundary = wb;
	Snake s;
	s.snake_length = 1;
	int halfway_down = (int)floor(NUM_VERTICAL_GRIDS / 2);
	Vector2 starting_pos = {
		WB_OFFSET + WB_WEIGHT,
		WB_OFFSET + WB_WEIGHT + halfway_down * GRID_HEIGHT
	};
	s.head.position = starting_pos;
	s.head.segment = (Rectangle){
		starting_pos.x,
		starting_pos.y,
		RECT_WIDTH,
		RECT_HEIGHT
	};
	game_state->snake = s; 	
	game_state->is_game_over = false;
	game_state->grow_longer = false;
	game_state->is_level_complete = false;
	game_state->snake.head.direction = RIGHT;
	game_state->target_FPS = TARGET_FPS;
	game_state->move_timer = 0.0f;
	game_state->move_delay = TIME_DELAY;
	InitWindow(SCREEN_WIDTH , SCREEN_HEIGHT, "SSSSSnake");
	SetTargetFPS(game_state->target_FPS);
	return game_state;
}

GameState *GameUpdate(GameState *gs){
	if (IsKeyPressed(KEY_DOWN) && (gs->snake.head.direction != UP)) {
		gs->snake.head.direction = DOWN;
		
	} else if (IsKeyPressed(KEY_UP) && (gs->snake.head.direction != DOWN)) {
		gs->snake.head.direction = UP;
		
	} else if (IsKeyPressed(KEY_LEFT) && (gs->snake.head.direction != RIGHT)) {
		gs->snake.head.direction = LEFT;
		
	} else if (IsKeyPressed(KEY_RIGHT) && (gs->snake.head.direction != LEFT)) {
		gs->snake.head.direction = RIGHT;
	}

	gs->move_timer += GetFrameTime();

	if (gs->move_timer >= gs->move_delay) {
		UpdateHeadLocation(&gs->snake);
		UpdateBodyLocation(&gs->snake);
		MoveSnake(&gs->snake);
		gs->move_timer = 0.0f;
	}

	bool collision_with_wb = CheckCollisionRecs(gs->snake.head.segment, gs->world_boundary.wb_top) || \
							 CheckCollisionRecs(gs->snake.head.segment, gs->world_boundary.wb_bottom) || \
							 CheckCollisionRecs(gs->snake.head.segment, gs->world_boundary.wb_left) || \
							 CheckCollisionRecs(gs->snake.head.segment, gs->world_boundary.wb_right);

	bool collision_with_body = false;
	for (int i = 0; i < gs->snake.snake_length - 1; i++) {
		if (CheckCollisionRecs(gs->snake.head.segment, gs->snake.body[i].segment)) {
			collision_with_body = true;
		}
	}

	if (collision_with_wb || collision_with_body) {
		gs->is_game_over = true;
	}

	bool found_food = CheckCollisionRecs(gs->snake.head.segment, gs->food.food_rect);
	if (found_food) {
		gs->grow_longer = true;
		RepositionFood(&gs->food);
	}

	// Grow snake after next motion of snake. 
	// This way, No chance of accidental collisions with wb.
	if (gs->grow_longer) { 
		//ClearBackground((Color){ 240, 255, 255, 255});
		gs->snake.snake_length++;
		AddNewSegment(&gs->snake);
		//gs->snake.snake[gs->snake.snake_length - 1].segment
		if (gs->snake.snake_length == 15) gs->is_level_complete = true;
		gs->grow_longer = false;
	}

	return gs;
}

void GameDraw(GameState *gs) {
	BeginDrawing();
		if (gs->is_game_over) {
			ClearBackground(BLACK);
			DrawText("GAME OVER", SCREEN_WIDTH / 3, SCREEN_HEIGHT / 3, 32, RED);
		}
		else if (gs->is_level_complete) {
			ClearBackground(BLACK);
			DrawText("LEVEL COMPLETE", SCREEN_WIDTH / 3, SCREEN_HEIGHT / 3, 32, GREEN);
		}
		// Add new segment in location just before last motion of last segment in snake.
		// Increment count of snake.
		else {
			Color custom_color = { 240, 255, 255, 255 };
			ClearBackground(custom_color);
			DrawRectangle(
				gs->snake.head.segment.x, 
				gs->snake.head.segment.y,
				gs->snake.head.segment.width, 
				gs->snake.head.segment.height,
				MAROON);
			for (int i = 0; i < gs->snake.snake_length; i++) {
				DrawRectangle(
					gs->snake.body[i].segment.x, 
					gs->snake.body[i].segment.y,
					gs->snake.body[i].segment.width, 
					gs->snake.body[i].segment.height,
					MAROON);
			}
			DrawRectangle(
				gs->food.food_rect.x,
				gs->food.food_rect.y,
				gs->food.food_rect.width,
				gs->food.food_rect.height,
				DARKGREEN
			);
			DrawRectangle(
				gs->world_boundary.wb_top.x,
				gs->world_boundary.wb_top.y,
				gs->world_boundary.wb_top.width,
				gs->world_boundary.wb_top.height,
				DARKBLUE);
			DrawRectangle(
				gs->world_boundary.wb_bottom.x,
				gs->world_boundary.wb_bottom.y,
				gs->world_boundary.wb_bottom.width,
				gs->world_boundary.wb_bottom.height,
				DARKBLUE);
			DrawRectangle(
				gs->world_boundary.wb_left.x,
				gs->world_boundary.wb_left.y,
				gs->world_boundary.wb_left.width,
				gs->world_boundary.wb_left.height,
				DARKBLUE);
			DrawRectangle(
				gs->world_boundary.wb_right.x,
				gs->world_boundary.wb_right.y,
				gs->world_boundary.wb_right.width,
				gs->world_boundary.wb_right.height,
				DARKBLUE);
		}


	EndDrawing();
			
}
void GameClose(void) {
	CloseWindow();
}
