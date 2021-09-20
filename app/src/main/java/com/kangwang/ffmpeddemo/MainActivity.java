package com.kangwang.ffmpeddemo;

import androidx.appcompat.app.AppCompatActivity;

import android.Manifest;
import android.content.Intent;
import android.os.Bundle;
import android.os.Environment;
import android.os.FileObserver;
import android.view.SurfaceView;
import android.view.View;
import android.widget.TextView;

import com.kangwang.ffmpeddemo.play.FFmpegdiaPlayerMp3;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;

public class MainActivity extends AppCompatActivity {
    static {
        System.loadLibrary("play");
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
    }

    public void convert(View view) {
    }

    public void init(View view) {
    }

    public void playvideo(View view) {
        selectItem(2);
    }

    public void playMp3(View view) {
        selectItem(3);
    }

    public void convertMp3(View view) {
       selectItem(4);
    }

    public void readvsersion(View view) {
        selectItem(1);
    }

    private void selectItem(int index){
        switch (index){
            case 1:{
                Intent intent = new Intent(this,ReadVersion.class);
                startActivity(intent);
                break;
            }
            case 2: {
                Intent intent = new Intent(this, PlayerView.class);
                startActivity(intent);
                break;
            }
            case 3:{
                Intent intent = new Intent(this, PlayerMp3.class);
                startActivity(intent);
                break;
            }
            case 4:{
                Intent intent = new Intent(this,ConvertMp3.class);
                startActivity(intent);
                break;
            }
        }
    }
}
