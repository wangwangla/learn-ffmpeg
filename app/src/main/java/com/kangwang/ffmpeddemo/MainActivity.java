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
            player.start(s);
        }
//        player.start(Environment.getExternalStorageDirectory().
//                getAbsolutePath() + "/input.mp4");
//        String s = Environment.getExternalStorageDirectory().
//                getAbsolutePath() + "/input.mp4";
//        player.start(s);
//        try {
//            FileInputStream i = new FileInputStream(new File(s));
//        } catch (FileNotFoundException e) {
//            e.printStackTrace();
//        }
//        System.out.println(player.stringFromJNI());

    }

    public void convert(View view){
        String s = Environment.getExternalStorageDirectory().
                getAbsolutePath() + "/1.mp3";
        String s1 = Environment.getExternalStorageDirectory().
                getAbsolutePath() + "/22.pcm";
        player.playMp3(s,s1);
//        try {
//            FileInputStream i = new FileInputStream(new File(s));
//            System.out.println("文件 存在");
//        } catch (FileNotFoundException e) {
//            e.printStackTrace();
//        }

    }

    private void checkPermissions() {
        PermissionUtils.requestPermissions(this, new String[] {
                Manifest.permission.WRITE_EXTERNAL_STORAGE,
        }, REQUEST_CODE);
    }
}