//
// Created by jackhammer on 03.05.24.
//

#include "sdl-player.h"
#include "error-handler.h"
#include <pthread.h>
#include <unistd.h>

#define CLIP(x, min, max) (((x) < (min)) ? (min) : (((x) > (max)) ? (max) : (x)))

static SDL_Surface* screen;
static volatile int running = 1;
pthread_t rtsp_thread;

void close_handler() {
    SDL_Quit();
    pthread_join(rtsp_thread, NULL);
}

void *sdl_event_loop(void *args) {
    SDL_Event event;
    while (running && SDL_WaitEvent(&event)) {
        if (event.type == SDL_QUIT) {
            running = 0;
            break;
        }
    }

    pid_t pid = getpid();
    kill(pid, SIGTERM);

    pthread_exit(NULL);
}

void init_sdl(int width, int height) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        char errorDescription[250];
        sprintf(errorDescription, "Не удалось инициализировать SDL: %s\n", SDL_GetError());
        fatal_error(errorDescription, SDL_INIT_ERROR);
    }

    screen = SDL_SetVideoMode(width, height, 32, SDL_NOFRAME | SDL_SWSURFACE);
    if (!screen) {
        char errorDescription[250];
        sprintf(errorDescription, "Не удалось создать окно: %s\n", SDL_GetError());
        fatal_error(errorDescription, SDL_CREATE_WINDOW_ERROR);
    }

    if (pthread_create(&rtsp_thread, NULL, sdl_event_loop, NULL) != 0) {
        fatal_error("Ошибка при создании потока для отслеживания события закрытия окна.", 1);
    }
}

void render_frame(const AVFrame* frame) {
    if (!screen) {
        fatal_error("SDL surface is not initialized. Cannot render frame.\n", 1);
    }

    // Предположим, что кадр в формате YUV420p
    int width = frame->width;
    int height = frame->height;

    // Преобразование YUV420p в RGB24
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int Y = frame->data[0][y * frame->linesize[0] + x];
            int U = frame->data[1][y / 2 * frame->linesize[1] + x / 2];
            int V = frame->data[2][y / 2 * frame->linesize[2] + x / 2];

            // Конвертация YUV в RGB
            int R = Y + 1.402 * (V - 128);
            int G = Y - 0.344136 * (U - 128) - 0.714136 * (V - 128);
            int B = Y + 1.772 * (U - 128);

            // Ограничение значений RGB до допустимого диапазона [0, 255]
            R = CLIP(R, 0, 255);
            G = CLIP(G, 0, 255);
            B = CLIP(B, 0, 255);

            // Установка цвета пикселя на экране SDL
            Uint32 pixel = SDL_MapRGB(screen->format, R, G, B);
            int bpp = screen->format->BytesPerPixel;
            Uint8 *p = (Uint8 *)screen->pixels + y * screen->pitch + x * bpp;
            *(Uint32 *)p = pixel;
        }
    }

    SDL_Flip(screen);
}