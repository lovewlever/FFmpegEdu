//
// Created by lovew on 2022/4/19.
//

#include <iostream>
#include "../include/RtmpPush.h"
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
#include <libavdevice/avdevice.h>
}

using namespace std;

int RtmpPush::localFileStream() {
    // h265 编码器
    const AVCodec *codecHevcEncoder = avcodec_find_encoder(AV_CODEC_ID_HEVC);
    AVCodecContext *codecHevcContext = avcodec_alloc_context3(codecHevcEncoder);
    printf("%s", "123");
    return 0;
}