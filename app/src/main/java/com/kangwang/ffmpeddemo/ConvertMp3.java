package com.kangwang.ffmpeddemo;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.os.Environment;
import android.view.View;

import com.kangwang.ffmpeddemo.play.FFmpegdiaConvertMp3;

public class ConvertMp3 extends AppCompatActivity {
    private FFmpegdiaConvertMp3 convertMp3;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_convert_mp3);
        convertMp3 = new FFmpegdiaConvertMp3();
    }

    public void convert(View view) {

        String input = Environment.getExternalStorageDirectory().
                getAbsolutePath() + "/12.mp3";

        String out = Environment.getExternalStorageDirectory().
                getAbsolutePath() + "/13.mp3";

        convertMp3.convert(input,out);
    }
}