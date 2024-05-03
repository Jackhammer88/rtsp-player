//
// Created by jackhammer on 03.05.24.
//

#ifndef RTSP_RTSP_LISTENER_H
#define RTSP_RTSP_LISTENER_H

#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavfilter/avfilter.h>
#include <libavutil/log.h>
#include <signal.h>
#include <unistd.h>

#include "error-handler.h"

typedef void (*InitWindowCallback)(int width, int height);
typedef void (*RenderFrameCallback)(const AVFrame* frame);
typedef void (*CloseWindowCallback)(void);

void startProcessing(
        const char* url, int jpegQuality,
        InitWindowCallback initWindowCallback,
        RenderFrameCallback renderCallback,
        CloseWindowCallback closeWindowCallback);

#endif //RTSP_RTSP_LISTENER_H
