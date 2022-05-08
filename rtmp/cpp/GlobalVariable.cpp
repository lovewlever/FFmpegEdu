//
// Created by lovew on 2022/4/25.
//

#include "../include/GlobalVariable.h"
extern "C" {
#include <libavformat/avformat.h>
#include "libavdevice/avdevice.h"
#include "libavutil/avutil.h"
#include "libavutil/time.h"
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
}

AVFormatContext *inputFormatCtx = nullptr;
AVFormatContext *outFormatCtx;
int64_t lastReadPacketTime = 0;

extern int interruptCall(void*) {
    int outTime = 10;
    if (av_gettime() - lastReadPacketTime > (outTime * 1000 * 1000)) {
        return -1;
    }
    return 0;
}
