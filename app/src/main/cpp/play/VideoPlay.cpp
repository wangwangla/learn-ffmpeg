//
// Created by 28188 on 2021/4/13.
//

#include "VideoPlay.h"

VideoPlay::VideoPlay(JNIEnv *env,char *url,jobject surface) {
    LOGCATE("Video play create--------------------");
    VideoDecoder *decoder = new VideoDecoder(url);
    decoder->createWindow(env,surface);
    decoder->initInstance();
//    AudioDecor *audioDecor = new AudioDecor(url);
}

