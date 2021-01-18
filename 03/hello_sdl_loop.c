#include <stdio.h>
#include <stdlib.h>
#include <SDL/SDL.h>
#include <emscripten.h>

/**
 * A context structure that we can use for passing variables to our loop
 * function, in this case it just contains a single integer
 */
struct context
{
    int x;
    SDL_Surface *screen;
    SDL_Window *window;
};

void quit()
{
    emscripten_cancel_main_loop();
    printf("quitting \n");
}

void process_events()
{
    /* Our SDL event placeholder. */
    SDL_Event event;
    
    /* Grab all the events off the queue. */
    while( SDL_PollEvent( &event ) ) {
        
        switch( event.type ) {
            case SDL_KEYDOWN:
                quit();
                /* Handle key presses. */
                break;
            case SDL_QUIT:
                quit();
                /* Handle quit requests (like Ctrl-c). */
                break;
        }
        
    }
    
}

/**
 * The loop handler, will be called repeatedly
 */
void loop_fn(void *arg)
{
    struct context *ctx = arg;
    process_events();

    SDL_Rect r;
    r.w = r.h = r.x = r.y = 5;
    r.x += (ctx->x % 60);

    Uint32 color = SDL_MapRGBA(ctx->screen->format, 120,120,0, 255);

    SDL_FillRect(ctx->screen, NULL, 0);
    SDL_FillRect(ctx->screen, &r, color);

    printf("x: %d, ticks: %d\n", ctx->x, SDL_GetTicks());
    SDL_UpdateWindowSurface(ctx->window);

    ctx->x += 1;
}

int main()
{
    struct context *ctx;
    int simulate_infinite_loop = 0;

    ctx = malloc(sizeof(struct context));
    ctx->x = 0;
    SDL_Init(SDL_INIT_VIDEO);

    ctx->window = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 128, 128, SDL_WINDOW_SHOWN);
    ctx->screen = SDL_GetWindowSurface(ctx->window);

    emscripten_set_main_loop_arg(loop_fn, ctx, -1, simulate_infinite_loop); 
}
