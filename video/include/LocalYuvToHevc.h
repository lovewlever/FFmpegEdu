//
// Created by lovew on 2022/5/8.
//

#ifndef FFMPEGEDU_LOCALYUVTOHEVC_H
#define FFMPEGEDU_LOCALYUVTOHEVC_H
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

class LocalYuvToHevc {
private:
    const AVCodec *codec = nullptr;
    AVCodecContext *codecCtx = nullptr;
    AVFormatContext *formatCtx = nullptr;
public:
    LocalYuvToHevc();
    int start(char *localYuvPath, int height, int width, char *saveH264Path);
    ~LocalYuvToHevc();
};


#endif //FFMPEGEDU_LOCALYUVTOHEVC_H
