//
// Created by 28188 on 2021/4/12.
//

#ifndef FFMPEDDEMO_AUDIODECOR_H
#define FFMPEDDEMO_AUDIODECOR_H

extern "C" {
#include <libavformat/avformat.h>
#include <android/native_window.h>
#include <pthread.h>
#include <SLES/OpenSLES_Android.h>
#include <libswresample/swresample.h>
}
#include <jni.h>
#include <thread>
using namespace std;
class AudioDecor {

public:
    AudioDecor(JNIEnv *jniEnv,jobject instance,char *string);
    static void play(AudioDecor *audioDecor);
    void xx();
private:


protected:
    AVCodecContext *pCodecCtx;
    AVFormatContext *pFormatCtx;
    AVPacket *packet;
    int audio_stream_idx=-1;
    AVFrame *frame;
    SwrContext *swrContext;
    JNIEnv *env;
    jobject instance;
    thread *m_Thread = nullptr;
};

#endif //FFMPEDDEMO_AUDIODECOR_H
