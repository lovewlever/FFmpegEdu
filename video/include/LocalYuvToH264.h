//
// Created by lovew on 2022/5/8.
//

#ifndef FFMPEGEDU_LOCALYUVTOH264_H
#define FFMPEGEDU_LOCALYUVTOH264_H
#include <iostream>
#include <cstdio>
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libavutil/time.h>
#include <libavutil/opt.h>
#include <libavutil/imgutils.h>
}

class LocalYuvToH264 {
private:
    const AVCodec *codec = nullptr;
    AVCodecContext *codecCtx = nullptr;
    AVFormatContext *formatCtx = nullptr;
public:
    LocalYuvToH264();
    int init(char *localYuvPath, int height, int width, char *saveH264Path);
    void checkYuvInfo(char *localYuvPath);
    ~LocalYuvToH264();
};


#endif //FFMPEGEDU_LOCALYUVTOH264_H
