//
// Created by lovew on 2022/4/20.
//

#include "../include/SaveFileByRtmp.h"
#include "iostream"
#include "../include/GlobalVariable.h"



/**
 * 打开输入流
 * @param rtmpUrl
 * @return
 */
int SaveFileByRtmp::openInput(char *rtmpUrl) {
    inputFormatCtx = avformat_alloc_context();
    inputFormatCtx->interrupt_callback.callback = interruptCall;
    lastReadPacketTime = av_gettime();
    int ret = avformat_open_input(&inputFormatCtx, rtmpUrl, nullptr, nullptr);
    if (ret < 0) {
        std::cout << "avformat_open_input fail" << std::endl;
        return -1;
    }
    // 查询stream信息
    ret = avformat_find_stream_info(inputFormatCtx, nullptr);
    if (ret < 0) {
        std::cout << "avformat_find_stream_info fail" << std::endl;
        return -1;
    }
    return 0;
}

int SaveFileByRtmp::openOutput(char *outFile) {
    int ret = avformat_alloc_output_context2(&outFormatCtx, nullptr, "mpegts", "ffmpeg_save_rtmp_file");
    if (ret < 0) {
        std::cout << "avformat_alloc_output_context2 fail" << std::endl;
        return -1;
    }
    ret = avio_open2(&outFormatCtx->pb, outFile, AVIO_FLAG_WRITE, nullptr, nullptr);
    if (ret < 0) {
        std::cout << "avio_open2 fail" << std::endl;
        return -1;
    }
    for (int i = 0; i < inputFormatCtx->nb_streams; ++i) {
        // 创建流
        AVStream *avStream = avformat_new_stream(outFormatCtx, inputFormatCtx->video_codec);
        avcodec_parameters_copy(avStream->codecpar, inputFormatCtx->streams[i]->codecpar);
    }

    // 写入头部
    ret = avformat_write_header(outFormatCtx, nullptr);
    if (ret < 0) {
        std::cout << "avformat_write_header fail" << std::endl;
        return -1;
    }
    return 0;
}

/**
 * 读帧
 * @return
 */
AVPacket *SaveFileByRtmp::readPacketFromSource() {
    AVPacket *avPacket = av_packet_alloc();
    lastReadPacketTime = av_gettime();
    int ret = av_read_frame(inputFormatCtx, avPacket);
    if (ret < 0) {
        std::cout << "av_read_frame fail" << std::endl;
        return nullptr;
    }
    return avPacket;
}

/**
 * 写入 文件
 * @param avPacket
 * @return
 */
int SaveFileByRtmp::writePacket(AVPacket *avPacket) {
    auto inputStream = inputFormatCtx->streams[avPacket->stream_index];
    auto outStream = outFormatCtx->streams[avPacket->stream_index];
    av_packet_rescale_ts(avPacket, inputStream->time_base, outStream->time_base);
    int ret = av_interleaved_write_frame(outFormatCtx, avPacket);
    if (ret < 0) {
        std::cout << "av_read_frame fail" << std::endl;
        return -1;
    }
    return 0;
}

int SaveFileByRtmp::saveRtmpStreamToFile(char *rtmpUrl, char *outFile) {
    openInput(rtmpUrl);
    openOutput(outFile);
    while (true) {
        AVPacket *avPacket = readPacketFromSource();
        if (avPacket) {
            writePacket(avPacket);
        } else {
            break;
        }
    }
    return 0;
}