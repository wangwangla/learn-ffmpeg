//
// Created by 28188 on 2021/4/13.
//

#ifndef FFMPEDDEMO_VIDEOPLAY_H
#define FFMPEDDEMO_VIDEOPLAY_H

#include <jni.h>
extern "C"{

};

#include "decor/VideoDecoder.h"
#include "decor/AudioDecor.h"
#include <string>
#include <android/native_window_jni.h>
#include <LogUtils.h>


class VideoPlay {
public:
    VideoPlay(JNIEnv *env,char *url,jobject surface);
};


#endif //LEARNFFMPEG_FFMEDIAPLAYER_H
