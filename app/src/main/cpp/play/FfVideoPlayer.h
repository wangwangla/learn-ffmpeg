//
// Created by 28188 on 2021/9/20.
//

#ifndef LEARN_FFMPEG_FFVIDEOPLAYER_H
#define LEARN_FFMPEG_FFVIDEOPLAYER_H

#include <thread>
#include <VideoDecoder.h>
#include "FfVideoPlay.h"
#include "FfAudioPlay.h"

using namespace std;
class FfVideoPlayer {
public:
    FfVideoPlayer();
public:
    FfVideoPlayer(const char *string);
    FfVideoPlay *ffVideoPlay;
    FfAudioPlay *ffAudioPlay;
    int video_stream_index;
    int audeo_stream_index;
    AVFormatContext *formatContext;
    char *m_url;
protected:
    void init();
    void initDecor();
    void play();
};


#endif //LEARN_FFMPEG_FFVIDEOPLAYER_H
