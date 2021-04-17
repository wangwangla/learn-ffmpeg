package com.kangwang.ffmpeddemo;

import android.media.AudioFormat;
import android.media.AudioManager;
import android.media.AudioTrack;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

import androidx.annotation.NonNull;

public class FFmpegdiaPlayer implements SurfaceHolder.Callback {
    // Used to load the 'native-lib' library on application startup.
    private long videoHandle;
    private SurfaceHolder surfaceHolder;
    static {
        System.loadLibrary("play");
    }

    public FFmpegdiaPlayer(SurfaceView surfaceView) {
        if (surfaceHolder != null){
            surfaceHolder.removeCallback(this);
        }
        surfaceHolder = surfaceView.getHolder();
        surfaceHolder.addCallback(this);
    }

    public void initVideo(String path){
        //得到播放器对象
        videoHandle = native_init(path,surfaceHolder.getSurface());
    }

    private native long native_init(String path,Surface surface);

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

    public void start() {
        try {
            natvie_start(videoHandle);
        }catch (Exception e) {
            System.out.println("------------call  error");
        }
    }

    public void playMp3(String s, String absolutePath){
        natvie_startMp3(s,absolutePath);
    }

    public native void natvie_zhuanma(String input,String out);

    public native void natvie_startMp3(String input,String out);

    public native void natvie_start(long videoHandle);

    public void createTrack(int sampleRateInHz,int nb_channals) {
        int channaleConfig;//通道数
        if (nb_channals == 1) {
            channaleConfig = AudioFormat.CHANNEL_OUT_MONO;
        } else if (nb_channals == 2) {
            channaleConfig = AudioFormat.CHANNEL_OUT_STEREO;
        }else {
            channaleConfig = AudioFormat.CHANNEL_OUT_MONO;
        }
        int buffersize= AudioTrack.getMinBufferSize(sampleRateInHz,
                channaleConfig, AudioFormat.ENCODING_PCM_16BIT);
        audioTrack = new AudioTrack(AudioManager.STREAM_MUSIC,sampleRateInHz,channaleConfig,
                AudioFormat.ENCODING_PCM_16BIT,buffersize,AudioTrack.MODE_STREAM);
        audioTrack.play();
    }

    //C传入音频数据
    public void playTrack(byte[] buffer, int lenth) {
        if (audioTrack != null && audioTrack.getPlayState() == AudioTrack.PLAYSTATE_PLAYING) {
            audioTrack.write(buffer, 0, lenth);
        }
    }

    private AudioTrack audioTrack;
}
