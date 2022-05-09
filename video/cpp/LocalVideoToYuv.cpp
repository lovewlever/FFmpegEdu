//
// Created by lovew on 2022/5/9.
//

#include "../include/LocalVideoToYuv.h"

LocalVideoToYuv::LocalVideoToYuv(char *fileName, char *yumVideoDst, char *yumAudioDst){
    LocalVideoToYuv::fileName = fileName;
    LocalVideoToYuv::yumVideoDst = yumVideoDst;
    LocalVideoToYuv::yumAudioDst = yumAudioDst;
}

int LocalVideoToYuv::videoFileToYuv() {

    inFormat = avformat_alloc_context();
    int ret = avformat_open_input(&inFormat, fileName, nullptr, nullptr);
    if(ret < 0) {
        std::cout << "avformat_open_input fail" << std::endl;
        return -1;
    }
    ret = avformat_find_stream_info(inFormat, 0);
    if(ret < 0) {
        std::cout << "avformat_find_stream_info fail" << std::endl;
        return -1;
    }
    int videoIndex, audioIndex;
    for (int var = 0; var < inFormat->nb_streams; ++var) {
        // 视频流索引
        if (inFormat->streams[var]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            videoIndex = var;
        } else if (inFormat->streams[var]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
            audioIndex = var;
        } else {
            std::cout << "not find codec_type fail" << std::endl;
            return -1;
        }
    }

    //
    AVStream *videoStream = inFormat->streams[videoIndex];
    AVStream *audioStream = inFormat->streams[audioIndex];
    // 查找解码器
    const AVCodec *videoCodec = avcodec_find_decoder(videoStream->codecpar->codec_id);
    const AVCodec *audioCodec = avcodec_find_decoder(audioStream->codecpar->codec_id);

    // 上下文
    AVCodecContext *videoCodecCtx = avcodec_alloc_context3(videoCodec);
    AVCodecContext *audioCodecCtx = avcodec_alloc_context3(audioCodec);

    avcodec_parameters_to_context(videoCodecCtx, videoStream->codecpar);
    avcodec_parameters_to_context(audioCodecCtx, audioStream->codecpar);

    // 打开解码器
    avcodec_open2(videoCodecCtx, videoCodec, nullptr);
    avcodec_open2(audioCodecCtx, audioCodec, nullptr);

    /* 初始化一个用来转换数据的帧  */
    AVFrame* tempFrame = av_frame_alloc();
    tempFrame->width = videoCodecCtx->width;
    tempFrame->height = videoCodecCtx->height;
    tempFrame->format = AV_PIX_FMT_YUV420P;
    /* 调用此函数前需社会好宽高和格式，不然无法判断是视频 ;
       下面函数会初始化lineSize，指每一行占多少字节，按照
       对齐的方式来计算的，大于等于width */
    av_frame_get_buffer(tempFrame,4);
    int videoDataLen = videoCodecCtx->width * videoCodecCtx->height;
    FILE *outFile = fopen(yumVideoDst, "wb");
    FILE *outAudioFile = fopen(yumAudioDst, "wb");
    SwsContext *swsCtx = nullptr;
    AVPacket *packet = av_packet_alloc();
    while (av_read_frame(inFormat, packet) >= 0) {
        AVFrame *frame = av_frame_alloc();
        if(packet->stream_index == videoIndex) { // 视频帧
            ret = avcodec_send_packet(videoCodecCtx, packet);
            while(ret >= 0) {
                ret = avcodec_receive_frame(videoCodecCtx, frame);
                if(ret < 0) continue;
                swsCtx = sws_getContext(frame->width, frame->height, videoCodecCtx->pix_fmt,
                                        videoCodecCtx->width, videoCodecCtx->height, AV_PIX_FMT_YUV420P, SWS_BICUBIC, nullptr, nullptr, nullptr);
                sws_scale(swsCtx, frame->data, frame->linesize, 0, videoCodecCtx->height,
                          tempFrame->data, tempFrame->linesize);
                fwrite(tempFrame->data[0],1,videoDataLen, outFile);
                fwrite(tempFrame->data[1],1,videoDataLen/4, outFile);
                fwrite(tempFrame->data[2],1,videoDataLen/4, outFile);

                sws_freeContext(swsCtx);
            }
        } else if(packet->stream_index == audioIndex) { // 音频帧
            ret = avcodec_send_packet(audioCodecCtx,packet);
            while(ret >= 0)
            {
                ret =avcodec_receive_frame(audioCodecCtx,frame);
                int audioLen = av_get_bytes_per_sample(audioCodecCtx->sample_fmt);
                for(int i = 0;i< frame->nb_samples;++i)
                    for(int j = 0; j < audioCodecCtx->channels;++j)
                        fwrite(frame->data[j] + audioLen*i,1,audioLen, outAudioFile);
            }
        }
        av_frame_free(&frame);
    }

    fclose(outFile);
    fclose(outAudioFile);
    av_packet_free(&packet);
    return 0;
}
