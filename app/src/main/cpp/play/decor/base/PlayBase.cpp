//
// Created by Admin on 2021/4/24.
//

#include "PlayBase.h"

PlayBase::PlayBase(char *url) {
    this->m_url = url;
    init();
    startCoding();
}

void PlayBase::init() {

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

void PlayBase::pause() {

}

void PlayBase::resume() {

}

void PlayBase::stop() {

}