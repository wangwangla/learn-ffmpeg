package com.kangwang.ffmpeddemo;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.view.View;
import android.widget.TextView;

public class ReadVersion extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_read_version);
        TextView viewById = findViewById(R.id.textview);
        viewById.setText(stringFromJNI());
    }

    public native String stringFromJNI();

}