//
// Created by lovew on 2022/4/25.
//

#ifndef FFMPEGEDU_NETSTREAMFORWARD_H
#define FFMPEGEDU_NETSTREAMFORWARD_H


#include <libavcodec/packet.h>

class NetStreamForward {
public:
    int netSaveRtmpStreamToFile(char *rtmpUrl, char *outFile);

private:
    int netOpenInput(char *rtmpUrl);
    int netOpenOutput(char *outFile);
    AVPacket *netReadPacketFromSource();
    int netWritePacket(AVPacket *avPacket);
};


#endif //FFMPEGEDU_NETSTREAMFORWARD_H
