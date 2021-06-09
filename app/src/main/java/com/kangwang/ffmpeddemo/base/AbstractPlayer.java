package com.kangwang.ffmpeddemo.base;


import android.content.Context;
import android.content.res.AssetFileDescriptor;
import android.media.MediaPlayer;
import android.net.Uri;
import android.util.AttributeSet;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

import androidx.annotation.NonNull;

import java.util.Map;

public abstract class AbstractPlayer extends SurfaceView implements SurfaceHolder.Callback {
    public Context context;
    /**
     * size
     */
    protected int mVideoWidth;
    protected int mVideoHeight;
    /**
     * uri
     */
    protected Uri mUri;
    /**
     * des
     */
    protected AssetFileDescriptor descriptor;
    /**
     * surholder
     */
    protected SurfaceHolder surfaceHolder;
    protected int currentStatus;

    /**
     * fanhui
     */
    public static final int RESUME = 3;

    /**
     * 暂停
     */
    public static final int PAUSE = 2;

    /**
     * bofang
     */
    public static final int PLAY = 1;

    /**
     * 准备好
     */
    public static final int IDEA = 0;
    //    /**
//     * 开始渲染视频画面
//     */
    public static final int MEDIA_INFO_VIDEO_RENDERING_START = 3;
    //
//    /**
//     * 缓冲开始
//     */
    public static final int MEDIA_INFO_BUFFERING_START = 701;
    //
//    /**
//     * 缓冲结束
//     */
    public static final int MEDIA_INFO_BUFFERING_END = 702;
    //
//    /**
//     * 视频旋转信息
//     */
    public static final int MEDIA_INFO_VIDEO_ROTATION_CHANGED = 10001;

    /**
     * TAG
     */
    public final String TAG = getClass().getName();
    /**
     * 播放器事件回调
     */
    protected PlayerEventListener mPlayerEventListener;

    public AbstractPlayer(Context context) {
        super(context);
        this.context = context;
    }

    public AbstractPlayer(Context context, AttributeSet attrs) {
        super(context, attrs);
        this.context = context;
    }

    public AbstractPlayer(Context context, AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
        this.context = context;
    }

    public void init(Context context) {
        this.context = context;
        getHolder().addCallback(this);
        getHolder().setType(SurfaceHolder.SURFACE_TYPE_PUSH_BUFFERS);
    }

    public abstract void setVideoURI(Uri parse);
    /**
     * 设置播放地址
     *
     * @param path    播放地址
     * @param headers 播放地址请求头
     */
    public abstract void setDataSource(String path, Map<String, String> headers);

    /**
     * 用于播放raw和asset里面的视频文件
     */
    public abstract void setDataSource(AssetFileDescriptor fd);

    /**
     * 初始化播放器实例
     */
    public abstract void initPlayer();
    /**
     * 播放
     */
    public abstract void start();

    /**
     * 暂停
     */
    public abstract void pause();

    /**
     * 停止
     */
    public abstract void stop();

    /**
     * 准备开始播放（异步）
     */
    public abstract void prepareAsync();

    /**
     * 重置播放器
     */
    public abstract void reset();

    /**
     * 是否正在播放
     */
    public abstract boolean isPlaying();

    /**
     * 调整进度
     */
    public abstract void seekTo(long time);

    /**
     * 获取当前播放的位置
     * @return
     */
    public abstract int getCurrentPosition();

    /**
     * 获取视频总时长
     */
    public abstract long getDuration();

    /**
     * 获取缓冲百分比
     */
    public abstract int getBufferedPercentage();

    /**
     * 设置渲染视频的View,主要用于TextureView
     */
    public abstract void setSurface(Surface surface);

    /**
     * 设置渲染视频的View,主要用于SurfaceView
     */
    public abstract void setDisplay(SurfaceHolder holder);

    /**
     * 设置音量
     */
    public abstract void setVolume(float v1, float v2);

    /**
     * 设置是否循环播放
     */
    public abstract void setLooping(boolean isLooping);

    /**
     * 设置其他播放配置
     */
    public abstract void setOptions();

    /**
     * 设置播放速度
     */
    public abstract void setSpeed(float speed);

    /**
     * 获取播放速度
     */
    public abstract float getSpeed();

    /**
     * 获取当前缓冲的网速
     */
    public abstract long getTcpSpeed();

    /**
     * 绑定VideoView
     */
    public void setPlayerEventListener(PlayerEventListener playerEventListener) {
        this.mPlayerEventListener = playerEventListener;
    }

    @Override
    public void surfaceCreated(@NonNull SurfaceHolder holder) {
        this.surfaceHolder = holder;
    }

    @Override
    public void surfaceChanged(@NonNull SurfaceHolder holder, int format, int width, int height) {
        this.surfaceHolder = holder;
        if (currentStatus == PAUSE) {
            return;
        }
        openVideo();
    }

    protected abstract void openVideo();

    /**
     * This is called immediately before a surface is being destroyed. After
     * returning from this call, you should no longer try to access this
     * surface.  If you have a rendering thread that directly accesses
     * the surface, you must ensure that thread is no longer touching the
     * Surface before returning from this function.
     *
     * @param holder The SurfaceHolder whose surface is being destroyed.
     */
    @Override
    public void surfaceDestroyed(@NonNull SurfaceHolder holder) {
        onDestroy();
    }

    public abstract void onDestroy();

    public abstract void resume();

    /**
     * 释放播放器
     */
    public abstract void release();


    public interface PlayerEventListener {

        void onError();

        void onCompletion();

        void onInfo(int what, int extra);

        void onPrepared(MediaPlayer mp);

        void onVideoSizeChanged(int width, int height);
    }

    @Override
    protected void onMeasure(int widthMeasureSpec, int heightMeasureSpec) {
        int width = getDefaultSize(mVideoWidth, widthMeasureSpec);
        int height = getDefaultSize(mVideoHeight, heightMeasureSpec);
        if (mVideoWidth > 0 && mVideoHeight > 0) {
            int widthSpecMode = MeasureSpec.getMode(widthMeasureSpec);
            int widthSpecSize = MeasureSpec.getSize(widthMeasureSpec);
            int heightSpecMode = MeasureSpec.getMode(heightMeasureSpec);
            int heightSpecSize = MeasureSpec.getSize(heightMeasureSpec);
            if (widthSpecMode == MeasureSpec.EXACTLY && heightSpecMode == MeasureSpec.EXACTLY) {
                // the size is fixed
                width = widthSpecSize;
                height = heightSpecSize;

                // for compatibility, we adjust size based on aspect ratio
                if (mVideoWidth * height < width * mVideoHeight) {
                    //Log.i("@@@", "image too wide, correcting");
                    width = height * mVideoWidth / mVideoHeight;
                } else if (mVideoWidth * height > width * mVideoHeight) {
                    //Log.i("@@@", "image too tall, correcting");
                    height = width * mVideoHeight / mVideoWidth;
                }
            } else if (widthSpecMode == MeasureSpec.EXACTLY) {
                // only the width is fixed, adjust the height to match aspect ratio if possible
                width = widthSpecSize;
                height = width * mVideoHeight / mVideoWidth;
                if (heightSpecMode == MeasureSpec.AT_MOST && height > heightSpecSize) {
                    // couldn't match aspect ratio within the constraints
                    height = heightSpecSize;
                }
            } else if (heightSpecMode == MeasureSpec.EXACTLY) {
                // only the height is fixed, adjust the width to match aspect ratio if possible
                height = heightSpecSize;
                width = height * mVideoWidth / mVideoHeight;
                if (widthSpecMode == MeasureSpec.AT_MOST && width > widthSpecSize) {
                    // couldn't match aspect ratio within the constraints
                    width = widthSpecSize;
                }
            } else {
                // neither the width nor the height are fixed, try to use actual video size
                width = mVideoWidth;
                height = mVideoHeight;
                if (heightSpecMode == MeasureSpec.AT_MOST && height > heightSpecSize) {
                    // too tall, decrease both width and height
                    height = heightSpecSize;
                    width = height * mVideoWidth / mVideoHeight;
                }
                if (widthSpecMode == MeasureSpec.AT_MOST && width > widthSpecSize) {
                    // too wide, decrease both width and height
                    width = widthSpecSize;
                    height = width * mVideoHeight / mVideoWidth;
                }
            }
        } else {
            // no size yet, just adopt the given spec sizes
        }
        setMeasuredDimension(width, height);
        if (mPlayerEventListener!=null){
            mPlayerEventListener.onVideoSizeChanged(width,height);
        }
    }
}

