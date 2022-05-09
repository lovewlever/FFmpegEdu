cmake_minimum_required(VERSION 3.17)
project(FFmpegEdu)

set(CMAKE_CXX_STANDARD 14)
SET(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

include_directories(D:/ffmpeg-5.0.1/include)
link_directories(D:/ffmpeg-5.0.1/lib)

add_executable(FFmpegEdu main.cpp rtmp/cpp/RtmpPush.cpp rtmp/include/RtmpPush.h rtmp/cpp/SaveFileByRtmp.cpp rtmp/include/SaveFileByRtmp.h rtmp/cpp/NetStreamForward.cpp rtmp/include/NetStreamForward.h rtmp/cpp/GlobalVariable.cpp rtmp/include/GlobalVariable.h video/cpp/LocalYuvToH264.cpp video/include/LocalYuvToH264.h video/cpp/LocalYuvToHevc.cpp video/include/LocalYuvToHevc.h video/cpp/LocalVideoToYuv.cpp video/include/LocalVideoToYuv.h)

target_link_libraries(
        FFmpegEdu
        avutil
        avdevice
        avcodec
        avformat
        postproc
        swresample
        swscale
)