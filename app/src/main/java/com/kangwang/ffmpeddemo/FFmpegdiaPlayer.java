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
    private SurfaceHolder surfaceHolder;
    private String path;
    public FFmpegdiaPlayer(SurfaceView surfaceView) {
        if (surfaceHolder != null){
            surfaceHolder.removeCallback(this);
        }
        surfaceHolder = surfaceView.getHolder();
        surfaceHolder.addCallback(this);
        path = Environment.getExternalStorageDirectory().
                getAbsolutePath() + "/input.mp4";

    }

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
//        initPlay(path,surfaceHolder.getSurface());
//        player(s);
        playMp3();
        
    }

    private native void playMp3();

    @Override
    public void surfaceDestroyed(@NonNull SurfaceHolder holder) {

    }

    public String versionInfo() {
        return native_version();
    }

    private native String native_version();

    private native long initPlay(String path,Surface surface);

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
