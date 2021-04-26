# FFmpeg教程

源码编译等有空在编译，目前没有自己编译，我是github上下载别人的项目，直接拿来使用的。

## FFmpeg在android studio下使用

1.创建一个c++项目

<img src="image/image-20210410204303533.png" alt="image-20210410204303533" style="zoom:50%;" />

2.添加库，静态库复制到制定目录

<img src="image/image-20210411091438877.png" alt="image-20210411091438877" style="zoom:50%;" />

支持在平台，自己导入了那些包，就支持那些平臺

<img src="image/image-20210411091651220.png" alt="image-20210411091651220" style="zoom:50%;" />



配置cMakeKists文件

```
project("ffmpeddemo")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11")
set(libs ${CMAKE_SOURCE_DIR}/../libs)
set(libname play)  #编译之后的名字  不一样要和文件名相同

#编译的文件 在include文件夹下、、、

include_directories(
        include
        play
        utils
        play/decor
        play/decor/base
#        ${CMAKE_SOURCE_DIR}/util
)

link_directories(
        ${libs}/${ANDROID_ABI})

#编译那些文件
file(GLOB src-files
        ${CMAKE_SOURCE_DIR}/*.cpp
        ${CMAKE_SOURCE_DIR}/play/*.cpp
        ${CMAKE_SOURCE_DIR}/play/*.h
        ${CMAKE_SOURCE_DIR}/utils/*.cpp
        ${CMAKE_SOURCE_DIR}/utils/*.h
        ${CMAKE_SOURCE_DIR}/play/decor/*.cpp
        ${CMAKE_SOURCE_DIR}/play/decor/*.h
        ${CMAKE_SOURCE_DIR}/play/decor/base/*.cpp
        ${CMAKE_SOURCE_DIR}/play/decor/base/*.h
        )

//可以立即为导入的包
add_library( # Sets the name of the library.
        ${libname}
        # Sets the library as a shared library.
        SHARED
        # Provides a relative path to your source file(s).
        ${src-files}  #加入的就是上面指定文件下的所有
        )
//第3方库
set(third-party-libs
        avformat
        avcodec
        avfilter
        swresample
        swscale
        avutil
        fdk-aac
        x264
        )
//native 库
set(native-libs
        android
        log
        OpenSLES
        )
//链接哪些库
target_link_libraries( # Specifies the target library.
        ${libname}
        ${log-lib}
        ${third-party-libs}
        ${native-libs}
        android
        z
        OpenSLES
        )
```

配置编译  链接 哪些东西

## 测试环境

```java
extern "C"
JNIEXPORT jstring JNICALL
Java_com_kangwang_ffmpeddemo_FFmpegdiaPlayer_native_1version(JNIEnv *env, jobject thiz) {
    string hello = av_version_info();
    string avutil = to_string(avutil_version());
    hello.append(avutil);
    LOGCATI("avutil");
    return env->NewStringUTF(hello.c_str());
}
```

java层

```
public String versionInfo() {
    return native_version();
}
```

## 视频播放和音频播放

### 通用代码

1,分析一下解析的套路

```java
formatContext = avformat_alloc_context();  //封装上下文
AVDictionary *opts = NULL;//从字典中取值
av_dict_set(&opts, "timeout", "30000000", 0); //如果这么久都没有打开，就认为有问题
int ret = avformat_open_input(&formatContext,m_url,NULL,&opts);//打开文件
if (ret){   //为0表示成功
    LOGCATE("open file failed");
    return;
}else{
    LOGCATE("open file success");
}

avformat_find_stream_info(formatContext,NULL); //读取成功    读取视频流  解析出来

//便利
for(int i= 0; i<formatContext->nb_streams;i++){
    //视频流
    if(formatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO){
        video_stream_index = i;
    } else if(formatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO){
        audeo_stream_index = i;
    }
}
```

分析：首先创建一个全局上下文，然后读取文件，将流文件写入到全局上下文中。我们限制就需要去遍历流，区分出视频和音频流。这就是上面一段代码的所有内容

它不仅仅是音频是那样写，视频也是一样的。对于视频都需要上面一段共同代码，下来就属于它们自己了。

### 播放方式一   使用nativeWindow方式进行播放 

1.使用nativeWindow方式进行播放

```java
nativeWindow = ANativeWindow_fromSurface(env,surface);
```

2.对流进行分析

- 得到视频流的参数
- 根据ID得到解码器
- 得到解码器上下文
- 将参数复制到上下文中
- 使用上下文打开文件
- 数据是由一个一个的packet组成的，我们创建一个容器
- 读取视频流
- 给窗口设置内存缓存

```java
OGCATE("init_video decoder");
    //得到流的参数
AVCodecParameters *parameters = formatContext->streams[video_stream_index]->codecpar;
LOGCATE("find stream");
//我们现在对流进行解码   使用解码器的id  得到解码器
aVCodec = avcodec_find_decoder(parameters->codec_id);
//创建解码器 的上下文
avCodecContext = avcodec_alloc_context3(aVCodec);

LOGCATE("avCodeContext");
//将解码器的 参数复制到 上下文
avcodec_parameters_to_context(avCodecContext,parameters);
avcodec_open2(avCodecContext,aVCodec,NULL);
avPacket = av_packet_alloc(); //比如H.264压缩数据
LOGCATE("getAvpackk");
swsContext =//    读取视频流
    sws_getContext(
    avCodecContext->width,
    avCodecContext->height,
    avCodecContext->pix_fmt,
    avCodecContext->width,
    avCodecContext->height,
    AV_PIX_FMT_RGBA,
    SWS_BILINEAR,0,0,0);
LOGCATE("dis[lay begin");
ANativeWindow_setBuffersGeometry(
    nativeWindow,
    avCodecContext->width,
    avCodecContext->height,
    WINDOW_FORMAT_RGBA_8888);
LOGCATE("displey end");
```

3.进行播放

- 参数 
  - 可以获取时间   
  - 获取名称
  - 从format中获取packet

```
long time = formatContext->duration; //获取的是微秒    10  6次方
string name = formatContext->iformat->name;
//读取每一帧的 数据    数据帧   avpacket   frame
while (av_read_frame(formatContext, avPacket) >= 0){
    //将数据放入 到解码器中
    avcodec_send_packet(avCodecContext,avPacket);
    AVFrame  *frame = av_frame_alloc();  
    //解码之后的数据  frame
    int ret = avcodec_receive_frame(avCodecContext,frame);
    if (ret == AVERROR(EAGAIN)){
        continue;
    } else if (ret<0){
        break;
    }

    //将yuv转化为RGB
    uint8_t  *dst_data[4];
    int dst_linesize[4];
    av_image_alloc(dst_data,
            dst_linesize,
            avCodecContext->width,
            avCodecContext->height,
            AV_PIX_FMT_RGBA,
            1);
    sws_scale(swsContext,frame->data,frame->linesize,0,frame->height,dst_data,dst_linesize);
    ANativeWindow_lock(nativeWindow,&outBuffer,NULL);
    uint8_t *fistWindown = static_cast<uint8_t *>(outBuffer.bits);
    uint8_t *src_data = dst_data[0];
    int destStride = outBuffer.stride *4;
    int src_linesize = dst_linesize[0];
    for(int  i=0;i<outBuffer.height;i++){
        memcpy(fistWindown+i*destStride,src_data+i*src_linesize,destStride);
    }
    ANativeWindow_unlockAndPost(nativeWindow);
    LOGCATE("显示----");
}
```

## 音频播放

1.创建引擎

```java
slCreateEngine(&engineObject,0,NULL,0,NULL,NULL);//创建引擎
(*engineObject)->Realize(engineObject,SL_BOOLEAN_FALSE);//实现engineObject接口对象
(*engineObject)->GetInterface(engineObject,SL_IID_ENGINE,&engineEngine);//通过引擎调用接口初始化SLEngineItf
```

2.创建混音

```
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
```

3.创建播放器

```
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
        SLuint32      formatType;  pcm
        SLuint32      numChannels;  通道数
        SLuint32      samplesPerSec;  采样率
        SLuint32      bitsPerSample;  采样位数
        SLuint32      containerSize;  包含位数
        SLuint32      channelMask;     立体声
        SLuint32      endianness;    end标志位
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
```

4.播放

```
void AudioDecor::xx() {
    buffersize=0;
    getPcm(&buffer,&buffersize);
    if(buffer!=NULL&&buffersize!=0){
        //将得到的数据加入到队列中
        (*slBufferQueueItf)->Enqueue(slBufferQueueItf,buffer,buffersize);
    }
}

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
```

## 同步