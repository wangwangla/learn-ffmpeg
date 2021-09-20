package com.kangwang.ffmpeddemo;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.os.Environment;
import android.view.SurfaceView;
import android.view.View;

import com.kangwang.ffmpeddemo.play.FFmpegdiaPlayerVideo;

public class PlayerView extends AppCompatActivity {

    private FFmpegdiaPlayerVideo player;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_player_view);
        SurfaceView surfaceView = findViewById(R.id.surfaceView);
        player = new FFmpegdiaPlayerVideo(surfaceView);
    }

    public void player(){
        String s = Environment.getExternalStorageDirectory().
                getAbsolutePath() + "/input.mp4";
        player.start(s);
    }

    public void play(View view) {
        player();
    }
}