//
// Created by 28188 on 2021/4/12.
//

extern "C" {
#include <libavformat/avformat.h>
#include <android/native_window.h>
#include <pthread.h>
#include <SLES/OpenSLES_Android.h>
#include <libswresample/swresample.h>
}
#include <thread>
#include <LogUtils.h>

#include "AudioDecor.h"

AudioDecor::AudioDecor(JNIEnv *env,jobject instance,char *input) {
    av_register_all();
    this->env = env;
    this->instance = instance;
    pFormatCtx = avformat_alloc_context();
    //open
    if (avformat_open_input(&pFormatCtx, input, NULL, NULL) != 0) {
        LOGCATE("%s","打开输入视频文件失败");
        return;
    }
    //获取视频信息
    if(avformat_find_stream_info(pFormatCtx,NULL) < 0){
        LOGCATE("%s","获取视频信息失败");
        return;
    }

    int i=0;
    for (int i = 0; i < pFormatCtx->nb_streams; ++i) {
        if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO) {
            LOGCATE("  找到音频id %d", pFormatCtx->streams[i]->codec->codec_type);
            audio_stream_idx=i;
            break;
        }
    }
    //获取解码器上下文
    pCodecCtx=pFormatCtx->streams[audio_stream_idx]->codec;
    //获取解码器
    AVCodec *pCodex = avcodec_find_decoder(pCodecCtx->codec_id);
    //打开解码器
    if (avcodec_open2(pCodecCtx, pCodex, NULL)<0) {
    }
    //申请avpakcet，装解码前的数据
    packet = (AVPacket *)av_malloc(sizeof(AVPacket));
    //申请avframe，装解码后的数据
    frame = av_frame_alloc();
    //得到SwrContext ，进行重采样，具体参考http://blog.csdn.net/jammg/article/details/52688506
    SwrContext *swrContext = swr_alloc();
    //缓存区

//输出的声道布局（立体声）
    uint64_t  out_ch_layout=AV_CH_LAYOUT_STEREO;
//输出采样位数  16位
    enum AVSampleFormat out_formart=AV_SAMPLE_FMT_S16;
//输出的采样率必须与输入相同
    int out_sample_rate = pCodecCtx->sample_rate;

//swr_alloc_set_opts将PCM源文件的采样格式转换为自己希望的采样格式
    swr_alloc_set_opts(swrContext, out_ch_layout, out_formart, out_sample_rate,
                       pCodecCtx->channel_layout, pCodecCtx->sample_fmt, pCodecCtx->sample_rate, 0,
                       NULL);


    m_Thread = new thread(play,this);

}

void AudioDecor::play(AudioDecor *audioDecor) {
   audioDecor->xx();
}

void AudioDecor::xx(){
    swr_init(swrContext);
//    获取通道数  2
    int out_channer_nb = av_get_channel_layout_nb_channels(AV_CH_LAYOUT_STEREO);
//    反射得到Class类型
    jclass david_player = env->GetObjectClass(instance);
//    反射得到createAudio方法
    jmethodID createAudio = env->GetMethodID(david_player, "createTrack", "(II)V");
//    反射调用createAudio
    env->CallVoidMethod(instance, createAudio, 44100, out_channer_nb);

    jmethodID audio_write = env->GetMethodID(david_player, "playTrack", "([BI)V");

    int got_frame;
    uint8_t *out_buffer = (uint8_t *) av_malloc(44100 * 2);
    while (av_read_frame(pFormatCtx, packet) >= 0) {
        if (packet->stream_index == audio_stream_idx) {
//            解码  mp3   编码格式frame----pcm   frame
            avcodec_decode_audio4(pCodecCtx, frame, &got_frame, packet);
            if (got_frame) {
                LOGCATE("解码");
                swr_convert(swrContext, &out_buffer, 44100 * 2, (const uint8_t **) frame->data, frame->nb_samples);
//                缓冲区的大小
                int size = av_samples_get_buffer_size(NULL, out_channer_nb, frame->nb_samples,
                                                      AV_SAMPLE_FMT_S16, 1);
                jbyteArray audio_sample_array = env->NewByteArray(size);
                env->SetByteArrayRegion(audio_sample_array, 0, size, (const jbyte *) out_buffer);
                env->CallVoidMethod(instance, audio_write, audio_sample_array, size);
                env->DeleteLocalRef(audio_sample_array);
            }
        }
    }
    av_frame_free(&frame);
    swr_free(&swrContext);
    avcodec_close(pCodecCtx);
    avformat_close_input(&pFormatCtx);
}