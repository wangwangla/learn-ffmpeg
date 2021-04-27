//
// Created by 28188 on 2021/4/12.
//
#include "AudioDecor.h"
#include "LogUtils.h"
#define LOGE(FORMAT,...) __android_log_print(ANDROID_LOG_ERROR,"LC",FORMAT,##__VA_ARGS__);


void AudioDecor::codingReady() {
    LOGCATE("Audio codingready ------------------");
    already = true;
}

void AudioDecor::codingLoop() {
    LOGCATE("aloop--------");
    createPlayer();
}

AudioDecor::~AudioDecor(){
    LOGCATE("Audio :xigou");
}
AudioDecor *audioDecor;
int AudioDecor::createFFmpeg(int *rate,int *channel){
    AVCodecParameters *avCodecParameters = formatContext->streams[audeo_stream_index]->codecpar;
//    获取音频编解码器   废弃
//    pCodecCtx=avcodec_find_decoder(avCodecParameters->codec_id);
    LOGE("获取视频编码器上下文 %p  ",pCodecCtx);
    pCodex = avcodec_find_decoder(pCodecCtx->codec_id);
    pCodecCtx = avcodec_alloc_context3(pCodex);
    LOGE("获取视频编码 %p",pCodex);
    if (avcodec_open2(pCodecCtx, pCodex, NULL)<0) {
    }
    packet = (AVPacket *)av_malloc(sizeof(AVPacket));
//    音频数据
    frame = av_frame_alloc();
//    mp3  里面所包含的编码格式   转换成  pcm   SwcContext
    swrContext = swr_alloc();
//    44100*2
    out_buffer = (uint8_t *) av_malloc(44100 * 2);
    uint64_t  out_ch_layout=AV_CH_LAYOUT_STEREO;
//    输出采样位数  16位
    enum AVSampleFormat out_formart=AV_SAMPLE_FMT_S16;
//输出的采样率必须与输入相同
    int out_sample_rate = pCodecCtx->sample_rate;
    swr_alloc_set_opts(
            swrContext,
            out_ch_layout,
            out_formart,
            out_sample_rate,
            pCodecCtx->channel_layout,
            pCodecCtx->sample_fmt,
            pCodecCtx->sample_rate,
            0,
            NULL);
    swr_init(swrContext);
//    获取通道数  2
    out_channer_nb =
            av_get_channel_layout_nb_channels(
                    AV_CH_LAYOUT_STEREO);
    *rate = pCodecCtx->sample_rate;
    *channel = pCodecCtx->channels;
    return 0;
}
//
    int AudioDecor::getPcm(void **pcm,size_t *pcm_size){
        int frameCount=0;
        int got_frame;
        while (av_read_frame(formatContext, packet) >= 0) {
            if (packet->stream_index == audeo_stream_index) {
    //            解码  mp3   编码格式frame----pcm   frame
                avcodec_decode_audio4(pCodecCtx, frame, &got_frame, packet);
                if (got_frame) {
                    LOGE("解码");
                    /**
                     * int swr_convert(struct SwrContext *s, uint8_t **out, int out_count,
                                    const uint8_t **in , int in_count);
                     */
                    swr_convert(swrContext, &out_buffer,
                            44100 * 2, (const uint8_t **) frame->data, frame->nb_samples);
    //                缓冲区的大小
                    int size = av_samples_get_buffer_size(NULL,
                            out_channer_nb, frame->nb_samples,
                                                          AV_SAMPLE_FMT_S16, 1);
                    *pcm = out_buffer;
                    *pcm_size = size;
                    break;
                }
            }
        }
        return 0;
    }


//void realseFFmpeg(){
//    av_free_packet(packet);
//    av_free(out_buffer);
//    av_frame_free(&frame);
//    swr_free(&swrContext);
//    avcodec_close(pCodecCtx);
//    avformat_close_input(&pFormatCtx);
//}


//将pcm数据添加到缓冲区中
void AudioDecor::getQueueCallBack(SLAndroidSimpleBufferQueueItf  slBufferQueueItf, void* context){
//    if (audioDemo==NULL){
    audioDecor->xx();
//    }
}

void AudioDecor::xx() {
    buffersize=0;
    getPcm(&buffer,&buffersize);
    if(buffer!=NULL&&buffersize!=0){
        //将得到的数据加入到队列中
        (*slBufferQueueItf)->Enqueue(slBufferQueueItf,buffer,buffersize);
    }
}
//创建引擎
void AudioDecor::createEngine(){
    slCreateEngine(&engineObject,0,NULL,0,NULL,NULL);//创建引擎
    (*engineObject)->Realize(engineObject,SL_BOOLEAN_FALSE);//实现engineObject接口对象
    (*engineObject)->GetInterface(engineObject,SL_IID_ENGINE,&engineEngine);//通过引擎调用接口初始化SLEngineItf
}

//创建混音器
void AudioDecor::createMixVolume(){
    (*engineEngine)->CreateOutputMix(engineEngine,&outputMixObject,0,0,0);//用引擎对象创建混音器接口对象
    (*outputMixObject)->Realize(outputMixObject,SL_BOOLEAN_FALSE);//实现混音器接口对象
    SLresult   sLresult = (*outputMixObject)->GetInterface(outputMixObject,SL_IID_ENVIRONMENTALREVERB,&outputMixEnvironmentalReverb);//利用混音器实例对象接口初始化具体的混音器对象
    //设置
    if (SL_RESULT_SUCCESS == sLresult) {
        (*outputMixEnvironmentalReverb)->
                SetEnvironmentalReverbProperties(outputMixEnvironmentalReverb, &settings);
    }
}

//创建播放器
void AudioDecor::createPlayer(){
    //初始化ffmpeg
    int rate;
    int channels;
    createFFmpeg(&rate,&channels);
    LOGE("RATE %d",rate);
    LOGE("channels %d",channels);
    SLDataLocator_AndroidBufferQueue android_queue = {SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE,2};
    /**
    typedef struct SLDataFormat_PCM_ {
        SLuint32 		formatType;  pcm
        SLuint32 		numChannels;  通道数
        SLuint32 		samplesPerSec;  采样率
        SLuint32 		bitsPerSample;  采样位数
        SLuint32 		containerSize;  包含位数
        SLuint32 		channelMask;     立体声
        SLuint32		endianness;    end标志位
    } SLDataFormat_PCM;
     */
    SLDataFormat_PCM pcm = {SL_DATAFORMAT_PCM,static_cast<SLuint32>(channels),
                            static_cast<SLuint32>(rate*1000)
            ,SL_PCMSAMPLEFORMAT_FIXED_16
            ,SL_PCMSAMPLEFORMAT_FIXED_16
            ,SL_SPEAKER_FRONT_LEFT|SL_SPEAKER_FRONT_RIGHT,SL_BYTEORDER_LITTLEENDIAN};
    SLDataSource dataSource = {&android_queue,&pcm};
    SLDataLocator_OutputMix slDataLocator_outputMix={SL_DATALOCATOR_OUTPUTMIX,outputMixObject};
    SLDataSink slDataSink = {&slDataLocator_outputMix,NULL};
    const SLInterfaceID ids[3]={SL_IID_BUFFERQUEUE,SL_IID_EFFECTSEND,SL_IID_VOLUME};
    const SLboolean req[3]={SL_BOOLEAN_FALSE,SL_BOOLEAN_FALSE,SL_BOOLEAN_FALSE};
    LOGE("执行到此处");
    (*engineEngine)->CreateAudioPlayer(engineEngine,&audioplayer,&dataSource,&slDataSink,3,ids,req);
    (*audioplayer)->Realize(audioplayer,SL_BOOLEAN_FALSE);
    LOGE("执行到此处2");
    (*audioplayer)->GetInterface(audioplayer,SL_IID_PLAY,&slPlayItf);//初始化播放器
    //注册缓冲区,通过缓冲区里面 的数据进行播放
    (*audioplayer)->GetInterface(audioplayer,SL_IID_BUFFERQUEUE,&slBufferQueueItf);
    //设置回调接口
    (*slBufferQueueItf)->RegisterCallback(slBufferQueueItf,getQueueCallBack,NULL);
    //播放
    (*slPlayItf)->SetPlayState(slPlayItf,SL_PLAYSTATE_PLAYING);
    //开始播放
    getQueueCallBack(slBufferQueueItf,NULL);
}
AudioDecor ::AudioDecor(JNIEnv *env, jobject instance, char *url) : PlayBase(url) {
    LOGCATE("Audio create ---------------------");
    audioDecor = this;
    createEngine();
    createMixVolume();
    play();
}