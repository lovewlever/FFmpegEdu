//
// Created by lovew on 2022/5/9.
//

#ifndef FFMPEGEDU_LOCALVIDEOTOYUV_H
#define FFMPEGEDU_LOCALVIDEOTOYUV_H
#include <iostream>
#include <cstdio>
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libavutil/time.h>
#include <libavutil/opt.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
}

class LocalVideoToYuv {
private:
    AVFormatContext *inFormat = avformat_alloc_context();
    const AVCodec *avCodec = nullptr;
    AVCodecContext *avCodecCtx = nullptr;
    char *fileName;
    char *yumVideoDst;
    char *yumAudioDst;

public:
    LocalVideoToYuv(char *fileName, char *yumVideoDst, char *yumAudioDst);
    int videoFileToYuv();
//    int processVideoFrame(int index);
//    int processAudioFrame(int index);
    ~LocalVideoToYuv();
};


#endif //FFMPEGEDU_LOCALVIDEOTOYUV_H
