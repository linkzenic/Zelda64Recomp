#include <android/log.h>
#include <jni.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_gamecontroller.h>
#include <SDL2/SDL_system.h>

namespace {
constexpr const char* kLogTag = "ZeldaSDLActivity";
int virtualJoystickId = -1;
SDL_Joystick* virtualJoystick = nullptr;
SDL_JoystickID virtualJoystickInstanceId = -1;
constexpr SDL_JoystickID kAndroidMotionInstanceId = -0x5A64;

jobject getActivity(JNIEnv* env) {
    jobject activity = static_cast<jobject>(SDL_AndroidGetActivity());
    if (activity == nullptr) {
        __android_log_print(ANDROID_LOG_WARN, kLogTag, "Android activity is not available");
    }
    return activity;
}
}

extern "C" void plume_set_android_surface_ready(int ready);

extern "C" __attribute__((visibility("default"))) void Java_io_github_zelda64recomp_ZeldaSDLActivity_nativeSetAndroidSurfaceReady(
    JNIEnv*,
    jclass,
    jboolean ready) {
    __android_log_print(ANDROID_LOG_VERBOSE, kLogTag, "surface ready=%d", ready ? 1 : 0);
    plume_set_android_surface_ready(ready == JNI_TRUE ? 1 : 0);
}

extern "C" __attribute__((visibility("default"))) void Java_io_github_zelda64recomp_ZeldaSDLActivity_nativeSetAppAudioActive(
    JNIEnv*,
    jclass,
    jboolean active) {
    __android_log_print(ANDROID_LOG_VERBOSE, kLogTag, "audio active=%d", active ? 1 : 0);
}

extern "C" __attribute__((visibility("default"))) jboolean Java_io_github_zelda64recomp_ZeldaSDLActivity_attachController(
    JNIEnv*,
    jobject) {
    if (virtualJoystick != nullptr) {
        return JNI_TRUE;
    }

    if ((SDL_WasInit(SDL_INIT_JOYSTICK) & SDL_INIT_JOYSTICK) == 0) {
        __android_log_print(ANDROID_LOG_WARN, kLogTag, "SDL joystick subsystem is not ready for touch controller");
        return JNI_FALSE;
    }

    virtualJoystickId = SDL_JoystickAttachVirtual(SDL_JOYSTICK_TYPE_GAMECONTROLLER, 6, 16, 0);
    if (virtualJoystickId < 0) {
        __android_log_print(ANDROID_LOG_ERROR, kLogTag, "Could not attach touch controller: %s", SDL_GetError());
        return JNI_FALSE;
    }

    virtualJoystick = SDL_JoystickOpen(virtualJoystickId);
    if (virtualJoystick == nullptr) {
        __android_log_print(ANDROID_LOG_ERROR, kLogTag, "Could not open touch controller: %s", SDL_GetError());
        SDL_JoystickDetachVirtual(virtualJoystickId);
        virtualJoystickId = -1;
        virtualJoystickInstanceId = -1;
        return JNI_FALSE;
    }

    virtualJoystickInstanceId = SDL_JoystickInstanceID(virtualJoystick);
    __android_log_print(ANDROID_LOG_INFO, kLogTag, "Touch controller attached as virtual joystick %d instance %d", virtualJoystickId, virtualJoystickInstanceId);
    return JNI_TRUE;
}

extern "C" __attribute__((visibility("default"))) void Java_io_github_zelda64recomp_ZeldaSDLActivity_detachController(
    JNIEnv*,
    jobject) {
    if (virtualJoystick != nullptr) {
        SDL_JoystickClose(virtualJoystick);
        virtualJoystick = nullptr;
    }
    if (virtualJoystickId >= 0) {
        SDL_JoystickDetachVirtual(virtualJoystickId);
        virtualJoystickId = -1;
    }
    virtualJoystickInstanceId = -1;
}

extern "C" __attribute__((visibility("default"))) void Java_io_github_zelda64recomp_ZeldaSDLActivity_setButton(
    JNIEnv*,
    jobject,
    jint button,
    jboolean value) {
    if (virtualJoystick == nullptr) {
        return;
    }

    SDL_JoystickSetVirtualButton(virtualJoystick, button, value ? 1 : 0);
}

extern "C" __attribute__((visibility("default"))) void Java_io_github_zelda64recomp_ZeldaSDLActivity_setAxis(
    JNIEnv*,
    jobject,
    jint axis,
    jshort value) {
    if (virtualJoystick == nullptr) {
        return;
    }

    SDL_JoystickSetVirtualAxis(virtualJoystick, axis, value);
}

extern "C" __attribute__((visibility("default"))) void Java_io_github_zelda64recomp_ZeldaSDLActivity_submitAndroidMotionSensor(
    JNIEnv*,
    jobject,
    jint sensorType,
    jfloat x,
    jfloat y,
    jfloat z,
    jlong timestampNs) {
    SDL_Event event{};
    event.type = SDL_CONTROLLERSENSORUPDATE;
    event.csensor.timestamp = static_cast<Uint32>(timestampNs / 1000000);
    event.csensor.which = virtualJoystickInstanceId >= 0 ? virtualJoystickInstanceId : kAndroidMotionInstanceId;
    event.csensor.sensor = sensorType == 0 ? SDL_SENSOR_ACCEL : SDL_SENSOR_GYRO;
    event.csensor.data[0] = x;
    event.csensor.data[1] = y;
    event.csensor.data[2] = z;
    event.csensor.timestamp_us = static_cast<Uint64>(timestampNs / 1000);
    SDL_PushEvent(&event);
}

extern "C" __attribute__((visibility("default"))) bool zelda64_android_are_touch_controls_disabled() {
    JNIEnv* env = static_cast<JNIEnv*>(SDL_AndroidGetJNIEnv());
    if (env == nullptr) {
        return false;
    }

    jobject activity = getActivity(env);
    if (activity == nullptr) {
        return false;
    }

    jclass activityClass = env->GetObjectClass(activity);
    jmethodID method = env->GetMethodID(activityClass, "areTouchControlsDisabledFromNative", "()Z");
    if (method == nullptr) {
        __android_log_print(ANDROID_LOG_WARN, kLogTag, "areTouchControlsDisabledFromNative method not found");
        env->DeleteLocalRef(activityClass);
        env->DeleteLocalRef(activity);
        return false;
    }

    const bool disabled = env->CallBooleanMethod(activity, method);
    env->DeleteLocalRef(activityClass);
    env->DeleteLocalRef(activity);
    return disabled;
}

extern "C" __attribute__((visibility("default"))) void zelda64_android_set_touch_controls_disabled(bool disabled) {
    JNIEnv* env = static_cast<JNIEnv*>(SDL_AndroidGetJNIEnv());
    if (env == nullptr) {
        return;
    }

    jobject activity = getActivity(env);
    if (activity == nullptr) {
        return;
    }

    jclass activityClass = env->GetObjectClass(activity);
    jmethodID method = env->GetMethodID(activityClass, "setTouchControlsDisabledFromNative", "(Z)V");
    if (method == nullptr) {
        __android_log_print(ANDROID_LOG_WARN, kLogTag, "setTouchControlsDisabledFromNative method not found");
        env->DeleteLocalRef(activityClass);
        env->DeleteLocalRef(activity);
        return;
    }

    env->CallVoidMethod(activity, method, disabled ? JNI_TRUE : JNI_FALSE);
    env->DeleteLocalRef(activityClass);
    env->DeleteLocalRef(activity);
}

extern "C" __attribute__((visibility("default"))) int zelda64_android_get_touch_face_button_layout() {
    JNIEnv* env = static_cast<JNIEnv*>(SDL_AndroidGetJNIEnv());
    if (env == nullptr) {
        return 0;
    }

    jobject activity = getActivity(env);
    if (activity == nullptr) {
        return 0;
    }

    jclass activityClass = env->GetObjectClass(activity);
    jmethodID method =
        env->GetMethodID(activityClass, "getTouchFaceButtonLayoutFromNative", "()I");
    if (method == nullptr) {
        __android_log_print(
            ANDROID_LOG_WARN, kLogTag, "getTouchFaceButtonLayoutFromNative method not found");
        env->DeleteLocalRef(activityClass);
        env->DeleteLocalRef(activity);
        return 0;
    }

    const int layout = env->CallIntMethod(activity, method);
    env->DeleteLocalRef(activityClass);
    env->DeleteLocalRef(activity);
    return layout;
}

extern "C" __attribute__((visibility("default"))) void zelda64_android_set_touch_face_button_layout(
    int layout) {
    JNIEnv* env = static_cast<JNIEnv*>(SDL_AndroidGetJNIEnv());
    if (env == nullptr) {
        return;
    }

    jobject activity = getActivity(env);
    if (activity == nullptr) {
        return;
    }

    jclass activityClass = env->GetObjectClass(activity);
    jmethodID method =
        env->GetMethodID(activityClass, "setTouchFaceButtonLayoutFromNative", "(I)V");
    if (method == nullptr) {
        __android_log_print(
            ANDROID_LOG_WARN, kLogTag, "setTouchFaceButtonLayoutFromNative method not found");
        env->DeleteLocalRef(activityClass);
        env->DeleteLocalRef(activity);
        return;
    }

    env->CallVoidMethod(activity, method, static_cast<jint>(layout));
    env->DeleteLocalRef(activityClass);
    env->DeleteLocalRef(activity);
}

namespace {
int getTouchIntegerPreference(const char* methodName, int fallback) {
    JNIEnv* env = static_cast<JNIEnv*>(SDL_AndroidGetJNIEnv());
    if (env == nullptr) {
        return fallback;
    }

    jobject activity = getActivity(env);
    if (activity == nullptr) {
        return fallback;
    }

    jclass activityClass = env->GetObjectClass(activity);
    jmethodID method = env->GetMethodID(activityClass, methodName, "()I");
    if (method == nullptr) {
        __android_log_print(ANDROID_LOG_WARN, kLogTag, "%s method not found", methodName);
        env->DeleteLocalRef(activityClass);
        env->DeleteLocalRef(activity);
        return fallback;
    }

    const int value = env->CallIntMethod(activity, method);
    env->DeleteLocalRef(activityClass);
    env->DeleteLocalRef(activity);
    return value;
}

void setTouchIntegerPreference(const char* methodName, int value) {
    JNIEnv* env = static_cast<JNIEnv*>(SDL_AndroidGetJNIEnv());
    if (env == nullptr) {
        return;
    }

    jobject activity = getActivity(env);
    if (activity == nullptr) {
        return;
    }

    jclass activityClass = env->GetObjectClass(activity);
    jmethodID method = env->GetMethodID(activityClass, methodName, "(I)V");
    if (method == nullptr) {
        __android_log_print(ANDROID_LOG_WARN, kLogTag, "%s method not found", methodName);
        env->DeleteLocalRef(activityClass);
        env->DeleteLocalRef(activity);
        return;
    }

    env->CallVoidMethod(activity, method, static_cast<jint>(value));
    env->DeleteLocalRef(activityClass);
    env->DeleteLocalRef(activity);
}
}

extern "C" __attribute__((visibility("default"))) int zelda64_android_get_touch_camera_x_sensitivity() {
    return getTouchIntegerPreference("getTouchCameraXSensitivityFromNative", 100);
}

extern "C" __attribute__((visibility("default"))) void zelda64_android_set_touch_camera_x_sensitivity(
    int sensitivity) {
    setTouchIntegerPreference("setTouchCameraXSensitivityFromNative", sensitivity);
}

extern "C" __attribute__((visibility("default"))) int zelda64_android_get_touch_camera_y_sensitivity() {
    return getTouchIntegerPreference("getTouchCameraYSensitivityFromNative", 100);
}

extern "C" __attribute__((visibility("default"))) void zelda64_android_set_touch_camera_y_sensitivity(
    int sensitivity) {
    setTouchIntegerPreference("setTouchCameraYSensitivityFromNative", sensitivity);
}

extern "C" __attribute__((visibility("default"))) int zelda64_android_get_touch_targeting_mode() {
    return getTouchIntegerPreference("getTouchTargetingModeFromNative", 0);
}

extern "C" __attribute__((visibility("default"))) void zelda64_android_set_touch_targeting_mode(
    int mode) {
    setTouchIntegerPreference("setTouchTargetingModeFromNative", mode);
}
