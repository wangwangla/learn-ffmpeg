//
// Created by 28188 on 2021/4/12.
//

#ifndef FFMPEDDEMO_AUDIODECOR_H
#define FFMPEDDEMO_AUDIODECOR_H

#include <jni.h>
#include <decor/base/PlayBase.h>
extern "C"{

#include <libswresample/swresample.h>

};

class AudioDecor : public PlayBase{
public:
    AudioDecor(JNIEnv *env,jobject instance,char *url);
    ~AudioDecor();
protected:
    virtual void codingLoop();
    virtual void codingReady();
    SwrContext *swrContext;
    jobject instance;
    JNIEnv *env;

};
#endif //FFMPEDDEMO_AUDIODECOR_H
