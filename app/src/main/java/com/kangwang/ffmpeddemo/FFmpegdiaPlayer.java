package com.kangwang.ffmpeddemo;

import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

import androidx.annotation.NonNull;

public class FFmpegdiaPlayer implements SurfaceHolder.Callback {
    // Used to load the 'native-lib' library on application startup.
    private SurfaceView surfaceView; //绘制表面
    private SurfaceHolder surfaceHolder;
    static {
        System.loadLibrary("play");
    }

    public FFmpegdiaPlayer(SurfaceView surfaceView) {
        this.surfaceView = surfaceView;
        if (surfaceHolder != null){
            surfaceHolder.removeCallback(this);
        }
        surfaceHolder = surfaceView.getHolder();
        surfaceHolder.addCallback(this);

    }

    public native String stringFromJNI();

    @Override
    public void surfaceCreated(@NonNull SurfaceHolder holder) {

    }

    @Override
    public void surfaceChanged(@NonNull SurfaceHolder holder, int format, int width, int height) {
        this.surfaceHolder = holder;
    }

    @Override
    public void surfaceDestroyed(@NonNull SurfaceHolder holder) {

    }

    public void start(String absolutePath) {
        try {
            natvie_start(absolutePath, surfaceHolder.getSurface());
        }catch (Exception e) {
            System.out.println("------------call  error");
        }
    }

    public native void natvie_start(String absolutePath, Surface surface);
}
