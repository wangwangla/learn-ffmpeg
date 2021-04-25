//
// Created by Admin on 2021/4/24.
//

extern "C"{
#include <libavformat/avformat.h>

};
#include <LogUtils.h>
#include "PlayBase.h"

PlayBase::PlayBase(char *url) {
    this->m_url = url;
    init();
}

void PlayBase::init() {
    LOGCATE("init data");
    formatContext = avformat_alloc_context();  //处理封装格式的
    AVDictionary *opts = NULL;//从字典中取值
    av_dict_set(&opts, "timeout", "30000000", 0); //如果这么久都没有打开，就认为有问题
    int ret = avformat_open_input(&formatContext,m_url,NULL,&opts);//打开文件
    if (ret){   //为0表示成功
        LOGCATE("open file failed");
        return;
    }else{
        LOGCATE("open file success");
    }

    avformat_find_stream_info(formatContext,NULL); //读取数据流信息   读取视频流  解析出来

    //便利
    for(int i= 0; i<formatContext->nb_streams;i++){
        //视频流
        if(formatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO){
            video_stream_index = i;
        } else if(formatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO){
            audeo_stream_index = i;
        }
    }
}

void PlayBase::play() {
    if(m_thread== nullptr){
        startCoding();
    } else{
        std::unique_lock<std::mutex> lock(m_mutex);
        m_Cond.notify_all();
    }
}

void PlayBase::startCoding() {
    m_thread = new thread(doing, this);
}

void PlayBase::doing(PlayBase *playBase) {
//    开始执行
    playBase->codingReady();
    playBase->codingLoop();
}

