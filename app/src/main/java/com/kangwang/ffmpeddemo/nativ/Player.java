package com.kangwang.ffmpeddemo.nativ;

public class Player {
    static {
        System.loadLibrary("play");
    }

    public static String version_av(){
        return native_version();
    }

    private static native String native_version();
}
