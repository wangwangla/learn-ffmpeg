//
// Created by 28188 on 2021/4/13.
//

#ifndef FFMPEDDEMO_VIDEODECODER_H
#define FFMPEDDEMO_VIDEODECODER_H

#include <decor/base/PlayBase.h>
#include <android/native_window.h>
#include "jni.h"
extern "C"{
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
#include <libavcodec/version.h>
#include <libavcodec/avcodec.h>
#include <libavformat/version.h>
#include <libavutil/version.h>
#include <libavfilter/version.h>
#include <libswresample/version.h>
#include <libswscale/version.h>
#include <libavformat/avformat.h>
#include <libswresample/swresample.h>
};

class VideoDecoder: public PlayBase{
public:
    VideoDecoder(char *url);
    ~VideoDecoder();
    void createWindow(JNIEnv *env,jobject surface);
    void initInstance();
protected:

    virtual void codingLoop();
    virtual void codingReady();



private:
    ANativeWindow *nativeWindow;
    AVCodec *aVCodec;
    AVCodecContext *avCodecContext;
    AVPacket *avPacket;
    SwsContext *swsContext;
    ANativeWindow_Buffer outBuffer;
};
#endif //FFMPEDDEMO_VIDEODECODER_H
