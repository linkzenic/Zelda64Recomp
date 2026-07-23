package io.github.zelda64recomp;

import android.content.Context;
import android.util.Log;

import com.chaquo.python.PyObject;
import com.chaquo.python.Python;
import com.chaquo.python.android.AndroidPlatform;

public final class RandoGenerator {
    private static final String TAG = "ZeldaRando";
    private static Context appContext;

    private RandoGenerator() {
    }

    public static void init(Context context) {
        appContext = context.getApplicationContext();
    }

    public static boolean generate(String yamlDir, String outputDir) {
        try {
            if (appContext == null) {
                Log.e(TAG, "Cannot generate randomizer seed before app context is ready");
                return false;
            }
            if (!Python.isStarted()) {
                Python.start(new AndroidPlatform(appContext));
            }

            PyObject result = Python.getInstance()
                    .getModule("rando_generator")
                    .callAttr("generate", yamlDir, outputDir);
            return result.toBoolean();
        } catch (Throwable t) {
            Log.e(TAG, "Randomizer seed generation failed", t);
            return false;
        }
    }
}
