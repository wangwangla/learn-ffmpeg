//
// Created by 28188 on 2021/4/12.
//

extern "C" {
#include <libavformat/avformat.h>
#include <android/native_window.h>
#include <pthread.h>
#include <SLES/OpenSLES_Android.h>
}

#include <LogUtils.h>
#include <libswresample/swresample.h>
#include "AudioDecor.h"

AudioDecor::AudioDecor(JNIEnv *env, char *input) {
}
