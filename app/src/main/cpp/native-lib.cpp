#include <jni.h>
#include <string>

#include <LogUtils.h>
#include <decor/VideoDecoder.h>
#include <VideoPlay.h>

#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
#include <decor/AudioDemo.h>

extern "C" {
#include <android/native_window_jni.h>
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
#define MAX_AUDIO_FRME_SIZE 48000 * 4
using namespace std;
//测试环境
extern "C"
JNIEXPORT jstring JNICALL
Java_com_kangwang_ffmpeddemo_FFmpegdiaPlayer_native_1version(JNIEnv *env, jobject thiz) {
    string hello = av_version_info();
    string avutil = to_string(avutil_version());
    hello.append(avutil);
    LOGCATI("avutil");
    return env->NewStringUTF(hello.c_str());
}extern "C"
JNIEXPORT jlong JNICALL
Java_com_kangwang_ffmpeddemo_FFmpegdiaPlayer_initPlay(JNIEnv *env, jobject thiz, jstring path,
                                                      jobject surface) {
    char *input = const_cast<char *>(env->GetStringUTFChars(path, 0));
    VideoPlay *videoPlay = new VideoPlay(env,input,surface,thiz);
    return reinterpret_cast<jlong>(videoPlay);
}

//extern "C"
//JNIEXPORT void JNICALL
//Java_com_kangwang_ffmpeddemo_FFmpegdiaPlayer_player(JNIEnv *env, jobject instance, jstring input_) {
//    // TODO: implement player()
//    const char *input = env->GetStringUTFChars(input_, 0);
//    av_register_all();
//    AVFormatContext *pFormatCtx = avformat_alloc_context();
//    //open
//    if (avformat_open_input(&pFormatCtx, input, NULL, NULL) != 0) {
//        LOGCATE("%s","打开输入视频文件失败");
//        return;
//    }
//    //获取视频信息
//    if(avformat_find_stream_info(pFormatCtx,NULL) < 0){
//        LOGCATE("%s","获取视频信息失败");
//        return;
//    }
//    int audio_stream_idx=-1;
//    int i=0;
//    for (int i = 0; i < pFormatCtx->nb_streams; ++i) {
//        if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO) {
//            LOGCATE("  找到音频id %d", pFormatCtx->streams[i]->codec->codec_type);
//            audio_stream_idx=i;
//            break;
//        }
//    }
//    //获取解码器上下文
//    AVCodecContext *pCodecCtx=pFormatCtx->streams[audio_stream_idx]->codec;
//    //获取解码器
//    AVCodec *pCodex = avcodec_find_decoder(pCodecCtx->codec_id);
//    //打开解码器
//    if (avcodec_open2(pCodecCtx, pCodex, NULL)<0) {
//    }
//    //申请avpakcet，装解码前的数据
//    AVPacket *packet = (AVPacket *)av_malloc(sizeof(AVPacket));
//    //申请avframe，装解码后的数据
//    AVFrame *frame = av_frame_alloc();
//    //得到SwrContext ，进行重采样，具体参考http://blog.csdn.net/jammg/article/details/52688506
//    SwrContext *swrContext = swr_alloc();
//    //缓存区
//    uint8_t *out_buffer = (uint8_t *) av_malloc(44100 * 2);
////输出的声道布局（立体声）
//    uint64_t  out_ch_layout=AV_CH_LAYOUT_STEREO;
////输出采样位数  16位
//    enum AVSampleFormat out_formart=AV_SAMPLE_FMT_S16;
////输出的采样率必须与输入相同
//    int out_sample_rate = pCodecCtx->sample_rate;
//
////swr_alloc_set_opts将PCM源文件的采样格式转换为自己希望的采样格式
//    swr_alloc_set_opts(swrContext, out_ch_layout, out_formart, out_sample_rate,
//                       pCodecCtx->channel_layout, pCodecCtx->sample_fmt, pCodecCtx->sample_rate, 0,
//                       NULL);
//
//    swr_init(swrContext);
////    获取通道数  2
//    int out_channer_nb = av_get_channel_layout_nb_channels(AV_CH_LAYOUT_STEREO);
////    反射得到Class类型
//    jclass david_player = env->GetObjectClass(instance);
////    反射得到createAudio方法
//    jmethodID createAudio = env->GetMethodID(david_player, "createTrack", "(II)V");
////    反射调用createAudio
//    env->CallVoidMethod(instance, createAudio, 44100, out_channer_nb);
//
////    int channaleConfig;//通道数
////    if (nb_channals == 1) {
////        channaleConfig = AudioFormat.CHANNEL_OUT_MONO;
////    } else if (nb_channals == 2) {
////        channaleConfig = AudioFormat.CHANNEL_OUT_STEREO;
////    }else {
////        channaleConfig = AudioFormat.CHANNEL_OUT_MONO;
////    }
////    int buffersize= AudioTrack.getMinBufferSize(sampleRateInHz,
////                                                channaleConfig, AudioFormat.ENCODING_PCM_16BIT);
////    audioTrack = new AudioTrack(AudioManager.STREAM_MUSIC,sampleRateInHz,channaleConfig,
////                                AudioFormat.ENCODING_PCM_16BIT,buffersize,AudioTrack.MODE_STREAM);
////    audioTrack.play();
//
//    jmethodID audio_write = env->GetMethodID(david_player, "playTrack", "([BI)V");
//
//    int got_frame;
//    while (av_read_frame(pFormatCtx, packet) >= 0) {
//        if (packet->stream_index == audio_stream_idx) {
////            解码  mp3   编码格式frame----pcm   frame
//            avcodec_decode_audio4(pCodecCtx, frame, &got_frame, packet);
//            if (got_frame) {
//                LOGCATE("解码");
//                swr_convert(swrContext, &out_buffer, 44100 * 2, (const uint8_t **) frame->data, frame->nb_samples);
////                缓冲区的大小
//                int size = av_samples_get_buffer_size(NULL, out_channer_nb, frame->nb_samples,
//                                                      AV_SAMPLE_FMT_S16, 1);
//                jbyteArray audio_sample_array = env->NewByteArray(size);
//                env->SetByteArrayRegion(audio_sample_array, 0, size, (const jbyte *) out_buffer);
//                env->CallVoidMethod(instance, audio_write, audio_sample_array, size);
//                env->DeleteLocalRef(audio_sample_array);
//            }
//        }
//    }
//    av_frame_free(&frame);
//    swr_free(&swrContext);
//    avcodec_close(pCodecCtx);
//    avformat_close_input(&pFormatCtx);
//    env->ReleaseStringUTFChars(input_, input);


    //    const char *inputPath = env->GetStringUTFChars(s,0);
//
//    //实现总的上下文
//    avformat_network_init();
//    AVFormatContext *context = avformat_alloc_context();
//    //打开音频
//    int ret = avformat_open_input(&context,inputPath,NULL,NULL);
//    if(ret){
//        LOGCATE("error load file !!!");
//    }
//    //读取流通道
//    avformat_find_stream_info(context,NULL);
//    int audioIndex = 0;
//    for(int i = 0;i<context->nb_streams;i++){
//        if (context->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO){
//            audioIndex = i;
//            break;
//        }
//    }
//    AVCodecParameters *parameters =  context->streams[audioIndex]->codecpar;
//
//    //创建解码器
//    AVCodec  *aucode = avcodec_find_decoder(parameters->codec_id);
//    //创建上下文
//    AVCodecContext *avCodecContext = avcodec_alloc_context3(aucode);
//    avcodec_parameters_to_context(avCodecContext,parameters);
//    avcodec_open2(avCodecContext,aucode,NULL);
//    SwrContext *swrContext = swr_alloc();
//    //采样率
//    AVSampleFormat in_sample = avCodecContext->sample_fmt;
//    int in_sample_rate = avCodecContext->sample_rate;
//    //输入声道
//    uint64_t in_ch_layout = avCodecContext->channel_layout;
//
//    //输出采样格式
//    AVSampleFormat out_sample = AV_SAMPLE_FMT_S16;
//    //输出采样率
//    int out_sample_rate = 44100;
//    //输出声道
//    uint64_t  out_ch_layout = AV_CH_LAYOUT_STEREO;
//    //设置转换器
//    swr_alloc_set_opts(swrContext,out_ch_layout,out_sample,out_sample_rate,in_ch_layout,in_sample,in_sample_rate,0,NULL);
//    swr_init(swrContext);
//    uint8_t *out_buffer = (uint8_t *)av_malloc(2*44100);  //通道 数*采样率
//    //打开文件
//    FILE *fp_pcm = fopen(outputPath,"wb");
//    //读取packet数据
//    AVPacket *avPacket = av_packet_alloc();
//    while (av_read_frame(context,avPacket)>=0){
//        avcodec_send_packet(avCodecContext,avPacket);
//        //我们需要对其进行一次解压缩
//        AVFrame *frame = av_frame_alloc();
//        int ret = avcodec_receive_frame(avCodecContext,frame);
//
//        LOGCATE("解码中");
//        if (ret == AVERROR(EAGAIN)){
//            continue;
//        } else if (ret < 0 ){
//            LOGCATE("解码完成");
//            break;
//        }
//        if (avPacket->stream_index != audioIndex){
//            //不是音频流就跳过
//            continue;
//        }
//        //frame         转换为一个统一的格式
//        swr_convert(swrContext,&out_buffer,2*44100,(const uint8_t **)frame->data,frame->nb_samples);
//        int out_channel_nb = av_get_channel_layout_nb_channels(out_ch_layout);
//        //输出到一个文件 里面
//        int out_buffrt_size = av_samples_get_buffer_size(NULL,out_channel_nb,frame->nb_samples,out_sample,1);
//        //内存对其
//        fwrite(out_buffer,1,out_buffrt_size,fp_pcm);
//    }
//    fclose(fp_pcm);
//    av_free(out_buffer);
//    swr_free(&swrContext);
//    avcodec_close(avCodecContext);
//    avformat_close_input(&context);

//}

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
//将pcm数据添加到缓冲区中
void getQueueCallBack(SLAndroidSimpleBufferQueueItf  slBufferQueueItf, void* context){

    buffersize=0;

    getPcm(&buffer,&buffersize);
    if(buffer!=NULL&&buffersize!=0){
        //将得到的数据加入到队列中
        (*slBufferQueueItf)->Enqueue(slBufferQueueItf,buffer,buffersize);
    }
}

//创建引擎
void createEngine(){
    slCreateEngine(&engineObject,0,NULL,0,NULL,NULL);//创建引擎
    (*engineObject)->Realize(engineObject,SL_BOOLEAN_FALSE);//实现engineObject接口对象
    (*engineObject)->GetInterface(engineObject,SL_IID_ENGINE,&engineEngine);//通过引擎调用接口初始化SLEngineItf
}

//创建混音器
void createMixVolume(){
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
void createPlayer(){
    //初始化ffmpeg
    int rate;
    int channels;
    createFFmpeg(&rate,&channels);
    LOGE("RATE %d",rate);
    LOGE("channels %d",channels);
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
    LOGE("执行到此处")
            (*engineEngine)->CreateAudioPlayer(engineEngine,&audioplayer,&dataSource,&slDataSink,3,ids,req);
    (*audioplayer)->Realize(audioplayer,SL_BOOLEAN_FALSE);
    LOGE("执行到此处2")
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
//释放资源
void realseResource(){
    if(audioplayer!=NULL){
        (*audioplayer)->Destroy(audioplayer);
        audioplayer=NULL;
        slBufferQueueItf=NULL;
        slPlayItf=NULL;
    }
    if(outputMixObject!=NULL){
        (*outputMixObject)->Destroy(outputMixObject);
        outputMixObject=NULL;
        outputMixEnvironmentalReverb=NULL;
    }
    if(engineObject!=NULL){
        (*engineObject)->Destroy(engineObject);
        engineObject=NULL;
        engineEngine=NULL;
    }
    realseFFmpeg();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_test_ffmpegopensles_MusicPlay_play(JNIEnv *env, jobject instance) {

}


extern "C"
JNIEXPORT void JNICALL
Java_com_test_ffmpegopensles_MusicPlay_stop(JNIEnv *env, jobject instance) {
    realseResource();
}extern "C"
JNIEXPORT void JNICALL
Java_com_kangwang_ffmpeddemo_FFmpegdiaPlayer_playMp3(JNIEnv *env, jobject thiz) {
    // TODO: implement playMp3()
    createEngine();
    createMixVolume();
    createPlayer();

}