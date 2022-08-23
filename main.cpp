#include <iostream>
#include "video/include/LocalYuvToH264.h"
#include "rtmp/include/SaveFileByRtmp.h"
#include "video/include/LocalYuvToHevc.h"
#include "rtmp/include/RtmpPush.h"
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavdevice/avdevice.h>
#include <libavutil/avutil.h>
#include <libavformat/avformat.h>
#include <libswresample/swresample.h>
}

using namespace std;

int pcmToAac(char *inputFile, char *outFile) {
    cout << "开始" << endl;
    const AVCodec *codec = avcodec_find_encoder(AV_CODEC_ID_AAC);
    if (codec == nullptr) {
        cout << "avcodec_find_encoder fail" << endl;
        return -1;
    }
    AVCodecContext *codecContext = avcodec_alloc_context3(codec);
    if (codecContext == nullptr) {
        cout << "avcodec_alloc_context3 fail" << endl;
        return -1;
    }
    codecContext->sample_rate = 44100;
    codecContext->channels = 2;
    codecContext->sample_fmt = AV_SAMPLE_FMT_FLTP;
    codecContext->bit_rate = 64000;
    codecContext->channel_layout = AV_CH_LAYOUT_STEREO;
    codecContext->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;

    // 打开音频编码器
    int f = avcodec_open2(codecContext, codec, nullptr);
    if (f < 0) {
        if (!f) {
            cout << "avcodec_open2 fail" << endl;
            return -1;
        }
    }

    // 创建输出上下文
    AVFormatContext *formatContext = nullptr;
    f = avformat_alloc_output_context2(&formatContext, nullptr, nullptr, outFile);
    if (f < 0) {
        cout << "avformat_alloc_output_context2 fail" << endl;
        return -1;
    }

    cout << "avformat_new_stream" << endl;
    // 创建音频流
    AVStream *avStream = avformat_new_stream(formatContext, nullptr);
    avStream->codecpar->codec_tag = 0;
    avcodec_parameters_from_context(avStream->codecpar, codecContext);

    // 打印信息 0音频 1视频
    //av_dump_format(formatContext, 0, outFile, 1);
    // 打开IO流
    f = avio_open(&formatContext->pb, outFile, AVIO_FLAG_WRITE);
    if (f < 0) {
        cout << "avformat_alloc_output_context2 fail" << endl;
        return -1;
    }

    // 开始
    // 写入头部信息
    f = avformat_write_header(formatContext, nullptr);
    if (f < 0) {
        cout << "avformat_write_header fail" << endl;
        return -1;
    }

    // 重采样上下文
    SwrContext *swrContext = nullptr;
    swrContext = swr_alloc_set_opts(
            swrContext, codecContext->channel_layout, codecContext->sample_fmt, codecContext->sample_rate,
            codecContext->channel_layout, AV_SAMPLE_FMT_S16, codecContext->sample_rate, 0, 0
    );
    if (!swrContext) {
        cout << "swr_alloc_set_opts fail" << endl;
        return -1;
    }

    f = swr_init(swrContext);
    if (f < 0) {
        cout << "swr_init fail" << endl;
        return -1;
    }
    // 读取输入流数据
    AVFrame *avFrame = av_frame_alloc();
    avFrame->format = AV_SAMPLE_FMT_FLTP;
    avFrame->channels = 2;
    avFrame->channel_layout = AV_CH_LAYOUT_STEREO;
    avFrame->nb_samples = 1024; // 样本数
    // 缓存空间
    f = av_frame_get_buffer(avFrame, 0);
    if (f < 0) {
        cout << "swr_init fail" << endl;
        return -1;
    }
    // 一帧样本大小
    int readSize = avFrame->nb_samples * 2 * 2;
    char *pcms = new char[readSize];
    FILE *file = fopen(inputFile, "rb");
    for (;;) {
        int len = fread(pcms, 1, readSize, file);
        if (len <= 0) {
            break;
        }
        const uint8_t *data[1];
        data[0] = (uint8_t *) pcms;

        len = swr_convert(swrContext, avFrame->data, avFrame->nb_samples, data, avFrame->nb_samples);
        if (len <= 0) {
            break;
        }
        // 编码
        AVPacket *avPacket = av_packet_alloc();
        // 发送到编码线程
        f = avcodec_send_frame(codecContext, avFrame);
        av_packet_unref(avPacket);
        if (f != 0) continue;
        // 接收编码数据
        f = avcodec_receive_packet(codecContext, avPacket);
        if (f != 0) continue;
        cout << f << ",";
        // 指定为音频流
        avPacket->stream_index = 0;
        avPacket->dts = 0;
        avPacket->pts = 0;
        // 写入输出上下文
        av_interleaved_write_frame(formatContext, avPacket);
        av_packet_free(&avPacket);

    }

    delete[] pcms;
    pcms = nullptr;
    // 写入索引
    av_write_trailer(formatContext);
    // 关闭io流
    avio_close(formatContext->pb);
    // 关闭 codec
    avcodec_close(codecContext);
    // 关闭设置的参数
    avcodec_free_context(&codecContext);
    avformat_free_context(formatContext);
    return 0;
}

int main() {
    /*auto localYuvToH264 = new LocalYuvToH264();
    localYuvToH264->init("D:\\BigBuckBunny_CIF_24fps.yuv", 288, 352,"D:\\CIF_24fps.mp4");
    //localYuvToH264->checkYuvInfo("D:\\CIF_24fps.mp4");
    delete localYuvToH264;*/

    /*auto localYuvToHevc = new LocalYuvToHevc();
    localYuvToHevc->start("D:\\BigBuckBunny_CIF_24fps.yuv", 288, 352,"D:\\CIF_24fps265.mp4");
    delete localYuvToHevc;*/
    cout << "1234" << endl;
    // 推流
    RtmpPush rtmpPush;
    rtmpPush.localFileStream("D:\\迅雷下载\\2022-08-23 16-23-02.flv");
    return 0;
}
