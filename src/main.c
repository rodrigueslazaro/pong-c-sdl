#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "./constants.h"
#include "./game.h"
#include "./player.h"
#include "./ball.h"

int initialize_window(struct game *game) {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        fprintf(stderr, "Error intializing SDL.\n");
        return FALSE;
    }
    game->window = SDL_CreateWindow(
        NULL,                   // window title
        SDL_WINDOWPOS_CENTERED, // window position x
        SDL_WINDOWPOS_CENTERED, // window position y
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        SDL_WINDOW_BORDERLESS);
    if (!game->window) {
        fprintf(stderr, "Error creating SDL Window.\n");
        return FALSE;
    }
    game->renderer = SDL_CreateRenderer(
        game->window, // window to attach the render
        -1,     // driver code, -1 for default
        0       // special flags
    );
    if (!game->renderer) {
        fprintf(stderr, "Error creating SDL Renderer.\n");
        return FALSE;
    }
    if (TTF_Init() != 0) {
        printf("TTF initialization failed: %s\n", TTF_GetError());
        return FALSE;
    }
    return TRUE;
}

void process_input(struct game *game, struct player *p1, struct player *p2, struct ball *b) {
    SDL_Event event;
    SDL_PollEvent(&event);
    switch (event.type) {
    case SDL_QUIT:
        game->is_running = FALSE;
        break;
    case SDL_KEYDOWN:
        switch (event.key.keysym.sym) {
        case SDLK_ESCAPE:
            game->is_running = FALSE;
            break;
        case SDLK_w:
            p1->up = TRUE;
            break;
        case SDLK_s:
            p1->down = TRUE;
            break;
        case SDLK_k:
            p2->up = TRUE;
            break;
        case SDLK_j:
            p2->down = TRUE;
            break;
        case SDLK_SPACE:
            switch (game->game_state) {
                case 0:
                    game->game_state = 1;
                    b->w = 25;
                    b->h = 25;
                    break;
                case 1:
                    game->game_state = 2;
                    break;
                case 2:
                    game->game_state = 1;
                    break;
            }
            break;
        }
        break;
    case SDL_KEYUP:
        switch (event.key.keysym.sym) {
        case SDLK_w:
            p1->up = FALSE;
            break;
        case SDLK_s:
            p1->down = FALSE;
            break;
        case SDLK_k:
            p2->up = FALSE;
            break;
        case SDLK_j:
            p2->down = FALSE;
            break;
        }
        break;
    default:
        break;
    }
}

int check_collision(struct player *p1, struct player *p2, struct ball *ball) {
    // xpos + width 
    if (ball->x+ball->w >= p2->x && ball->y >= p2->y && ball->y <= p2->y+p2->h){
        ball->xspeed *= -1;
        return TRUE;
    }
    if (ball->x <= p1->x+p1->w && ball->y >= p1->y && ball->y <= p1->y+p1->h) {
        ball->xspeed *= -1;
        return TRUE;
    }
    if (ball->y+ball->h >= WINDOW_HEIGHT) {
        ball->yspeed *= -1;
        return TRUE;
    }
    if (ball->y <= 0) {
        ball->yspeed *= -1;
        return TRUE;
    }
    return FALSE;
}

void update(struct game *game, struct player *p1, struct player *p2, struct ball *ball) {
    // logic to keep a fixed timestamp
    // must waste some time until we reach the target time
    int time_to_wait = FRAME_TARGET_TIME - (SDL_GetTicks() - game->last_frame_time);
    if (time_to_wait > 0 && time_to_wait <= FRAME_TARGET_TIME)
        SDL_Delay(time_to_wait);
    // get a delta time factor to update objects
    float delta_time = (SDL_GetTicks() - game->last_frame_time) / 1000.0f;

    if (p1->up)
        p1->y -= 200 * delta_time;
    if (p1->down)
        p1->y += 200 * delta_time;
    if (p2->up)
        p2->y -= 200 * delta_time;
    if (p2->down)
        p2->y += 200 * delta_time;

    if (check_collision(p1, p2, ball))
        game->ball_multiplier += 0.0625;
    ball->x += ball->xspeed * game->ball_multiplier * delta_time;
    ball->y += ball->yspeed * game->ball_multiplier * delta_time;
}

void setup(struct player *p1, struct player *p2, struct ball *b) {
    p1->x = 20;
    p1->y = 20;
    p1->w = 30;
    p1->h = 100;
    p2->x = 750;
    p2->y = 400;
    p2->w = 30;
    p2->h = 100;
    p1->up = FALSE;
    p1->down = FALSE;
    p2->up = FALSE;
    p2->down = FALSE;
    b->x = 60;
    b->y = 1;
    b->w = 0;
    b->h = 0;
    b->xspeed = 200;
    b->yspeed = 100;
}

void render(struct game *game, struct player p1, struct player p2, struct ball b) {
    SDL_SetRenderDrawColor(game->renderer, 0, 0, 0, 255);
    SDL_RenderClear(game->renderer);

    SDL_Color textColor = {255, 255, 255};
    SDL_Surface* textSurface = TTF_RenderText_Solid(game->font, game->message, textColor);
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(game->renderer, textSurface);

    SDL_Rect textRect;
    textRect.x = WINDOW_WIDTH/2-textSurface->w/2; // X-coordinate
    textRect.y = WINDOW_HEIGHT/2; // Y-coordinate
    textRect.w = textSurface->w;
    textRect.h = textSurface->h;

    SDL_RenderCopy(game->renderer, textTexture, NULL, &textRect);

    SDL_Rect p1render = {
        (int)p1.x,
        (int)p1.y,
        (int)p1.w,
        (int)p1.h
    };

    SDL_Rect p2render = {
        (int)p2.x,
        (int)p2.y,
        (int)p2.w,
        (int)p2.h
    };

    SDL_Rect ballrender = {
        (int)b.x,
        (int)b.y,
        (int)b.w,
        (int)b.h
    };

    SDL_SetRenderDrawColor(game->renderer, 150, 20, 20, 255);
    SDL_RenderFillRect(game->renderer, &p1render);
    SDL_RenderFillRect(game->renderer, &p2render);
    SDL_SetRenderDrawColor(game->renderer, 20, 20, 150, 255);
    SDL_RenderFillRect(game->renderer, &ballrender);

    SDL_RenderPresent(game->renderer); // swaps back buffer with front buffer
}

void destroy_window(struct game *game) {
    SDL_DestroyRenderer(game->renderer);
    SDL_DestroyWindow(game->window);
    SDL_Quit();
}

int main() {
    struct game game = newGame();
    game.is_running = initialize_window(&game);
    game.font = TTF_OpenFont("./assets/fonts/bigblue.ttf", 24); // You need a .ttf file for your font

    struct player player1, player2;
    struct ball ball;
    setup(&player1, &player2, &ball);

    while (game.is_running) {
        process_input(&game, &player1, &player2, &ball);
        switch (game.game_state) {
            case 0:
                game.message = "Welcome to P0NG!";
                break;
            case 1:
                game.message = " ";
                game.last_frame_time = SDL_GetTicks(); // time since game began
                update(&game, &player1, &player2, &ball);
                break;
            case 2:
                game.message = "[PAUSED]";
                break;
        }
        render(&game, player1, player2, ball);
    }

    destroy_window(&game);

    return 0;
}