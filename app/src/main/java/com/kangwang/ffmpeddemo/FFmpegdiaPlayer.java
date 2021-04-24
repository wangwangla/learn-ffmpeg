package com.kangwang.ffmpeddemo;

import android.media.AudioFormat;
import android.media.AudioManager;
import android.media.AudioTrack;
import android.os.Environment;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

import androidx.annotation.NonNull;

public class FFmpegdiaPlayer implements SurfaceHolder.Callback {
    static {
        System.loadLibrary("play");
    }
    private long playerHandle;
    private SurfaceHolder surfaceHolder;
    private SurfaceView surfaceView;

    public FFmpegdiaPlayer(SurfaceView surfaceView) {
        this.surfaceView = surfaceView;
        if (surfaceHolder != null){
            surfaceHolder.removeCallback(this);
        }
        surfaceHolder = surfaceView.getHolder();
        surfaceHolder.addCallback(this);
        s = Environment.getExternalStorageDirectory().
                getAbsolutePath() + "/input.mp4";

    }
    String s;
    private native void player(String s,Surface surface);

    @Override
    public void surfaceCreated(@NonNull SurfaceHolder holder) {

    }

    @Override
    public void surfaceChanged(@NonNull SurfaceHolder holder, int format, int width, int height) {
        this.surfaceHolder = holder;
//        player(s,surfaceHolder.getSurface());

    }

    public void play(){
//        player(s,surfaceHolder.getSurface());
        initPlay(s,surfaceHolder.getSurface());
    }

    @Override
    public void surfaceDestroyed(@NonNull SurfaceHolder holder) {

    }

    public String versionInfo() {
        return native_version();
    }

    private native String native_version();

    private native long initPlay(String path,Surface surface);

}
