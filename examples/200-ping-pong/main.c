#include <X11/Xutil.h>
#include <X11/XKBlib.h>
#include <X11/keysym.h>
#include <spawn.h>

#define  GLUTIL_IMPLEMENTATION
#include "../../glutil.h"

#define WINDOW_WIDTH     1280
#define WINDOW_HEIGHT     720

#define BALL_SIZE        20.0f
#define BALL_SPEED       420.69f
#define BALL_SPEEDUP     1.05f

#define PADDLE_SPEED     (BALL_SPEED * 1.2f)
#define PADDLE_SPEEDUP   1.03f
#define PADDLE_LENGTH    (6.0f * BALL_SIZE)
#define PADDLE_PADDING   (4.0f * BALL_SIZE)
#define PADDLE_THICKNESS BALL_SIZE

#define NET_COUNT        51
#define NET_GIRTH        3.0f
#define DIGIT_SIZE       BALL_SIZE

typedef struct {
	float        ball_x, ball_y, ball_dx, ball_dy;
	float        ball_speed, paddle_speed;
	float        paddle_left_y, paddle_right_y;
	unsigned int score_left, score_right;
} Game;

typedef struct {
	bool left_up, left_down, right_up, right_down;
} Controls;

typedef enum { SOUND_HIT, SOUND_SCORE, SOUND_SPECIAL, SOUND_ANOTHER_SPECIAL } Sound;

static const unsigned char digits[10][5][3] = {
    {{1, 1, 1}, {1, 0, 1}, {1, 0, 1}, {1, 0, 1}, {1, 1, 1}},
    {{0, 1, 0}, {1, 1, 0}, {0, 1, 0}, {0, 1, 0}, {1, 1, 1}},
    {{1, 1, 1}, {0, 0, 1}, {1, 1, 1}, {1, 0, 0}, {1, 1, 1}},
    {{1, 1, 1}, {0, 0, 1}, {1, 1, 1}, {0, 0, 1}, {1, 1, 1}},
    {{1, 0, 1}, {1, 0, 1}, {1, 1, 1}, {0, 0, 1}, {0, 0, 1}},
    {{1, 1, 1}, {1, 0, 0}, {1, 1, 1}, {0, 0, 1}, {1, 1, 1}},
    {{1, 1, 1}, {1, 0, 0}, {1, 1, 1}, {1, 0, 1}, {1, 1, 1}},
    {{1, 1, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}},
    {{1, 1, 1}, {1, 0, 1}, {1, 1, 1}, {1, 0, 1}, {1, 1, 1}},
    {{1, 1, 1}, {1, 0, 1}, {1, 1, 1}, {0, 0, 1}, {1, 1, 1}},
};

static const Color foreground = {255, 255, 255, 255};
static const Color background = {0x20, 0x1e, 0x18, 255};

static bool rects_collide(float x1, float y1, float w1, float h1, 
						  float x2, float y2, float w2, float h2)
{
	return !(x1 + w1 < x2 || x2 + w2 < x1 ||
			 y1 + h1 < y2 || y2 + h2 < y1);
}

static void center_ball(Game *game)
{
	game->ball_x = (WINDOW_WIDTH - BALL_SIZE) * 0.5f;
	game->ball_y = (WINDOW_HEIGHT - BALL_SIZE) * 0.5f;
}

static void center_paddle(Game *game)
{
	game->paddle_left_y = game->paddle_right_y = (WINDOW_HEIGHT - PADDLE_LENGTH) * 0.5f;
}

extern char **environ;
static void play_sound(Sound sound)
{
	const char *path;
	switch (sound) {
	case SOUND_HIT: path             = "examples/200-ping-pong/assets/ball-hit-sound.mp3";        break; // https://www.myinstants.com/en/instant/fart/
	case SOUND_SCORE: path           = "examples/200-ping-pong/assets/score-sound.mp3";           break; // https://www.myinstants.com/en/instant/gol-galvao-54786/
	case SOUND_SPECIAL: path         = "examples/200-ping-pong/assets/special-sound.mp3";         break; // https://www.myinstants.com/en/instant/faz-o-l-vinheta-77929/
	case SOUND_ANOTHER_SPECIAL: path = "examples/200-ping-pong/assets/another-special-sound.mp3"; break; // https://www.myinstants.com/en/instant/bolsonaro-pegaram-meu-telefone-55053/
	default: return;
	}
	char *const args[] = {"paplay", (char *)path, NULL};
	pid_t child;
	posix_spawnp(&child, "paplay", NULL, NULL, args, environ);
}

static void play_score_sound(const Game *game)
{
	unsigned int total = game->score_left + game->score_right;
	if      (total != 0x0Du && total != 0x16u) play_sound(SOUND_SCORE);
	else if (total == 0x0Du && total != 0x16u) play_sound(SOUND_SPECIAL);
	else if (total == 0x16u && total != 0x0Du) play_sound(SOUND_ANOTHER_SPECIAL);
}

static void move_paddle(Game *game, float *paddle_y, float paddle_x,
			            float direction, float dt)
{
	float next_y = *paddle_y + direction * game->paddle_speed * dt;
	if (next_y > 0.0f
		&& next_y + PADDLE_LENGTH < WINDOW_HEIGHT
		&& !rects_collide(game->ball_x, game->ball_y, BALL_SIZE, BALL_SIZE, paddle_x, next_y, PADDLE_THICKNESS, PADDLE_LENGTH)) {
		*paddle_y = next_y;
	}
}

static void update_game(Game *game, float dt, Controls *controls)
{
	float left_x  = PADDLE_PADDING;
	float right_x = WINDOW_WIDTH - PADDLE_PADDING - PADDLE_THICKNESS;
	float next_x  = game->ball_x + game->ball_dx * game->ball_speed * dt;

	if ((game->ball_dx < 0.0f    && rects_collide(next_x, game->ball_y, BALL_SIZE, BALL_SIZE, left_x, game->paddle_left_y, PADDLE_THICKNESS, PADDLE_LENGTH))
		|| (game->ball_dx > 0.0f && rects_collide(next_x, game->ball_y, BALL_SIZE, BALL_SIZE, right_x, game->paddle_right_y, PADDLE_THICKNESS, PADDLE_LENGTH))) {
		game->ball_dx       = -game->ball_dx;
		game->ball_speed   *= BALL_SPEEDUP;
		game->paddle_speed *= PADDLE_SPEEDUP;
		play_sound(SOUND_HIT);
	} else if (next_x < 0.0f) {
		++game->score_right;
		center_ball  (game);
		center_paddle(game);
		game->ball_dx      = game->ball_dy = 1.0f;
		game->ball_speed   = BALL_SPEED;
		game->paddle_speed = PADDLE_SPEED;
		play_score_sound(game);
	} else if (next_x + BALL_SIZE >= WINDOW_WIDTH) {
		++game->score_left;
		center_ball  (game);
		center_paddle(game);
		game->ball_dx      = -1.0f;
		game->ball_dy      = 1.0f;
		game->ball_speed   = BALL_SPEED;
		game->paddle_speed = PADDLE_SPEED;
		play_score_sound(game);
	} else {
		game->ball_x = next_x;
	}

	float next_y = game->ball_y + game->ball_dy * game->ball_speed * dt;
	if (next_y < 0.0f 
		|| next_y + BALL_SIZE >= WINDOW_HEIGHT
		|| rects_collide(game->ball_x, next_y, BALL_SIZE, BALL_SIZE, left_x,  game->paddle_left_y,  PADDLE_THICKNESS, PADDLE_LENGTH)
		|| rects_collide(game->ball_x, next_y, BALL_SIZE, BALL_SIZE, right_x, game->paddle_right_y, PADDLE_THICKNESS, PADDLE_LENGTH))
	{
		game->ball_dy = -game->ball_dy;
		play_sound(SOUND_HIT);
	} else {
		game->ball_y = next_y;
	}

	if (controls->left_up)    move_paddle(game, &game->paddle_left_y,  left_x,  -1.0f, dt);
	if (controls->left_down)  move_paddle(game, &game->paddle_left_y,  left_x,   1.0f, dt);
	if (controls->right_up)   move_paddle(game, &game->paddle_right_y, right_x, -1.0f, dt);
	if (controls->right_down) move_paddle(game, &game->paddle_right_y, right_x,  1.0f, dt);
}

static unsigned int digit_count(unsigned int number)
{
	unsigned int count = 1;
	while (number >= 10) {
		number /= 10;
		++count;
	}
	return count;
}

static void draw_digit(unsigned int d, float x, float y)
{
	for (int r = 0; r < 5; ++r) {
		for (int c = 0; c < 3; ++c) {
			if (digits[d][r][c]) {
				rect(
					(Vec2f) {
						x + c * DIGIT_SIZE,
						y + r * DIGIT_SIZE
					},
					DIGIT_SIZE,
					DIGIT_SIZE,
					foreground
				);
			}
		}
	}
}

static void draw_number(unsigned int number, float x, float y)
{
	x += (digit_count(number) - 1) * 4.0f * DIGIT_SIZE;
	do {
		draw_digit(number % 10, x, y);
		number /= 10;
		x      -= 4.0f * DIGIT_SIZE;
	} while (number);
}

static void draw_game(const Game *game)
{
	clear_bg(background);

	float segment_length = (float)WINDOW_HEIGHT / NET_COUNT;
	for (int i = 0; i < NET_COUNT; i += 2) {
		float x = WINDOW_WIDTH * 0.5f;
		line(
			(Vec2f) {
				x,
				i       * segment_length
			},
			(Vec2f) { 
				x,
				(i + 1) * segment_length
			},
			foreground,
			NET_GIRTH
		);
	}

	rect(
		(Vec2f) {
			game->ball_x,
			game->ball_y
		}, 
		BALL_SIZE,
		BALL_SIZE,
		foreground
	);
	rect(
		(Vec2f) {
			PADDLE_PADDING,
			game->paddle_left_y
		}, 
		PADDLE_THICKNESS,
		PADDLE_LENGTH,
		foreground
	);
	rect(
		(Vec2f) {
			WINDOW_WIDTH - PADDLE_PADDING - PADDLE_THICKNESS,
			game->paddle_right_y
		}, 
		PADDLE_THICKNESS,
		PADDLE_LENGTH,
		foreground
	);

	unsigned int count      = digit_count(game->score_left);
	float        left_width = count * 3.0f * DIGIT_SIZE + (count - 1) * DIGIT_SIZE;
	draw_number(game->score_left,  WINDOW_WIDTH * 0.5f - left_width - 2.0f * DIGIT_SIZE, 0.0f);
	draw_number(game->score_right, WINDOW_WIDTH * 0.5f + 2.0f * DIGIT_SIZE,              0.0f);
}

int main(void)
{
	GLutil glutil = {0};
	init          (&glutil, WINDOW_WIDTH, WINDOW_HEIGHT);
	set_target_fps(&glutil, 60.0f);

	XStoreName    (glutil.display, glutil.window, "Ping Pong");

	Game game           = {
		.ball_dx        = -1.0f,
		.ball_dy        = 1.0f,
		.ball_speed     = BALL_SPEED,
		.paddle_speed   = PADDLE_SPEED,
	};
	center_ball  (&game);
	center_paddle(&game);

	Controls controls = {0};
	bool     running  = true;
	while (running) {
		while (XPending(glutil.display)) {
			XEvent event;
			XNextEvent(glutil.display, &event);

			if (event.type == KeyPress || event.type == KeyRelease) {
				bool   pressed = event.type == KeyPress;
				KeySym key     = XLookupKeysym(&event.xkey, 0);
				switch (key) {
				case XK_w: controls.left_up       = pressed; break;
				case XK_s: controls.left_down     = pressed; break;
				case XK_Up: controls.right_up     = pressed; break;
				case XK_Down: controls.right_down = pressed; break;
				case XK_k: controls.right_up      = pressed; break;
				case XK_j: controls.right_down    = pressed; break;
				case XK_Escape: case XK_q: if (pressed) running   = false; break;
				}
			}
		}
		if (!running) break;

		uint64_t dt_nanos = begin_frame(&glutil);
		{
			float dt = glutil.prev_t_nanos ? (float)dt_nanos / 1000000000.0f : 0.0f;
			if (dt > 0.05f) dt = 0.05f;
			update_game(&game, dt, &controls);
			draw_game  (&game);
		}
		end_frame(&glutil);
	}

	destroy(&glutil);
	return 0;
}
