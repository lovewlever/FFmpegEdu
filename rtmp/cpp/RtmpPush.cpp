//
// Created by lovew on 2022/4/19.
//

#include <iostream>
#include "../include/RtmpPush.h"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
#include <libavdevice/avdevice.h>
#include <libavutil/time.h>
}

using namespace std;

int RtmpPush::localFileStream(std::string filePath) {

    AVFormatContext *inFmtCtx = avformat_alloc_context();
    int ret = avformat_open_input(&inFmtCtx, filePath.c_str(), nullptr, nullptr);
    if (ret < 0) {
        cout << "avformat_open_input fail" << endl;
        return -1;
    }
    ret = avformat_find_stream_info(inFmtCtx, nullptr);
    if (ret < 0) {
        cout << "avformat_alloc_output_context2 fail" << endl;
        return -1;
    }

    av_dump_format(inFmtCtx, 1, filePath.c_str(), 0);

    AVFormatContext *outFmtCtx = avformat_alloc_context();
    ret = avformat_alloc_output_context2(&outFmtCtx, nullptr, "flv", "rtmp://127.0.0.1:1935/stream");
    if (ret < 0) {
        cout << "avformat_alloc_output_context2 fail" << endl;
        return -1;
    }

    // 复制参数到输出
    for (int i = 0; i < inFmtCtx->nb_streams; ++i) {
        const AVCodec *avCodec = avcodec_find_decoder(inFmtCtx->streams[i]->codecpar->codec_id);
        AVStream *stream = avformat_new_stream(outFmtCtx, avCodec);
        avcodec_parameters_copy(stream->codecpar, inFmtCtx->streams[i]->codecpar);
    }

    ret = avio_open2(&outFmtCtx->pb, "rtmp://127.0.0.1:1935/stream", AVIO_FLAG_WRITE, nullptr, nullptr);
    if (ret < 0) {
        cout << "avio_open2 fail" << endl;
        return -1;
    }

    avformat_write_header(outFmtCtx, nullptr);

    AVPacket *avPacket = av_packet_alloc();
    int64_t startTime = av_gettime();

    while(true) {
        ret = av_read_frame(inFmtCtx, avPacket);
        if (ret != 0) {
            break;
        }
        // 视频帧
        if (inFmtCtx->streams[avPacket->stream_index]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            AVRational tb = inFmtCtx->streams[avPacket->stream_index]->time_base;
            int64_t time = av_gettime() - startTime;
            int64_t dts = avPacket->dts * (1000 * 1000 * av_q2d(tb));
            if (dts > time) {
                av_usleep(dts - time);
            }
        }
        ret = av_interleaved_write_frame(outFmtCtx, avPacket);
        /*if (ret < 0) {
            break;
        }*/
        av_packet_unref(avPacket);
    }

   return 0;
}