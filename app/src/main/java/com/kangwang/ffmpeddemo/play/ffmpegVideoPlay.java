package com.kangwang.ffmpeddemo.play;

import android.content.Context;
import android.content.res.AssetFileDescriptor;
import android.net.Uri;
import android.util.AttributeSet;
import android.view.Surface;
import android.view.SurfaceHolder;

import com.kangwang.ffmpeddemo.base.AbstractPlayer;
import com.kangwang.ffmpeddemo.nativ.Player;

import java.util.Map;

public class ffmpegVideoPlay extends AbstractPlayer {
    private Player player;
    public ffmpegVideoPlay(Context context) {
        super(context);
        init();
    }

    public ffmpegVideoPlay(Context context, AttributeSet attrs) {
        super(context, attrs);
        init();
    }

    public ffmpegVideoPlay(Context context, AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
        init();
    }

    private void init() {
        player = new Player();
        showVersion();
    }

    private void showVersion(){
        System.out.println(player.version_av()+"vvvvvvvvvvv");
    }

    @Override
    public void setVideoURI(Uri parse) {

    }

    @Override
    public void setDataSource(String path, Map<String, String> headers) {

    }

    @Override
    public void setDataSource(AssetFileDescriptor fd) {

    }

    @Override
    public void initPlayer() {

    }

    @Override
    public void start() {

    }

    @Override
    public void pause() {

    }

    @Override
    public void stop() {

    }

    @Override
    public void prepareAsync() {

    }

    @Override
    public void reset() {

    }

    @Override
    public boolean isPlaying() {
        return false;
    }

    @Override
    public void seekTo(long time) {

    }

    @Override
    public int getCurrentPosition() {
        return 0;
    }

    @Override
    public long getDuration() {
        return 0;
    }

    @Override
    public int getBufferedPercentage() {
        return 0;
    }

    @Override
    public void setSurface(Surface surface) {

    }

    @Override
    public void setDisplay(SurfaceHolder holder) {

    }

    @Override
    public void setVolume(float v1, float v2) {

    }

    @Override
    public void setLooping(boolean isLooping) {

    }

    @Override
    public void setOptions() {

    }

    @Override
    public void setSpeed(float speed) {

    }

    @Override
    public float getSpeed() {
        return 0;
    }

    @Override
    public long getTcpSpeed() {
        return 0;
    }

    @Override
    protected void openVideo() {

    }

    @Override
    public void onDestroy() {

    }

    @Override
    public void resume() {

    }

    @Override
    public void release() {

    }
}
