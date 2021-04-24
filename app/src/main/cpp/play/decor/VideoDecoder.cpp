//
// Created by 28188 on 2021/4/13.
//
#include "VideoDecoder.h"
#include "LogUtils.h"

VideoDecoder::VideoDecoder(char *url) : PlayBase(url) {
    LOGCATE("create VideoDecoder---------");
}

void VideoDecoder::codingLoop() {
    LOGCATE("codingLoop VideoDecoder---------------------");
}

void VideoDecoder::codingReady() {
    LOGCATE("ready video-----------");
    already = true;
}

VideoDecoder::~VideoDecoder() {

}