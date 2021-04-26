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
};
using namespace std;

extern "C" {

    JNIEXPORT jstring JNICALL
Java_com_kangwang_ffmpeddemo_FFmpegdiaPlayer_native_1version(JNIEnv *env, jobject thiz) {
    string hello = av_version_info();
    string avutil = to_string(avutil_version());
    hello.append(avutil);
    LOGCATI("avutil");
    return env->NewStringUTF(hello.c_str());
}

JNIEXPORT jlong JNICALL
Java_com_kangwang_ffmpeddemo_FFmpegdiaPlayer_initPlay(JNIEnv *env, jobject thiz, jstring path,
                                                      jobject surface) {
    char *input = const_cast<char *>(env->GetStringUTFChars(path, 0));
    VideoPlay *videoPlay = new VideoPlay(env, input, surface, thiz);
    return reinterpret_cast<jlong>(videoPlay);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_kangwang_ffmpeddemo_FFmpegdiaPlayer_playMp3(JNIEnv *env, jobject thiz) {

}
};