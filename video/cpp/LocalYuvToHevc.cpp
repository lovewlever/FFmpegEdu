//
// Created by lovew on 2022/5/8.
//

#include "../include/LocalYuvToHevc.h"

LocalYuvToHevc::LocalYuvToHevc() {}
LocalYuvToHevc::~LocalYuvToHevc() {}

int LocalYuvToHevc::start(char *localYuvPath, int height, int width, char *saveH264Path) {

    codec = avcodec_find_encoder(AV_CODEC_ID_HEVC);
    codecCtx = avcodec_alloc_context3(codec);
    codecCtx->time_base = { 1, 25 };
    codecCtx->framerate = { 25, 1 };
    codecCtx->codec = codec;
    codecCtx->codec_type = AVMEDIA_TYPE_VIDEO;
    codecCtx->codec_id = codec->id;
    codecCtx->profile = FF_PROFILE_HEVC_MAIN;
    codecCtx->gop_size = 200;
    codecCtx->level = 50;
    codecCtx->height = height;
    codecCtx->width = width;
    codecCtx->max_b_frames = 0;
    codecCtx->bit_rate = 300000;
    codecCtx->pix_fmt = AV_PIX_FMT_YUV420P;

    codecCtx->qmin = 10;
    codecCtx->qmax = 51;
    if (codec->id == AV_CODEC_ID_HEVC) {
        av_opt_set(codecCtx->priv_data, "preset", "ultrafast", 0);
        av_opt_set(codecCtx->priv_data, "tune", "zero-latency", 0);
    }
    avformat_alloc_output_context2(&formatCtx, nullptr, nullptr, saveH264Path);
    AVStream *avStream = avformat_new_stream(formatCtx, codec);
    avcodec_parameters_from_context(avStream->codecpar, codecCtx);

    // yuv 缓存大小
    int imgBufSize = av_image_get_buffer_size(codecCtx->pix_fmt, codecCtx->width, codecCtx->height, 1);
    int ySize = codecCtx->height * codecCtx->width;

    AVFrame *avFrame = av_frame_alloc();
    avFrame->height = codecCtx->height;
    avFrame->width = codecCtx->width;
    avFrame->format = codecCtx->pix_fmt;
    AVPacket *avPacket = av_packet_alloc();

    auto *imgBuf = (uint8_t *)av_malloc(imgBufSize);

    int ret = av_image_fill_arrays(avFrame->data, avFrame->linesize, imgBuf, codecCtx->pix_fmt, codecCtx->width, codecCtx->height, 1);
    if (ret < 0) {
        std::cout << "av_image_fill_arrays fail " << ret << std::endl;
        return -1;
    }
    ret = avcodec_open2(codecCtx, codec, nullptr);
    if (ret < 0) {
        std::cout << "avcodec_open2 fail " << ret << std::endl;
        return -1;
    }

    ret = avio_open2(&formatCtx->pb, saveH264Path, AVIO_FLAG_WRITE, nullptr, nullptr);
    if (ret < 0) {
        std::cout << "avio_open2 fail " << ret << std::endl;
        return -1;
    }

    ret = avformat_write_header(formatCtx, nullptr);
    if (ret < 0) {
        std::cout << "avio_open2 fail " << ret << std::endl;
        return -1;
    }

    FILE *inFile = fopen(localYuvPath, "rb+");
    if (!inFile) {
        std::cout << "fopen fail " << ret << std::endl;
        return -1;
    }

    int pts = 0;
    while (true) {
        ret = fread(imgBuf, 1, ySize * 3 / 2, inFile);
        if (ret <= 0) break;
        avFrame->pts = pts ++;
        ret = avcodec_send_frame(codecCtx, avFrame);
        while (ret >= 0) {
            ret = avcodec_receive_packet(codecCtx, avPacket);
            if (ret < 0) break;
            av_packet_rescale_ts(avPacket, codecCtx->time_base, avStream->time_base);
            av_interleaved_write_frame(formatCtx, avPacket);
            av_packet_unref(avPacket);
        }

    }
    av_write_trailer(formatCtx);

    avformat_free_context(formatCtx);
    av_free(imgBuf);
    avcodec_close(codecCtx);
    avcodec_free_context(&codecCtx);
    fclose(inFile);
    av_packet_free(&avPacket);
    av_frame_free(&avFrame);
}