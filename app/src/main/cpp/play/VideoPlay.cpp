//
// Created by 28188 on 2021/4/13.
//

#include "VideoPlay.h"

VideoPlay::VideoPlay(JNIEnv *env,char *url,jobject surface,jobject instance) {
    LOGCATE("Video play create--------------------");
    loadCommon(url);
    VideoDecoder *decoder = new VideoDecoder(url);
    decoder->createWindow(env,surface);
    decoder->initInstance();
    AudioDecor *audioDecor = new AudioDecor(env,instance,url);
}

void VideoPlay::loadCommon(char *url) {
    av_register_all();
    avformat_network_init();
    AVFormatContext *avFormatContext ;
    avFormatContext = avformat_alloc_context();
    //加载
    AVDictionary *avDictionary = NULL;
    av_dict_set(&avDictionary, "timeout", "30000000", 0); //如果这么久都没有打开，就认为有问题
    int ret = avformat_open_input(&avFormatContext,url,NULL,&avDictionary);//打开文件
    if(ret<0){
        LOGCATE("load file");
    }
    //流
    avformat_find_stream_info(avFormatContext,NULL);
    int videoIndex = 0;
    int audioIndex = 0;
    for(int i = 0; i<avFormatContext->nb_streams;i++){
        if(avFormatContext->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO){
            videoIndex = i;
        }
        if(avFormatContext->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO){
            audioIndex = i;
        }
    }
}

void VideoPlay::saveFrame(AVFrame *pFrame, int width, int height, int iFrame) {
    FILE *pfile; //文件指针
    char fileName[32];//文件名
    int y;
    sprintf(fileName,"frame%04d.pcm",iFrame);
    //打开文件，只写入
    pfile = fopen(fileName,"wb");
    if(pfile == NULL){
        return;
    }
    for (int i = 0; i < height; ++i) {
        fwrite(pFrame->data[0]+y*pFrame->linesize[0],1,width * 3 ,pfile);
    }

    fclose(pfile);
}

void VideoPlay::runMain(char *url) {
    av_register_all();  //注册
    avformat_network_init();
    //创建封装上下文
    AVFormatContext *avFormatContext;
    avFormatContext = avformat_alloc_context();
    //open file  打开文件
    if(avformat_open_input(&avFormatContext,url,NULL,NULL)!=0){
        return;
    }
    //找流
    if(avformat_find_stream_info(avFormatContext,0)<0){
        return;
    }

    //找出音频下标
    av_dump_format(avFormatContext,0,url,0);
    int i;
    AVCodecContext  *avCodecContext;
    int videoStreamIndex = -1;
    for(i=0;i<avFormatContext->nb_streams;i++){
        if(avFormatContext->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO){
            videoStreamIndex = i;
            break;
        }
    }
    if(videoStreamIndex == -1){
        return;
    }

    //根据下标得到解码器上下文
    avCodecContext = avFormatContext->streams[videoStreamIndex]->codec;
    AVCodec  *avCodec;
    //得到解码器
    avCodec = avcodec_find_decoder(avCodecContext->codec_id);
    if(avCodec == NULL)return;
    //一帧
    AVFrame  *avFrame;
    avFrame = av_frame_alloc();
    AVFrame  *avFrame1 = av_frame_alloc();
    uint8_t  *buffer;
    int numbytes;
    //计算大小   横列  格式     多少个数据。每一个数据使用uint8进行存储
    numbytes = avpicture_get_size(
            AV_PIX_FMT_RGB24,
            avCodecContext->width,
            avCodecContext->height);
    buffer = (uint8_t*)av_malloc(numbytes*sizeof(uint8_t));

    avpicture_fill(
            (AVPicture *)avFrame1,
            buffer,
            AV_PIX_FMT_RGB24,
            avCodecContext->width,
            avCodecContext->height);
    int frameFInished;
    AVPacket packet;
    i = 0;
    while (av_read_frame(avFormatContext,&packet)>=0){
        if(packet.stream_index == videoStreamIndex){
            avcodec_decode_video2(avCodecContext,avFrame,&frameFInished,&packet);
            if(frameFInished){
                SwsContext *img_convert_ctx =
                        sws_getContext(
                            avCodecContext->width,
                            avCodecContext->height,
                            avCodecContext->pix_fmt,
                            avCodecContext->width,
                            avCodecContext->height,
                            AV_PIX_FMT_RGB24,
                            SWS_BICUBIC,
                            NULL,NULL,NULL);
                sws_scale(img_convert_ctx,(const uint8_t * const *)avFrame->data,
                        avFrame->linesize,0,
                        avCodecContext->height,
                        avFrame1->data,
                        avFrame1->linesize);
                if(++i <= 5){
                    saveFrame(avFrame1,avCodecContext->width,avCodecContext->height,i);
                }
            }
            av_free_packet(&packet);
        }
    }
    av_free(buffer);
    av_free(avFrame);
    av_free(avFrame1);
    avcodec_close(avCodecContext);
    avformat_close_input(&avFormatContext);
}



















