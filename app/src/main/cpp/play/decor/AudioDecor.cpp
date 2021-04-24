//
// Created by 28188 on 2021/4/12.
//
#include "AudioDecor.h"
#include "LogUtils.h"

AudioDecor ::AudioDecor(char *url) : PlayBase(url) {
    LOGCATE("Audio create ---------------------");
}

void AudioDecor::codingReady() {
    LOGCATE("Audio codingready ------------------");
    already = true;
}

void AudioDecor::codingLoop() {
    LOGCATE("Audio loop--------");
}

AudioDecor::~AudioDecor(){
    LOGCATE("Audio :xigou");
}
