//
// Created by 28188 on 2021/4/13.
//

#include "VideoPlay.h"

void VideoPlay::Init(JNIEnv *jniEnv, jobject obj, char *url, int renderType, jobject surface) {
    jniEnv->GetJavaVM(&m_JavaVM);
    m_JavaObj = jniEnv->NewGlobalRef(obj);

    m_VideoDecoder = new VideoDecoder(jniEnv,url);
    m_AudioDecoder = new AudioDecor(jniEnv,url);

//    if(renderType == VIDEO_RENDER_OPENGL) {
//        m_VideoDecoder->SetVideoRender(VideoGLRender::GetInstance());
//    } else if (videoRenderType == VIDEO_RENDER_ANWINDOW) {
//        m_VideoRender = new NativeRender(jniEnv, surface);
//        m_VideoDecoder->SetVideoRender(m_VideoRender);
//    } else if (videoRenderType == VIDEO_RENDER_3D_VR) {
//        m_VideoDecoder->SetVideoRender(VRGLRender::GetInstance());
//    }

//    m_AudioRender = new OpenSLRender();
//    m_AudioDecoder->SetAudioRender(m_AudioRender);
//
//    m_VideoDecoder->SetMessageCallback(this, PostMessage);
//    m_AudioDecoder->SetMessageCallback(this, PostMessage);

}

