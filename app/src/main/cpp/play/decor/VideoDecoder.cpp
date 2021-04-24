//
// Created by 28188 on 2021/4/13.
//
extern "C"{

#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>

};
#include "VideoDecoder.h"
#include "LogUtils.h"

VideoDecoder::VideoDecoder(char *url) : PlayBase(url) {
    LOGCATE("create VideoDecoder---------");
    //  渲染  创建渲染窗口
}

void VideoDecoder::initInstance() {
    LOGCATE("init_video decoder");
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
    play();
}

void VideoDecoder::codingLoop() {
    LOGCATE("codingLoop VideoDecoder---------------------");
    long time = formatContext->duration; //获取的是微秒    10  6次方
    string name = formatContext->iformat->name;
    while (av_read_frame(formatContext, avPacket) >= 0){
        avcodec_send_packet(avCodecContext,avPacket);
        AVFrame  *frame = av_frame_alloc();
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
    LOGCATE("播放结束");

}

void VideoDecoder::codingReady() {
    LOGCATE("ready video-----------");
    already = true;
}

VideoDecoder::~VideoDecoder() {

}

void VideoDecoder::createWindow(JNIEnv *env,jobject surface) {
    nativeWindow = ANativeWindow_fromSurface(env,surface);
    if(nativeWindow) {
        LOGCATE("success");
    } else{
        LOGCATE("error");
    }
    LOGCATE("native init");
    long saa = reinterpret_cast<long>(nativeWindow);
    const char *sa = reinterpret_cast<const char *>(reinterpret_cast<jstring>(saa));
}
