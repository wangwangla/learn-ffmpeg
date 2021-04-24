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
        player = new FFmpegdiaPlayer(findViewById(R.id.surfaceView));
        System.out.println("current use version"+player.versionInfo());
    }

    private void checkPermissions() {
        PermissionUtils.requestPermissions(this, new String[] {
                Manifest.permission.WRITE_EXTERNAL_STORAGE,
        }, REQUEST_CODE);
    }

    public void open(View view) {
        player.play();
    }

    public void convert(View view) {
    }
}