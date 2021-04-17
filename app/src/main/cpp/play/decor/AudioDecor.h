//
// Created by 28188 on 2021/4/12.
//

#ifndef FFMPEDDEMO_AUDIODECOR_H
#define FFMPEDDEMO_AUDIODECOR_H


#include <jni.h>
class AudioDecor {

public:
    AudioDecor(JNIEnv *jniEnv,char *string);
    void CreateEngine();
};

#endif //FFMPEDDEMO_AUDIODECOR_H
