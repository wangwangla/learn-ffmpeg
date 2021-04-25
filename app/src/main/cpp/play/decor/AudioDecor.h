//
// Created by 28188 on 2021/4/12.
//

#ifndef FFMPEDDEMO_AUDIODECOR_H
#define FFMPEDDEMO_AUDIODECOR_H

#include <jni.h>
#include <decor/base/PlayBase.h>
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>

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

protected:
    int createFFmpeg(int *rate, int *channel);

    int getPcm(void **pcm, size_t *pcm_size);

    void realseFFmpeg();

    static void getQueueCallBack(SLAndroidSimpleBufferQueueItf  slBufferQueueItf, void* context);
    void xx();

//    AVFormatContext *pFormatCtx;
    AVCodecContext *pCodecCtx;
    AVCodec *pCodex;
    AVPacket *packet;
    AVFrame *frame;
    SwrContext *swrContext;
    uint8_t *out_buffer;
    int out_channer_nb;
//    int audio_stream_idx=-1;
    SLObjectItf engineObject=NULL;//用SLObjectItf声明引擎接口对象
    SLEngineItf engineEngine = NULL;//声明具体的引擎对象
    SLObjectItf outputMixObject = NULL;//用SLObjectItf创建混音器接口对象
    SLEnvironmentalReverbItf outputMixEnvironmentalReverb = NULL;////具体的混音器对象实例
    SLEnvironmentalReverbSettings settings = SL_I3DL2_ENVIRONMENT_PRESET_DEFAULT;//默认情况
    SLObjectItf audioplayer=NULL;//用SLObjectItf声明播放器接口对象
    SLPlayItf  slPlayItf=NULL;//播放器接口
    SLAndroidSimpleBufferQueueItf  slBufferQueueItf=NULL;//缓冲区队列接口
    size_t buffersize =0;
    void *buffer;

    void createEngine();

    void createMixVolume();

    void createPlayer();
};
#endif //FFMPEDDEMO_AUDIODECOR_H
