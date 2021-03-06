#include <jni.h>
#include <string>

#include <decor/VideoDecoder.h>
#include <VideoPlay.h>

#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
#include <FfVideoPlayer.h>

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
#include <jni.h>
#include <libavutil/audio_fifo.h>
#include <libavutil/avstring.h>
#include <libavutil/audio_fifo.h>
#include <libavutil/avstring.h>
};

extern "C"
JNIEXPORT jstring JNICALL
Java_com_kangwang_ffmpeddemo_ReadVersion_stringFromJNI(JNIEnv *env, jobject thiz) {
    char strBuffer[1024 * 4] = {0};
    strcat(strBuffer, "libavcodec : ");
    strcat(strBuffer, AV_STRINGIFY(LIBAVCODEC_VERSION));
    strcat(strBuffer, "\nlibavformat : ");
    strcat(strBuffer, AV_STRINGIFY(LIBAVFORMAT_VERSION));
    strcat(strBuffer, "\nlibavutil : ");
    strcat(strBuffer, AV_STRINGIFY(LIBAVUTIL_VERSION));
    strcat(strBuffer, "\nlibavfilter : ");
    strcat(strBuffer, AV_STRINGIFY(LIBAVFILTER_VERSION));
    strcat(strBuffer, "\nlibswresample : ");
    strcat(strBuffer, AV_STRINGIFY(LIBSWRESAMPLE_VERSION));
    strcat(strBuffer, "\nlibswscale : ");
    strcat(strBuffer, AV_STRINGIFY(LIBSWSCALE_VERSION));
    strcat(strBuffer, "\navcodec_configure : \n");
    strcat(strBuffer, avcodec_configuration());
    strcat(strBuffer, "\navcodec_license : ");
    strcat(strBuffer, avcodec_license());
    LOGCATE("GetFFmpegVersion\n%s", strBuffer);
    return env->NewStringUTF(strBuffer);
}extern "C"
JNIEXPORT void JNICALL
Java_com_kangwang_ffmpeddemo_play_FFmpegdiaPlayerVideo_natvie_1start(JNIEnv *env, jobject thiz,
                                                                     jstring absolute_path,
                                                                     jobject surface) {
    //  ??????
    ANativeWindow  *nativeWindow = ANativeWindow_fromSurface(env,surface);
//  ??????????????? ??? ?????????
    LOGCATE("start_path");
    const char *path = env->GetStringUTFChars(absolute_path,0);
//    ??????  ??????  ??????
    LOGCATE("init_net");
    avformat_network_init();
    //????????????
    AVFormatContext  *formatContext = avformat_alloc_context();
    //??????????????????
    AVDictionary *opts = NULL;
    //???????????????????????????????????????????????????
    av_dict_set(&opts, "timeout", "30000000", 0);

    //????????????
    int ret = avformat_open_input(&formatContext,path,NULL,&opts);
    if (ret){   //???0????????????
        LOGCATE("---------------->??????");
        return;
    }else{
        LOGCATE("------------->>success");
    }
    int video_stream_index = 0;
    //????????????    ???????????????  ????????????
    avformat_find_stream_info(formatContext,NULL);
    //??????
    for(int i= 0; i<formatContext->nb_streams;i++){
        //?????????
        if(formatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO){
            video_stream_index = i;
            break;
        }
    }
    //??????????????????
    AVCodecParameters *parameters = formatContext->streams[video_stream_index]->codecpar;
    //??????????????????????????????   ??????????????????id  ???????????????
    AVCodec *vcode = avcodec_find_decoder(parameters->codec_id);
    //??????????????? ????????????
    AVCodecContext *avCodecContex = avcodec_alloc_context3(vcode);
    //??????????????? ??????????????? ?????????
    avcodec_parameters_to_context(avCodecContex,parameters);\

    //???????????????
    avcodec_open2(avCodecContex,vcode,NULL);
//    ??????yuv??????
    AVPacket *avPacket = av_packet_alloc();
//    ???????????????

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

        //???yuv?????????RGB
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
        LOGCATE("??????----");
    }
    LOGCATE("????????????");
    env->ReleaseStringUTFChars(absolute_path,path);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_kangwang_ffmpeddemo_play_FFmpegdiaPlayerMp3_natvie_1startMp3(JNIEnv *env,
                                                                      jobject thiz,
                                                                      jstring input_) {

    // TODO: implement natvie_startMp3()
    const char *input = env->GetStringUTFChars(input_, 0);
    av_register_all();
    AVFormatContext *pFormatCtx = avformat_alloc_context();
    //open
    if (avformat_open_input(&pFormatCtx, input, NULL, NULL) != 0) {
        LOGCATE("%s","??????????????????????????????");
        return;
    }
    //??????????????????
    if(avformat_find_stream_info(pFormatCtx,NULL) < 0){
        LOGCATE("%s","????????????????????????");
        return;
    }
    int audio_stream_idx=-1;
    int i=0;
    for (int i = 0; i < pFormatCtx->nb_streams; ++i) {
        if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO) {
            LOGCATE("  ????????????id %d", pFormatCtx->streams[i]->codec->codec_type);
            audio_stream_idx=i;
            break;
        }
    }
    //????????????????????????
    AVCodecContext *pCodecCtx=pFormatCtx->streams[audio_stream_idx]->codec;
    //???????????????
    AVCodec *pCodex = avcodec_find_decoder(pCodecCtx->codec_id);
    //???????????????
    if (avcodec_open2(pCodecCtx, pCodex, NULL)<0) {
    }
    //??????avpakcet????????????????????????
    AVPacket *packet = (AVPacket *)av_malloc(sizeof(AVPacket));
    //??????avframe????????????????????????
    AVFrame *frame = av_frame_alloc();
    //??????SwrContext ?????????????????????????????????http://blog.csdn.net/jammg/article/details/52688506
    SwrContext *swrContext = swr_alloc();
    //?????????
    uint8_t *out_buffer = (uint8_t *) av_malloc(44100 * 2);
//????????????????????????????????????
    uint64_t  out_ch_layout=AV_CH_LAYOUT_STEREO;
//??????????????????  16???
    enum AVSampleFormat out_formart=AV_SAMPLE_FMT_S16;
//???????????????????????????????????????
    int out_sample_rate = pCodecCtx->sample_rate;

//swr_alloc_set_opts???PCM????????????????????????????????????????????????????????????
    swr_alloc_set_opts(swrContext, out_ch_layout, out_formart, out_sample_rate,
                       pCodecCtx->channel_layout, pCodecCtx->sample_fmt, pCodecCtx->sample_rate, 0,
                       NULL);

    swr_init(swrContext);
//    ???????????????  2
    int out_channer_nb = av_get_channel_layout_nb_channels(AV_CH_LAYOUT_STEREO);
//    ????????????Class??????
    jclass david_player = env->GetObjectClass(thiz);
//    ????????????createAudio??????
    jmethodID createAudio = env->GetMethodID(david_player, "createTrack", "(II)V");
//    ????????????createAudio
    env->CallVoidMethod(thiz, createAudio, 44100, out_channer_nb);
    jmethodID audio_write = env->GetMethodID(david_player, "playTrack", "([BI)V");
    int got_frame;
    while (av_read_frame(pFormatCtx, packet) >= 0) {
        if (packet->stream_index == audio_stream_idx) {
//            ??????  mp3   ????????????frame----pcm   frame
            avcodec_decode_audio4(pCodecCtx, frame, &got_frame, packet);
            if (got_frame) {
                LOGCATE("??????");
                swr_convert(swrContext, &out_buffer, 44100 * 2, (const uint8_t **) frame->data, frame->nb_samples);
//                ??????????????????
                int size = av_samples_get_buffer_size(NULL, out_channer_nb, frame->nb_samples,
                                                      AV_SAMPLE_FMT_S16, 1);
                jbyteArray audio_sample_array = env->NewByteArray(size);
                env->SetByteArrayRegion(audio_sample_array, 0, size, (const jbyte *) out_buffer);
                env->CallVoidMethod(thiz, audio_write, audio_sample_array, size);
                env->DeleteLocalRef(audio_sample_array);
            }
        }
    }
    av_frame_free(&frame);
    swr_free(&swrContext);
    avcodec_close(pCodecCtx);
    avformat_close_input(&pFormatCtx);
    env->ReleaseStringUTFChars(input_, input);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_kangwang_ffmpeddemo_play_FFmpegdiaConvertMp3_natvie_1convertMp3(
        JNIEnv *env, jobject thiz,jstring input,jstring output
        ) {
    const char *outputPath = env->GetStringUTFChars(output,0);
    const char *inputPath = env->GetStringUTFChars(input,0);
    //?????????????????????
    av_register_all();
    AVFormatContext *context = avformat_alloc_context();
    //????????????
    if (avformat_open_input(&context,inputPath,NULL,NULL)!=0){
        LOGCATE("error load file !!!");
        return;
    }
    //???????????????
    avformat_find_stream_info(context,NULL);
    int audioIndex = 0;
    for(int i = 0;i<context->nb_streams;i++){
        if (context->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO){
            audioIndex = i;
            break;
        }
    }
    AVCodecParameters *parameters =  context->streams[audioIndex]->codecpar;
    //???????????????
    AVCodec  *aucode = avcodec_find_decoder(parameters->codec_id);
    //???????????????
    AVCodecContext *avCodecContext = avcodec_alloc_context3(aucode);
    avcodec_parameters_to_context(avCodecContext,parameters);
    avcodec_open2(avCodecContext,aucode,NULL);
    SwrContext *swrContext = swr_alloc();
    //?????????
    AVSampleFormat in_sample = avCodecContext->sample_fmt;
    int in_sample_rate = avCodecContext->sample_rate;
    //????????????
    uint64_t in_ch_layout = avCodecContext->channel_layout;
    //??????????????????
    AVSampleFormat out_sample = AV_SAMPLE_FMT_S16;
    //???????????????
    int out_sample_rate = 44100;
    //????????????
    uint64_t  out_ch_layout = AV_CH_LAYOUT_STEREO;
    //???????????????
    swr_alloc_set_opts(swrContext,out_ch_layout,out_sample,out_sample_rate,in_ch_layout,in_sample,in_sample_rate,0,NULL);
    swr_init(swrContext);
    uint8_t *out_buffer = (uint8_t *)av_malloc(2*44100);  //?????? ???*?????????
    //????????????
    FILE *fp_pcm = fopen(outputPath,"wb");
    //??????packet??????
    AVPacket *avPacket = av_packet_alloc();
    while (av_read_frame(context,avPacket)>=0){
        avcodec_send_packet(avCodecContext,avPacket);
        //???????????????????????????????????????
        AVFrame *frame = av_frame_alloc();
        int ret = avcodec_receive_frame(avCodecContext,frame);
        LOGCATE("?????????");
        if (ret == AVERROR(EAGAIN)){
            continue;
        } else if (ret < 0 ){
            LOGCATE("????????????");
            break;
        }
        if (avPacket->stream_index != audioIndex){
            //????????????????????????
            continue;
        }
        //frame         ??????????????????????????????
        swr_convert(swrContext,&out_buffer,2*44100,(const uint8_t **)frame->data,frame->nb_samples);
        int out_channel_nb = av_get_channel_layout_nb_channels(out_ch_layout);
        //????????????????????? ??????
        int out_buffrt_size = av_samples_get_buffer_size(NULL,out_channel_nb,frame->nb_samples,out_sample,1);
        //????????????
        fwrite(out_buffer,1,out_buffrt_size,fp_pcm);
    }
    fclose(fp_pcm);
    av_free(out_buffer);
    swr_free(&swrContext);
    avcodec_close(avCodecContext);
    avformat_close_input(&context);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_kangwang_ffmpeddemo_play_FFmpegdiaplayerVideo2_natvie_1playerVideo(JNIEnv *env,
                                         jobject thiz,
                                         jstring absolute_path,
                                         jobject surface) {
    const char *absoutePath = env->GetStringUTFChars(absolute_path,0);
    FfVideoPlayer *ffVideoPlayer = new FfVideoPlayer(absoutePath);
    ffVideoPlayer->init();
    ffVideoPlayer->initDecor();
    ffVideoPlayer->
}