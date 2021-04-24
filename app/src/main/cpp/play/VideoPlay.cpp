//
// Created by 28188 on 2021/4/13.
//

#include "VideoPlay.h"

VideoPlay::VideoPlay(char *url) {
    LOGCATE("Video play create--------------------");
    VideoDecoder *decoder = new VideoDecoder(url);
    AudioDecor *audioDecor = new AudioDecor(url);
}