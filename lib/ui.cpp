#include <ui.h>
#include <emu.h>
#include <bus.h>
#include <ppu.h>
#include <gamepad.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

SDL_Window* sdlWindow = nullptr;
SDL_Renderer* sdlRenderer = nullptr;

SDL_Texture* sdlTexture = nullptr;
SDL_Surface* screen = nullptr;

SDL_Window* sdlDebugWindow = nullptr;
SDL_Renderer* sdlDebugRenderer = nullptr;

SDL_Texture* sdlDebugTexture = nullptr;
SDL_Surface* debugScreen = nullptr;


static int scale = 4;


void ui_init() {
    SDL_Init(SDL_INIT_VIDEO);
    std::cout << "SDL_INIT\n";

    TTF_Init();
    std::cout << "TTF_INIT\n";

    sdlWindow = SDL_CreateWindow(
        "SDL2 Basic Window",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH,
        SCREEN_HEIGHT,
        SDL_WINDOW_SHOWN
    );

    sdlDebugWindow = SDL_CreateWindow(
        "Debug Window",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        16 * 8 * scale,
        32 * 8 * scale,
        SDL_WINDOW_SHOWN
    );

    if (sdlWindow == nullptr) {
        std::cout << "Window could not be created! SDL_Error : "
                  << SDL_GetError() << std::endl;

        SDL_Quit();
        return;
    }

    sdlRenderer = SDL_CreateRenderer(
        sdlWindow,
        -1,
        SDL_RENDERER_ACCELERATED
    );

    sdlDebugRenderer = SDL_CreateRenderer(
        sdlDebugWindow,
        -1,
        SDL_RENDERER_ACCELERATED
    );

    if (sdlRenderer == nullptr) {
        std::cout << "Renderer could not be created! SDL_Error : "
                  << SDL_GetError() << std::endl;

        SDL_DestroyWindow(sdlWindow);
        sdlWindow = nullptr;

        SDL_Quit();
        return;
    }


    SDL_SetRenderDrawColor(sdlRenderer, 0, 0, 0, 255);
    SDL_RenderClear(sdlRenderer);
    SDL_RenderPresent(sdlRenderer);

    SDL_SetRenderDrawColor(sdlDebugRenderer, 0, 0, 0, 255);
    SDL_RenderClear(sdlDebugRenderer);
    SDL_RenderPresent(sdlDebugRenderer);

    screen = SDL_CreateRGBSurface(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);

    sdlTexture = SDL_CreateTexture(sdlRenderer,
                                            SDL_PIXELFORMAT_ARGB8888,
                                            SDL_TEXTUREACCESS_STREAMING,
                                            SCREEN_WIDTH, SCREEN_HEIGHT);

    debugScreen = SDL_CreateRGBSurface(0, (16 * 8 * scale) + (16 * scale),
                                            (32 * 8 * scale) + (64 * scale), 32,
                                            0x00FF0000,
                                            0x0000FF00,
                                            0x000000FF,
                                            0xFF000000);

    sdlDebugTexture = SDL_CreateTexture(sdlDebugRenderer,
                                            SDL_PIXELFORMAT_ARGB8888,
                                            SDL_TEXTUREACCESS_STREAMING,
                                            (16 * 8 * scale) + (16 * scale),
                                            (32 * 8 * scale) + (64 * scale));

    int x, y;
    SDL_GetWindowPosition(sdlWindow, &x, &y);
    SDL_SetWindowPosition(sdlDebugWindow, x + SCREEN_WIDTH + 10, y);
}

void delay(uint32_t ms) {
    SDL_Delay(ms);
}

uint32_t get_ticks() {
    return SDL_GetTicks();
}

static unsigned long tile_colors[4] = {0xFFFFFFFF, 0xFFAAAAAA, 0xFF555555, 0xFF000000};

void display_tile(SDL_Surface *surface, uint16_t startLocation, uint16_t tileNum, int x, int y) {
    SDL_Rect rc;

    for(int tileY = 0; tileY < 16; tileY += 2) {

        uint8_t b1 = bus_read(startLocation + (tileNum * 16) + tileY);
        uint8_t b2 = bus_read(startLocation + (tileNum * 16) + tileY + 1);

        for(int bit = 7; bit >= 0; bit--) {
            uint8_t hi = !!(b1 & (1 << bit)) << 1;
            uint8_t lo = !!(b2 & (1 << bit));

            uint8_t color = hi | lo;

            rc.x = x + ((7 - bit) * scale);
            rc.y = y + (tileY / 2 * scale);
            rc.w = scale;
            rc.h = scale;

            SDL_FillRect(surface, &rc, tile_colors[color]);
        }

    }
}

void update_dbg_window() {
    int xDraw = 0;
    int yDraw = 0;
    int tileNum = 0;

    SDL_Rect rc;
    rc.x = 0;
    rc.y = 0;
    rc.w = debugScreen->w;
    rc.h = debugScreen->h;
    SDL_FillRect(debugScreen, &rc, 0xFF111111);

    uint16_t addr = 0x8000;

    //384 tiles, 24 x 16

    for(int y = 0; y < 24; y++) {
        for(int x = 0; x < 16; x++) {
            display_tile(debugScreen, addr, tileNum, xDraw + (x * scale), yDraw + (y * scale));
            xDraw += (8 * scale);
            tileNum++;
        }

        yDraw += (8 * scale);
        xDraw = 0;
    }

    SDL_UpdateTexture(sdlDebugTexture, NULL, debugScreen->pixels, debugScreen->pitch);
    SDL_RenderClear(sdlDebugRenderer);
    SDL_RenderCopy(sdlDebugRenderer, sdlDebugTexture, NULL, NULL);
    SDL_RenderPresent(sdlDebugRenderer);

}

void ui_update() {
    SDL_Rect rc;
    rc.x = rc.y = 0;
    rc.w = rc.h = 2048;

    uint32_t *video_buffer = ppu_get_context()->video_buffer;

    for(int line_num = 0; line_num < YRES; line_num++) {
        for(int x = 0; x < XRES; x++) {
            rc.x = x * scale;
            rc.y = line_num * scale;
            rc.w = scale;
            rc.h = scale;

            SDL_FillRect(screen, &rc, video_buffer[x + (line_num * XRES)]);
        }
    }

    SDL_UpdateTexture(sdlTexture, NULL, screen->pixels, screen->pitch);
    SDL_RenderClear(sdlRenderer);
    SDL_RenderCopy(sdlRenderer, sdlTexture, NULL, NULL);
    SDL_RenderPresent(sdlRenderer);

    update_dbg_window();
}

void ui_on_key(bool down, uint32_t key_code) {

    switch(key_code) {
        case SDLK_z: gamepad_get_state()->b = down; break;
        case SDLK_x: gamepad_get_state()->a = down; break;
        case SDLK_RETURN: gamepad_get_state()->start = down; break;
        case SDLK_TAB: gamepad_get_state()->select = down; break;
        case SDLK_UP: gamepad_get_state()->up = down; break;
        case SDLK_DOWN: gamepad_get_state()->down = down; break;
        case SDLK_LEFT: gamepad_get_state()->left = down; break;
        case SDLK_RIGHT: gamepad_get_state()->right = down; break;
    }
}


void ui_handle_events() {
    SDL_Event e;

    while (SDL_PollEvent(&e)) {

        if (e.type == SDL_KEYDOWN) {
            ui_on_key(true, e.key.keysym.sym);
        }

        if (e.type == SDL_KEYUP) {
            ui_on_key(false, e.key.keysym.sym);
        }

        if (e.type == SDL_QUIT) {
            emu_get_context()->die = true;
        }

        if (e.type == SDL_WINDOWEVENT &&
            e.window.event == SDL_WINDOWEVENT_CLOSE) {

            emu_get_context()->die = true;
        }
    }
}

void ui_end() {

    if (sdlDebugTexture) {
        SDL_DestroyTexture(sdlDebugTexture);
        sdlDebugTexture = nullptr;
    }

    if (debugScreen) {
        SDL_FreeSurface(debugScreen);
        debugScreen = nullptr;
    }

    if (sdlDebugRenderer) {
        SDL_DestroyRenderer(sdlDebugRenderer);
        sdlDebugRenderer = nullptr;
    }

    if (sdlDebugWindow) {
        SDL_DestroyWindow(sdlDebugWindow);
        sdlDebugWindow = nullptr;
    }

    if (sdlRenderer) {
        SDL_DestroyRenderer(sdlRenderer);
        sdlRenderer = nullptr;
    }

    if (sdlWindow) {
        SDL_DestroyWindow(sdlWindow);
        sdlWindow = nullptr;
    }

    TTF_Quit();
    SDL_Quit();
}