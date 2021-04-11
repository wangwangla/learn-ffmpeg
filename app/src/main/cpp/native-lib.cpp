#include <jni.h>
#include <string>
#include <android/native_window_jni.h>
#include <android/log.h>

extern "C" {
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
};


#define  LOG_TAG    "hb-4"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...)  __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

extern "C" JNIEXPORT jstring JNICALL
Java_com_kangwang_ffmpeddemo_FFmpegdiaPlayer_stringFromJNI(
        JNIEnv* env,
        jobject /* this */) {
    LOGI("------------read vseion  kkkww");
    std::string hello = av_version_info();
    std::string avutil = std::to_string(avutil_version());
    hello.append(avutil);
//    hello.append(reinterpret_cast<const char *>(avcodec_version()));
//    hello.append(reinterpret_cast<const char *>(avutil_version()));
//            +avutil_version();
    return env->NewStringUTF(hello.c_str());
}

extern "C"
JNIEXPORT void JNICALL
Java_com_kangwang_ffmpeddemo_FFmpegdiaPlayer_natvie_1start(JNIEnv *env, jobject thiz,
                                                           jstring absolute_path, jobject surface) {
    //  渲染
    ANativeWindow  *nativeWindow = ANativeWindow_fromSurface(env,surface);
//  将数据写入 到 缓存区
    LOGE("start_path");
    const char *path = env->GetStringUTFChars(absolute_path,0);
//    视频  音频  绘制
    LOGE("init_net");
    avformat_network_init();
    //总上下文
    AVFormatContext  *formatContext = avformat_alloc_context();
    //从字典中取值
    AVDictionary *opts = NULL;
    //如果这么久都没有打开，就认为有问题
    av_dict_set(&opts, "timeout", "30000000", 0);

    //打开文件
    int ret = avformat_open_input(&formatContext,path,NULL,&opts);
    if (ret){   //为0表示成功
        LOGI("---------------->失败");
        return;
    }else{
        LOGI("------------->>success");
    }
    int video_stream_index = 0;
    //读取成功    读取视频流  解析出来
    avformat_find_stream_info(formatContext,NULL);
    //便利
    for(int i= 0; i<formatContext->nb_streams;i++){
        //视频流
        if(formatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO){
            video_stream_index = i;
            break;
        }
    }
    //得到流的参数
    AVCodecParameters *parameters = formatContext->streams[video_stream_index]->codecpar;
    //我们现在对流进行解码   使用解码器的id  得到解码器
    AVCodec *vcode = avcodec_find_decoder(parameters->codec_id);
    //创建解码器 的上下文
    AVCodecContext *avCodecContex = avcodec_alloc_context3(vcode);
    //将解码器的 参数复制到 上下文
    avcodec_parameters_to_context(avCodecContex,parameters);\

    //打开解码器
    avcodec_open2(avCodecContex,vcode,NULL);
//    解码yuv数据
    AVPacket *avPacket = av_packet_alloc();
//    读取视频流

    SwsContext *swsContext = sws_getContext(avCodecContex->width,avCodecContex->height,
            avCodecContex->pix_fmt,avCodecContex->width,
            avCodecContex->height,AV_PIX_FMT_RGBA,
            SWS_BILINEAR,0,0,0);
    ANativeWindow_setBuffersGeometry(nativeWindow,avCodecContex->width,avCodecContex->height,WINDOW_FORMAT_RGBA_8888);
    ANativeWindow_Buffer outBuffer;
    while (av_read_frame(formatContext,avPacket)>=0){
        avcodec_send_packet(avCodecContex,avPacket);
        AVFrame  *frame = av_frame_alloc();
        ret = avcodec_receive_frame(avCodecContex,frame);
        if (ret == AVERROR(EAGAIN)){
            continue;
        } else if (ret<0){
            break;
        }

        //将yuv转化为RGB
        uint8_t  *dst_data[4];
        int dst_linesize[4];
        av_image_alloc(dst_data,dst_linesize,avCodecContex->width,avCodecContex->height,
                AV_PIX_FMT_RGBA,1);
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
        LOGI("显示----");
    }
    LOGE("播放结束");
    env->ReleaseStringUTFChars(absolute_path,path);
}