package com.kangwang.ffmpeddemo;

import androidx.appcompat.app.AppCompatActivity;

import android.Manifest;
import android.os.Bundle;
import android.os.Environment;
import android.os.FileObserver;
import android.view.SurfaceView;
import android.view.View;
import android.widget.TextView;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;

public class MainActivity extends AppCompatActivity {
    private static final int REQUEST_CODE = 0;
    private FFmpegdiaPlayer player;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        checkPermissions();
        SurfaceView surfaceView = findViewById(R.id.surfaceView);
        player = new FFmpegdiaPlayer(surfaceView);
    }

    public void open(View view){
        if (PermissionUtils.permissionsChecking(
                this,
                new String[] {
                        Manifest.permission.WRITE_EXTERNAL_STORAGE
                })) {
            String s = Environment.getExternalStorageDirectory().
                    getAbsolutePath() + "/input.mp4";
            player.initVideo(s);
        }
    }

    public void convert(View view){
        player.start();
    }

    public void playMp3(View view){
        String s = Environment.getExternalStorageDirectory().
                getAbsolutePath() + "/1.mp3";
        player.playMp3(s);
    }



    private void checkPermissions() {
        PermissionUtils.requestPermissions(this, new String[] {
                Manifest.permission.WRITE_EXTERNAL_STORAGE,
        }, REQUEST_CODE);
    }
}