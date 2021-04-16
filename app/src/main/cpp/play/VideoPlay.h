//
// Created by 28188 on 2021/4/13.
//

#ifndef FFMPEDDEMO_VIDEOPLAY_H
#define FFMPEDDEMO_VIDEOPLAY_H

#include <jni.h>
#include "decor/VideoDecoder.h"
#include "decor/AudioDecor.h"
#include <string>
#include <android/native_window_jni.h>
#include <LogUtils.h>


class VideoPlay {
public:
    VideoPlay(){};
    ~VideoPlay(){};

    void Init(JNIEnv *jniEnv, jobject obj, char *url, jobject surface);
    void UnInit();

    void Play();
    void Pause();
    void Stop();
    void SeekToPosition(float position);
    long GetMediaParams(int paramType);

private:
    JNIEnv *GetJNIEnv(bool *isAttach);
    jobject GetJavaObj();
    JavaVM *GetJavaVM();

    static void PostMessage(void *context, int msgType, float msgCode);

    JavaVM *m_JavaVM = nullptr;
    jobject m_JavaObj = nullptr;

    VideoDecoder *m_VideoDecoder = nullptr;
    AudioDecor *m_AudioDecoder = nullptr;
//
//    VideoRender *m_VideoRender = nullptr;
//    AudioRender *m_AudioRender = nullptr;

};


#endif //LEARNFFMPEG_FFMEDIAPLAYER_H
