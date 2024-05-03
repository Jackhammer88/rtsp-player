//
// Created by jackhammer on 03.05.24.
//

#ifndef RTSP_PLAYER_SDL_PLAYER_H
#define RTSP_PLAYER_SDL_PLAYER_H

#include <SDL.h>
#include <libavcodec/avcodec.h>
#include <signal.h>
#include "error-handler.h"

void close_handler();

void init_sdl(int width, int height);

void render_frame(const AVFrame* frame);

#endif //RTSP_PLAYER_SDL_PLAYER_H
