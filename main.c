#include <stdio.h>
#include <SDL2/SDL.h>
#include "./constants.h"

int game_is_running = FALSE;
int last_frame_time = 0;
SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;

struct move {
    int up;
    int down;
} p1move, p2move;

struct rectangle {
    float x;
    float y;
    float w;
    float h;
} p1body, p2body;

int initialize_window(void) {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        fprintf(stderr, "Error intializing SDL.\n");
        return FALSE;
    }
    window = SDL_CreateWindow(
        NULL,                   // window title
        SDL_WINDOWPOS_CENTERED, // window position x
        SDL_WINDOWPOS_CENTERED, // window position y
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        SDL_WINDOW_BORDERLESS);
    if (!window) {
        fprintf(stderr, "Error creating SDL Window.\n");
        return FALSE;
    }
    renderer = SDL_CreateRenderer(
        window, // window to attach the render
        -1,     // driver code, -1 for default
        0       // special flags
    );
    if (!renderer) {
        fprintf(stderr, "Error creating SDL Renderer.\n");
        return FALSE;
    }

    return TRUE;
}

void process_input() {
    SDL_Event event;
    SDL_PollEvent(&event);
    switch (event.type) {
    case SDL_QUIT:
        game_is_running = FALSE;
        break;
    case SDL_KEYDOWN:
        switch (event.key.keysym.sym) {
        case SDLK_ESCAPE:
            game_is_running = FALSE;
            break;
        case SDLK_w:
            p1move.up = TRUE;
            break;
        case SDLK_s:
            p1move.down = TRUE;
            break;
        case SDLK_k:
            p2move.up = TRUE;
            break;
        case SDLK_j:
            p2move.down = TRUE;
            break;
        }
        break;
    case SDL_KEYUP:
        switch (event.key.keysym.sym) {
        case SDLK_w:
            p1move.up = FALSE;
            break;
        case SDLK_s:
            p1move.down = FALSE;
            break;
        case SDLK_k:
            p2move.up = FALSE;
            break;
        case SDLK_j:
            p2move.down = FALSE;
            break;
        }
        break;
    default:
        break;
    }
}

void update() {
    // logic to keep a fixed timestamp
    // must waste some time until we reach the target time
    int time_to_wait = FRAME_TARGET_TIME - (SDL_GetTicks() - last_frame_time);
    if (time_to_wait > 0 && time_to_wait <= FRAME_TARGET_TIME)
        SDL_Delay(time_to_wait);
    // get a delta time factor to update objects
    float delta_time = (SDL_GetTicks() - last_frame_time) / 1000.0f;
    last_frame_time = SDL_GetTicks(); // time since game began

    if (p1move.up)
        p1body.y -= 100 * delta_time;
    if (p1move.down)
        p1body.y += 100 * delta_time;
    if (p2move.up)
        p2body.y -= 100 * delta_time;
    if (p2move.down)
        p2body.y += 100 * delta_time;
}

void setup() {
    p1body.x = 20;
    p1body.y = 20;
    p1body.w = 30;
    p1body.h = 100;
    p2body.x = 750;
    p2body.y = 20;
    p2body.w = 30;
    p2body.h = 100;
    p1move.up = FALSE;
    p1move.down = FALSE;
    p2move.up = FALSE;
    p2move.down = FALSE;
}

void render() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    SDL_Rect p1render = {
        (int)p1body.x,
        (int)p1body.y,
        (int)p1body.w,
        (int)p1body.h
    };

    SDL_Rect p2render = {
        (int)p2body.x,
        (int)p2body.y,
        (int)p2body.w,
        (int)p2body.h
    };

    SDL_SetRenderDrawColor(renderer, 150, 20, 20, 255);
    SDL_RenderFillRect(renderer, &p1render);
    SDL_RenderFillRect(renderer, &p2render);

    SDL_RenderPresent(renderer); // swaps back buffer with front buffer
}

void destroy_window() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

int main() {
    game_is_running = initialize_window();

    setup();

    while (game_is_running)
    {
        process_input();
        update();
        render();
    }

    destroy_window();

    return 0;
}