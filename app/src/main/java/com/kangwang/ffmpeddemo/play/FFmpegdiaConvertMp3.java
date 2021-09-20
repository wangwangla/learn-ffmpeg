package com.kangwang.ffmpeddemo.play;

import android.media.AudioFormat;
import android.media.AudioManager;
import android.media.AudioTrack;
import android.view.SurfaceHolder;

public class FFmpegdiaConvertMp3 {
    public void convert(String input, String output){
        natvie_convertMp3(input,output);
    }
    public native void natvie_convertMp3(String input, String output);
}
