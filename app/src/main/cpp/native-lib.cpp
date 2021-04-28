#include <jni.h>
#include <string>

#include <LogUtils.h>
#include <decor/VideoDecoder.h>
#include <VideoPlay.h>

#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>


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
#include <libavutil/avstring.h>#include <libavutil/audio_fifo.h>
#include <libavutil/avstring.h>
};

#define OUTPUT_CHANNELS 2
#define OUTPUT_BIT_RATE 96000
int64_t pts = 0;
int framecnt = 0;
int open_input_file(const char *filename, AVFormatContext **input_format_context, AVCodecContext **input_codec_context){
    AVCodec *input_codec;
    int error;
    /** 打开文件， 设置format参数*/
    if ((error = avformat_open_input(input_format_context, filename, NULL,NULL)) < 0) {
        *input_format_context = NULL;
        return error;
    }
    LOGCATE("打开文件");
    /** 读取流信息.   将流设置到format里 */
    if ((error = avformat_find_stream_info(*input_format_context, NULL)) < 0) {
        avformat_close_input(input_format_context);
        return error;
    }
    LOGCATE("找流");
    int  index = 0;
    int xxx = 0;
    //遍历所有的流信息  找出音频流下标
    for (index = 0; index <(*input_format_context)->nb_streams; index++) {
        AVStream *stream = (*input_format_context)->streams[index];
        AVCodecContext *code = stream->codec;
        if (code->codec_type == AVMEDIA_TYPE_AUDIO){
            xxx = index;
            break;
        }
    }

    /** Find a decoder for the audio stream. */
    /** 通过音频流找解码器. */
    if (!(input_codec = avcodec_find_decoder((*input_format_context)->streams[xxx]->codec->codec_id))) {
        fprintf(stderr, "Could not find input codec\n");
        avformat_close_input(input_format_context);
        return AVERROR_EXIT;
    }

    /** Find a decoder for the audio stream. */
    /** 解码器找到了   正式的打开流文件. */
    LOGCATE("input  for mcontext",input_codec_context);
    if ((error = avcodec_open2((*input_format_context)->streams[xxx]->codec,input_codec, NULL)) < 0) {
        fprintf(stderr, "Could not open input codec (error '%s')\n",(error));
        avformat_close_input(input_format_context);
        return error;
    }
    LOGCATE("input  for mcontext11111111",input_codec_context);
    /** Save the decoder context for easier access later. */
    /** 流的上下文*/
    *input_codec_context = (*input_format_context)->streams[0]->codec;
    return 0;
}

int open_output_file(const char *filename,AVCodecContext *input_codec_context,AVFormatContext **output_format_context,AVCodecContext **output_codec_context)
{
    AVIOContext *output_io_context = NULL;
    AVStream *stream = NULL;
    AVCodec *output_codec = NULL;
    int error;
    /** Open the output file to write to it. */
    if ((error = avio_open(&output_io_context, filename,
                           AVIO_FLAG_WRITE)) < 0) {
        LOGCATE("Could not open output file '%s' (error '%s')\n",filename);
        return error;
    }
    /** Create a new format context for the output container format. */
    if (!(*output_format_context = avformat_alloc_context())) {
        LOGCATE("Could not allocate output format context\n");
        return AVERROR(ENOMEM);
    }
    /** Associate the output file (pointer) with the container format context. */
    (*output_format_context)->pb = output_io_context;
    /** Guess the desired container format based on the file extension. */
    if (!((*output_format_context)->oformat = av_guess_format(NULL, filename,
                                                              NULL))) {
        LOGCATE( "Could not find output file format\n");
        goto cleanup;
    }
    av_strlcpy((*output_format_context)->filename, filename,
               sizeof((*output_format_context)->filename));

    /** Find the encoder to be used by its name. */
    if (!(output_codec = avcodec_find_encoder(AV_CODEC_ID_AAC))) {
        fprintf(stderr, "Could not find an AAC encoder.\n");
        goto cleanup;
    }
    LOGCATE("000000000000000000000");
    /** Create a new audio stream in the output file container. */
    if (!(stream = avformat_new_stream(*output_format_context, output_codec))) {
        fprintf(stderr, "Could not create new stream\n");
        error = AVERROR(ENOMEM);
        goto cleanup;
    }
    *output_codec_context = stream->codec;

    /**
    * Set the basic encoder parameters.
    * The input file's sample rate is used to avoid a sample rate conversion.
    */
    (*output_codec_context)->codec_id = (*output_format_context)->oformat->audio_codec;
    (*output_codec_context)->codec_type = AVMEDIA_TYPE_AUDIO;
    (*output_codec_context)->channels = OUTPUT_CHANNELS;
    (*output_codec_context)->channel_layout = av_get_default_channel_layout(OUTPUT_CHANNELS);
    //samples per second
    (*output_codec_context)->sample_rate = input_codec_context->sample_rate;
    (*output_codec_context)->sample_fmt = output_codec->sample_fmts[0];
    (*output_codec_context)->bit_rate = OUTPUT_BIT_RATE;

    /** Allow the use of the experimental AAC encoder */
    (*output_codec_context)->strict_std_compliance = FF_COMPLIANCE_EXPERIMENTAL;

    /** Set the sample rate for the container. */
    stream->time_base.den = input_codec_context->sample_rate;
    stream->time_base.num = 1;

    /**
    * Some container formats (like MP4) require global headers to be present
    * Mark the encoder so that it behaves accordingly.
    */
    if ((*output_format_context)->oformat->flags & AVFMT_GLOBALHEADER)
        (*output_codec_context)->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;

    /** Open the encoder for the audio stream to use it later. */
    if ((error = avcodec_open2(*output_codec_context, output_codec, NULL)) < 0) {
//        fprintf(stderr, "Could not open output codec (error '%s')\n",
//                get_error_text(error));
        goto cleanup;
    }

    return 0;
    cleanup:
    avio_closep(&(*output_format_context)->pb);
    avformat_free_context(*output_format_context);
    *output_format_context = NULL;
    return error < 0 ? error : AVERROR_EXIT;
}

int init_resampler(AVCodecContext *input_codec_context,
                   AVCodecContext *output_codec_context,
                   SwrContext **resample_context){
    int error;

    *resample_context = swr_alloc_set_opts(NULL,
                                           av_get_default_channel_layout(output_codec_context->channels),
                                           output_codec_context->sample_fmt,
                                           output_codec_context->sample_rate,
                                           av_get_default_channel_layout(input_codec_context->channels),
                                           input_codec_context->sample_fmt,
                                           input_codec_context->sample_rate,
                                           0, NULL);
    if (!*resample_context) {
        fprintf(stderr, "Could not allocate resample context\n");
        return AVERROR(ENOMEM);
    }
    if ((error = swr_init(*resample_context)) < 0) {
        fprintf(stderr, "Could not open resample context\n");
        swr_free(resample_context);
        return error;
    }
    return 0;
}

extern "C" jstring
Java_com_kangwang_ffmpeddemo_FFmpegdiaPlayer_native_1version(JNIEnv *env, jobject thiz) {
    string hello = av_version_info();
    string avutil = to_string(avutil_version());
    hello.append(avutil);
    LOGCATI("avutil");
    return env->NewStringUTF(hello.c_str());
}
extern "C" jlong
Java_com_kangwang_ffmpeddemo_FFmpegdiaPlayer_initPlay(JNIEnv *env, jobject thiz, jstring path,
                                                      jobject surface) {
    char *input = const_cast<char *>(env->GetStringUTFChars(path, 0));
    VideoPlay *videoPlay = new VideoPlay(env, input, surface, thiz);
    return reinterpret_cast<jlong>(videoPlay);
}

void init_packet(AVPacket *packet){
    av_init_packet(packet);
    packet->data = NULL;
    packet->size = 0;
}

int encode_audio_frame(AVFrame *frame,int nbsamples,
                       AVFormatContext *output_format_context,
                       AVCodecContext *output_codec_context){
    int got_packet;
    AVPacket enc_pkt;
    init_packet(&enc_pkt);

    int ret = avcodec_encode_audio2(output_codec_context, &enc_pkt,
                                    frame, &got_packet);
    printf("ret:%d\t got_packet:%d\n", ret, got_packet);
    if (ret < 0 || !got_packet){
        av_free_packet(&enc_pkt);
        av_frame_free(&frame);
        return 1;
    }

    /** Set a timestamp based on the sample rate for the container. */
    if (enc_pkt.pts == AV_NOPTS_VALUE){
        pts += nbsamples;
        enc_pkt.pts = pts;
    }

    ret = av_interleaved_write_frame(output_format_context, &enc_pkt);
    av_free_packet(&enc_pkt);
    av_frame_free(&frame);
    if (ret < 0){
        return 1;
    }else{
        framecnt++;
    }
    return 0;
}

/**
实现步骤：
1.先将音频进行解码
2.将pcm放入队列中，然后从队列中取出，重新编码
*/
extern "C"
JNIEXPORT void JNICALL
Java_com_kangwang_ffmpeddemo_FFmpegdiaPlayer_zhuanma(JNIEnv * env,jobject thiz,jstring outpath,jstring path) {
    const char *out_file = env->GetStringUTFChars(outpath, 0);
    const char *infile = env->GetStringUTFChars(path, 0);
    av_register_all();
    //编码格式上下文
    AVFormatContext *input_format_context = NULL, *output_format_context = NULL;
    //编码器上下文
    AVCodecContext *input_codec_context = NULL, *output_codec_context = NULL;
    SwrContext *resample_context = NULL;
    AVAudioFifo* audiofifo = NULL;
    //可以理解为  上下文设置值
    /** Open the input file for reading. */
    open_input_file(infile, &input_format_context, &input_codec_context);
    //        goto cleanup;
    /** Open the output file for writing. */
    open_output_file(out_file, input_codec_context,&output_format_context, &output_codec_context);
    //设置采样参数
    init_resampler(input_codec_context, output_codec_context, &resample_context);
    //Write Header
    avformat_write_header(output_format_context, NULL);
    int out_framesize = output_codec_context->frame_size;
    AVSampleFormat out_sample_fmt = output_codec_context->sample_fmt;
    int out_channels = av_get_channel_layout_nb_channels(output_codec_context->channel_layout);
    audiofifo = av_audio_fifo_alloc(out_sample_fmt, out_channels, 1);
    int readFinished = 0;
    while (1){
        while (av_audio_fifo_size(audiofifo) < out_framesize){
            AVPacket input_packet;
            init_packet(&input_packet);
            if (!av_read_frame(input_format_context, &input_packet)){
                AVFrame *input_frame = av_frame_alloc();
                uint8_t ** audio_data_buffer = NULL;
                int got_frame = 0;
                avcodec_decode_audio4(input_codec_context, input_frame, &got_frame, &input_packet);
                if (got_frame){
                //开辟一个空间   用来存储
                    av_samples_alloc_array_and_samples(&audio_data_buffer, NULL, out_channels, input_frame->nb_samples, out_sample_fmt, 1);
                    //这里的out nb_samples 必须传输入采样数
                    int convert_nb_samples = swr_convert(resample_context, audio_data_buffer, input_frame->nb_samples,
                                                         (const uint8_t**)input_frame->data, input_frame->nb_samples);
                    av_audio_fifo_realloc(audiofifo, av_audio_fifo_size(audiofifo) + input_frame->nb_samples);
                    av_audio_fifo_write(audiofifo, (void **)audio_data_buffer, input_frame->nb_samples);

                }
                av_free_packet(&input_packet);
                av_frame_free(&input_frame);
                if (audio_data_buffer) {
                    av_free(audio_data_buffer[0]);
                    av_free(audio_data_buffer);
                }
            }else{
                readFinished = 1;
                av_free_packet(&input_packet);
                break;
            }
        }
        while (av_audio_fifo_size(audiofifo) >= out_framesize || (readFinished&&av_audio_fifo_size(audiofifo)>0)){
            int frame_size = FFMIN(av_audio_fifo_size(audiofifo), out_framesize);
            AVFrame* output_frame = NULL;
            output_frame = av_frame_alloc();
            output_frame->nb_samples = frame_size;
            output_frame->channel_layout = output_codec_context->channel_layout;
            output_frame->format = output_codec_context->sample_fmt;
            output_frame->sample_rate = output_codec_context->sample_rate;
            av_frame_get_buffer(output_frame, 0);
            av_audio_fifo_read(audiofifo, (void **)output_frame->data, frame_size);
            encode_audio_frame(output_frame, frame_size, output_format_context, output_codec_context);
        }
        if (readFinished){
            if (output_codec_context->codec->capabilities &AV_CODEC_CAP_DELAY){
                while (!encode_audio_frame(NULL, out_framesize, output_format_context, output_codec_context)){ ; }
            }
            break;
        }
    }

//  末尾写入音轨，可以拖动控制位置
    if (av_write_trailer(output_format_context) < 0) {
        printf("Could not write output file trailer (error '%s')\n");
//        goto cleanup;
    }
    printf("framecnt:%d\n", framecnt);
    int ret = 0;
    cleanup:

    if (audiofifo)
        av_audio_fifo_free(audiofifo);
    swr_free(&resample_context);
    if (output_codec_context)
        avcodec_close(output_codec_context);
    if (output_format_context) {
        avio_closep(&output_format_context->pb);
        avformat_free_context(output_format_context);
    }
    if (input_codec_context)
        avcodec_close(input_codec_context);
    if (input_format_context)
        avformat_close_input(&input_format_context);
}


//    const char *out_file = env->GetStringUTFChars(outpath, 0);
//    const char *infile = env->GetStringUTFChars(path, 0);
//
//    av_register_all();
//    avcodec_register_all();
//
//    AVFormatContext *inputFormatCtx = NULL;
//
//    // 打开输入音频文件
//    int ret = avformat_open_input(&inputFormatCtx, infile, NULL, 0);
//
//    if (ret != 0) {
//        LOGCATE("打开文件失败");
//        return;
//    }
//
//    //获取音频中流的相关信息
//    ret = avformat_find_stream_info(inputFormatCtx, 0);
//
//    if (ret != 0) {
//        LOGCATE("不能获取流信息");
//        return;
//    }
//
//    // 获取数据中音频流的序列号，这是一个标识符
//    int  index = 0,audioStream = -1;
//    AVCodecContext *inputCodecCtx;
//
//    for (index = 0; index <inputFormatCtx->nb_streams; index++) {
//
//        AVStream *stream = inputFormatCtx->streams[index];
//        AVCodecContext *code = stream->codec;
//        if (code->codec_type == AVMEDIA_TYPE_AUDIO){
//            audioStream = index;
//            break;
//        }
//    }
//
//    //从音频流中获取输入编解码相关的上下文
//    inputCodecCtx = inputFormatCtx->streams[audioStream]->codec;
//    //查找解码器
//    AVCodec *pCodec = avcodec_find_decoder(inputCodecCtx->codec_id);
//    // 打开解码器
//    int result =  avcodec_open2(inputCodecCtx, pCodec, NULL);
//    if (result < 0) {
////        NSLog(@"打开音频解码器失败");
//        return;
//    }
//
//    // 创建aac编码器
//    AVCodec *aacCodec = avcodec_find_encoder(AV_CODEC_ID_AAC);
//
//    if (!aacCodec){
////        printf("Can not find encoder!\n");
//        return ;
//    }
//
//    //常见aac编码相关上下文信息
//    AVCodecContext *aacCodeContex = avcodec_alloc_context3(aacCodec);
//    // 设置编码相关信息
//    aacCodeContex->sample_fmt = aacCodec->sample_fmts[0];
//    aacCodeContex->sample_rate= inputCodecCtx->sample_rate;             // 音频的采样率
//    aacCodeContex->channel_layout = av_get_default_channel_layout(2);
//    aacCodeContex->channels = inputCodecCtx->channels;
//    aacCodeContex->strict_std_compliance = FF_COMPLIANCE_EXPERIMENTAL;
//
//    //打开编码器
//    AVDictionary *opts = NULL;
//    result = avcodec_open2(aacCodeContex, aacCodec, &opts);
//
//    if (result < 0) {
////        NSLog(@"failure open code");
//        return;
//    }
//
//    //初始化先进先出缓存队列
//    AVAudioFifo *fifo = av_audio_fifo_alloc(AV_SAMPLE_FMT_FLTP,aacCodeContex->channels, aacCodeContex->frame_size);
//
//    //获取编码每帧的最大取样数
//    int output_frame_size = aacCodeContex->frame_size;
//
//    // 初始化重采样上下文
//    SwrContext *resample_context = NULL;
//    sws_init_context(resample_context,)
//    if (init_s(inputCodecCtx, aacCodeContex,
//                       &resample_context)){
//    }
//
//    bool finished  = false;
//    while (1) {
//
//        if (finished){
//            break;
//        }
//
//        // 查看fifo队列中的大小是否超过可以编码的一帧的大小
//        while (av_audio_fifo_size(fifo) < output_frame_size) {
//
//            // 如果没超过，则继续进行解码
//
//            if (finished)
//            {
//                break;
//            }
//
//            AVFrame *audioFrame = av_frame_alloc();
//            AVPacket packet;
//            packet.data = NULL;
//            packet.size = 0;
//            int data_present;
//
//            // 读取出一帧未解码数据
//            finished =  (av_read_frame(inputFormatCtx, &packet) == AVERROR_EOF);
//
//            // 判断该帧数据是否为音频数据
//            if (packet.stream_index != audioStream) {
//                continue;
//            }
//
//            // 开始进行解码
//            if ( avcodec_decode_audio4(inputCodecCtx, audioFrame, &data_present, &packet) < 0) {
//                NSLog(@"音频解码失败");
//                return ;
//            }
//
//            if (data_present)
//            {
//                //将pcm数据写入文件
//                for(int i = 0 ; i <audioFrame->channels;i++)
//                {
//                    NSData *data = [NSData dataWithBytes:audioFrame->data[i] length:audioFrame->linesize[0]];
//                    [pcmfileHandle writeData:data];
//
//                }
//            }
//
//            // 初始化进行重采样的存储空间
//            uint8_t **converted_input_samples = NULL;
//            if (init_converted_samples(&converted_input_samples, aacCodeContex,
//                                       audioFrame->nb_samples))
//            {
//                return;
//            }
//
//            // 进行重采样
//            if (convert_samples((const uint8_t**)audioFrame->extended_data, converted_input_samples,
//                                audioFrame->nb_samples, resample_context))
//            {
//                return;
//            }
//
//            //将采样结果加入进fifo中
//            add_samples_to_fifo(fifo, converted_input_samples,audioFrame->nb_samples);
//
//
//            // 释放重采样存储空间
//            if (converted_input_samples)
//            {
//                av_freep(&converted_input_samples[0]);
//                free(converted_input_samples);
//            }
//        }
//
//        // 从fifo队列中读入数据
//        while (av_audio_fifo_size(fifo) >= output_frame_size || finished) {
//
//            AVFrame *frame;
//
//            frame = av_frame_alloc();
//
//            const int frame_size = FFMIN(av_audio_fifo_size(fifo),aacCodeContex->frame_size);
//
//            // 设置输入帧的相关参数
//            (frame)->nb_samples     = frame_size;
//            (frame)->channel_layout = aacCodeContex->channel_layout;
//            (frame)->format         = aacCodeContex->sample_fmt;
//            (frame)->sample_rate    = aacCodeContex->sample_rate;
//
//            int error;
//
//            //根据帧的相关参数，获取数据存储空间
//            if ((error = av_frame_get_buffer(frame, 0)) < 0)
//            {
//                av_frame_free(&frame);
//                return ;
//            }
//
//            // 从fifo中读取frame_size个样本数据
//            if (av_audio_fifo_read(fifo, (void **)frame->data, frame_size) < frame_size)
//            {
//                av_frame_free(&frame);
//                return ;
//            }
//
//            AVPacket pkt;
//            av_init_packet(&pkt);
//            pkt.data = NULL;
//            pkt.size = 0;
//
//            int data_present = 0;
//
//            frame->pts = av_frame_get_best_effort_timestamp(frame);
//            frame->pict_type=AV_PICTURE_TYPE_NONE;
//
//            // 将pcm数据进行编码
//            if ((error = avcodec_encode_audio2(aacCodeContex, &pkt,frame, &data_present)) < 0)
//            {
//                av_free_packet(&pkt);
//                return ;
//            }
//            av_frame_free(&frame);
//
//            // 如果编码成功，写入文件
//            if (data_present) {
////                NSData *data = [NSData dataWithBytes:pkt.data length:pkt.size];
////                NSLog(@"pkt length = %d",pkt.size);
////                [fileHandle writeData:[self adtsDataForPacketLength:pkt.size]];
////                [fileHandle writeData:data];
//            }
//
//            av_free_packet(&pkt);
//        }
//
//    }

////    const char* infile = "H:/near_work/music.mp3";
////    const char* out_file = "music.aac";
//    AVFormatContext *input_context = 0;
//    AVFormatContext *output_context = 0;
//    AVCodec *dec, *out_encode;
//    AVPacket inpkt;
//    AVFrame inframe;
//    int got_inframe = 0;
//    AVStream* in_stream = 0;
//    AVStream* out_stream = 0;
//
//    av_register_all();
//    avformat_open_input(&input_context, infile, 0, 0);
//    avformat_find_stream_info(input_context, 0);
//    av_find_best_stream(input_context, AVMEDIA_TYPE_AUDIO, -1, -1, &dec, 0);
//    in_stream = input_context->streams[0];
//    in_stream->time_base = in_stream->codec->time_base;
//    avcodec_open2(in_stream->codec, dec, NULL);
//
//    avformat_alloc_output_context2(&output_context, NULL, NULL, out_file);
//    avio_open(&output_context->pb, out_file, AVIO_FLAG_READ_WRITE);
//    out_encode = avcodec_find_encoder(AV_CODEC_ID_AAC);
//    out_stream = avformat_new_stream(output_context, out_encode);
//    avcodec_open2(out_stream->codec, out_encode, NULL);
//    out_stream->time_base = out_stream->codec->time_base;
//    avformat_write_header(output_context, NULL);
//
//    out_stream->codec->frame_size = 1152;
//    out_stream->codec->codec_id = output_context->oformat->audio_codec;
//    out_stream->codec->codec_type = AVMEDIA_TYPE_AUDIO;
//    out_stream->codec->sample_fmt = AV_SAMPLE_FMT_S16;
//    out_stream->codec->sample_rate= 44100;
//    out_stream->codec->channel_layout=AV_CH_LAYOUT_STEREO;
//    out_stream->codec->channels = av_get_channel_layout_nb_channels(out_stream->codec->channel_layout);
//    out_stream->codec->bit_rate = 64000;
//    int size = av_samples_get_buffer_size(NULL, out_stream->codec->channels,out_stream->codec->frame_size,out_stream->codec->sample_fmt, 1);
//
//    AVPacket out_pkt;
//    av_new_packet(&out_pkt, size);
//
//    while (av_read_frame(input_context,avPacket)>=0){
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
//
//
//    while(1)
//    {
//        if (av_read_frame(input_context, &inpkt) < 0) break;
//        avcodec_get_frame_defaults(&inframe);
//
//        got_inframe = 0;
//        avcodec_decode_audio4(in_stream->codec, &inframe, &got_inframe, &inpkt);
//        if(got_inframe)
//        {
//            int gf = 0;
//            avcodec_encode_audio2(out_stream->codec, &out_pkt, &inframe, &gf);
//            inpkt.stream_index = out_stream->index;
//            av_write_frame(output_context, &inpkt);
//            av_free_packet(&inpkt);
//        }
//    }
//}


    //初始化先进先出缓存队列
//
//    const char *outputPath = env->GetStringUTFChars(outpath,0);
//    const char *inputPath = env->GetStringUTFChars(path,0);
//    //实现总的上下文
//    avformat_network_init();
//    AVFormatContext *context = avformat_alloc_context();
//    AVFormatContext *out = avformat_alloc_context();
//
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
//    //创建解码器
//    AVCodec  *aucode = avcodec_find_decoder(parameters->codec_id);
//    //创建上下文
//    AVCodecContext *avCodecContext = avcodec_alloc_context3(aucode);
//
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
//    swr_alloc_set_opts(swrContext,
//             out_ch_layout,out_sample,
//    out_sample_rate,in_ch_layout,in_sample,in_sample_rate,
//    0,NULL);
//    swr_init(swrContext);
//    uint8_t *out_buffer = (uint8_t *)av_malloc(2*44100);  //通道 数*采样率
//    //打开文件
//    FILE *fp_pcm = fopen(outputPath,"wb");
//    //读取packet数据
//    AVPacket *avPacket = av_packet_alloc();
//
//
//    // 创建aac编码器
//    AVCodec *aacCodec = avcodec_find_encoder(AV_CODEC_ID_AAC);
//
//    if (!aacCodec){
//        printf("Can not find encoder!\n");
//        return ;
//    }
//
//    //常见aac编码相关上下文信息
//    AVCodecContext *aacCodeContex = avcodec_alloc_context3(aacCodec);
//    // 设置编码相关信息
//    aacCodeContex->sample_fmt = aacCodec->sample_fmts[0];
//    aacCodeContex->sample_rate= avCodecContext->sample_rate;             // 音频的采样率
//    aacCodeContex->channel_layout = av_get_default_channel_layout(2);
//    aacCodeContex->channels = avCodecContext->channels;
//
//    aacCodeContex->strict_std_compliance = FF_COMPLIANCE_EXPERIMENTAL;
//    //打开编码器
//    AVDictionary *opts = NULL;
//    int result = avcodec_open2(aacCodeContex, aacCodec, &opts);
//    //初始化先进先出缓存队列
//    AVAudioFifo *fifo = av_audio_fifo_alloc(AV_SAMPLE_FMT_FLTP,aacCodeContex->channels, aacCodeContex->frame_size);
//
////    //获取编码每帧的最大取样数
////    int output_frame_size = aacCodeContex->frame_size;
////    // 初始化重采样上下文
////    SwrContext *resample_context = NULL;
////    if (init_resampler(inputCodecCtx, aacCodeContex,
////                       &resample_context)){
////    }
//
//
//
//    while (av_read_frame(context,avPacket)>=0){
//        avcodec_send_packet(avCodecContext,avPacket);
//        //我们需要对其进行一次解压缩
//        AVFrame *frame = av_frame_alloc();
//        int ret = avcodec_receive_frame(avCodecContext,frame);
//
//
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
//        add_samples_to
//    }
//    fclose(fp_pcm);
//    av_free(out_buffer);
//    swr_free(&swrContext);
//    avcodec_close(avCodecContext);
//    avformat_close_input(&context);
//}
//// TODO: implement zhuanma()
//    //注册yiqie
//    const char *outFileName = env->GetStringUTFChars(outpath,0);
//    const char *inFileName = env->GetStringUTFChars(path,0);
//    av_register_all();  //注册解码器
//    avcodec_register_all();  //注册编码器
////    av_register_input_format();
////    av_register_output_format();
//    //文件
//
//
//    LOGCATE("111111");
//
//    AVCodec *code = avcodec_find_encoder(AV_CODEC_ID_AAC);
//    if(!code){
//        LOGCATE("avcodec find failed!");
//        return ;
//    }
//
//    //配置编码器的上下文
//    AVCodecContext *ac = avcodec_alloc_context3(code);
//    if(!ac){
//        LOGCATE("avcodec context failed");
//    }
//
//    //采样率
//    ac->sample_rate = 44100;
//    //通道个数
//    ac->channels = 2;
//    //存储格式
//    ac->sample_fmt = AV_SAMPLE_FMT_FLTP;
//
//
//    //码率
//    ac->bit_rate = 64000;
//    //声道 类型
//    ac->channel_layout = AV_CH_LAYOUT_STEREO;
//    //设置头
//    ac->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
//
//
//    //打开 音频编码器
//    int ret = avcodec_open2(ac,code,NULL);
//
//    if(ret<0){
//        return;
//    }
//
//    //输出结构体   上下文
//    AVFormatContext *oc = NULL;
//    //
//    avformat_alloc_output_context2(&oc,NULL,NULL,outFileName);
//
//    if(oc){
//
//    }
//
//    //创建一个音频流
//    AVStream *st = avformat_new_stream(oc,NULL);
//    //设置 参数
//    st->codecpar->codec_tag = 0;
//
//    int xxx = avcodec_parameters_from_context(st->codecpar,ac);
//    if(xxx<0){
//        LOGCATE("xxxxxx");
//    }
//    //输出音频或者视频 信息
////    av_dump_format(oc,0,outFileName,1);
//
//    //打开输出流
//    int ccc = avio_open(&oc->pb,outFileName,AVIO_FLAG_WRITE);
//
//    if(ccc<0){
//        LOGCATE("ecccccccc");
//    }
//    //写入头部信息
//    avformat_write_header(oc,NULL);
//
//    //重采样的上下文
//    SwrContext  *ctx = NULL;
//    //输出的参数
//    // 配置参数    重采样的上下文   输出文件声道布局（立体声）  输出的样本格式   采样率（44100）
//    // 输入的
//    // 输入的声道布局   输入的样本格式  采样率   偏移量   日志
//    LOGCATE("33333 %s",ac);
//    ctx = swr_alloc_set_opts(ctx,
//                       AV_CH_LAYOUT_STEREO,
//                       AV_SAMPLE_FMT_S16,
//            44100,
//            AV_CH_LAYOUT_STEREO,
//            AV_SAMPLE_FMT_S16,
//            44100,
//            0,NULL);
////    是否设置成功
////    进行一次初始化
//    LOGCATE("44444 %s",ctx);
//    swr_init(ctx);
//
//    LOGCATE("222222");
////    打开输入文件    一帧一帧的读取
//    AVFrame *frame = av_frame_alloc();
//    //给 frame设置参数
////    frame    是1024个样本
//    frame->format = AV_SAMPLE_FMT_FLTP;
//    frame->channels = 2;
//    frame->channel_layout = AV_CH_LAYOUT_STEREO;
//    frame->nb_samples = 1024;  //一帧读取的样本数两
//
//    //创建缓存空间
//    av_frame_get_buffer(frame,0);
//
//    //准备缓存空间 ，存储一帧样本的数两    样本数量是nb->samples   双声道   16位储存（一位8字节）
//    int readSize = frame->nb_samples *2*2;
//    //缓存空间
//    char *pcm = new char[readSize];
//
//    LOGCATE("准备读取文件");
//    //读取输入文件   二进制的方式读取
//    FILE  *fp = fopen(inFileName,"rb");
//    LOGCATE("读取之后");
//    //for循环读取
//    for (;  ; ) {
//        LOGCATE("开始遍历  %s",fp);
//        //将一帧 一帧的数据放入到了 缓存区中
//        int len = fread(pcm,1,readSize,fp);
//        LOGCATE("duqu数据  %s",len);
//        if(len<=0){
//            LOGCATE("数据");
//            break;  //读取结束
//        }
//
////        const uint8_t  *data[1];
////        data[0] = (uint8_t *)pcm;
//        //对一帧数据进行重新采样
////        //framedata 来接收 数据
////        //重采样
////        swr_convert(ctx,frame->data,frame->nb_samples,data,frame->nb_samples);
////        //AVFrame一般存储的是原始的数据
////        //packet存储编码之后的数据
////        AVPacket pkt;
////        //初始化
////        av_init_packet(&pkt);
////        //重采样之后的数据   编码
////        //发送到编码 线程
////        avcodec_send_frame(ac,frame);
////        //发送之后，   数据取出来
////        //发送之后我们需要接收他们
////        avcodec_receive_packet(ac,&pkt);
////        //将数据保存到
////        pkt.stream_index = 0;
////        pkt.dts = 0;
////        pkt.pts = 0;
////
////        //将数据写入输出上下文
////        av_interleaved_write_frame(oc,&pkt);
//        LOGCATE("len  %s",len);
//
//    }
//    //删除pcms
//    delete pcm;
//    pcm = NULL;
//
//    //写入 索引   播放可以切换音频的任意地方进行播放
//    av_write_trailer(oc);
//
//
//    //关闭流
//    avio_close(oc->pb);
//    //关闭编码器
//    avcodec_close(ac);
//    avcodec_free_context(&ac);
//    avformat_free_context(oc);
//}