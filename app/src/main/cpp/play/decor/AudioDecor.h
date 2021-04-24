//
// Created by 28188 on 2021/4/12.
//

#ifndef FFMPEDDEMO_AUDIODECOR_H
#define FFMPEDDEMO_AUDIODECOR_H

#include <jni.h>
#include <decor/base/PlayBase.h>


class AudioDecor : public PlayBase{
public:
    AudioDecor(char *url);
    ~AudioDecor();
protected:
    virtual void codingLoop();
    virtual void codingReady();
    virtual void play();
    virtual void stop();
    virtual void pause();
    virtual void resume();

};
#endif //FFMPEDDEMO_AUDIODECOR_H
