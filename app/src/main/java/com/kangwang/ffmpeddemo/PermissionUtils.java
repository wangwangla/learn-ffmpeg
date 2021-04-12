package com.kangwang.ffmpeddemo;

import android.content.Context;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.os.Build;

import androidx.annotation.NonNull;
import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;
import androidx.core.content.PermissionChecker;
import androidx.fragment.app.FragmentActivity;

public class PermissionUtils {
    /**
     * 请求权限列表
     * @param activity
     * @param permissions
     */
    public static void requestPermissions(@NonNull FragmentActivity activity, @NonNull String[] permissions, int request_code) {
        boolean hasPermissions = permissionsChecking(activity, permissions);
        if (!hasPermissions) {
            ActivityCompat.requestPermissions(activity, permissions, request_code);
        }
    }

    /**
     * 检查权限列表是否授权
     * @param context
     * @param permissions
     * @return
     */
    public static boolean permissionsChecking(@NonNull Context context, @NonNull String[] permissions) {
        int targetVersion = 1;
        try {
            final PackageInfo info = context.getPackageManager()
                    .getPackageInfo(context.getPackageName(), 0);
            targetVersion = info.applicationInfo.targetSdkVersion;
        } catch (PackageManager.NameNotFoundException e) {

        }

        boolean result = true;
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M
                && targetVersion >= Build.VERSION_CODES.M) {
            for (int i = 0; i < permissions.length; i++) {
                result = (ContextCompat.checkSelfPermission(context, permissions[i])
                        == PackageManager.PERMISSION_GRANTED);
                if (!result) {
                    break;
                }
            }
        } else {
            for (int i = 0; i < permissions.length; i++) {
                result = (PermissionChecker.checkSelfPermission(context, permissions[i])
                        == PermissionChecker.PERMISSION_GRANTED);
                if (!result) {
                    break;
                }
            }
        }
        return result;
    }
}
