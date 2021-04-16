//
// Created by 28188 on 2021/4/13.
//

#include "VideoPlay.h"

void VideoPlay:: Init(JNIEnv *jniEnv, jobject obj, char *url, jobject surface) {
    jniEnv->GetJavaVM(&m_JavaVM);
    m_JavaObj = jniEnv->NewGlobalRef(obj);
    m_VideoDecoder = new VideoDecoder(jniEnv,url);
    m_AudioDecoder = new AudioDecor(jniEnv,url);
    m_VideoDecoder->Init(jniEnv,obj,url,surface);

}

