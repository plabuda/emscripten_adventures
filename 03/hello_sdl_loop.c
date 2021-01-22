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
    int x, y, r;
    SDL_Surface *screen;
    SDL_Window *window;
    int left, up, down, right;
};

void quit()
{
    emscripten_cancel_main_loop();
    printf("quitting \n");
}

void process_events(struct context *ctx)
{
    /* Our SDL event placeholder. */
    SDL_Event event;

    /* Grab all the events off the queue. */
    while (SDL_PollEvent(&event))
    {

        switch (event.type)
        {
        case SDL_KEYDOWN:
            if (!event.key.repeat)
            {
                printf("%d\n", event.key.keysym.sym);
                switch (event.key.keysym.sym)
                {
                case SDL_SCANCODE_LEFT:
                    ctx->left = 1;
                    break;
                    
                case SDL_SCANCODE_UP:
                    ctx->up = 1;
                    break;
                    
                case SDL_SCANCODE_RIGHT:
                    ctx->right = 1;
                    break;
                    
                case SDL_SCANCODE_DOWN:
                    ctx->down = 1;
                    break;
                }
                break;
            }
        case SDL_KEYUP:

            if (!event.key.repeat)
            {
                switch (event.key.keysym.sym)
                {
                case SDL_SCANCODE_LEFT:
                    ctx->left = 0;
                    break;
                    
                case SDL_SCANCODE_UP:
                    ctx->up = 0;
                    break;
                    
                case SDL_SCANCODE_RIGHT:
                    ctx->right = 0;
                    break;
                    
                case SDL_SCANCODE_DOWN:
                    ctx->down = 0;
                    break;
                }
                break;
            }
        case SDL_QUIT:
            quit();
            /* Handle quit requests (like Ctrl-c). */
            break;
        }
    }
}

void frame(void *arg)
{
    struct context *ctx = arg;

    process_events(ctx);
    SDL_Rect r;
    r.w = r.h = 32;
    ctx->x = (ctx->x + (ctx->right - ctx->left)) % (128 - 32);
    ctx->y = (ctx->y + (ctx->down - ctx->up)) % (128 - 32);
    r.x = ctx->x;
    r.y = ctx->y;

    Uint32 color = SDL_MapRGBA(ctx->screen->format, 120, 120, 0, 255);

    SDL_FillRect(ctx->screen, NULL, 0);
    SDL_FillRect(ctx->screen, &r, color);

    SDL_UpdateWindowSurface(ctx->window);
}

/**
 * The loop handler, will be called repeatedly
 */
void loop_fn(void *arg)
{
    const int nominator = 60;
    const int denominator = 1000;
    static int accumulated_ticks = 0;
    static int old_ticks = 0;
    static int new_ticks = 0;

    new_ticks = SDL_GetTicks();

    accumulated_ticks += (new_ticks - old_ticks) * nominator;
    old_ticks = new_ticks;

    if (accumulated_ticks > denominator)
    {
        frame(arg);
        accumulated_ticks -= denominator;
    }
}

int main()
{
    struct context *ctx;
    int simulate_infinite_loop = 0;

    ctx = calloc(1, sizeof(struct context));
    SDL_Init(SDL_INIT_VIDEO);

    ctx->window = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 128, 128, SDL_WINDOW_SHOWN);
    ctx->screen = SDL_GetWindowSurface(ctx->window);

    emscripten_set_main_loop_arg(loop_fn, ctx, -1, simulate_infinite_loop);
}
