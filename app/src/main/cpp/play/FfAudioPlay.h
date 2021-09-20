//
// Created by 28188 on 2021/9/20.
//

#ifndef FFMPEDDEMO_FFAUDIOPLAY_H
#define FFMPEDDEMO_FFAUDIOPLAY_H


#include "FfVideoPlayer.h"

class FfAudioPlay{
public:
    FfVideoPlayer *ffVideoPlayer;
    FfAudioPlay(FfVideoPlayer *pPlayer);
    AVFormatContext *pFormatCtx{};
    int audio_stream_idx{};
    JNIEnv *env{};
    jobject thiz{},
    FfAudioPlay();
    void initDecor();
    thread *audioThread;
    static void audioLoop();
};


#endif //FFMPEDDEMO_FFAUDIOPLAY_H
