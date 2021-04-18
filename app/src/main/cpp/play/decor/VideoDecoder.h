//
// Created by 28188 on 2021/4/13.
//

#ifndef FFMPEDDEMO_VIDEODECODER_H
#define FFMPEDDEMO_VIDEODECODER_H


#include <jni.h>
extern "C"{
#include <libavcodec/avcodec.h>
#include <android/native_window.h>
#include <libavformat/avformat.h>
};
#include "AudioDecor.h"
#include <thread>
using namespace std;
class VideoDecoder {

public:
    VideoDecoder(JNIEnv *jniEnv, char *url);
    void Init(JNIEnv *jniEnv, jobject obj, char *url, jobject surface);
    void Play();

private:
    AVCodecContext *avCodecContex;
    ANativeWindow  *nativeWindow;
    AVFormatContext *formatContext;
    int ret;
    AVCodec *vcode;
    thread *m_Thread = nullptr;
    static void DoAVDecoding(VideoDecoder *videoDecoder);
    void DoLoop();
};



#endif //FFMPEDDEMO_VIDEODECODER_H
