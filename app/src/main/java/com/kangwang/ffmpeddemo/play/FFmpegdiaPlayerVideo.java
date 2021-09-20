package com.kangwang.ffmpeddemo.play;

import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

import androidx.annotation.NonNull;

public class FFmpegdiaPlayerVideo implements SurfaceHolder.Callback {
    // Used to load the 'native-lib' library on application startup.
    private SurfaceHolder surfaceHolder;
    public FFmpegdiaPlayerVideo(SurfaceView surfaceView) {
        if (surfaceHolder != null){
            surfaceHolder.removeCallback(this);
        }
        surfaceHolder = surfaceView.getHolder();
        surfaceHolder.addCallback(this);

    }

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
