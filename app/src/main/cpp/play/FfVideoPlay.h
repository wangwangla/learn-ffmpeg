//
// Created by 28188 on 2021/9/20.
//

#ifndef FFMPEDDEMO_FFVIDEOPLAY_H
#define FFMPEDDEMO_FFVIDEOPLAY_H

#include <thread>
#include "FfVideoPlayer.h"

using namespace std;
class FfVideoPlay {
public:
    FfVideoPlayer *ffVideoPlayer;
    FfVideoPlay(FfVideoPlayer *pPlayer);
    JNIEnv *env;
    jobject surface;
    AVFormatContext *formatContext;
    int video_stream_index;
    thread *videoThread;
    void decoder();
    static void loop();
};


#endif //FFMPEDDEMO_FFVIDEOPLAY_H
