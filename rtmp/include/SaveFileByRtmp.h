//
// Created by lovew on 2022/4/20.
//

#ifndef FFMPEGEDU_SAVEFILEBYRTMP_H
#define FFMPEGEDU_SAVEFILEBYRTMP_H

extern "C" {
#include <libavcodec/packet.h>
}

class SaveFileByRtmp {
public:
    int saveRtmpStreamToFile(char *rtmpUrl, char *outFile);

private:
    int openInput(char *rtmpUrl);
    int openOutput(char *outFile);
    AVPacket *readPacketFromSource();
    int writePacket(AVPacket *avPacket);
};


#endif //FFMPEGEDU_SAVEFILEBYRTMP_H
