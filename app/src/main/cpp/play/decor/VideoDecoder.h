//
// Created by 28188 on 2021/4/13.
//

#ifndef FFMPEDDEMO_VIDEODECODER_H
#define FFMPEDDEMO_VIDEODECODER_H

#include <decor/base/PlayBase.h>
#include "jni.h"

class VideoDecoder: public PlayBase{
public:
    VideoDecoder(char *url);
    ~VideoDecoder();
protected:
    virtual void codingLoop();
    virtual void codingReady();
    virtual void play();
    virtual void stop();
    virtual void pause();
    virtual void resume();

};
#endif //FFMPEDDEMO_VIDEODECODER_H
