//
// Created by 28188 on 2021/4/12.
//
#include "AudioDecor.h"
#include "LogUtils.h"

AudioDecor ::AudioDecor(JNIEnv *env, jobject instance, char *url) : PlayBase(url) {
    LOGCATE("Audio create ---------------------");
    this->env = env;
    this->instance = instance;
    play();
}

void AudioDecor::codingReady() {
    LOGCATE("Audio codingready ------------------");
    already = true;
}

void AudioDecor::codingLoop() {
    LOGCATE("aloop--------");
    //获取解码器上下文
    AVCodecContext *pCodecCtx=formatContext->streams[audeo_stream_index]->codec;
    //获取解码器
    AVCodec *pCodex = avcodec_find_decoder(pCodecCtx->codec_id);
    //打开解码器
    if (avcodec_open2(pCodecCtx, pCodex, NULL)<0) {
    }
    //申请avpakcet，装解码前的数据
    AVPacket *packet = (AVPacket *)av_malloc(sizeof(AVPacket));
    //申请avframe，装解码后的数据
    AVFrame *frame = av_frame_alloc();
    //得到SwrContext ，进行重采样，具体参考http://blog.csdn.net/jammg/article/details/52688506
    swrContext = swr_alloc();
    //缓存区
    uint8_t *out_buffer = (uint8_t *) av_malloc(44100 * 2);
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
    while (av_read_frame(formatContext, packet) >= 0) {
        if (packet->stream_index == audeo_stream_index) {
//            解码  mp3   编码格式frame----pcm   frame
            avcodec_decode_audio4(pCodecCtx, frame, &got_frame, packet);
            if (got_frame) {
                LOGCATE("音乐解码");
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
    avformat_close_input(&formatContext);
}

AudioDecor::~AudioDecor(){
    LOGCATE("Audio :xigou");
}

void AudioDecor::createInstance() {
    slCreateEngine(&engineObject,0,NULL,0,NULL,NULL);//创建引擎
    (*engineObject)->Realize(engineObject,SL_BOOLEAN_FALSE);//实现engineObject接口对象
    (*engineObject)->GetInterface(engineObject,SL_IID_ENGINE,&engineEngine);//通过引擎调用接口初始化SLEngineItf
}

void AudioDecor::createMixVolue() {
    (*engineEngine)->CreateOutputMix(engineEngine,&outputMixObject,0,0,0);//用引擎对象创建混音器接口对象
    (*outputMixObject)->Realize(outputMixObject,SL_BOOLEAN_FALSE);//实现混音器接口对象
    SLresult   sLresult = (*outputMixObject)->GetInterface(outputMixObject,SL_IID_ENVIRONMENTALREVERB,&outputMixEnvironmentalReverb);//利用混音器实例对象接口初始化具体的混音器对象
    //设置
    if (SL_RESULT_SUCCESS == sLresult) {
        (*outputMixEnvironmentalReverb)->
                SetEnvironmentalReverbProperties(outputMixEnvironmentalReverb, &settings);
    }
}

void AudioDecor::createPlayer() {
    //创建播放器
        //初始化ffmpeg
        int rate;
        int channels;
        createFFmpeg(&rate,&channels);
        LOGCATE("RATE %d",rate);
        LOGCATE("channels %d",channels);
        /*
         * typedef struct SLDataLocator_AndroidBufferQueue_ {
        SLuint32    locatorType;//缓冲区队列类型
        SLuint32    numBuffers;//buffer位数
    } */

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

        /*
         * typedef struct SLDataSource_ {
                void *pLocator;//缓冲区队列
                void *pFormat;//数据样式,配置信息
            } SLDataSource;
         * */
        SLDataSource dataSource = {&android_queue,&pcm};


        SLDataLocator_OutputMix slDataLocator_outputMix={SL_DATALOCATOR_OUTPUTMIX,outputMixObject};


        SLDataSink slDataSink = {&slDataLocator_outputMix,NULL};


        const SLInterfaceID ids[3]={SL_IID_BUFFERQUEUE,SL_IID_EFFECTSEND,SL_IID_VOLUME};
        const SLboolean req[3]={SL_BOOLEAN_FALSE,SL_BOOLEAN_FALSE,SL_BOOLEAN_FALSE};

        /*
         * SLresult (*CreateAudioPlayer) (
            SLEngineItf self,
            SLObjectItf * pPlayer,
            SLDataSource *pAudioSrc,//数据设置
            SLDataSink *pAudioSnk,//关联混音器
            SLuint32 numInterfaces,
            const SLInterfaceID * pInterfaceIds,
            const SLboolean * pInterfaceRequired
        );
         * */
        LOGCATE("执行到此处");
                (*engineEngine)->CreateAudioPlayer(engineEngine,&audioplayer,&dataSource,&slDataSink,3,ids,req);
        (*audioplayer)->Realize(audioplayer,SL_BOOLEAN_FALSE);
        LOGCATE("执行到此处2");
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

int AudioDecor::createFFmpeg(int *rate,int *channel){
//    av_register_all();
//    char *input = "/sdcard/1.mp3";
//    pFormatCtx = avformat_alloc_context();
//    LOGE("Lujng %s",input);
//    LOGE("xxx %p",pFormatCtx);
//    int error;
//    char buf[] = "";
//    //打开视频地址并获取里面的内容(解封装)
//    if (error = avformat_open_input(&pFormatCtx, input, NULL, NULL) < 0) {
//        av_strerror(error, buf, 1024);
//        // LOGE("%s" ,inputPath)
//        LOGE("Couldn't open file %s: %d(%s)", input, error, buf);
//        // LOGE("%d",error)
//        LOGE("打开视频失败")
//    }
//    //3.获取视频信息
//    if(avformat_find_stream_info(pFormatCtx,NULL) < 0){
//        LOGE("%s","获取视频信息失败");
//        return -1;
//    }
//




//    int i=0;
//    for (int i = 0; i < formatContext->nb_streams; ++i) {
//        if (formatContext->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO) {
//            LOGE("  找到音频id %d", formatContext->streams[i]->codec->codec_type);
//            audeo_stream_index=i;
//            break;
//        }
//    }
// mp3的解码器

//    获取音频编解码器
    pCodecCtx=pFormatCtx->streams[audio_stream_idx]->codec;
    LOGE("获取视频编码器上下文 %p  ",pCodecCtx);

    pCodex = avcodec_find_decoder(pCodecCtx->codec_id);
    LOGE("获取视频编码 %p",pCodex);

    if (avcodec_open2(pCodecCtx, pCodex, NULL)<0) {
    }
    packet = (AVPacket *)av_malloc(sizeof(AVPacket));
//    av_init_packet(packet);
//    音频数据

    frame = av_frame_alloc();

//    mp3  里面所包含的编码格式   转换成  pcm   SwcContext
    swrContext = swr_alloc();

    int length=0;
    int got_frame;
//    44100*2
    out_buffer = (uint8_t *) av_malloc(44100 * 2);
    uint64_t  out_ch_layout=AV_CH_LAYOUT_STEREO;
//    输出采样位数  16位
    enum AVSampleFormat out_formart=AV_SAMPLE_FMT_S16;
//输出的采样率必须与输入相同
    int out_sample_rate = pCodecCtx->sample_rate;


    swr_alloc_set_opts(swrContext, out_ch_layout, out_formart, out_sample_rate,
                       pCodecCtx->channel_layout, pCodecCtx->sample_fmt, pCodecCtx->sample_rate, 0,
                       NULL);

    swr_init(swrContext);
//    获取通道数  2
    out_channer_nb = av_get_channel_layout_nb_channels(AV_CH_LAYOUT_STEREO);
    *rate = pCodecCtx->sample_rate;
    *channel = pCodecCtx->channels;
    return 0;
}
//
int getPcm(void **pcm,size_t *pcm_size){
    int frameCount=0;
    int got_frame;
    while (av_read_frame(pFormatCtx, packet) >= 0) {
        if (packet->stream_index == audio_stream_idx) {
//            解码  mp3   编码格式frame----pcm   frame
            avcodec_decode_audio4(pCodecCtx, frame, &got_frame, packet);
            if (got_frame) {
                LOGE("解码");
                /**
                 * int swr_convert(struct SwrContext *s, uint8_t **out, int out_count,
                                const uint8_t **in , int in_count);
                 */
                swr_convert(swrContext, &out_buffer, 44100 * 2, (const uint8_t **) frame->data, frame->nb_samples);
//                缓冲区的大小
                int size = av_samples_get_buffer_size(NULL, out_channer_nb, frame->nb_samples,
                                                      AV_SAMPLE_FMT_S16, 1);
                *pcm = out_buffer;
                *pcm_size = size;
                break;
            }
        }
    }
    return 0;
}

//
//#include <SDL.h>
//#define SDL_AUDIO_BUFFER_SIZE 1024
//typedef struct PacketQueue {
//    AVPacketList * first_pkt, *last_pkt;
//    int nb_packets;
//    int size;
//    SDL_mutex *mutex;		//mutex主要是用来实现资源的互拆的，跟java里在synchronize关键作用有相似之处
//    //，虽然本和序中没有用多线程，但sdl在播放另启了一个线程。
//    SDL_cond * cond;
//} PacketQueue;
//PacketQueue audioq;
//
//void packet_queue_init(PacketQueue *q) {
//    memset(q, 0, sizeof(PacketQueue));
//    q->mutex = SDL_CreateMutex();
//    q->cond = SDL_CreateCond();
//}
//
//int packet_queue_put(PacketQueue *q, AVPacket *pkt) {
//
//    AVPacketList *pkt1;
//    if (av_dup_packet(pkt) < 0)
//        return -1;
//    pkt1 = av_malloc(sizeof(AVPacketList));
//    if (!pkt1)
//        return -1;
//    pkt1->pkt = *pkt;
//    pkt1->next = NULL;
//
//    SDL_LockMutex(q->mutex);
//
//    if (!q->last_pkt)
//        q->first_pkt = pkt1;
//    else
//        q->last_pkt->next = pkt1;
//    q->last_pkt = pkt1;
//    q->nb_packets++;
//    q->size += pkt1->pkt.size;
//    SDL_CondSignal(q->cond);
//
//    SDL_UnlockMutex(q->mutex);
//    return 0;
//}
//
//int quit = 0;
//static int packet_queue_get(PacketQueue *q, AVPacket *pkt, int block) {
//    AVPacketList *pkt1;
//    int ret;
//
//    SDL_LockMutex(q->mutex);
//
//    for (;;) {
//
//        if (quit) {
//            ret = -1;
//            break;
//        }
//
//        pkt1 = q->first_pkt;
//        if (pkt1) {
//            q->first_pkt = pkt1->next;
//            if (!q->first_pkt)
//                q->last_pkt = NULL;
//            q->nb_packets--;
//            q->size -= pkt1->pkt.size;
//            *pkt = pkt1->pkt;
//            av_free(pkt1);			//这招我很赞赏，他在取去一个packet后，将上一个paket置空，而在下面的程序就不用再调用av_free操作了
//            ret = 1;
//            break;
//        } else if (!block) {
//            ret = 0;
//            break;
//        } else {
//            SDL_CondWait(q->cond, q->mutex);
//        }
//    }
//    SDL_UnlockMutex(q->mutex);
//    return ret;
//}
//
////int audio_decode_frame(AVCodecContext *aCodecCtx, uint8_t *audio_buf, int buf_size) {
//int audio_decode_frame(AVCodecContext *aCodecCtx, AVFrame *frame,
//                       uint8_t *audio_buf) {
//    static AVPacket pkt_temp;
//    int len1, data_size, got_frame;
//    int new_packet;
//    for (;;) {
//        while (pkt_temp.size > 0 || (!pkt_temp.data && new_packet)) {
//            if (!frame) {
//                if (!(frame = avcodec_alloc_frame()))
//                    return AVERROR(ENOMEM);
//            } else {
//                avcodec_get_frame_defaults(frame);
//            }
//            new_packet = 0;
//
//            len1 = avcodec_decode_audio4(aCodecCtx, frame, &got_frame,
//                                         &pkt_temp);
//            if (len1 < 0) {
//                /* if error, skip frame */
//                pkt_temp.size = 0;
//                break;
//            }
//            pkt_temp.data += len1;
//            pkt_temp.size -= len1;
//
//            if (got_frame <= 0) /* No data yet, get more frames */
//                continue;
//            data_size = av_samples_get_buffer_size(NULL, aCodecCtx->channels,
//                                                   frame->nb_samples, aCodecCtx->sample_fmt, 1);
//            memcpy(audio_buf, frame->data[0], frame->linesize[0]);
//            /* We have data, return it and come back for more later */
//            return data_size;
//        }
//        if (quit)
//            return -1;
//
//        if ((new_packet = packet_queue_get(&audioq, &pkt_temp, 1)) < 0)
//            return -1;
//
//    }
//}
//
//void audio_callback(void *userdata, Uint8 *stream, int len) {
//    AVCodecContext *aCodecCtx = (AVCodecContext *) userdata;
//    //以后变量全定义成static ,确保下次循环，变量不会被初始化
//    static uint8_t audio_buf[(AVCODEC_MAX_AUDIO_FRAME_SIZE * 3) / 2];
//    static unsigned int audio_buf_remain_size=0;		//记录下audio_buffer剩余数据量
//    static unsigned int audio_buf_total_size=0;			//记录下audio_buffer总数据量
//    static unsigned int audio_buf_index = 0;
//
//    int read_size; //第次送入＊stream中数据的真正长度，理论值是len,但在最后一次操作实际值可能会小于len;
//
//    AVFrame *frame = NULL;
//    int flag=0;
//    while(len){
//        if(audio_buf_index>=audio_buf_total_size){
//            audio_buf_remain_size = audio_decode_frame(aCodecCtx, frame, audio_buf);
//            audio_buf_total_size=audio_buf_remain_size;
//            audio_buf_index=0;
//            if(audio_buf_total_size<0){
//                audio_buf_remain_size=audio_buf_total_size = 1024;
//                memset(audio_buf, 0, audio_buf_total_size);
//                continue;
//            }
//        }
//        read_size=(audio_buf_remain_size > len)? len : audio_buf_remain_size;
//        memcpy(stream, (uint8_t *) audio_buf + audio_buf_index, read_size);
//        audio_buf_index += read_size;
//        audio_buf_remain_size -= read_size;
//        stream += read_size;
//        len -= read_size;
//    }
//}