#include "apcpp-solo-gen.h"

#include <android/log.h>
#include <dlfcn.h>
#include <jni.h>
#include <string>

namespace sologen {
    namespace {
        constexpr const char* LOG_TAG = "ZeldaRando";

        std::string path_to_utf8(const std::filesystem::path& path) {
            std::u8string path_u8 = path.u8string();
            return std::string(reinterpret_cast<const char*>(path_u8.c_str()), path_u8.size());
        }

        jclass find_app_class(JNIEnv* env, const char* name) {
            jclass activity_thread = env->FindClass("android/app/ActivityThread");
            if (env->ExceptionCheck() || activity_thread == nullptr) {
                env->ExceptionClear();
                return nullptr;
            }

            jmethodID current_application = env->GetStaticMethodID(
                activity_thread,
                "currentApplication",
                "()Landroid/app/Application;"
            );
            if (env->ExceptionCheck() || current_application == nullptr) {
                env->ExceptionClear();
                return nullptr;
            }

            jobject application = env->CallStaticObjectMethod(activity_thread, current_application);
            if (env->ExceptionCheck() || application == nullptr) {
                env->ExceptionClear();
                return nullptr;
            }

            jclass context_class = env->FindClass("android/content/Context");
            jmethodID get_class_loader = env->GetMethodID(
                context_class,
                "getClassLoader",
                "()Ljava/lang/ClassLoader;"
            );
            jobject class_loader = env->CallObjectMethod(application, get_class_loader);
            if (env->ExceptionCheck() || class_loader == nullptr) {
                env->ExceptionClear();
                return nullptr;
            }

            jclass class_loader_class = env->FindClass("java/lang/ClassLoader");
            jmethodID load_class = env->GetMethodID(
                class_loader_class,
                "loadClass",
                "(Ljava/lang/String;)Ljava/lang/Class;"
            );
            jstring class_name = env->NewStringUTF(name);
            jclass found_class = static_cast<jclass>(
                env->CallObjectMethod(class_loader, load_class, class_name)
            );
            env->DeleteLocalRef(class_name);

            if (env->ExceptionCheck()) {
                env->ExceptionDescribe();
                env->ExceptionClear();
                return nullptr;
            }

            return found_class;
        }
    }

    bool generate(const std::filesystem::path& yaml_dir, const std::filesystem::path& output_dir) {
        using GetCreatedJavaVMs = jint (*)(JavaVM**, jsize, jsize*);

        auto get_created_java_vms = reinterpret_cast<GetCreatedJavaVMs>(
            dlsym(RTLD_DEFAULT, "JNI_GetCreatedJavaVMs")
        );
        if (get_created_java_vms == nullptr) {
            void* native_helper = dlopen("libnativehelper.so", RTLD_NOW);
            if (native_helper != nullptr) {
                get_created_java_vms = reinterpret_cast<GetCreatedJavaVMs>(
                    dlsym(native_helper, "JNI_GetCreatedJavaVMs")
                );
            }
        }
        if (get_created_java_vms == nullptr) {
            __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "JNI_GetCreatedJavaVMs was not found");
            return false;
        }

        JavaVM* vm = nullptr;
        jsize vm_count = 0;
        if (get_created_java_vms(&vm, 1, &vm_count) != JNI_OK || vm == nullptr || vm_count == 0) {
            __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "No Java VM available for randomizer generation");
            return false;
        }

        JNIEnv* env = nullptr;
        bool did_attach = false;
        jint env_status = vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6);
        if (env_status == JNI_EDETACHED) {
            if (vm->AttachCurrentThread(&env, nullptr) != JNI_OK) {
                __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "Failed to attach randomizer thread to Java VM");
                return false;
            }
            did_attach = true;
        } else if (env_status != JNI_OK || env == nullptr) {
            __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "Failed to get JNI environment for randomizer generation");
            return false;
        }

        bool success = false;
        jclass generator_class = find_app_class(env, "io.github.zelda64recomp.RandoGenerator");
        if (generator_class != nullptr) {
            jmethodID generate_method = env->GetStaticMethodID(
                generator_class,
                "generate",
                "(Ljava/lang/String;Ljava/lang/String;)Z"
            );
            if (!env->ExceptionCheck() && generate_method != nullptr) {
                std::string yaml_path = path_to_utf8(yaml_dir);
                std::string output_path = path_to_utf8(output_dir);
                jstring yaml_arg = env->NewStringUTF(yaml_path.c_str());
                jstring output_arg = env->NewStringUTF(output_path.c_str());
                success = env->CallStaticBooleanMethod(generator_class, generate_method, yaml_arg, output_arg);
                env->DeleteLocalRef(yaml_arg);
                env->DeleteLocalRef(output_arg);
            }

            if (env->ExceptionCheck()) {
                env->ExceptionDescribe();
                env->ExceptionClear();
                success = false;
            }
        } else {
            __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "RandoGenerator Java class was not found");
        }

        if (did_attach) {
            vm->DetachCurrentThread();
        }
        return success;
    }
}
