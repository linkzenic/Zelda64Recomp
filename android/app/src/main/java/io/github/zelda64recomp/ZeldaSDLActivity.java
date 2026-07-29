package io.github.zelda64recomp;

import android.Manifest;
import android.app.Activity;
import android.content.ClipData;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.content.pm.PackageManager;
import android.content.pm.ActivityInfo;
import android.content.pm.PackageInfo;
import android.content.res.AssetManager;
import android.database.Cursor;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.os.Looper;
import android.provider.OpenableColumns;
import android.provider.Settings;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.Display;
import android.view.MotionEvent;
import android.view.Surface;
import android.view.View;
import android.view.ViewGroup;
import android.view.Window;
import android.view.WindowInsets;
import android.view.WindowInsetsController;
import android.view.Gravity;
import android.widget.Button;
import android.widget.FrameLayout;
import android.widget.ImageView;
import android.widget.TextView;
import android.widget.Toast;

import org.libsdl.app.SDLActivity;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.FileWriter;
import java.io.IOException;
import java.io.InputStream;
import java.io.PrintWriter;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.Locale;
import java.util.zip.ZipEntry;
import java.util.zip.ZipInputStream;

public class ZeldaSDLActivity extends SDLActivity implements SensorEventListener {
    private static final String TAG = "ZeldaSDLActivity";
    private static final int ANDROID_SENSOR_ACCEL = 0;
    private static final int ANDROID_SENSOR_GYRO = 1;
    private static final int TOUCH_CONTROLLER_ATTACH_RETRY_LIMIT = 20;
    private static final long TOUCH_CONTROLLER_ATTACH_RETRY_DELAY_MS = 250;
    // Match the responsive, velocity-style touch camera used by the other ports:
    // each swipe updates the held camera speed, and lifting the finger stops it.
    private static final float RIGHT_STICK_X_AXIS_PER_PIXEL = 0.10f;
    private static final float RIGHT_STICK_Y_AXIS_PER_PIXEL = 0.30f;
    private static final float LEFT_STICK_DRAG_RADIUS_DP = 51.0f;
    private static final float LEFT_TOUCH_ZONE_WIDTH = 0.46f;
    private static final float RIGHT_TOUCH_ZONE_START = 0.52f;
    private static final int REQUEST_OPEN_FILE = 0x5A64;
    private static final int REQUEST_STORAGE_PERMISSION = 0x5A65;
    private static final int REQUEST_OPEN_MOD_FILES = 0x5A66;
    private static final int REQUEST_OPEN_DRIVER_FILE = 0x5A67;
    private static final int REQUEST_OPEN_CLOCK_TEXTURE_FILE = 0x5A68;
    private static final int REQUEST_OPEN_JSON_FILE = 0x5A69;
    private static final String PUBLIC_FOLDER_NAME = "Zelda64";
    private static final String PREFS_NAME = "io.github.zelda64recomp.prefs";
    private static final String PREF_TOUCH_CONTROLS_DISABLED = "touchControlsDisabled";
    private static final String PREF_TOUCH_CONTROLS_HIDDEN = "touchControlsHidden";
    private static final String PREF_TOUCH_FACE_BUTTON_LAYOUT = "touchFaceButtonLayout";
    private static final String PREF_TOUCH_CAMERA_X_SENSITIVITY = "touchCameraXSensitivity";
    private static final String PREF_TOUCH_CAMERA_Y_SENSITIVITY = "touchCameraYSensitivity";
    private static final int TOUCH_FACE_BUTTON_LAYOUT_ABXY = 0;
    private static final int TOUCH_FACE_BUTTON_LAYOUT_BAYX = 1;
    private static final int TOUCH_FACE_BUTTON_LAYOUT_GAMECUBE = 2;
    private static final String[] USER_DATA_SUBDIRS = {
            "mods",
            "mod_config",
            "roms",
            "saves",
            "clock_texture_packs"
    };
    private static final String BUNDLED_MODS_ASSET_DIR = "bundled_mods";
    private static final String BUNDLED_MODS_SEEDED_MARKER = ".android_bundled_mods_seeded_v3";
    private static final String LOG_FILE_NAME = "Zelda64Recompiled.log";
    private static final String CRASH_FILE_NAME = "Zelda64Recompiled_crash.txt";
    private static final String SAFE_MODE_FILE_NAME = "Zelda64Recompiled_safe_mode.flag";
    private static final String AUTO_SAFE_MODE_FILE_NAME = "Zelda64Recompiled_auto_safe_mode.flag";
    private static final String CUSTOM_DRIVER_DIR_NAME = "custom_driver";
    private static final String CUSTOM_DRIVER_NAME_FILE = "selected_driver.txt";
    private static final String CUSTOM_DRIVER_DISPLAY_NAME_FILE = "selected_driver_display.txt";
    private static final String CLOCK_TEXTURE_PACK_DIR_NAME = "clock_texture_packs";
    private static final String CLOCK_TEXTURE_PACK_FILE_NAME = "clock_texture_pack.o2r";
    private static final String CLOCK_TEXTURE_PACK_DISPLAY_FILE = "clock_texture_pack_display.txt";
    private static final boolean LITE_BUILD = BuildConfig.ZELDA_LITE_BUILD;
    private static final boolean SDL_NO_THREAD_PROBE = BuildConfig.ZELDA_SDL_NO_THREAD_PROBE;
    private static final boolean RT64_SETUP_PROBE = BuildConfig.ZELDA_RT64_SETUP_PROBE;
    private static final boolean RT64_POST_INIT_PROBE = BuildConfig.ZELDA_RT64_POST_INIT_PROBE;
    private static final boolean RT64_FIRST_UPDATE_PROBE = BuildConfig.ZELDA_RT64_FIRST_UPDATE_PROBE;
    private static final boolean RT64_FIRST_DL_PROBE = BuildConfig.ZELDA_RT64_FIRST_DL_PROBE;
    private static final String RT64_DL_PROBE_STAGE = BuildConfig.ZELDA_RT64_DL_PROBE_STAGE;
    private static final String[] BUNDLED_ANDROID_MODS = {
            "ProxyMM_KV.nrm",
            "ProxyRecomp_KV005.so",
            "yazmt_mm_corelib.nrm",
            "yazmt_mm_global_objects.nrm",
            "yazmt_mm_playermodelmanager.nrm",
            "yazmt_mm_playermodelmanager_fsmodels.nrm",
            "yazmt_mm_playermodelmanager_fsmodels_extlib.so",
            "magemods_audio_api.nrm",
            "magemods_audio_api.so"
    };
    private static final String[] LITE_BUILD_REMOVED_MODS = {
            "ProxyMM_KV.nrm",
            "ProxyRecomp_KV005.so",
            "yazmt_mm_corelib.nrm",
            "yazmt_mm_global_objects.nrm",
            "yazmt_mm_playermodelmanager.nrm",
            "yazmt_mm_playermodelmanager_fsmodels.nrm",
            "yazmt_mm_playermodelmanager_fsmodels_extlib.so",
            "magemods_audio_api.nrm",
            "magemods_audio_api.so",
            ".android_bundled_mods_seeded_v3",
            "mm_recomp_save_editor-2.nrm"
    };
    private static final String[] OBSOLETE_BUNDLED_MODS = {
            "mm_recomp_save_editor-2.nrm"
    };
    private static ZeldaSDLActivity currentActivity;
    private static Thread.UncaughtExceptionHandler previousUncaughtExceptionHandler;
    private static boolean javaCrashHandlerInstalled;

    public static native void nativeSetAndroidSurfaceReady(boolean ready);
    public static native void nativeSetAppAudioActive(boolean active);
    public native boolean attachController();
    public native void detachController();
    public native void setButton(int button, boolean value);
    public native void setAxis(int axis, short value);
    public native void submitAndroidMotionSensor(int sensorType, float x, float y, float z, long timestampNs);
    private static native void nativeOnFileDialogResult(boolean success, String path);
    private static native void nativeOnFileDialogMultipleResult(boolean success, String[] paths);
    private static native void nativeSetLogPaths(String logPath, String crashPath);
    private static native void nativeReloadClockTexturePack();

    private SharedPreferences preferences;
    private boolean activityResumed;
    private boolean windowFocused;
    private boolean appAudioActive;
    private boolean usingPublicDataDir;
    private boolean storageSettingsRequested;
    private boolean publicStorageReadyNotified;
    private File programDir;
    private File appDataDir;
    private File nativeModLibDir;
    private View overlayView;
    private ViewGroup buttonGroup;
    private Button buttonToggle;
    private FrameLayout leftJoystick;
    private ImageView leftJoystickKnob;
    private View rightScreenArea;
    private boolean touchControllerAttached;
    private SensorManager sensorManager;
    private Sensor accelerometerSensor;
    private Sensor gyroscopeSensor;
    private boolean motionSensorsRegistered;
    private File logFile;
    private File crashFile;
    private File safeModeFile;
    private File autoSafeModeFile;
    private File customDriverDir;
    private File customDriverNameFile;
    private File customDriverDisplayNameFile;
    private String selectedCustomDriverName;
    private String selectedCustomDriverDisplayName;
    private boolean safeModeEnabled;
    private final Handler mainHandler = new Handler(Looper.getMainLooper());
    private int touchControllerAttachRetries;
    private int leftStickPointerId = MotionEvent.INVALID_POINTER_ID;
    private float leftStickStartX;
    private float leftStickStartY;
    private int rightStickPointerId = MotionEvent.INVALID_POINTER_ID;
    private float rightStickLastX;
    private float rightStickLastY;
    private volatile int touchCameraXSensitivity = 100;
    private volatile int touchCameraYSensitivity = 100;
    private final Runnable touchControllerAttachRetry = this::retryTouchControllerAttach;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        Log.i(TAG, "onCreate begin");
        currentActivity = this;
        lockLandscape();
        preferences = getSharedPreferences(PREFS_NAME, Context.MODE_PRIVATE);
        touchCameraXSensitivity = clampTouchSensitivity(
                preferences.getInt(PREF_TOUCH_CAMERA_X_SENSITIVITY, 100));
        touchCameraYSensitivity = clampTouchSensitivity(
                preferences.getInt(PREF_TOUCH_CAMERA_Y_SENSITIVITY, 100));
        installJavaCrashHandler();

        programDir = new File(getFilesDir(), "program");
        appDataDir = resolveAppDataDir();
        prepareAppDataDir(appDataDir);
        setupPersistentLogs();
        configureSafeModeState();
        configureCustomDriverState();
        setupMotionSensors();
        nativeModLibDir = new File(getCodeCacheDir(), "native_mods");
        if (!programDir.exists() && !programDir.mkdirs()) {
            Log.w(TAG, "Failed to create program dir: " + programDir);
            appendLog("Failed to create program dir: " + programDir);
        }
        if (!nativeModLibDir.exists() && !nativeModLibDir.mkdirs()) {
            Log.w(TAG, "Failed to create native mod library dir: " + nativeModLibDir);
            appendLog("Failed to create native mod library dir: " + nativeModLibDir);
        }

        try {
            Log.i(TAG, "extracting bundled program assets");
            appendLog("Extracting bundled program assets");
            copyAssetTree("program", programDir);
            if (LITE_BUILD) {
                removeLiteBuildBundledMods(appDataDir);
            } else {
                seedBundledAndroidMods(appDataDir);
            }
            Log.i(TAG, "bundled program assets extracted");
            appendLog("Bundled program assets extracted");
        } catch (IOException e) {
            Log.e(TAG, "Failed to extract bundled program assets", e);
            appendLog("Failed to extract bundled program assets", e);
        }

        Log.i(TAG, "calling SDLActivity.onCreate");
        appendLog("Calling SDLActivity.onCreate");
        try {
            super.onCreate(savedInstanceState);
        } catch (Throwable throwable) {
            writeJavaCrash("Crash during SDLActivity.onCreate", throwable);
            throw throwable;
        }
        Log.i(TAG, "SDLActivity.onCreate returned");
        appendLog("SDLActivity.onCreate returned");
        RandoGenerator.init(this);
        lockLandscape();
        applyImmersiveFullscreen();
        requestHighRefreshRate();
        if (SDL_NO_THREAD_PROBE) {
            showNoThreadProbeOverlay();
            appendLog("SDL no-thread probe overlay shown");
            return;
        }
        setupControllerOverlay();
        attachTouchController();

        Log.i(TAG, "APP_PROGRAM_PATH=" + programDir.getAbsolutePath());
        Log.i(TAG, "APP_FOLDER_PATH=" + appDataDir.getAbsolutePath());
        Log.i(TAG, "APP_NATIVE_LIBS_PATH=" + nativeModLibDir.getAbsolutePath());
        Log.i(TAG, "APP_NATIVE_LIBRARY_DIR=" + getApplicationInfo().nativeLibraryDir);
        Log.i(TAG, "APP_ANDROID_VERSION_NAME=" + getAndroidVersionName());
        Log.i(TAG, "APP_ANDROID_MANUFACTURER=" + Build.MANUFACTURER);
        Log.i(TAG, "APP_ANDROID_MODEL=" + Build.MODEL);
        Log.i(TAG, "APP_ANDROID_SDK=" + Build.VERSION.SDK_INT);
        Log.i(TAG, "APP_LITE_BUILD=" + LITE_BUILD);
        Log.i(TAG, "APP_RT64_SETUP_PROBE=" + RT64_SETUP_PROBE);
        Log.i(TAG, "APP_RT64_POST_INIT_PROBE=" + RT64_POST_INIT_PROBE);
        Log.i(TAG, "APP_RT64_FIRST_UPDATE_PROBE=" + RT64_FIRST_UPDATE_PROBE);
        Log.i(TAG, "APP_RT64_FIRST_DL_PROBE=" + RT64_FIRST_DL_PROBE);
        Log.i(TAG, "APP_RT64_DL_PROBE_STAGE=" + RT64_DL_PROBE_STAGE);
        Log.i(TAG, "APP_SAFE_MODE=" + effectiveSafeModeEnabled());
        appendLog("APP_PROGRAM_PATH=" + programDir.getAbsolutePath());
        appendLog("APP_FOLDER_PATH=" + appDataDir.getAbsolutePath());
        appendLog("APP_NATIVE_LIBS_PATH=" + nativeModLibDir.getAbsolutePath());
        appendLog("APP_NATIVE_LIBRARY_DIR=" + getApplicationInfo().nativeLibraryDir);
        appendLog("APP_ANDROID_VERSION_NAME=" + getAndroidVersionName());
        appendLog("APP_ANDROID_MANUFACTURER=" + Build.MANUFACTURER);
        appendLog("APP_ANDROID_MODEL=" + Build.MODEL);
        appendLog("APP_ANDROID_SDK=" + Build.VERSION.SDK_INT);
        appendLog("APP_LITE_BUILD=" + LITE_BUILD);
        appendLog("APP_RT64_SETUP_PROBE=" + RT64_SETUP_PROBE);
        appendLog("APP_RT64_POST_INIT_PROBE=" + RT64_POST_INIT_PROBE);
        appendLog("APP_RT64_FIRST_UPDATE_PROBE=" + RT64_FIRST_UPDATE_PROBE);
        appendLog("APP_RT64_FIRST_DL_PROBE=" + RT64_FIRST_DL_PROBE);
        appendLog("APP_RT64_DL_PROBE_STAGE=" + RT64_DL_PROBE_STAGE);
        appendLog("APP_SAFE_MODE=" + effectiveSafeModeEnabled());

        if (!usingPublicDataDir) {
            requestPublicStorageAccess();
        }
    }

    private void showNoThreadProbeOverlay() {
        TextView label = new TextView(this);
        label.setText("SDLActivity no-thread probe alive");
        label.setTextColor(0xFFFFFFFF);
        label.setTextSize(24.0f);
        label.setGravity(android.view.Gravity.CENTER);
        label.setBackgroundColor(0xCC221438);
        FrameLayout.LayoutParams params = new FrameLayout.LayoutParams(
                FrameLayout.LayoutParams.MATCH_PARENT,
                FrameLayout.LayoutParams.WRAP_CONTENT);
        params.gravity = android.view.Gravity.CENTER;
        params.leftMargin = 48;
        params.rightMargin = 48;
        addContentView(label, params);
    }

    @Override
    protected String[] getLibraries() {
        return new String[] { "SDL2", "main" };
    }

    @Override
    protected void onNativeLibrariesLoaded() {
        configureNativeLogging();
        applyStartupNativeEnvironment();
    }

    @Override
    protected String getMainFunction() {
        return "SDL_main";
    }

    @Override
    protected void onResume() {
        Log.i(TAG, "onResume begin");
        super.onResume();
        lockLandscape();
        applyImmersiveFullscreen();
        requestHighRefreshRate();
        activityResumed = true;
        updateAppAudioActive();
        notifyIfPublicStorageNowAvailable();
        registerMotionSensors();
        if (!touchControllerAttached) {
            scheduleTouchControllerAttachRetry();
        }
        Log.i(TAG, "onResume end");
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, String[] permissions, int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);
        if (requestCode == REQUEST_STORAGE_PERMISSION) {
            notifyIfPublicStorageNowAvailable();
        }
    }

    @Override
    protected void onPause() {
        activityResumed = false;
        updateAppAudioActive();
        unregisterMotionSensors();
        super.onPause();
    }

    @Override
    public void onWindowFocusChanged(boolean hasFocus) {
        Log.i(TAG, "onWindowFocusChanged=" + hasFocus);
        windowFocused = hasFocus;
        if (hasFocus) {
            applyImmersiveFullscreen();
            requestHighRefreshRate();
        }
        updateAppAudioActive();
        super.onWindowFocusChanged(hasFocus);
    }

    @Override
    protected void onDestroy() {
        if (currentActivity == this) {
            currentActivity = null;
        }
        appendLog("onDestroy");
        if (touchControllerAttached) {
            detachController();
            touchControllerAttached = false;
        }
        mainHandler.removeCallbacks(touchControllerAttachRetry);
        super.onDestroy();
    }

    private void setupMotionSensors() {
        sensorManager = (SensorManager) getSystemService(Context.SENSOR_SERVICE);
        if (sensorManager == null) {
            Log.w(TAG, "Sensor manager is not available");
            appendLog("Sensor manager is not available");
            return;
        }

        accelerometerSensor = sensorManager.getDefaultSensor(Sensor.TYPE_ACCELEROMETER);
        gyroscopeSensor = sensorManager.getDefaultSensor(Sensor.TYPE_GYROSCOPE);
        Log.i(TAG, "Android motion sensors accel=" + (accelerometerSensor != null) + " gyro=" + (gyroscopeSensor != null));
        appendLog("Android motion sensors accel=" + (accelerometerSensor != null) + " gyro=" + (gyroscopeSensor != null));
    }

    private void registerMotionSensors() {
        if (sensorManager == null || motionSensorsRegistered) {
            return;
        }

        boolean registeredAny = false;
        if (accelerometerSensor != null) {
            registeredAny |= sensorManager.registerListener(this, accelerometerSensor, SensorManager.SENSOR_DELAY_GAME);
        }
        if (gyroscopeSensor != null) {
            registeredAny |= sensorManager.registerListener(this, gyroscopeSensor, SensorManager.SENSOR_DELAY_GAME);
        }
        motionSensorsRegistered = registeredAny;
    }

    private void unregisterMotionSensors() {
        if (sensorManager == null || !motionSensorsRegistered) {
            return;
        }

        sensorManager.unregisterListener(this);
        motionSensorsRegistered = false;
    }

    @Override
    public void onSensorChanged(SensorEvent event) {
        if (event.sensor == null) {
            return;
        }

        int sensorType = event.sensor.getType();
        if (sensorType != Sensor.TYPE_ACCELEROMETER && sensorType != Sensor.TYPE_GYROSCOPE) {
            return;
        }

        float[] values = orientMotionValues(event.values);
        if (sensorType == Sensor.TYPE_ACCELEROMETER) {
            submitAndroidMotionSensor(ANDROID_SENSOR_ACCEL, -values[0], values[1], values[2], event.timestamp);
        } else {
            submitAndroidMotionSensor(ANDROID_SENSOR_GYRO, values[0], values[1], values[2], event.timestamp);
        }
    }

    @Override
    public void onAccuracyChanged(Sensor sensor, int accuracy) {
    }

    private float[] orientMotionValues(float[] values) {
        int rotation = getWindowManager().getDefaultDisplay().getRotation();
        float x;
        float y;
        switch (rotation) {
            case Surface.ROTATION_90:
                x = -values[1];
                y = values[0];
                break;
            case Surface.ROTATION_270:
                x = values[1];
                y = -values[0];
                break;
            case Surface.ROTATION_180:
                x = -values[0];
                y = -values[1];
                break;
            case Surface.ROTATION_0:
            default:
                x = values[0];
                y = values[1];
                break;
        }

        return new float[] { x, y, values[2] };
    }

    private void attachTouchController() {
        touchControllerAttached = attachController();
        if (!touchControllerAttached) {
            Log.w(TAG, "Touch controller attach failed; controls will retry on first touch");
            appendLog("Touch controller attach failed; controls will retry");
            scheduleTouchControllerAttachRetry();
        } else {
            touchControllerAttachRetries = 0;
            mainHandler.removeCallbacks(touchControllerAttachRetry);
        }
    }

    private void ensureTouchControllerAttached() {
        if (!touchControllerAttached) {
            attachTouchController();
        }
    }

    private void scheduleTouchControllerAttachRetry() {
        mainHandler.removeCallbacks(touchControllerAttachRetry);
        if (touchControllerAttached || touchControllerAttachRetries >= TOUCH_CONTROLLER_ATTACH_RETRY_LIMIT) {
            return;
        }
        mainHandler.postDelayed(touchControllerAttachRetry, TOUCH_CONTROLLER_ATTACH_RETRY_DELAY_MS);
    }

    private void retryTouchControllerAttach() {
        if (touchControllerAttached) {
            return;
        }
        touchControllerAttachRetries++;
        touchControllerAttached = attachController();
        if (!touchControllerAttached) {
            scheduleTouchControllerAttachRetry();
        } else {
            touchControllerAttachRetries = 0;
        }
    }

    private void setupControllerOverlay() {
        LayoutInflater inflater = (LayoutInflater) getSystemService(LAYOUT_INFLATER_SERVICE);
        overlayView = inflater.inflate(R.layout.touchcontrol_overlay, null);
        FrameLayout.LayoutParams layoutParams = new FrameLayout.LayoutParams(
                FrameLayout.LayoutParams.MATCH_PARENT,
                FrameLayout.LayoutParams.MATCH_PARENT);
        overlayView.setLayoutParams(layoutParams);
        addContentView(overlayView, layoutParams);

        buttonGroup = overlayView.findViewById(R.id.button_group);
        buttonToggle = overlayView.findViewById(R.id.buttonToggle);
        leftJoystick = overlayView.findViewById(R.id.left_joystick);
        leftJoystickKnob = overlayView.findViewById(R.id.left_joystick_knob);
        rightScreenArea = overlayView.findViewById(R.id.right_screen_area);

        addButtonTouchListener(overlayView.findViewById(R.id.buttonSouth), ControllerButtons.BUTTON_A);
        addButtonTouchListener(overlayView.findViewById(R.id.buttonEast), ControllerButtons.BUTTON_B);
        addButtonTouchListener(overlayView.findViewById(R.id.buttonWest), ControllerButtons.BUTTON_X);
        addButtonTouchListener(overlayView.findViewById(R.id.buttonNorth), ControllerButtons.BUTTON_Y);
        applyTouchFaceButtonLayout(preferences.getInt(
                PREF_TOUCH_FACE_BUTTON_LAYOUT, TOUCH_FACE_BUTTON_LAYOUT_ABXY));
        addButtonTouchListener(overlayView.findViewById(R.id.buttonL), ControllerButtons.BUTTON_LB);
        addButtonTouchListener(overlayView.findViewById(R.id.buttonR), ControllerButtons.BUTTON_RB);
        addAxisButtonTouchListener(overlayView.findViewById(R.id.buttonZ),
                ControllerButtons.AXIS_LT, Short.MAX_VALUE);
        addAxisButtonTouchListener(overlayView.findViewById(R.id.buttonZR),
                ControllerButtons.AXIS_RT, Short.MAX_VALUE);
        addButtonTouchListener(overlayView.findViewById(R.id.buttonStart), ControllerButtons.BUTTON_START);
        addButtonTouchListener(overlayView.findViewById(R.id.buttonBack), ControllerButtons.BUTTON_BACK);

        addButtonTouchListener(overlayView.findViewById(R.id.buttonDpadUp), ControllerButtons.BUTTON_DPAD_UP);
        addButtonTouchListener(overlayView.findViewById(R.id.buttonDpadDown), ControllerButtons.BUTTON_DPAD_DOWN);
        addButtonTouchListener(overlayView.findViewById(R.id.buttonDpadLeft), ControllerButtons.BUTTON_DPAD_LEFT);
        addButtonTouchListener(overlayView.findViewById(R.id.buttonDpadRight), ControllerButtons.BUTTON_DPAD_RIGHT);

        setupTouchAreas();
        setupToggleButton();
        applyTouchControlsVisibility();
    }

    private void setupToggleButton() {
        buttonToggle.setOnClickListener((view) -> {
            boolean hidden = buttonGroup.getVisibility() == View.VISIBLE;
            preferences.edit().putBoolean(PREF_TOUCH_CONTROLS_HIDDEN, hidden).apply();
            applyTouchControlsVisibility();
        });
    }

    private void applyTouchControlsVisibility() {
        if (buttonGroup == null) {
            return;
        }
        boolean disabled = preferences.getBoolean(PREF_TOUCH_CONTROLS_DISABLED, false);
        overlayView.setVisibility(disabled ? View.GONE : View.VISIBLE);

        boolean hidden = preferences.getBoolean(PREF_TOUCH_CONTROLS_HIDDEN, false);
        buttonGroup.setVisibility(hidden ? View.INVISIBLE : View.VISIBLE);
    }

    public boolean areTouchControlsDisabledFromNative() {
        return preferences != null && preferences.getBoolean(PREF_TOUCH_CONTROLS_DISABLED, false);
    }

    public void setTouchControlsDisabledFromNative(boolean disabled) {
        if (preferences != null) {
            preferences.edit().putBoolean(PREF_TOUCH_CONTROLS_DISABLED, disabled).apply();
        }
        runOnUiThread(this::applyTouchControlsVisibility);
    }

    public int getTouchFaceButtonLayoutFromNative() {
        return preferences == null ? TOUCH_FACE_BUTTON_LAYOUT_ABXY
                : preferences.getInt(PREF_TOUCH_FACE_BUTTON_LAYOUT, TOUCH_FACE_BUTTON_LAYOUT_ABXY);
    }

    public void setTouchFaceButtonLayoutFromNative(int layout) {
        int normalizedLayout = layout >= TOUCH_FACE_BUTTON_LAYOUT_ABXY &&
                layout <= TOUCH_FACE_BUTTON_LAYOUT_GAMECUBE
                ? layout : TOUCH_FACE_BUTTON_LAYOUT_ABXY;
        if (preferences != null) {
            preferences.edit().putInt(PREF_TOUCH_FACE_BUTTON_LAYOUT, normalizedLayout).apply();
        }
        runOnUiThread(() -> applyTouchFaceButtonLayout(normalizedLayout));
    }

    private void applyTouchFaceButtonLayout(int layout) {
        if (overlayView == null) {
            return;
        }

        Button buttonSouth = overlayView.findViewById(R.id.buttonSouth);
        Button buttonEast = overlayView.findViewById(R.id.buttonEast);
        Button buttonWest = overlayView.findViewById(R.id.buttonWest);
        Button buttonNorth = overlayView.findViewById(R.id.buttonNorth);
        FrameLayout actionButtonCluster = overlayView.findViewById(R.id.action_button_cluster);
        if (layout == TOUCH_FACE_BUTTON_LAYOUT_GAMECUBE) {
            setViewSize(actionButtonCluster, 200, 170);
            configureFaceButton(buttonSouth, "A", 70, 70,
                    Gravity.BOTTOM | Gravity.CENTER_HORIZONTAL, 20, 0, 0, 20);
            configureFaceButton(buttonEast, "B", 46, 46,
                    Gravity.BOTTOM | Gravity.START, 30, 0, 0, 20);
            configureFaceButton(buttonWest, "X", 46, 46,
                    Gravity.TOP | Gravity.END, 0, 37, 0, 0);
            configureFaceButton(buttonNorth, "Y", 46, 46,
                    Gravity.TOP | Gravity.CENTER_HORIZONTAL, 0, 15, 0, 0);
        } else if (layout == TOUCH_FACE_BUTTON_LAYOUT_BAYX) {
            setViewSize(actionButtonCluster, 132, 132);
            configureFaceButton(buttonSouth, "A", 44, 44,
                    Gravity.BOTTOM | Gravity.CENTER_HORIZONTAL, 0, 0, 0, 0);
            configureFaceButton(buttonEast, "B", 44, 44,
                    Gravity.CENTER_VERTICAL | Gravity.END, 0, 0, 0, 0);
            configureFaceButton(buttonWest, "X", 44, 44,
                    Gravity.CENTER_VERTICAL | Gravity.START, 0, 0, 0, 0);
            configureFaceButton(buttonNorth, "Y", 44, 44,
                    Gravity.TOP | Gravity.CENTER_HORIZONTAL, 0, 0, 0, 0);
        } else {
            setViewSize(actionButtonCluster, 132, 132);
            configureFaceButton(buttonSouth, "B", 44, 44,
                    Gravity.BOTTOM | Gravity.CENTER_HORIZONTAL, 0, 0, 0, 0);
            configureFaceButton(buttonEast, "A", 44, 44,
                    Gravity.CENTER_VERTICAL | Gravity.END, 0, 0, 0, 0);
            configureFaceButton(buttonWest, "Y", 44, 44,
                    Gravity.CENTER_VERTICAL | Gravity.START, 0, 0, 0, 0);
            configureFaceButton(buttonNorth, "X", 44, 44,
                    Gravity.TOP | Gravity.CENTER_HORIZONTAL, 0, 0, 0, 0);
        }
    }

    private void configureFaceButton(Button button, String text, int widthDp, int heightDp,
                                     int gravity, int marginStartDp, int marginTopDp,
                                     int marginEndDp, int marginBottomDp) {
        FrameLayout.LayoutParams params = (FrameLayout.LayoutParams) button.getLayoutParams();
        params.width = dpToPixels(widthDp);
        params.height = dpToPixels(heightDp);
        params.gravity = gravity;
        int marginStart = dpToPixels(marginStartDp);
        int marginEnd = dpToPixels(marginEndDp);
        params.setMargins(marginStart, dpToPixels(marginTopDp),
                marginEnd, dpToPixels(marginBottomDp));
        params.setMarginStart(marginStart);
        params.setMarginEnd(marginEnd);
        button.setLayoutParams(params);
        button.setText(text);
    }

    private void setViewSize(View view, int widthDp, int heightDp) {
        ViewGroup.LayoutParams params = view.getLayoutParams();
        params.width = dpToPixels(widthDp);
        params.height = dpToPixels(heightDp);
        view.setLayoutParams(params);
    }

    private int dpToPixels(float dp) {
        return Math.round(dp * getResources().getDisplayMetrics().density);
    }

    public int getTouchCameraXSensitivityFromNative() {
        return touchCameraXSensitivity;
    }

    public void setTouchCameraXSensitivityFromNative(int sensitivity) {
        int normalizedSensitivity = clampTouchSensitivity(sensitivity);
        touchCameraXSensitivity = normalizedSensitivity;
        if (preferences != null) {
            preferences.edit().putInt(PREF_TOUCH_CAMERA_X_SENSITIVITY,
                    normalizedSensitivity).apply();
        }
    }

    public int getTouchCameraYSensitivityFromNative() {
        return touchCameraYSensitivity;
    }

    public void setTouchCameraYSensitivityFromNative(int sensitivity) {
        int normalizedSensitivity = clampTouchSensitivity(sensitivity);
        touchCameraYSensitivity = normalizedSensitivity;
        if (preferences != null) {
            preferences.edit().putInt(PREF_TOUCH_CAMERA_Y_SENSITIVITY,
                    normalizedSensitivity).apply();
        }
    }

    private static int clampTouchSensitivity(int sensitivity) {
        return Math.max(25, Math.min(400, sensitivity));
    }

    private void setTouchButtonPressed(Button button, boolean pressed) {
        button.setPressed(pressed);
        float scale = pressed ? 0.92f : 1.0f;
        button.animate().scaleX(scale).scaleY(scale).setDuration(60).start();
    }

    private void addButtonTouchListener(Button button, int buttonNum) {
        button.setOnTouchListener((view, event) -> {
            switch (event.getActionMasked()) {
                case MotionEvent.ACTION_DOWN:
                    ensureTouchControllerAttached();
                    setButton(buttonNum, true);
                    setTouchButtonPressed(button, true);
                    return true;
                case MotionEvent.ACTION_UP:
                case MotionEvent.ACTION_CANCEL:
                    setButton(buttonNum, false);
                    setTouchButtonPressed(button, false);
                    return true;
                default:
                    return true;
            }
        });
    }

    private void addAxisButtonTouchListener(Button button, int axis, short value) {
        button.setOnTouchListener((view, event) -> {
            switch (event.getActionMasked()) {
                case MotionEvent.ACTION_DOWN:
                    ensureTouchControllerAttached();
                    setAxis(axis, value);
                    setTouchButtonPressed(button, true);
                    return true;
                case MotionEvent.ACTION_UP:
                case MotionEvent.ACTION_CANCEL:
                    setAxis(axis, (short) 0);
                    setTouchButtonPressed(button, false);
                    return true;
                default:
                    return true;
            }
        });
    }

    private void setupTouchAreas() {
        final float leftMaxRadius = LEFT_STICK_DRAG_RADIUS_DP *
                getResources().getDisplayMetrics().density;
        leftJoystick.setVisibility(View.INVISIBLE);

        rightScreenArea.setOnTouchListener((view, event) -> {
            switch (event.getActionMasked()) {
                case MotionEvent.ACTION_DOWN:
                case MotionEvent.ACTION_POINTER_DOWN:
                    return startTouchAreaPointer(view, event, event.getActionIndex());
                case MotionEvent.ACTION_MOVE:
                    updateTouchAreaPointers(event, leftMaxRadius);
                    return leftStickPointerId != MotionEvent.INVALID_POINTER_ID ||
                            rightStickPointerId != MotionEvent.INVALID_POINTER_ID;
                case MotionEvent.ACTION_POINTER_UP:
                    releaseTouchAreaPointer(event.getPointerId(event.getActionIndex()));
                    return true;
                case MotionEvent.ACTION_UP:
                    releaseTouchAreaPointer(event.getPointerId(event.getActionIndex()));
                    return true;
                case MotionEvent.ACTION_CANCEL:
                    resetLeftStick();
                    resetRightStick();
                    return true;
                default:
                    return true;
            }
        });
    }

    private boolean startTouchAreaPointer(View view, MotionEvent event, int pointerIndex) {
        float x = event.getX(pointerIndex);
        float y = event.getY(pointerIndex);
        int pointerId = event.getPointerId(pointerIndex);
        if (x < view.getWidth() * LEFT_TOUCH_ZONE_WIDTH &&
                leftStickPointerId == MotionEvent.INVALID_POINTER_ID) {
            ensureTouchControllerAttached();
            leftStickPointerId = pointerId;
            leftStickStartX = x;
            leftStickStartY = y;
            leftJoystick.setX(x - leftJoystick.getWidth() / 2.0f);
            leftJoystick.setY(y - leftJoystick.getHeight() / 2.0f);
            leftJoystickKnob.setX(leftJoystick.getWidth() / 2.0f -
                    leftJoystickKnob.getWidth() / 2.0f);
            leftJoystickKnob.setY(leftJoystick.getHeight() / 2.0f -
                    leftJoystickKnob.getHeight() / 2.0f);
            leftJoystick.setVisibility(View.VISIBLE);
            return true;
        }
        if (x > view.getWidth() * RIGHT_TOUCH_ZONE_START &&
                rightStickPointerId == MotionEvent.INVALID_POINTER_ID) {
            ensureTouchControllerAttached();
            rightStickPointerId = pointerId;
            rightStickLastX = x;
            rightStickLastY = y;
            setAxis(ControllerButtons.AXIS_RX, (short) 0);
            setAxis(ControllerButtons.AXIS_RY, (short) 0);
            return true;
        }
        return false;
    }

    private void updateTouchAreaPointers(MotionEvent event, float leftMaxRadius) {
        int leftIndex = event.findPointerIndex(leftStickPointerId);
        if (leftIndex >= 0) {
            float deltaX = event.getX(leftIndex) - leftStickStartX;
            float deltaY = event.getY(leftIndex) - leftStickStartY;
            float distance = (float) Math.sqrt(deltaX * deltaX + deltaY * deltaY);
            if (distance > leftMaxRadius && distance > 0.0f) {
                float scale = leftMaxRadius / distance;
                deltaX *= scale;
                deltaY *= scale;
            }
            leftJoystickKnob.setX(leftJoystick.getWidth() / 2.0f + deltaX -
                    leftJoystickKnob.getWidth() / 2.0f);
            leftJoystickKnob.setY(leftJoystick.getHeight() / 2.0f + deltaY -
                    leftJoystickKnob.getHeight() / 2.0f);
            setAxis(ControllerButtons.AXIS_LX,
                    (short) (deltaX / leftMaxRadius * Short.MAX_VALUE));
            setAxis(ControllerButtons.AXIS_LY,
                    (short) (deltaY / leftMaxRadius * Short.MAX_VALUE));
        }

        int rightIndex = event.findPointerIndex(rightStickPointerId);
        if (rightIndex >= 0) {
            float sensitivityX = touchCameraXSensitivity / 100.0f;
            float sensitivityY = touchCameraYSensitivity / 100.0f;
            float x = event.getX(rightIndex);
            float y = event.getY(rightIndex);
            float deltaX = (x - rightStickLastX) * sensitivityX;
            float deltaY = (y - rightStickLastY) * sensitivityY;
            rightStickLastX = x;
            rightStickLastY = y;
            setAxis(ControllerButtons.AXIS_RX,
                    touchCameraDeltaToAxis(deltaX, RIGHT_STICK_X_AXIS_PER_PIXEL));
            setAxis(ControllerButtons.AXIS_RY,
                    touchCameraDeltaToAxis(deltaY, RIGHT_STICK_Y_AXIS_PER_PIXEL));
        }
    }

    private static short touchCameraDeltaToAxis(float delta, float axisPerPixel) {
        float normalized = Math.max(-1.0f, Math.min(1.0f, delta * axisPerPixel));
        return (short) Math.round(normalized * Short.MAX_VALUE);
    }

    private void releaseTouchAreaPointer(int pointerId) {
        if (pointerId == leftStickPointerId) {
            resetLeftStick();
        }
        if (pointerId == rightStickPointerId) {
            resetRightStick();
        }
    }

    private void resetLeftStick() {
        leftStickPointerId = MotionEvent.INVALID_POINTER_ID;
        setAxis(ControllerButtons.AXIS_LX, (short) 0);
        setAxis(ControllerButtons.AXIS_LY, (short) 0);
        leftJoystick.setVisibility(View.INVISIBLE);
    }

    private void resetRightStick() {
        rightStickPointerId = MotionEvent.INVALID_POINTER_ID;
        setAxis(ControllerButtons.AXIS_RX, (short) 0);
        setAxis(ControllerButtons.AXIS_RY, (short) 0);
    }

    public static void openFileDialog() {
        ZeldaSDLActivity activity = currentActivity;
        if (activity == null) {
            Log.w(TAG, "openFileDialog requested without an active activity");
            nativeOnFileDialogResult(false, "");
            return;
        }

        activity.runOnUiThread(() -> {
            try {
                Intent intent = new Intent(Intent.ACTION_OPEN_DOCUMENT);
                intent.addCategory(Intent.CATEGORY_OPENABLE);
                intent.setType("*/*");
                intent.putExtra(Intent.EXTRA_MIME_TYPES, new String[] {
                        "application/octet-stream",
                        "application/x-n64-rom",
                        "application/vnd.nintendo.snes.rom"
                });
                activity.startActivityForResult(intent, REQUEST_OPEN_FILE);
            } catch (Exception e) {
                Log.e(TAG, "Failed to launch file picker", e);
                activity.appendLog("Failed to launch file picker", e);
                nativeOnFileDialogResult(false, "");
            }
        });
    }

    public static void openJsonFileDialog() {
        ZeldaSDLActivity activity = currentActivity;
        if (activity == null) {
            Log.w(TAG, "openJsonFileDialog requested without an active activity");
            nativeOnFileDialogResult(false, "");
            return;
        }

        activity.runOnUiThread(() -> {
            try {
                Intent intent = new Intent(Intent.ACTION_OPEN_DOCUMENT);
                intent.addCategory(Intent.CATEGORY_OPENABLE);
                intent.setType("*/*");
                intent.putExtra(Intent.EXTRA_MIME_TYPES, new String[] {
                        "application/json",
                        "text/json",
                        "text/plain",
                        "application/octet-stream"
                });
                activity.startActivityForResult(intent, REQUEST_OPEN_JSON_FILE);
            } catch (Exception e) {
                Log.e(TAG, "Failed to launch JSON file picker", e);
                activity.appendLog("Failed to launch JSON file picker", e);
                nativeOnFileDialogResult(false, "");
            }
        });
    }

    public static void openModFileDialog() {
        ZeldaSDLActivity activity = currentActivity;
        if (activity == null) {
            Log.w(TAG, "openModFileDialog requested without an active activity");
            nativeOnFileDialogMultipleResult(false, new String[0]);
            return;
        }

        activity.runOnUiThread(() -> {
            try {
                Intent intent = new Intent(Intent.ACTION_OPEN_DOCUMENT);
                intent.addCategory(Intent.CATEGORY_OPENABLE);
                intent.setType("*/*");
                intent.putExtra(Intent.EXTRA_ALLOW_MULTIPLE, true);
                intent.putExtra(Intent.EXTRA_MIME_TYPES, new String[] {
                        "application/zip",
                        "application/octet-stream",
                        "application/x-zip-compressed"
                });
                activity.startActivityForResult(intent, REQUEST_OPEN_MOD_FILES);
            } catch (Exception e) {
                Log.e(TAG, "Failed to launch mod file picker", e);
                activity.appendLog("Failed to launch mod file picker", e);
                nativeOnFileDialogMultipleResult(false, new String[0]);
            }
        });
    }

    public static void openDriverFileDialog() {
        ZeldaSDLActivity activity = currentActivity;
        if (activity == null) {
            Log.w(TAG, "openDriverFileDialog requested without an active activity");
            return;
        }

        activity.runOnUiThread(() -> {
            try {
                Intent intent = new Intent(Intent.ACTION_OPEN_DOCUMENT);
                intent.addCategory(Intent.CATEGORY_OPENABLE);
                intent.setType("*/*");
                intent.putExtra(Intent.EXTRA_MIME_TYPES, new String[] {
                        "application/zip",
                        "application/octet-stream",
                        "application/x-zip-compressed"
                });
                activity.startActivityForResult(intent, REQUEST_OPEN_DRIVER_FILE);
            } catch (Exception e) {
                Log.e(TAG, "Failed to launch driver file picker", e);
                activity.appendLog("Failed to launch driver file picker", e);
                Toast.makeText(activity, "Unable to open file picker.", Toast.LENGTH_LONG).show();
            }
        });
    }

    public static void openClockTextureFileDialog() {
        ZeldaSDLActivity activity = currentActivity;
        if (activity == null) {
            Log.w(TAG, "openClockTextureFileDialog requested without an active activity");
            return;
        }

        activity.runOnUiThread(() -> {
            try {
                Intent intent = new Intent(Intent.ACTION_OPEN_DOCUMENT);
                intent.addCategory(Intent.CATEGORY_OPENABLE);
                intent.setType("*/*");
                intent.putExtra(Intent.EXTRA_MIME_TYPES, new String[] {
                        "application/zip",
                        "application/octet-stream",
                        "application/x-zip-compressed"
                });
                activity.startActivityForResult(intent, REQUEST_OPEN_CLOCK_TEXTURE_FILE);
            } catch (Exception e) {
                Log.e(TAG, "Failed to launch clock texture file picker", e);
                activity.appendLog("Failed to launch clock texture file picker", e);
                Toast.makeText(activity, "Unable to open file picker.", Toast.LENGTH_LONG).show();
            }
        });
    }

    public static void clearCustomDriver() {
        ZeldaSDLActivity activity = currentActivity;
        if (activity == null) {
            Log.w(TAG, "clearCustomDriver requested without an active activity");
            return;
        }

        activity.runOnUiThread(() -> {
            activity.clearCustomDriverSelection();
            Toast.makeText(activity, "Using system GPU driver after restart.", Toast.LENGTH_LONG).show();
        });
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        if (requestCode == REQUEST_OPEN_FILE) {
            if (resultCode == Activity.RESULT_OK && data != null && data.getData() != null) {
                handlePickedFile(data.getData());
            } else {
                nativeOnFileDialogResult(false, "");
            }
            return;
        }

        if (requestCode == REQUEST_OPEN_MOD_FILES) {
            if (resultCode == Activity.RESULT_OK && data != null) {
                handlePickedModFiles(data);
            } else {
                nativeOnFileDialogMultipleResult(false, new String[0]);
            }
            return;
        }

        if (requestCode == REQUEST_OPEN_JSON_FILE) {
            if (resultCode == Activity.RESULT_OK && data != null && data.getData() != null) {
                handlePickedJsonFile(data.getData());
            } else {
                nativeOnFileDialogResult(false, "");
            }
            return;
        }

        if (requestCode == REQUEST_OPEN_DRIVER_FILE) {
            if (resultCode == Activity.RESULT_OK && data != null && data.getData() != null) {
                handlePickedDriverFile(data.getData());
            } else {
                Toast.makeText(this, "GPU driver selection cancelled.", Toast.LENGTH_SHORT).show();
            }
            return;
        }

        if (requestCode == REQUEST_OPEN_CLOCK_TEXTURE_FILE) {
            if (resultCode == Activity.RESULT_OK && data != null && data.getData() != null) {
                handlePickedClockTexturePack(data.getData());
            } else {
                Toast.makeText(this, "Clock texture import cancelled.", Toast.LENGTH_SHORT).show();
            }
            return;
        }

        super.onActivityResult(requestCode, resultCode, data);
    }

    private void lockLandscape() {
        setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_SENSOR_LANDSCAPE);
    }

    private void applyImmersiveFullscreen() {
        Window window = getWindow();
        if (window == null) {
            return;
        }
        View decorView = window.getDecorView();
        if (decorView == null) {
            return;
        }

        window.addFlags(android.view.WindowManager.LayoutParams.FLAG_FULLSCREEN);
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.R) {
            window.setDecorFitsSystemWindows(false);
            WindowInsetsController controller = decorView.getWindowInsetsController();
            if (controller != null) {
                controller.hide(WindowInsets.Type.statusBars() | WindowInsets.Type.navigationBars());
                controller.setSystemBarsBehavior(
                        WindowInsetsController.BEHAVIOR_SHOW_TRANSIENT_BARS_BY_SWIPE);
            }
        }

        decorView.setSystemUiVisibility(
                View.SYSTEM_UI_FLAG_FULLSCREEN
                        | View.SYSTEM_UI_FLAG_HIDE_NAVIGATION
                        | View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY
                        | View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
                        | View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION
                        | View.SYSTEM_UI_FLAG_LAYOUT_STABLE);
    }

    private void requestHighRefreshRate() {
        Window window = getWindow();
        if (window == null) {
            return;
        }

        Display display = getWindowManager().getDefaultDisplay();
        if (display == null) {
            return;
        }

        float bestRefreshRate = 0.0f;
        int bestModeId = 0;
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
            for (Display.Mode mode : display.getSupportedModes()) {
                if (mode.getRefreshRate() > bestRefreshRate) {
                    bestRefreshRate = mode.getRefreshRate();
                    bestModeId = mode.getModeId();
                }
            }
        } else {
            bestRefreshRate = display.getRefreshRate();
        }

        if (bestRefreshRate <= 0.0f) {
            return;
        }

        android.view.WindowManager.LayoutParams params = window.getAttributes();
        params.preferredRefreshRate = bestRefreshRate;
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M && bestModeId != 0) {
            params.preferredDisplayModeId = bestModeId;
        }
        window.setAttributes(params);

        Log.i(TAG, "Requested display refresh rate " + bestRefreshRate + " Hz modeId=" + bestModeId);
        appendLog("Requested display refresh rate " + bestRefreshRate + " Hz modeId=" + bestModeId);
    }

    private void setupPersistentLogs() {
        logFile = new File(appDataDir, LOG_FILE_NAME);
        crashFile = new File(appDataDir, CRASH_FILE_NAME);
        safeModeFile = new File(appDataDir, SAFE_MODE_FILE_NAME);
        autoSafeModeFile = new File(appDataDir, AUTO_SAFE_MODE_FILE_NAME);
        appendLog("");
        appendLog("==== Zelda64 Recompiled Android startup ====");
        appendLog("Device=" + Build.MANUFACTURER + " " + Build.MODEL
                + " SDK=" + Build.VERSION.SDK_INT
                + " ABI=" + Build.SUPPORTED_ABIS[0]);
    }

    private void configureSafeModeState() {
        boolean requestedSafeMode = safeModeFile != null && safeModeFile.exists();
        boolean requestedAutoSafeMode = autoSafeModeFile != null && autoSafeModeFile.exists();

        if (requestedSafeMode && !safeModeFile.delete()) {
            appendLog("Failed to clear legacy safe mode marker");
        }
        if (requestedAutoSafeMode && !autoSafeModeFile.delete()) {
            appendLog("Failed to clear auto safe mode marker");
        }

        safeModeEnabled = requestedSafeMode || requestedAutoSafeMode;
    }

    private void configureCustomDriverState() {
        customDriverDir = new File(getFilesDir(), CUSTOM_DRIVER_DIR_NAME);
        customDriverNameFile = new File(customDriverDir, CUSTOM_DRIVER_NAME_FILE);
        customDriverDisplayNameFile = new File(customDriverDir, CUSTOM_DRIVER_DISPLAY_NAME_FILE);

        String driverName = readSelectedDriverName();
        if (driverName == null) {
            selectedCustomDriverName = null;
            selectedCustomDriverDisplayName = null;
            return;
        }

        File driverFile = new File(customDriverDir, driverName);
        if (!driverFile.exists()) {
            appendLog("Selected custom GPU driver is missing; clearing selection");
            clearCustomDriverFiles();
            return;
        }

        selectedCustomDriverName = driverName;
        selectedCustomDriverDisplayName = readSelectedDriverDisplayName(driverName);
        appendLog("Custom GPU driver selected: " + selectedCustomDriverDisplayName + " (" + driverName + ")");
    }

    private void installJavaCrashHandler() {
        if (javaCrashHandlerInstalled) {
            return;
        }

        javaCrashHandlerInstalled = true;
        previousUncaughtExceptionHandler = Thread.getDefaultUncaughtExceptionHandler();
        Thread.setDefaultUncaughtExceptionHandler((thread, throwable) -> {
            ZeldaSDLActivity activity = currentActivity;
            if (activity != null) {
                activity.writeJavaCrash("Uncaught Java exception on " + thread.getName(), throwable);
            }
            if (previousUncaughtExceptionHandler != null) {
                previousUncaughtExceptionHandler.uncaughtException(thread, throwable);
            } else {
                System.exit(2);
            }
        });
    }

    private void configureNativeLogging() {
        try {
            nativeSetLogPaths(logFile.getAbsolutePath(), crashFile.getAbsolutePath());
        } catch (UnsatisfiedLinkError error) {
            appendLog("Native logging is unavailable", error);
        }
    }

    private void applyStartupNativeEnvironment() {
        if (programDir == null || appDataDir == null || nativeModLibDir == null) {
            appendLog("Native startup environment unavailable");
            return;
        }

        nativeSetenv("APP_PROGRAM_PATH", programDir.getAbsolutePath());
        nativeSetenv("APP_FOLDER_PATH", appDataDir.getAbsolutePath());
        nativeSetenv("APP_NATIVE_LIBS_PATH", nativeModLibDir.getAbsolutePath());
        nativeSetenv("APP_NATIVE_LIBRARY_DIR", getApplicationInfo().nativeLibraryDir);
        nativeSetenv("APP_ANDROID_VERSION_NAME", getAndroidVersionName());
        nativeSetenv("APP_ANDROID_MANUFACTURER", Build.MANUFACTURER);
        nativeSetenv("APP_ANDROID_MODEL", Build.MODEL);
        nativeSetenv("APP_ANDROID_SDK", Integer.toString(Build.VERSION.SDK_INT));
        nativeSetenv("APP_LITE_BUILD", LITE_BUILD ? "1" : "0");
        nativeSetenv("APP_RT64_SETUP_PROBE", RT64_SETUP_PROBE ? "1" : "0");
        nativeSetenv("APP_RT64_POST_INIT_PROBE", RT64_POST_INIT_PROBE ? "1" : "0");
        nativeSetenv("APP_RT64_FIRST_UPDATE_PROBE", RT64_FIRST_UPDATE_PROBE ? "1" : "0");
        nativeSetenv("APP_RT64_FIRST_DL_PROBE", RT64_FIRST_DL_PROBE ? "1" : "0");
        nativeSetenv("APP_RT64_DL_PROBE_STAGE", RT64_DL_PROBE_STAGE);
        nativeSetenv("APP_SAFE_MODE", effectiveSafeModeEnabled() ? "1" : "0");
        applyCustomDriverEnvironment();
        applyClockTexturePackEnvironment();
        appendLog("Native startup environment configured");
    }

    private boolean effectiveSafeModeEnabled() {
        return safeModeEnabled || LITE_BUILD;
    }

    private void appendLog(String message) {
        if (logFile == null) {
            return;
        }

        try (FileWriter writer = new FileWriter(logFile, true)) {
            writer.write(timestamp());
            writer.write(" ");
            writer.write(message);
            writer.write("\n");
        } catch (IOException e) {
            Log.w(TAG, "Failed to write persistent log", e);
        }
    }

    private void appendLog(String message, Throwable throwable) {
        if (logFile == null) {
            return;
        }

        try (PrintWriter writer = new PrintWriter(new FileWriter(logFile, true))) {
            writer.print(timestamp());
            writer.print(" ");
            writer.println(message);
            throwable.printStackTrace(writer);
        } catch (IOException e) {
            Log.w(TAG, "Failed to write persistent log", e);
        }
    }

    private void writeJavaCrash(String message, Throwable throwable) {
        appendLog(message, throwable);
        writeFlagFile(autoSafeModeFile, "Safe mode will be enabled after this crash.\n");
        File targetCrashFile = crashFile != null ? crashFile : new File(getFilesDir(), CRASH_FILE_NAME);
        File parent = targetCrashFile.getParentFile();
        if (parent != null && !parent.exists()) {
            parent.mkdirs();
        }

        try (PrintWriter writer = new PrintWriter(new FileWriter(targetCrashFile, false))) {
            writer.println("Zelda64 Recompiled Android crash");
            writer.println(timestamp());
            writer.println(message);
            writer.println("Device: " + Build.MANUFACTURER + " " + Build.MODEL);
            writer.println("Android SDK: " + Build.VERSION.SDK_INT);
            writer.println("App version: " + getAndroidVersionName());
            writer.println();
            throwable.printStackTrace(writer);
        } catch (IOException e) {
            Log.w(TAG, "Failed to write crash file", e);
        }
    }

    private void writeFlagFile(File targetFile, String contents) {
        if (targetFile == null) {
            return;
        }

        File parent = targetFile.getParentFile();
        if (parent != null && !parent.exists()) {
            parent.mkdirs();
        }

        try (FileWriter writer = new FileWriter(targetFile, false)) {
            writer.write(contents);
        } catch (IOException e) {
            Log.w(TAG, "Failed to write flag file: " + targetFile, e);
        }
    }

    private String readSelectedDriverName() {
        return readCustomDriverMetadata(customDriverNameFile);
    }

    private String readSelectedDriverDisplayName(String fallbackName) {
        String displayName = readCustomDriverMetadata(customDriverDisplayNameFile);
        return displayName != null ? displayName : fallbackName;
    }

    private String readCustomDriverMetadata(File metadataFile) {
        if (metadataFile == null || !metadataFile.exists()) {
            return null;
        }

        try (InputStream input = new java.io.FileInputStream(metadataFile)) {
            byte[] bytes = new byte[(int) metadataFile.length()];
            int offset = 0;
            while (offset < bytes.length) {
                int read = input.read(bytes, offset, bytes.length - offset);
                if (read < 0) {
                    break;
                }
                offset += read;
            }

            String name = new String(bytes, 0, offset, java.nio.charset.StandardCharsets.UTF_8).trim();
            return name.isEmpty() ? null : sanitizeFileName(name);
        } catch (IOException e) {
            Log.w(TAG, "Failed to read custom GPU driver metadata", e);
            return null;
        }
    }

    private void writeSelectedDriverMetadata(String driverName, String displayName) throws IOException {
        if (!customDriverDir.exists() && !customDriverDir.mkdirs()) {
            throw new IOException("Failed to create custom driver dir: " + customDriverDir);
        }
        try (FileWriter writer = new FileWriter(customDriverNameFile, false)) {
            writer.write(driverName);
            writer.write("\n");
        }
        try (FileWriter writer = new FileWriter(customDriverDisplayNameFile, false)) {
            writer.write(displayName);
            writer.write("\n");
        }
    }

    private void setCustomDriverEnvironment(String driverName, String displayName) {
        selectedCustomDriverName = driverName;
        selectedCustomDriverDisplayName = displayName;
        applyCustomDriverEnvironment();
    }

    private void applyCustomDriverEnvironment() {
        if (selectedCustomDriverName == null || selectedCustomDriverName.isEmpty()) {
            clearCustomDriverEnvironment();
            return;
        }

        String driverDir = customDriverDir.getAbsolutePath();
        if (!driverDir.endsWith(File.separator)) {
            driverDir += File.separator;
        }

        nativeSetenv("APP_CUSTOM_VULKAN_DRIVER_DIR", driverDir);
        nativeSetenv("APP_CUSTOM_VULKAN_DRIVER_NAME", selectedCustomDriverName);
        nativeSetenv("APP_CUSTOM_VULKAN_DRIVER_DISPLAY_NAME",
                selectedCustomDriverDisplayName != null && !selectedCustomDriverDisplayName.isEmpty()
                        ? selectedCustomDriverDisplayName : selectedCustomDriverName);
    }

    private void clearCustomDriverEnvironment() {
        selectedCustomDriverName = null;
        selectedCustomDriverDisplayName = null;
        nativeSetenv("APP_CUSTOM_VULKAN_DRIVER_DIR", "");
        nativeSetenv("APP_CUSTOM_VULKAN_DRIVER_NAME", "");
        nativeSetenv("APP_CUSTOM_VULKAN_DRIVER_DISPLAY_NAME", "");
    }

    private void clearCustomDriverSelection() {
        clearCustomDriverEnvironment();
        clearCustomDriverFiles();
        appendLog("Custom GPU driver cleared");
    }

    private void clearCustomDriverFiles() {
        selectedCustomDriverName = null;
        selectedCustomDriverDisplayName = null;
        deleteRecursively(customDriverDir);
        if (!customDriverDir.exists() && !customDriverDir.mkdirs()) {
            Log.w(TAG, "Failed to recreate custom driver dir: " + customDriverDir);
        }
    }

    private String timestamp() {
        return new SimpleDateFormat("yyyy-MM-dd HH:mm:ss.SSS", Locale.US).format(new Date());
    }

    private String getAndroidVersionName() {
        try {
            PackageInfo packageInfo = getPackageManager().getPackageInfo(getPackageName(), 0);
            if (packageInfo.versionName != null) {
                return packageInfo.versionName;
            }
        } catch (PackageManager.NameNotFoundException e) {
            Log.w(TAG, "Failed to read package version", e);
        }
        return "0.1.6";
    }

    private void updateAppAudioActive() {
        boolean active = activityResumed && windowFocused;
        if (active != appAudioActive) {
            appAudioActive = active;
            nativeSetAppAudioActive(active);
        }
    }

    private void copyAssetTree(String assetPath, File outputDir) throws IOException {
        AssetManager assetManager = getAssets();
        String[] children = assetManager.list(assetPath);
        if (children == null || children.length == 0) {
            copyAssetFile(assetPath, outputDir);
            return;
        }

        if (!outputDir.exists() && !outputDir.mkdirs()) {
            throw new IOException("Failed to create directory " + outputDir);
        }

        for (String child : children) {
            String childAssetPath = assetPath + "/" + child;
            File childOutput = new File(outputDir, child);
            copyAssetTree(childAssetPath, childOutput);
        }
    }

    private void copyAssetFile(String assetPath, File outputFile) throws IOException {
        File parent = outputFile.getParentFile();
        if (parent != null && !parent.exists() && !parent.mkdirs()) {
            throw new IOException("Failed to create directory " + parent);
        }

        try (InputStream input = getAssets().open(assetPath);
             FileOutputStream output = new FileOutputStream(outputFile)) {
            byte[] buffer = new byte[64 * 1024];
            int bytesRead;
            while ((bytesRead = input.read(buffer)) != -1) {
                output.write(buffer, 0, bytesRead);
            }
        }
    }

    private File resolveAppDataDir() {
        File publicDir = getPublicDataDir();
        if (canUsePublicDataDir(publicDir)) {
            usingPublicDataDir = true;
            Log.i(TAG, "Using public app data dir: " + publicDir.getAbsolutePath());
            return publicDir;
        }

        usingPublicDataDir = false;
        File privateDir = new File(getFilesDir(), "data");
        Log.w(TAG, "Using private app data dir until storage access is granted: " + privateDir.getAbsolutePath());
        return privateDir;
    }

    private File getPublicDataDir() {
        return new File(Environment.getExternalStorageDirectory(), PUBLIC_FOLDER_NAME);
    }

    private boolean canUsePublicDataDir(File publicDir) {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.R && !Environment.isExternalStorageManager()) {
            return false;
        }

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M
                && Build.VERSION.SDK_INT < Build.VERSION_CODES.R
                && checkSelfPermission(Manifest.permission.WRITE_EXTERNAL_STORAGE) != PackageManager.PERMISSION_GRANTED) {
            return false;
        }

        return publicDir.exists() || publicDir.mkdirs();
    }

    private void requestPublicStorageAccess() {
        if (storageSettingsRequested) {
            return;
        }
        storageSettingsRequested = true;

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.R) {
            Toast.makeText(this, "Grant all files access to use /sdcard/Zelda64 for mods and saves.", Toast.LENGTH_LONG).show();
            try {
                Intent intent = new Intent(Settings.ACTION_MANAGE_APP_ALL_FILES_ACCESS_PERMISSION);
                intent.setData(Uri.parse("package:" + getPackageName()));
                startActivity(intent);
            } catch (Exception e) {
                Log.w(TAG, "Unable to open app-specific all files access settings", e);
                startActivity(new Intent(Settings.ACTION_MANAGE_ALL_FILES_ACCESS_PERMISSION));
            }
            return;
        }

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M
                && checkSelfPermission(Manifest.permission.WRITE_EXTERNAL_STORAGE) != PackageManager.PERMISSION_GRANTED) {
            requestPermissions(new String[] { Manifest.permission.WRITE_EXTERNAL_STORAGE }, REQUEST_STORAGE_PERMISSION);
        }
    }

    private void prepareAppDataDir(File dataDir) {
        if (!dataDir.exists() && !dataDir.mkdirs()) {
            Log.w(TAG, "Failed to create app data dir: " + dataDir);
            return;
        }

        for (String subdir : USER_DATA_SUBDIRS) {
            File child = new File(dataDir, subdir);
            if (!child.exists() && !child.mkdirs()) {
                Log.w(TAG, "Failed to create app data subdir: " + child);
            }
        }
    }

    private void seedBundledAndroidMods(File dataDir) throws IOException {
        File modsDir = new File(dataDir, "mods");
        if (!modsDir.exists() && !modsDir.mkdirs()) {
            throw new IOException("Failed to create mods dir: " + modsDir);
        }

        File seededMarker = new File(modsDir, BUNDLED_MODS_SEEDED_MARKER);
        removeObsoleteBundledMods(modsDir);

        for (String filename : BUNDLED_ANDROID_MODS) {
            File outputFile = new File(modsDir, filename);
            String assetPath = BUNDLED_MODS_ASSET_DIR + "/" + filename;
            if (!outputFile.exists() || !assetMatchesFile(assetPath, outputFile)) {
                copyAssetFile(assetPath, outputFile);
                Log.i(TAG, "Bundled Android mod updated: " + filename);
                appendLog("Bundled Android mod updated: " + filename);
            }
            else {
                Log.i(TAG, "Bundled Android mod current: " + filename);
            }
        }

        if (!seededMarker.exists() && !seededMarker.createNewFile()) {
            Log.w(TAG, "Failed to create bundled mods seeded marker: " + seededMarker);
        }
    }

    private void removeLiteBuildBundledMods(File dataDir) {
        File modsDir = new File(dataDir, "mods");
        if (!modsDir.exists()) {
            appendLog("Lite build: bundled Android mods absent");
            return;
        }

        for (String filename : LITE_BUILD_REMOVED_MODS) {
            File oldFile = new File(modsDir, filename);
            if (oldFile.exists()) {
                if (oldFile.delete()) {
                    appendLog("Lite build removed bundled Android mod: " + filename);
                } else {
                    Log.w(TAG, "Failed to remove Lite bundled mod: " + oldFile);
                    appendLog("Failed to remove Lite bundled mod: " + filename);
                }
            }
        }

        appendLog("Lite build: bundled Android mod seeding skipped");
    }

    private boolean assetMatchesFile(String assetPath, File file) {
        try (InputStream assetInput = getAssets().open(assetPath);
             InputStream fileInput = new java.io.FileInputStream(file)) {
            byte[] assetBuffer = new byte[64 * 1024];
            byte[] fileBuffer = new byte[64 * 1024];
            while (true) {
                int assetBytes = assetInput.read(assetBuffer);
                int fileBytes = fileInput.read(fileBuffer);
                if (assetBytes != fileBytes) {
                    return false;
                }
                if (assetBytes == -1) {
                    return true;
                }
                for (int i = 0; i < assetBytes; i++) {
                    if (assetBuffer[i] != fileBuffer[i]) {
                        return false;
                    }
                }
            }
        } catch (IOException e) {
            Log.w(TAG, "Failed to compare bundled Android mod asset " + assetPath + " with " + file, e);
            return false;
        }
    }

    private void removeObsoleteBundledMods(File modsDir) {
        for (String filename : OBSOLETE_BUNDLED_MODS) {
            File oldFile = new File(modsDir, filename);
            if (oldFile.exists() && !oldFile.delete()) {
                Log.w(TAG, "Failed to remove obsolete bundled mod: " + oldFile);
            }
        }
    }

    private File getRomDir() {
        if (appDataDir == null) {
            appDataDir = resolveAppDataDir();
            prepareAppDataDir(appDataDir);
        }
        return new File(appDataDir, "roms");
    }

    private File getModImportDir() {
        File importDir = new File(getCacheDir(), "mod_imports");
        if (!importDir.exists() && !importDir.mkdirs()) {
            Log.w(TAG, "Failed to create mod import dir: " + importDir);
        }
        return importDir;
    }

    private File getJsonImportDir() {
        File importDir = new File(getCacheDir(), "json_imports");
        if (!importDir.exists() && !importDir.mkdirs()) {
            Log.w(TAG, "Failed to create JSON import dir: " + importDir);
        }
        return importDir;
    }

    private void notifyIfPublicStorageNowAvailable() {
        if (usingPublicDataDir) {
            return;
        }

        File publicDir = getPublicDataDir();
        if (!canUsePublicDataDir(publicDir)) {
            return;
        }
        if (publicStorageReadyNotified) {
            return;
        }
        publicStorageReadyNotified = true;

        prepareAppDataDir(publicDir);
        Toast.makeText(this, "Zelda64 folder is ready. Restart the app to use it for mods and saves.", Toast.LENGTH_LONG).show();
        Log.i(TAG, "Public storage became available: " + publicDir.getAbsolutePath());
    }

    private void handlePickedFile(Uri uri) {
        try {
            File romDir = getRomDir();
            if (!romDir.exists() && !romDir.mkdirs()) {
                throw new IOException("Failed to create ROM dir: " + romDir);
            }

            String displayName = getDisplayName(uri);
            File outputFile = uniqueFileForName(romDir, sanitizeFileName(displayName));

            try (InputStream input = getContentResolver().openInputStream(uri);
                 FileOutputStream output = new FileOutputStream(outputFile)) {
                if (input == null) {
                    throw new IOException("Content resolver returned null stream for " + uri);
                }

                byte[] buffer = new byte[1024 * 1024];
                int bytesRead;
                while ((bytesRead = input.read(buffer)) != -1) {
                    output.write(buffer, 0, bytesRead);
                }
            }

            Log.i(TAG, "Copied selected file to " + outputFile.getAbsolutePath());
            nativeSetenv("APP_PENDING_ROM_PATH", outputFile.getAbsolutePath());
            nativeOnFileDialogResult(true, outputFile.getAbsolutePath());
        } catch (Exception e) {
            Log.e(TAG, "Failed to import selected file", e);
            nativeOnFileDialogResult(false, "");
        }
    }

    private void handlePickedJsonFile(Uri uri) {
        try {
            File importDir = getJsonImportDir();
            String displayName = getDisplayName(uri);
            File outputFile = uniqueFileForName(importDir, sanitizeFileName(displayName));

            try (InputStream input = getContentResolver().openInputStream(uri);
                 FileOutputStream output = new FileOutputStream(outputFile)) {
                if (input == null) {
                    throw new IOException("Content resolver returned null stream for " + uri);
                }

                byte[] buffer = new byte[256 * 1024];
                int bytesRead;
                while ((bytesRead = input.read(buffer)) != -1) {
                    output.write(buffer, 0, bytesRead);
                }
            }

            Log.i(TAG, "Copied selected JSON to " + outputFile.getAbsolutePath());
            nativeOnFileDialogResult(true, outputFile.getAbsolutePath());
        } catch (Exception e) {
            Log.e(TAG, "Failed to import selected JSON file", e);
            nativeOnFileDialogResult(false, "");
        }
    }

    private void handlePickedModFiles(Intent data) {
        try {
            File importDir = getModImportDir();
            java.util.ArrayList<String> paths = new java.util.ArrayList<>();
            ClipData clipData = data.getClipData();

            if (clipData != null) {
                for (int i = 0; i < clipData.getItemCount(); i++) {
                    Uri uri = clipData.getItemAt(i).getUri();
                    if (uri != null) {
                        paths.add(copyPickedModFile(uri, importDir));
                    }
                }
            } else if (data.getData() != null) {
                paths.add(copyPickedModFile(data.getData(), importDir));
            }

            if (paths.isEmpty()) {
                nativeOnFileDialogMultipleResult(false, new String[0]);
                return;
            }

            String[] pathArray = paths.toArray(new String[0]);
            nativeSetenv("APP_PENDING_MOD_PATHS", joinPaths(pathArray));
            nativeOnFileDialogMultipleResult(true, pathArray);
        } catch (Exception e) {
            Log.e(TAG, "Failed to import selected mod file(s)", e);
            nativeOnFileDialogMultipleResult(false, new String[0]);
        }
    }

    private void handlePickedDriverFile(Uri uri) {
        try {
            if (customDriverDir == null) {
                customDriverDir = new File(getFilesDir(), CUSTOM_DRIVER_DIR_NAME);
                customDriverNameFile = new File(customDriverDir, CUSTOM_DRIVER_NAME_FILE);
                customDriverDisplayNameFile = new File(customDriverDir, CUSTOM_DRIVER_DISPLAY_NAME_FILE);
            }

            deleteRecursively(customDriverDir);
            if (!customDriverDir.exists() && !customDriverDir.mkdirs()) {
                throw new IOException("Failed to create custom driver dir: " + customDriverDir);
            }

            String displayName = sanitizeFileName(getDisplayName(uri));
            java.util.ArrayList<String> copiedLibraries = new java.util.ArrayList<>();
            if (displayName.toLowerCase(Locale.US).endsWith(".zip")) {
                copyDriverZip(uri, copiedLibraries);
            } else {
                String copiedName = copyDriverSharedLibrary(uri, displayName);
                copiedLibraries.add(copiedName);
            }

            String driverName = chooseDriverLibrary(copiedLibraries);
            if (driverName == null) {
                clearCustomDriverSelection();
                Toast.makeText(this, "No Vulkan driver .so found in selected file.", Toast.LENGTH_LONG).show();
                return;
            }

            writeSelectedDriverMetadata(driverName, displayName);
            setCustomDriverEnvironment(driverName, displayName);
            appendLog("Custom GPU driver imported: " + displayName + " (" + driverName + ")");
            Toast.makeText(this, "Custom GPU driver selected after restart: " + displayName, Toast.LENGTH_LONG).show();
        } catch (Exception e) {
            Log.e(TAG, "Failed to import custom GPU driver", e);
            appendLog("Failed to import custom GPU driver", e);
            clearCustomDriverSelection();
            Toast.makeText(this, "Failed to import GPU driver.", Toast.LENGTH_LONG).show();
        }
    }

    private void handlePickedClockTexturePack(Uri uri) {
        try {
            File clockPackDir = new File(appDataDir, CLOCK_TEXTURE_PACK_DIR_NAME);
            if (!clockPackDir.exists() && !clockPackDir.mkdirs()) {
                throw new IOException("Failed to create clock texture pack dir: " + clockPackDir);
            }

            String displayName = sanitizeFileName(getDisplayName(uri));
            File outputFile = new File(clockPackDir, CLOCK_TEXTURE_PACK_FILE_NAME);
            try (InputStream input = getContentResolver().openInputStream(uri);
                 FileOutputStream output = new FileOutputStream(outputFile)) {
                if (input == null) {
                    throw new IOException("Content resolver returned null stream for " + uri);
                }

                byte[] buffer = new byte[1024 * 1024];
                int bytesRead;
                while ((bytesRead = input.read(buffer)) != -1) {
                    output.write(buffer, 0, bytesRead);
                }
            }

            writeTextFile(new File(clockPackDir, CLOCK_TEXTURE_PACK_DISPLAY_FILE), displayName);
            applyClockTexturePackEnvironment();
            nativeReloadClockTexturePack();
            appendLog("Clock texture pack imported: " + displayName);
            Toast.makeText(this, "3DS clock texture pack imported.", Toast.LENGTH_SHORT).show();
        } catch (Exception e) {
            Log.e(TAG, "Failed to import clock texture pack", e);
            appendLog("Failed to import clock texture pack", e);
            Toast.makeText(this, "Unable to import clock texture pack.", Toast.LENGTH_LONG).show();
        }
    }

    private void applyClockTexturePackEnvironment() {
        File clockPackDir = new File(appDataDir, CLOCK_TEXTURE_PACK_DIR_NAME);
        File clockPackFile = new File(clockPackDir, CLOCK_TEXTURE_PACK_FILE_NAME);
        File displayFile = new File(clockPackDir, CLOCK_TEXTURE_PACK_DISPLAY_FILE);
        if (clockPackFile.isFile()) {
            nativeSetenv("APP_CLOCK_TEXTURE_PACK_PATH", clockPackFile.getAbsolutePath());
            nativeSetenv("APP_CLOCK_TEXTURE_PACK_DISPLAY_NAME", readTextFile(displayFile, clockPackFile.getName()));
        } else {
            nativeSetenv("APP_CLOCK_TEXTURE_PACK_PATH", "");
            nativeSetenv("APP_CLOCK_TEXTURE_PACK_DISPLAY_NAME", "Bundled");
        }
    }

    private void copyDriverZip(Uri uri, java.util.ArrayList<String> copiedLibraries) throws IOException {
        InputStream rawInput = getContentResolver().openInputStream(uri);
        if (rawInput == null) {
            throw new IOException("Content resolver returned null stream for " + uri);
        }

        try (InputStream input = rawInput;
             ZipInputStream zipInput = new ZipInputStream(input)) {
            ZipEntry entry;
            byte[] buffer = new byte[1024 * 1024];
            while ((entry = zipInput.getNextEntry()) != null) {
                if (entry.isDirectory()) {
                    continue;
                }

                String entryName = sanitizeFileName(new File(entry.getName()).getName());
                if (!entryName.toLowerCase(Locale.US).endsWith(".so")) {
                    continue;
                }

                File outputFile = new File(customDriverDir, entryName);
                try (FileOutputStream output = new FileOutputStream(outputFile)) {
                    int bytesRead;
                    while ((bytesRead = zipInput.read(buffer)) != -1) {
                        output.write(buffer, 0, bytesRead);
                    }
                }
                outputFile.setReadable(true, true);
                outputFile.setExecutable(true, true);
                copiedLibraries.add(entryName);
            }
        }
    }

    private String copyDriverSharedLibrary(Uri uri, String displayName) throws IOException {
        String outputName = displayName.toLowerCase(Locale.US).endsWith(".so")
                ? displayName
                : "libvulkan_freedreno.so";
        File outputFile = new File(customDriverDir, outputName);

        try (InputStream input = getContentResolver().openInputStream(uri);
             FileOutputStream output = new FileOutputStream(outputFile)) {
            if (input == null) {
                throw new IOException("Content resolver returned null stream for " + uri);
            }

            byte[] buffer = new byte[1024 * 1024];
            int bytesRead;
            while ((bytesRead = input.read(buffer)) != -1) {
                output.write(buffer, 0, bytesRead);
            }
        }

        outputFile.setReadable(true, true);
        outputFile.setExecutable(true, true);
        return outputName;
    }

    private String chooseDriverLibrary(java.util.ArrayList<String> libraries) {
        if (libraries.isEmpty()) {
            return null;
        }

        for (String name : libraries) {
            if ("libvulkan_freedreno.so".equals(name)) {
                return name;
            }
        }
        for (String name : libraries) {
            String lowerName = name.toLowerCase(Locale.US);
            if (lowerName.contains("vulkan") && lowerName.endsWith(".so")) {
                return name;
            }
        }
        return libraries.get(0);
    }

    private String copyPickedModFile(Uri uri, File importDir) throws IOException {
        String displayName = getDisplayName(uri);
        File outputFile = uniqueFileForName(importDir, sanitizeFileName(displayName));

        try (InputStream input = getContentResolver().openInputStream(uri);
             FileOutputStream output = new FileOutputStream(outputFile)) {
            if (input == null) {
                throw new IOException("Content resolver returned null stream for " + uri);
            }

            byte[] buffer = new byte[1024 * 1024];
            int bytesRead;
            while ((bytesRead = input.read(buffer)) != -1) {
                output.write(buffer, 0, bytesRead);
            }
        }

        Log.i(TAG, "Copied selected mod file to " + outputFile.getAbsolutePath());
        return outputFile.getAbsolutePath();
    }

    private static void writeTextFile(File file, String text) throws IOException {
        try (FileWriter writer = new FileWriter(file, false)) {
            writer.write(text == null ? "" : text);
        }
    }

    private static String readTextFile(File file, String fallback) {
        if (file == null || !file.isFile()) {
            return fallback;
        }

        try (FileInputStream input = new FileInputStream(file)) {
            byte[] data = new byte[(int) Math.min(file.length(), 4096)];
            int read = input.read(data);
            if (read <= 0) {
                return fallback;
            }
            String value = new String(data, 0, read).trim();
            return value.isEmpty() ? fallback : value;
        } catch (Exception e) {
            return fallback;
        }
    }

    private static void deleteRecursively(File file) {
        if (file == null || !file.exists()) {
            return;
        }

        if (file.isDirectory()) {
            File[] children = file.listFiles();
            if (children != null) {
                for (File child : children) {
                    deleteRecursively(child);
                }
            }
        }

        if (!file.delete()) {
            Log.w(TAG, "Failed to delete " + file);
        }
    }

    private static String joinPaths(String[] paths) {
        StringBuilder builder = new StringBuilder();
        for (String path : paths) {
            if (path == null || path.isEmpty()) {
                continue;
            }
            if (builder.length() > 0) {
                builder.append('\n');
            }
            builder.append(path);
        }
        return builder.toString();
    }

    private String getDisplayName(Uri uri) {
        try (Cursor cursor = getContentResolver().query(uri, null, null, null, null)) {
            if (cursor != null && cursor.moveToFirst()) {
                int nameIndex = cursor.getColumnIndex(OpenableColumns.DISPLAY_NAME);
                if (nameIndex >= 0) {
                    String name = cursor.getString(nameIndex);
                    if (name != null && !name.isEmpty()) {
                        return name;
                    }
                }
            }
        } catch (Exception e) {
            Log.w(TAG, "Unable to query display name for " + uri, e);
        }

        String fallback = uri.getLastPathSegment();
        return fallback == null || fallback.isEmpty() ? "selected.rom" : fallback;
    }

    private static String sanitizeFileName(String name) {
        String sanitized = name.replaceAll("[\\\\/:*?\"<>|]", "_").trim();
        return sanitized.isEmpty() ? "selected.rom" : sanitized;
    }

    private static File uniqueFileForName(File dir, String name) {
        File candidate = new File(dir, name);
        if (!candidate.exists()) {
            return candidate;
        }

        int dot = name.lastIndexOf('.');
        String base = dot > 0 ? name.substring(0, dot) : name;
        String extension = dot > 0 ? name.substring(dot) : "";
        int index = 1;
        do {
            candidate = new File(dir, base + "-" + index + extension);
            index++;
        } while (candidate.exists());
        return candidate;
    }
}
