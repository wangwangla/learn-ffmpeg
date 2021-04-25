//
// Created by 28188 on 2021/4/12.
//
#include "AudioDecor.h"
#include "LogUtils.h"

AudioDecor ::AudioDecor(JNIEnv *env, jobject instance, char *url) : PlayBase(url) {
    LOGCATE("Audio create ---------------------");
//    this->env = env;
//    this->instance = instance;
//    play();
}

void AudioDecor::codingReady() {
    LOGCATE("Audio codingready ------------------");
    already = true;
}

void AudioDecor::codingLoop() {
    LOGCATE("aloop--------");
}

AudioDecor::~AudioDecor(){
    LOGCATE("Audio :xigou");
}
