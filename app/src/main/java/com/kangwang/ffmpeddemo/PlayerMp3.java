package com.kangwang.ffmpeddemo;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.os.Environment;
import android.view.View;

import com.kangwang.ffmpeddemo.play.FFmpegdiaPlayerMp3;

public class PlayerMp3 extends AppCompatActivity {
    private FFmpegdiaPlayerMp3 mp3;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_player_mp3);
        mp3 = new FFmpegdiaPlayerMp3();
    }


    public void play(View view) {
        String s = Environment.getExternalStorageDirectory().
                getAbsolutePath() + "/12.mp3";
        mp3.playMp3(s);
    }
}