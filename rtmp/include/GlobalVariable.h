//
// Created by lovew on 2022/4/25.
//

#ifndef FFMPEGEDU_GLOBALVARIABLE_H
#define FFMPEGEDU_GLOBALVARIABLE_H

extern "C" {
#include <libavformat/avformat.h>
#include "libavdevice/avdevice.h"
#include "libavutil/avutil.h"
#include "libavutil/time.h"
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
}

extern AVFormatContext *inputFormatCtx;
extern AVFormatContext *outFormatCtx;
extern int64_t lastReadPacketTime;

int interruptCall(void*);

#endif //FFMPEGEDU_GLOBALVARIABLE_H
