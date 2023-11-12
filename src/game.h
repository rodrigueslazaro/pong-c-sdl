#include "./constants.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

struct game {
    int is_running;
    int is_active;
    float ball_multiplier;
    int last_frame_time;
    int game_state;
    const char *message;
    TTF_Font *font;
    SDL_Window *window;
    SDL_Renderer *renderer;
};

struct game newGame() {
    struct game newG;
    newG.ball_multiplier = 1;
    newG.last_frame_time = 0;
    newG.game_state = 0;
    newG.message = "";
    newG.is_active = FALSE;
    newG.font = NULL;
    newG.window = NULL;
    newG.renderer = NULL;
    return newG;
};