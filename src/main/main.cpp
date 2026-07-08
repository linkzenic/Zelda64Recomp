#include <cstdio>
#include <cassert>
#include <unordered_map>
#include <vector>
#include <array>
#include <filesystem>
#include <numeric>
#include <stdexcept>
#include <cinttypes>
#include <cstdlib>
#include <cctype>
#include <algorithm>
#include <fstream>
#include <iomanip>

#if defined(__ANDROID__)
#include <android/log.h>
#include <signal.h>
#include <sys/system_properties.h>
#include <unistd.h>
#include "../android/android_logging.h"
#define ZELDA_ANDROID_LOG(...) __android_log_print(ANDROID_LOG_INFO, "ZeldaNative", __VA_ARGS__)
#define ZELDA_ANDROID_STAGE(stage) zelda64::android::set_crash_stage(stage)
#else
#define ZELDA_ANDROID_LOG(...)
#define ZELDA_ANDROID_STAGE(stage)
#endif

#if !defined(__ANDROID__)
#include "nfd.h"
#endif

#include "ultramodern/ultra64.h"
#include "ultramodern/ultramodern.hpp"
#if !defined(__ANDROID__)
#define SDL_MAIN_HANDLED
#endif
#ifdef _WIN32
#include "SDL.h"
#else
#include "SDL2/SDL.h"
#include "SDL2/SDL_syswm.h"
// Undefine x11 macros that get included by SDL_syswm.h.
#undef None
#undef Status
#undef LockMask
#undef ControlMask
#undef Success
#undef Always
#endif

#include "recomp_ui.h"
#include "recomp_input.h"
#include "zelda_config.h"
#include "zelda_sound.h"
#include "zelda_render.h"
#include "zelda_support.h"
#include "zelda_game.h"
#include "recomp_files.h"
#include "recomp_data.h"
#include "../ui/ui_mod_installer.h"
#include "ovl_patches.hpp"
#include "librecomp/game.hpp"
#include "librecomp/mods.hpp"
#include "librecomp/helpers.hpp"
#include "json/json.hpp"

#include "../../patches/graphics.h"
#include "../../patches/input.h"
#include "../../patches/sound.h"
#include "../../patches/misc_funcs.h"

#include "mods/mm_recomp_dpad_builtin.h"
#include "mods/mm_recomp_save_editor.h"

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <timeapi.h>
#include "SDL_syswm.h"
#endif

#include "../../lib/rt64/src/contrib/stb/stb_image.h"

const std::string version_string = "1.2.2";

#if defined(__ANDROID__)
static bool android_property_equals(const char* name, const char* expected) {
    char value[PROP_VALUE_MAX] = {};
    return __system_property_get(name, value) > 0 && std::strcmp(value, expected) == 0;
}

static bool android_emulator_enabled() {
    return android_property_equals("ro.kernel.qemu", "1") ||
        android_property_equals("ro.boot.qemu", "1");
}

static bool android_safe_mode_enabled() {
    const char* safe_mode = std::getenv("APP_SAFE_MODE");
    return safe_mode != nullptr && safe_mode[0] == '1';
}

static bool android_skip_code_mods_enabled() {
    const char* skip_code_mods = std::getenv("APP_ANDROID_SKIP_CODE_MODS");
    return (skip_code_mods != nullptr && skip_code_mods[0] == '1') || android_safe_mode_enabled();
}

static bool android_compatibility_build_enabled() {
    const char* compatibility_build = std::getenv("APP_ANDROID_COMPATIBILITY_BUILD");
    return compatibility_build != nullptr && compatibility_build[0] == '1';
}

static bool android_string_equals_ci(const char* value, const char* expected) {
    if (value == nullptr || expected == nullptr) {
        return false;
    }

    while (*value != '\0' && *expected != '\0') {
        unsigned char value_char = static_cast<unsigned char>(*value);
        unsigned char expected_char = static_cast<unsigned char>(*expected);
        if (std::tolower(value_char) != std::tolower(expected_char)) {
            return false;
        }
        ++value;
        ++expected;
    }

    return *value == '\0' && *expected == '\0';
}
#endif

static void disable_obsolete_mods() {
    static constexpr const char* save_editor_mod_id = "mm_recomp_save_editor";
    const std::filesystem::path mods_config_path = zelda64::get_app_folder_path() / "mods.json";

    std::ifstream input_file{mods_config_path};
    if (!input_file.good()) {
        return;
    }

    nlohmann::json config_json{};
    try {
        input_file >> config_json;
    }
    catch (nlohmann::json::parse_error&) {
        ZELDA_ANDROID_LOG("obsolete mod scrub skipped; mods.json parse failed");
        return;
    }

    bool changed = false;
    auto remove_mod_id = [&](const char* key) {
        auto array_it = config_json.find(key);
        if (array_it == config_json.end() || !array_it->is_array()) {
            return;
        }

        auto& array = *array_it;
        const size_t old_size = array.size();
        array.erase(std::remove_if(array.begin(), array.end(), [](const nlohmann::json& entry) {
            return entry.is_string() && entry.get<std::string>() == save_editor_mod_id;
        }), array.end());
        changed = changed || array.size() != old_size;
    };

    remove_mod_id("enabled_mods");
    remove_mod_id("mod_order");

    if (!changed) {
        return;
    }

    {
        std::ofstream output_file = recomp::open_output_file_with_backup(mods_config_path);
        if (!output_file.good()) {
            ZELDA_ANDROID_LOG("obsolete mod scrub failed; could not open mods.json for write");
            return;
        }

        output_file << std::setw(4) << config_json;
    }

    if (!recomp::finalize_output_file_with_backup(mods_config_path)) {
        ZELDA_ANDROID_LOG("obsolete mod scrub failed; could not finalize mods.json");
        return;
    }

    ZELDA_ANDROID_LOG("disabled obsolete mod: %s", save_editor_mod_id);
}

#if defined(__ANDROID__)
static void android_configure_device_flags() {
    const char* manufacturer = SDL_getenv("APP_ANDROID_MANUFACTURER");
    if (manufacturer == nullptr) {
        manufacturer = std::getenv("APP_ANDROID_MANUFACTURER");
    }

    const bool is_samsung = android_string_equals_ci(manufacturer, "samsung");
    const bool disable_rumble = false;
    const bool sync_boot_dma = is_samsung;
    recomp::set_android_disable_rumble(disable_rumble);
    recomp::set_android_sync_boot_dma(sync_boot_dma);
    ZELDA_ANDROID_LOG("android device flags: manufacturer=%s disable_rumble=%d sync_boot_dma=%d",
        manufacturer != nullptr ? manufacturer : "<null>",
        disable_rumble ? 1 : 0,
        sync_boot_dma ? 1 : 0);
    zelda64::android::append_log("APP_ANDROID_DISABLE_RUMBLE=%s",
        disable_rumble ? "true" : "false");
    zelda64::android::append_log("APP_ANDROID_SYNC_BOOT_DMA=%s",
        sync_boot_dma ? "true" : "false");
}

static std::list<std::filesystem::path> parse_pending_mod_paths(const char* pending_mod_paths) {
    std::list<std::filesystem::path> paths;
    if (pending_mod_paths == nullptr || pending_mod_paths[0] == '\0') {
        return paths;
    }

    std::string pending_paths_string{pending_mod_paths};
    size_t start = 0;
    while (start <= pending_paths_string.size()) {
        size_t end = pending_paths_string.find('\n', start);
        std::string path_string = pending_paths_string.substr(start, end == std::string::npos ? std::string::npos : end - start);
        if (!path_string.empty()) {
            paths.emplace_back(path_string);
        }
        if (end == std::string::npos) {
            break;
        }
        start = end + 1;
    }
    return paths;
}

static void install_pending_android_mods() {
    if (android_skip_code_mods_enabled()) {
        ZELDA_ANDROID_LOG("code mods disabled; pending mod installation skipped");
        SDL_setenv("APP_PENDING_MOD_PATHS", "", true);
        return;
    }

    const char* pending_mod_paths = std::getenv("APP_PENDING_MOD_PATHS");
    std::list<std::filesystem::path> paths = parse_pending_mod_paths(pending_mod_paths);
    if (paths.empty()) {
        return;
    }

    ZELDA_ANDROID_LOG("pending mod import count=%zu", paths.size());

    recompui::ModInstaller::Result result;
    recompui::ModInstaller::start_mod_installation(paths, nullptr, result);

    if (!result.error_messages.empty()) {
        for (const std::string& error_message : result.error_messages) {
            ZELDA_ANDROID_LOG("pending mod import error: %s", error_message.c_str());
        }
        std::vector<std::string> cancel_errors;
        recompui::ModInstaller::cancel_mod_installation(result, cancel_errors);
    }
    else {
        std::vector<std::string> finish_errors;
        recompui::ModInstaller::finish_mod_installation(result, finish_errors);
        for (const std::string& error_message : finish_errors) {
            ZELDA_ANDROID_LOG("pending mod finish error: %s", error_message.c_str());
        }
        ZELDA_ANDROID_LOG("pending mod import installed=%zu finish_errors=%zu",
            result.pending_installations.size(),
            finish_errors.size());
    }

    SDL_setenv("APP_PENDING_MOD_PATHS", "", true);
}
#endif

template<typename... Ts>
void exit_error(const char* str, Ts ...args) {
    // TODO pop up an error
    ((void)fprintf(stderr, str, args), ...);
#if defined(__ANDROID__)
    char message[1024];
    std::snprintf(message, sizeof(message), str, args...);
    zelda64::android::append_log("Fatal error: %s", message);
    zelda64::android::write_crash_file(message);
#endif
    assert(false);
        
    ultramodern::error_handling::quick_exit(__FILE__, __LINE__, __FUNCTION__);
}

ultramodern::gfx_callbacks_t::gfx_data_t create_gfx() {
    SDL_SetHint(SDL_HINT_WINDOWS_DPI_AWARENESS, "permonitorv2");
    SDL_SetHint(SDL_HINT_GAMECONTROLLER_USE_BUTTON_LABELS, "0");
    SDL_SetHint(SDL_HINT_JOYSTICK_HIDAPI_PS4_RUMBLE, "1");
    SDL_SetHint(SDL_HINT_JOYSTICK_HIDAPI_PS5_RUMBLE, "1");
    SDL_SetHint(SDL_HINT_MOUSE_FOCUS_CLICKTHROUGH, "1");
    SDL_SetHint(SDL_HINT_JOYSTICK_ALLOW_BACKGROUND_EVENTS, "1");
#ifdef __ANDROID__
    SDL_SetHint(SDL_HINT_ORIENTATIONS, "LandscapeLeft LandscapeRight");
#endif

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER | SDL_INIT_HAPTIC) > 0) {
        exit_error("Failed to initialize SDL2: %s\n", SDL_GetError());
    }

    fprintf(stdout, "SDL Video Driver: %s\n", SDL_GetCurrentVideoDriver());

    return {};
}

#if defined(__gnu_linux__)
#include "icon_bytes.h"

bool SetImageAsIcon(const char* filename, SDL_Window* window)
{
    // Read data
    int width, height, bytesPerPixel;
    void* data = stbi_load_from_memory(reinterpret_cast<const uint8_t*>(icon_bytes), sizeof(icon_bytes), &width, &height, &bytesPerPixel, 4);

    // Calculate pitch
    int pitch;
    pitch = width * 4;
    pitch = (pitch + 3) & ~3;

    // Setup relevance bitmask
    int Rmask, Gmask, Bmask, Amask;

#if SDL_BYTEORDER == SDL_LIL_ENDIAN
    Rmask = 0x000000FF;
    Gmask = 0x0000FF00;
    Bmask = 0x00FF0000;
    Amask = 0xFF000000;
#else
    Rmask = 0xFF000000;
    Gmask = 0x00FF0000;
    Bmask = 0x0000FF00;
    Amask = 0x000000FF;
#endif

    SDL_Surface* surface = nullptr;
    if (data != nullptr) {
        surface = SDL_CreateRGBSurfaceFrom(data, width, height, 32, pitch, Rmask, Gmask,
                            Bmask, Amask);
    }

    if (surface == nullptr) {   
        if (data != nullptr) {
            stbi_image_free(data);
        }
        return false;
	} else {
        SDL_SetWindowIcon(window,surface);
        SDL_FreeSurface(surface);
        stbi_image_free(data);
        return true;
    }
}
#endif

SDL_Window* window;
#if defined(__APPLE__)
SDL_MetalView metal_view = nullptr;
#endif

ultramodern::renderer::WindowHandle create_window(ultramodern::gfx_callbacks_t::gfx_data_t) {
    ZELDA_ANDROID_LOG("create_window begin");
    uint32_t flags = SDL_WINDOW_RESIZABLE;

#if defined(__APPLE__)
    flags |= SDL_WINDOW_METAL;
#elif defined(RT64_SDL_WINDOW_VULKAN)
    flags |= SDL_WINDOW_VULKAN;
#   if defined(__ANDROID__)
    flags |= SDL_WINDOW_SHOWN;
#   endif
#endif

    window = SDL_CreateWindow("Zelda 64: Recompiled", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1600, 960,  flags);
    ZELDA_ANDROID_LOG("SDL_CreateWindow returned %p", window);
#if defined(__linux__) && !defined(__ANDROID__)
    SetImageAsIcon("icons/512.png",window);
    if (ultramodern::renderer::get_graphics_config().wm_option == ultramodern::renderer::WindowMode::Fullscreen) { // TODO: Remove once RT64 gets native fullscreen support on Linux
        SDL_SetWindowFullscreen(window,SDL_WINDOW_FULLSCREEN_DESKTOP);
    } else {
        SDL_SetWindowFullscreen(window,0);
    }
#endif

    if (window == nullptr) {
        exit_error("Failed to create window: %s\n", SDL_GetError());
    }

#if defined(_WIN32)
    SDL_SysWMinfo wmInfo;
    SDL_VERSION(&wmInfo.version);
    SDL_GetWindowWMInfo(window, &wmInfo);
    return ultramodern::renderer::WindowHandle{ wmInfo.info.win.window, GetCurrentThreadId() };
#elif defined(__linux__) || defined(__ANDROID__)
    return ultramodern::renderer::WindowHandle{ window };
#elif defined(__APPLE__)
    SDL_SysWMinfo wmInfo;
    SDL_VERSION(&wmInfo.version);
    SDL_GetWindowWMInfo(window, &wmInfo);
    metal_view = SDL_Metal_CreateView(window);
    return ultramodern::renderer::WindowHandle{ wmInfo.info.cocoa.window,  SDL_Metal_GetLayer(metal_view) };
#else
    static_assert(false && "Unimplemented");
#endif
}

void update_gfx(void*) {
    static uint32_t update_count = 0;
    if (update_count < 10) {
        std::printf("[Android] update_gfx begin %u\n", update_count);
    }
    if ((update_count++ % 600) == 0) {
        ZELDA_ANDROID_LOG("update_gfx count=%u", update_count);
    }
    recomp::handle_events();
    if (update_count <= 10) {
        std::printf("[Android] update_gfx end %u\n", update_count - 1);
    }
}

static SDL_AudioCVT audio_convert;
static SDL_AudioDeviceID audio_device = 0;

// Samples per channel per second.
static uint32_t sample_rate = 48000;
static uint32_t output_sample_rate = 48000;
// Channel count.
constexpr uint32_t input_channels = 2;
static uint32_t output_channels = 2;

// Terminology: a frame is a collection of samples for each channel. e.g. 2 input samples is one input frame. This is unrelated to graphical frames.

// Number of frames to duplicate for fixing interpolation at the start and end of a chunk.
constexpr uint32_t duplicated_input_frames = 4;
// The number of output frames to skip for playback (to avoid playing duplicate inputs twice).
static uint32_t discarded_output_frames;

constexpr uint32_t bytes_per_frame = input_channels * sizeof(float);

void queue_samples(int16_t* audio_data, size_t sample_count) {
    // Buffer for holding the output of swapping the audio channels. This is reused across
    // calls to reduce runtime allocations.
    static std::vector<float> swap_buffer;
    static std::array<float, duplicated_input_frames * input_channels> duplicated_sample_buffer;

    // Make sure the swap buffer is large enough to hold the audio data, including any extra space needed for resampling.
    size_t resampled_sample_count = sample_count + duplicated_input_frames * input_channels;
    size_t max_sample_count = std::max(resampled_sample_count, resampled_sample_count * audio_convert.len_mult);
    if (max_sample_count > swap_buffer.size()) {
        swap_buffer.resize(max_sample_count);
    }
    
    // Copy the duplicated frames from last chunk into this chunk
    for (size_t i = 0; i < duplicated_input_frames * input_channels; i++) {
        swap_buffer[i] = duplicated_sample_buffer[i];
    }

    // Convert the audio from 16-bit values to floats and swap the audio channels into the
    // swap buffer to correct for the address xor caused by endianness handling.
    float cur_main_volume = zelda64::get_main_volume() / 100.0f; // Get the current main volume, normalized to 0.0-1.0.
    for (size_t i = 0; i < sample_count; i += input_channels) {
        swap_buffer[i + 0 + duplicated_input_frames * input_channels] = audio_data[i + 1] * (0.5f / 32768.0f) * cur_main_volume;
        swap_buffer[i + 1 + duplicated_input_frames * input_channels] = audio_data[i + 0] * (0.5f / 32768.0f) * cur_main_volume;
    }
    
    // TODO handle cases where a chunk is smaller than the duplicated frame count.
    assert(sample_count > duplicated_input_frames * input_channels);

    // Copy the last converted samples into the duplicated sample buffer to reuse in resampling the next queued chunk.
    for (size_t i = 0; i < duplicated_input_frames * input_channels; i++) {
        duplicated_sample_buffer[i] = swap_buffer[i + sample_count];
    }
    
    audio_convert.buf = reinterpret_cast<Uint8*>(swap_buffer.data());
    audio_convert.len = (sample_count + duplicated_input_frames * input_channels) * sizeof(swap_buffer[0]);

    int ret = SDL_ConvertAudio(&audio_convert);

    if (ret < 0) {
        printf("Error using SDL audio converter: %s\n", SDL_GetError());
        throw std::runtime_error("Error using SDL audio converter");
    }

    uint64_t cur_queued_microseconds = uint64_t(SDL_GetQueuedAudioSize(audio_device)) / bytes_per_frame * 1000000 / sample_rate;
    uint32_t num_bytes_to_queue = audio_convert.len_cvt - output_channels * discarded_output_frames * sizeof(swap_buffer[0]);
    float* samples_to_queue = swap_buffer.data() + output_channels * discarded_output_frames / 2;

    // Prevent audio latency from building up by skipping samples in incoming audio when too many samples are already queued.
    // Skip samples based on how many microseconds of samples are queued already.
    uint32_t skip_factor = cur_queued_microseconds / 100000;
    if (skip_factor != 0) {
        uint32_t skip_ratio = 1 << skip_factor;
        num_bytes_to_queue /= skip_ratio;
        for (size_t i = 0; i < num_bytes_to_queue / (output_channels * sizeof(swap_buffer[0])); i++) {
            samples_to_queue[2 * i + 0] = samples_to_queue[2 * skip_ratio * i + 0];
            samples_to_queue[2 * i + 1] = samples_to_queue[2 * skip_ratio * i + 1];
        }
    }

    // Queue the swapped audio data.
    // Offset the data start by only half the discarded frame count as the other half of the discarded frames are at the end of the buffer.
    SDL_QueueAudio(audio_device, samples_to_queue, num_bytes_to_queue);
}

size_t get_frames_remaining() {
    constexpr float buffer_offset_frames = 1.0f;
    // Get the number of remaining buffered audio bytes.
    uint64_t buffered_byte_count = SDL_GetQueuedAudioSize(audio_device);

    // Scale the byte count based on the ratio of sample rates and channel counts.
    buffered_byte_count = buffered_byte_count * 2 * sample_rate / output_sample_rate / output_channels;

    // Adjust the reported count to be some number of refreshes in the future, which helps ensure that
    // there are enough samples even if the audio thread experiences a small amount of lag. This prevents
    // audio popping on games that use the buffered audio byte count to determine how many samples
    // to generate.
    uint32_t frames_per_vi = (sample_rate / 60);
    if (buffered_byte_count > (buffer_offset_frames * bytes_per_frame * frames_per_vi)) {
        buffered_byte_count -= (buffer_offset_frames * bytes_per_frame * frames_per_vi);
    }
    else {
        buffered_byte_count = 0;
    }
    // Convert from byte count to sample count.
    return static_cast<uint32_t>(buffered_byte_count / bytes_per_frame);
}

void update_audio_converter() {
    int ret = SDL_BuildAudioCVT(&audio_convert, AUDIO_F32, input_channels, sample_rate, AUDIO_F32, output_channels, output_sample_rate);

    if (ret < 0) {
        printf("Error creating SDL audio converter: %s\n", SDL_GetError());
        throw std::runtime_error("Error creating SDL audio converter");
    }

    // Calculate the number of samples to discard based on the sample rate ratio and the duplicate frame count.
    discarded_output_frames = duplicated_input_frames * output_sample_rate / sample_rate;
}

void set_frequency(uint32_t freq) {
    sample_rate = freq;
    
    update_audio_converter();
}

void reset_audio(uint32_t output_freq) {
    SDL_AudioSpec spec_desired{
        .freq = (int)output_freq,
        .format = AUDIO_F32,
        .channels = (Uint8)output_channels,
        .silence = 0, // calculated
        .samples = 0x100, // Fairly small sample count to reduce the latency of internal buffering
        .padding = 0, // unused
        .size = 0, // calculated
        .callback = nullptr,
        .userdata = nullptr
    };


    audio_device = SDL_OpenAudioDevice(nullptr, false, &spec_desired, nullptr, 0);
    if (audio_device == 0) {
        exit_error("SDL error opening audio device: %s\n", SDL_GetError());
    }
    SDL_PauseAudioDevice(audio_device, 0);

    output_sample_rate = output_freq;
    update_audio_converter();
}

extern RspUcodeFunc njpgdspMain;
extern RspUcodeFunc aspMain;

RspUcodeFunc* get_rsp_microcode(const OSTask* task) {
    switch (task->t.type) {
    case M_AUDTASK:
        return aspMain;

    case M_NJPEGTASK:
        return njpgdspMain;

    default:
        fprintf(stderr, "Unknown task: %" PRIu32 "\n", task->t.type);
        return nullptr;
    }
}

extern "C" void recomp_entrypoint(uint8_t * rdram, recomp_context * ctx);
extern "C" void recomp_get_dpad_items_enabled(uint8_t* rdram, recomp_context* ctx);
extern "C" void recomp_get_clock_style(uint8_t* rdram, recomp_context* ctx);
extern "C" void recomp_get_clock_texture_pack_loaded(uint8_t* rdram, recomp_context* ctx);
extern "C" void recomp_should_use_3ds_clock_overlay(uint8_t* rdram, recomp_context* ctx);
extern "C" void recomp_set_pause_save_prompt_overlay_state(uint8_t* rdram, recomp_context* ctx);
extern "C" void recomp_get_analog_camera_distance(uint8_t* rdram, recomp_context* ctx);
extern "C" void recomp_save_editor_set_snapshot_value(uint8_t* rdram, recomp_context* ctx);
extern "C" void recomp_save_editor_get_pending_value(uint8_t* rdram, recomp_context* ctx);
extern "C" void recomp_save_editor_should_apply_pending(uint8_t* rdram, recomp_context* ctx);
extern "C" void recomp_save_editor_clear_pending(uint8_t* rdram, recomp_context* ctx);
extern "C" void recomp_android_should_disable_rumble(uint8_t* rdram, recomp_context* ctx);
extern "C" void recomp_android_should_use_sync_boot_dma(uint8_t* rdram, recomp_context* ctx);
extern "C" void recomp_android_reset_effect_ss_table(uint8_t* rdram, recomp_context* ctx);
extern "C" void recomp_android_get_entrance_scene_spawn(uint8_t* rdram, recomp_context* ctx);
extern "C" void recomp_android_load_yaz0(uint8_t* rdram, recomp_context* ctx);
extern "C" void recomp_android_compat_pmm_apply_log(uint8_t* rdram, recomp_context* ctx);
extern "C" void PMMZobj_setPMMDir(uint8_t* rdram, recomp_context* ctx);
extern "C" void PMMZobj_scanForDiskEntries(uint8_t* rdram, recomp_context* ctx);
extern "C" void PMMZobj_getNumDiskEntries(uint8_t* rdram, recomp_context* ctx);
extern "C" void PMMZobj_entryInternalNameLength(uint8_t* rdram, recomp_context* ctx);
extern "C" void PMMZobj_entryDisplayNameLength(uint8_t* rdram, recomp_context* ctx);
extern "C" void PMMZobj_entryAuthorNameLength(uint8_t* rdram, recomp_context* ctx);
extern "C" void PMMZobj_writeInternalNameToBuffer(uint8_t* rdram, recomp_context* ctx);
extern "C" void PMMZobj_writeDisplayNameToBuffer(uint8_t* rdram, recomp_context* ctx);
extern "C" void PMMZobj_writeAuthorNameToBuffer(uint8_t* rdram, recomp_context* ctx);
extern "C" void PMMZobj_getEntryFileSize(uint8_t* rdram, recomp_context* ctx);
extern "C" void PMMZobj_getEntryFileData(uint8_t* rdram, recomp_context* ctx);
extern "C" void PMMZobj_createDirectory(uint8_t* rdram, recomp_context* ctx);
extern "C" void PMMZobj_isDirectoryExist(uint8_t* rdram, recomp_context* ctx);
extern "C" void PMMZobj_readEntryU8(uint8_t* rdram, recomp_context* ctx);
extern "C" void PMMZobj_readEntryU16(uint8_t* rdram, recomp_context* ctx);
extern "C" void PMMZobj_readEntryU32(uint8_t* rdram, recomp_context* ctx);
extern "C" void PMMZobj_isModelType(uint8_t* rdram, recomp_context* ctx);
extern "C" void PMMZobj_clearDiskEntries(uint8_t* rdram, recomp_context* ctx);
extern "C" void PMMZobj_tryLoadOOTROM(uint8_t* rdram, recomp_context* ctx);
extern "C" void PMMZobj_unloadOOTROM(uint8_t* rdram, recomp_context* ctx);
extern "C" void PMMZobj_isOOTRomLoaded(uint8_t* rdram, recomp_context* ctx);
extern "C" void PMMZobj_extractGameplayKeep(uint8_t* rdram, recomp_context* ctx);
extern "C" void PMMZobj_extractChildLink(uint8_t* rdram, recomp_context* ctx);
extern "C" void PMMZobj_extractAdultLink(uint8_t* rdram, recomp_context* ctx);
extern "C" void PMMZobj_extractMirrorShieldRay(uint8_t* rdram, recomp_context* ctx);
extern "C" void recomp_android_compat_pmm_begin_save(uint8_t* rdram, recomp_context* ctx);
extern "C" void recomp_android_compat_pmm_write_selection(uint8_t* rdram, recomp_context* ctx);
extern "C" void recomp_android_compat_pmm_load_selection(uint8_t* rdram, recomp_context* ctx);
extern "C" void recomp_android_compat_pmm_get_config_u32(uint8_t* rdram, recomp_context* ctx);
extern "C" void recomp_android_compat_pmm_get_config_string(uint8_t* rdram, recomp_context* ctx);
extern "C" void recomp_android_compat_pmm_free_config_string(uint8_t* rdram, recomp_context* ctx);
extern "C" void recomp_android_compat_cheats_get_config_u32(uint8_t* rdram, recomp_context* ctx);
extern "C" void recomp_android_compat_bomb_arrows_get_config_u32(uint8_t* rdram, recomp_context* ctx);
extern "C" void recomp_android_compat_mco_enabled(uint8_t* rdram, recomp_context* ctx);
extern "C" void recomp_android_compat_mco_get_config_u32(uint8_t* rdram, recomp_context* ctx);
gpr get_entrypoint_address();

// array of supported GameEntry objects
std::vector<recomp::GameEntry> supported_games = {
    {
        .rom_hash = 0xEF18B4A9E2386169ULL,
        .internal_name = "ZELDA MAJORA'S MASK",
        .game_id = u8"mm.n64.us.1.0",
        .mod_game_id = "mm",
        .save_type = recomp::SaveType::Flashram,
        .is_enabled = false,
        .decompression_routine = zelda64::decompress_mm,
        .has_compressed_code = true,
        .entrypoint_address = get_entrypoint_address(),
        .entrypoint = recomp_entrypoint,
    },
};

// TODO: move somewhere else
namespace zelda64 {
    std::string get_game_thread_name(const OSThread* t) {
        std::string name = "[Game] ";

        switch (t->id) {
            case 0:
                switch (t->priority) {
                    case 150:
                        name += "PIMGR";
                        break;

                    case 254:
                        name += "VIMGR";
                        break;

                    default:
                        name += std::to_string(t->id);
                        break;
                }
                break;

            case 1:
                name += "IDLE";
                break;

            case 2:
                switch (t->priority) {
                    case 5:
                        name += "SLOWLY";
                        break;

                    case 127:
                        name += "FAULT";
                        break;

                    default:
                        name += std::to_string(t->id);
                        break;
                }
                break;

            case 3:
                name += "MAIN";
                break;

            case 4:
                name += "GRAPH";
                break;

            case 5:
                name += "SCHED";
                break;

            case 7:
                name += "PADMGR";
                break;

            case 10:
                name += "AUDIOMGR";
                break;

            case 13:
                name += "FLASHROM";
                break;

            case 18:
                name += "DMAMGR";
                break;

            case 19:
                name += "IRQMGR";
                break;

            default:
                name += std::to_string(t->id);
                break;
        }

        return name;
    }
}

#ifdef _WIN32

struct PreloadContext {
    HANDLE handle;
    HANDLE mapping_handle;
    SIZE_T size;
    PVOID view;
};

bool preload_executable(PreloadContext& context) {
    wchar_t module_name[MAX_PATH];
    GetModuleFileNameW(NULL, module_name, MAX_PATH);

    context.handle = CreateFileW(module_name, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (context.handle == INVALID_HANDLE_VALUE) {
        fprintf(stderr, "Failed to load executable into memory!");
        context = {};
        return false;
    }

    LARGE_INTEGER module_size;
    if (!GetFileSizeEx(context.handle, &module_size)) {
        fprintf(stderr, "Failed to get size of executable!");
        CloseHandle(context.handle);
        context = {};
        return false;
    }

    context.size = module_size.QuadPart;

    context.mapping_handle = CreateFileMappingW(context.handle, nullptr, PAGE_READONLY, 0, 0, nullptr);
    if (context.mapping_handle == nullptr) {
        fprintf(stderr, "Failed to create file mapping of executable!");
        CloseHandle(context.handle);
        context = {};
        return EXIT_FAILURE;
    }

    context.view = MapViewOfFile(context.mapping_handle, FILE_MAP_READ, 0, 0, 0);
    if (context.view == nullptr) {
        fprintf(stderr, "Failed to map view of of executable!");
        CloseHandle(context.mapping_handle);
        CloseHandle(context.handle);
        context = {};
        return false;
    }

    DWORD pid = GetCurrentProcessId();
    HANDLE process_handle = OpenProcess(PROCESS_SET_QUOTA | PROCESS_QUERY_INFORMATION, FALSE, pid);
    if (process_handle == nullptr) {
        fprintf(stderr, "Failed to open own process!");
        CloseHandle(context.mapping_handle);
        CloseHandle(context.handle);
        context = {};
        return false;
    }

    SIZE_T minimum_set_size, maximum_set_size;
    if (!GetProcessWorkingSetSize(process_handle, &minimum_set_size, &maximum_set_size)) {
        fprintf(stderr, "Failed to get working set size!");
        CloseHandle(context.mapping_handle);
        CloseHandle(context.handle);
        context = {};
        return false;
    }

    if (!SetProcessWorkingSetSize(process_handle, minimum_set_size + context.size, maximum_set_size + context.size)) {
        fprintf(stderr, "Failed to set working set size!");
        CloseHandle(context.mapping_handle);
        CloseHandle(context.handle);
        context = {};
        return false;
    }

    if (VirtualLock(context.view, context.size) == 0) {
        fprintf(stderr, "Failed to lock view of executable! (Error: %08lx)\n", GetLastError());
        CloseHandle(context.mapping_handle);
        CloseHandle(context.handle);
        context = {};
        return false;
    }
    
    return true;
}

void release_preload(PreloadContext& context) {
    VirtualUnlock(context.view, context.size);
    CloseHandle(context.mapping_handle);
    CloseHandle(context.handle);
    context = {};
}

#else

struct PreloadContext {

};

// TODO implement on other platforms
bool preload_executable(PreloadContext& context) {
    return false;
}

void release_preload(PreloadContext& context) {
}

#endif

void enable_texture_pack(recomp::mods::ModContext& context, const recomp::mods::ModHandle& mod) {
    zelda64::renderer::enable_texture_pack(context, mod);
}

void disable_texture_pack(recomp::mods::ModContext&, const recomp::mods::ModHandle& mod) {
    zelda64::renderer::disable_texture_pack(mod);
}

void reorder_texture_pack(recomp::mods::ModContext&) {
    zelda64::renderer::trigger_texture_pack_update();
}

#define REGISTER_FUNC(name) recomp::overlays::register_base_export(#name, name)

int main(int argc, char** argv) {
    (void)argc;
    (void)argv;
#if defined(__ANDROID__)
    zelda64::android::install_crash_handlers();
    zelda64::android::redirect_stdio_to_log();
    ZELDA_ANDROID_STAGE("native main entered");
    zelda64::android::append_log("Native main entered");
#endif
    ZELDA_ANDROID_LOG("main entered");
    recomp::Version project_version{};
    if (!recomp::Version::from_string(version_string, project_version)) {
        ultramodern::error_handling::message_box(("Invalid version string: " + version_string).c_str());
        return EXIT_FAILURE;
    }

    // Map this executable into memory and lock it, which should keep it in physical memory. This ensures
    // that there are no stutters from the OS having to load new pages of the executable whenever a new code page is run.
    PreloadContext preload_context;
    bool preloaded = preload_executable(preload_context);

    if (!preloaded) {
        fprintf(stderr, "Failed to preload executable!\n");
    }

#ifdef _WIN32
    // Set up high resolution timing period.
    timeBeginPeriod(1);

    // Process arguments.
    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "--show-console") == 0)
        {
            if (GetConsoleWindow() == nullptr)
            {
                AllocConsole();
                freopen("CONIN$", "r", stdin);
                freopen("CONOUT$", "w", stderr);
                freopen("CONOUT$", "w", stdout);
            }

            break;
        }
    }

    // Set up console output to accept UTF-8 on windows
    SetConsoleOutputCP(CP_UTF8);

    // Initialize native file dialogs.
#if !defined(__ANDROID__)
    NFD_Init();
#endif

    // Change to a font that supports Japanese characters
    CONSOLE_FONT_INFOEX cfi;
    cfi.cbSize = sizeof cfi;
    cfi.nFont = 0;
    cfi.dwFontSize.X = 0;
    cfi.dwFontSize.Y = 16;
    cfi.FontFamily = FF_DONTCARE;
    cfi.FontWeight = FW_NORMAL;
    wcscpy_s(cfi.FaceName, L"NSimSun");
    SetCurrentConsoleFontEx(GetStdHandle(STD_OUTPUT_HANDLE), FALSE, &cfi);
#endif

#ifdef _WIN32
    // Force wasapi on Windows, as there seems to be some issue with sample queueing with directsound currently.
    SDL_setenv("SDL_AUDIODRIVER", "wasapi", true);
#endif
#if defined(__ANDROID__)
    if (android_emulator_enabled()) {
        SDL_setenv("SDL_AUDIODRIVER", "dummy", true);
        ZELDA_ANDROID_LOG("Android emulator detected; using dummy SDL audio driver");
        zelda64::android::append_log("Android emulator detected; using dummy SDL audio driver");
    }
#endif

#if defined(__linux__) && defined(RECOMP_FLATPAK)
    // When using Flatpak, applications tend to launch from the home directory by default.
    // Mods might use the current working directory to store the data, so we switch it to a directory
    // with persistent data storage and write permissions under Flatpak to ensure it works.
    std::error_code ec;
    std::filesystem::current_path("/var/data", ec);
#endif

    // Initialize SDL audio and set the output frequency.
    ZELDA_ANDROID_STAGE("initializing SDL audio");
    ZELDA_ANDROID_LOG("initializing SDL audio");
    if (SDL_InitSubSystem(SDL_INIT_AUDIO) < 0) {
        exit_error("SDL error initializing audio: %s\n", SDL_GetError());
    }
    reset_audio(48000);
    ZELDA_ANDROID_STAGE("SDL audio initialized");
    ZELDA_ANDROID_LOG("SDL audio initialized");

    // Source controller mappings file
    std::u8string controller_db_path = (zelda64::get_program_path() / "recompcontrollerdb.txt").u8string();
    if (SDL_GameControllerAddMappingsFromFile(reinterpret_cast<const char *>(controller_db_path.c_str())) < 0) {
        fprintf(stderr, "Failed to load controller mappings: %s\n", SDL_GetError());
    }

    recomp::register_config_path(zelda64::get_app_folder_path());
    ZELDA_ANDROID_STAGE("registered config path");
    ZELDA_ANDROID_LOG("registered config path: %s", zelda64::get_app_folder_path().string().c_str());
#if defined(__ANDROID__) || defined(__APPLE__)
    disable_obsolete_mods();
#endif
#if defined(__ANDROID__)
    android_configure_device_flags();
#endif

    // Register supported games and patches
    for (const auto& game : supported_games) {
        recomp::register_game(game);
    }

#if defined(__ANDROID__)
    if (const char* pending_rom_path = std::getenv("APP_PENDING_ROM_PATH");
        pending_rom_path != nullptr && pending_rom_path[0] != '\0' && !supported_games.empty()) {
        recomp::RomValidationError rom_error = recomp::select_rom(pending_rom_path, supported_games[0].game_id);
        ZELDA_ANDROID_LOG("pending ROM import path=%s result=%d", pending_rom_path, static_cast<int>(rom_error));
        SDL_setenv("APP_PENDING_ROM_PATH", "", true);
    }
#endif

#if !defined(ZELDA_ANDROID_RUNTIME_APK)
#if defined(__APPLE__) && !defined(__ANDROID__)
    recomp::mods::ignore_external_mod("mm_recomp_dpad_builtin");
#else
    recomp::mods::register_embedded_mod("mm_recomp_dpad_builtin", { (const uint8_t*)(mm_recomp_dpad_builtin), std::size(mm_recomp_dpad_builtin)});
#endif
#endif
#if !defined(__ANDROID__) && !defined(__APPLE__)
    recomp::mods::register_embedded_mod("mm_recomp_save_editor", { (const uint8_t*)(mm_recomp_save_editor), std::size(mm_recomp_save_editor)});
#elif defined(__ANDROID__)
    if (android_skip_code_mods_enabled()) {
        ZELDA_ANDROID_LOG("code mods disabled; optional embedded mods skipped");
    }
#endif
    ZELDA_ANDROID_STAGE("registered embedded mods");

    REGISTER_FUNC(recomp_get_window_resolution);
    REGISTER_FUNC(recomp_get_target_aspect_ratio);
    REGISTER_FUNC(recomp_get_target_framerate);
    REGISTER_FUNC(recomp_get_autosave_enabled);
    REGISTER_FUNC(recomp_get_save_anywhere_enabled);
    REGISTER_FUNC(recomp_save_editor_set_snapshot_value);
    REGISTER_FUNC(recomp_save_editor_get_pending_value);
    REGISTER_FUNC(recomp_save_editor_should_apply_pending);
    REGISTER_FUNC(recomp_save_editor_clear_pending);
    REGISTER_FUNC(recomp_get_analog_cam_enabled);
    REGISTER_FUNC(recomp_get_analog_camera_distance);
    REGISTER_FUNC(recomp_get_camera_inputs);
    REGISTER_FUNC(recomp_get_targeting_mode);
    REGISTER_FUNC(recomp_get_bgm_volume);
    REGISTER_FUNC(recomp_get_low_health_beeps_enabled);
    REGISTER_FUNC(recomp_get_gyro_deltas);
    REGISTER_FUNC(recomp_get_mouse_deltas);
    REGISTER_FUNC(recomp_get_inverted_axes);
    REGISTER_FUNC(recomp_get_analog_inverted_axes);
    REGISTER_FUNC(recomp_get_dpad_items_enabled);
    REGISTER_FUNC(recomp_get_fd_anywhere_enabled);
    REGISTER_FUNC(recomp_get_fast_mask_enabled);
    REGISTER_FUNC(recomp_get_owls_never_quit_enabled);
    REGISTER_FUNC(recomp_get_clock_style);
    REGISTER_FUNC(recomp_get_clock_texture_pack_loaded);
    REGISTER_FUNC(recomp_should_use_3ds_clock_overlay);
    REGISTER_FUNC(recomp_set_3ds_clock_state);
    REGISTER_FUNC(recomp_set_pause_save_prompt_overlay_state);
    REGISTER_FUNC(recomp_android_should_disable_rumble);
    REGISTER_FUNC(recomp_android_should_use_sync_boot_dma);
    REGISTER_FUNC(recomp_android_is_n64_mode);
    REGISTER_FUNC(recomp_android_reset_effect_ss_table);
    REGISTER_FUNC(recomp_android_get_entrance_scene_spawn);
    REGISTER_FUNC(recomp_android_load_yaz0);
#if defined(__ANDROID__)
    if (android_compatibility_build_enabled()) {
        REGISTER_FUNC(PMMZobj_setPMMDir);
        REGISTER_FUNC(PMMZobj_scanForDiskEntries);
        REGISTER_FUNC(PMMZobj_getNumDiskEntries);
        REGISTER_FUNC(PMMZobj_entryInternalNameLength);
        REGISTER_FUNC(PMMZobj_entryDisplayNameLength);
        REGISTER_FUNC(PMMZobj_entryAuthorNameLength);
        REGISTER_FUNC(PMMZobj_writeInternalNameToBuffer);
        REGISTER_FUNC(PMMZobj_writeDisplayNameToBuffer);
        REGISTER_FUNC(PMMZobj_writeAuthorNameToBuffer);
        REGISTER_FUNC(PMMZobj_getEntryFileSize);
        REGISTER_FUNC(PMMZobj_getEntryFileData);
        REGISTER_FUNC(PMMZobj_createDirectory);
        REGISTER_FUNC(PMMZobj_isDirectoryExist);
        REGISTER_FUNC(PMMZobj_readEntryU8);
        REGISTER_FUNC(PMMZobj_readEntryU16);
        REGISTER_FUNC(PMMZobj_readEntryU32);
        REGISTER_FUNC(PMMZobj_isModelType);
        REGISTER_FUNC(PMMZobj_clearDiskEntries);
        REGISTER_FUNC(PMMZobj_tryLoadOOTROM);
        REGISTER_FUNC(PMMZobj_unloadOOTROM);
        REGISTER_FUNC(PMMZobj_isOOTRomLoaded);
        REGISTER_FUNC(PMMZobj_extractGameplayKeep);
        REGISTER_FUNC(PMMZobj_extractChildLink);
        REGISTER_FUNC(PMMZobj_extractAdultLink);
        REGISTER_FUNC(PMMZobj_extractMirrorShieldRay);
        REGISTER_FUNC(recomp_android_compat_pmm_apply_log);
        REGISTER_FUNC(recomp_android_compat_pmm_begin_save);
        REGISTER_FUNC(recomp_android_compat_pmm_write_selection);
        REGISTER_FUNC(recomp_android_compat_pmm_load_selection);
        REGISTER_FUNC(recomp_android_compat_pmm_get_config_u32);
        REGISTER_FUNC(recomp_android_compat_pmm_get_config_string);
        REGISTER_FUNC(recomp_android_compat_pmm_free_config_string);
        REGISTER_FUNC(recomp_android_compat_cheats_get_config_u32);
        REGISTER_FUNC(recomp_android_compat_bomb_arrows_get_config_u32);
        REGISTER_FUNC(recomp_android_compat_mco_enabled);
        REGISTER_FUNC(recomp_android_compat_mco_get_config_u32);
    }
#endif
    recompui::register_ui_exports();
    recomputil::register_data_api_exports();
    ZELDA_ANDROID_STAGE("registered exports");
    ZELDA_ANDROID_LOG("registered exports");

    ZELDA_ANDROID_STAGE("registering overlays and patches");
    zelda64::register_overlays();
    zelda64::register_patches();
    recomputil::init_extended_actor_data();
    zelda64::load_config();
    ZELDA_ANDROID_STAGE("loaded config");
    ZELDA_ANDROID_LOG("loaded config");

    recomp::rsp::callbacks_t rsp_callbacks{
        .get_rsp_microcode = get_rsp_microcode,
    };

    ultramodern::renderer::callbacks_t renderer_callbacks{
        .create_render_context = zelda64::renderer::create_render_context,
    };

    ultramodern::gfx_callbacks_t gfx_callbacks{
        .create_gfx = create_gfx,
        .create_window = create_window,
        .update_gfx = update_gfx,
    };

    ultramodern::audio_callbacks_t audio_callbacks{
        .queue_samples = queue_samples,
        .get_frames_remaining = get_frames_remaining,
        .set_frequency = set_frequency,
    };

    ultramodern::input::callbacks_t input_callbacks{
        .poll_input = recomp::poll_inputs,
        .get_input = recomp::get_n64_input,
        .set_rumble = recomp::set_rumble,
        .get_connected_device_info = recomp::get_connected_device_info,
    };

    ultramodern::events::callbacks_t thread_callbacks{
        .vi_callback = recomp::update_rumble,
        .gfx_init_callback = recompui::update_supported_options,
    };

    ultramodern::error_handling::callbacks_t error_handling_callbacks{
        .message_box = recompui::message_box,
    };

    ultramodern::threads::callbacks_t threads_callbacks{
        .get_game_thread_name = zelda64::get_game_thread_name,
    };

    // Register the texture pack content type with rt64.json as its content file.
    recomp::mods::ModContentType texture_pack_content_type{
        .content_filename = "rt64.json",
        .allow_runtime_toggle = true,
        .on_enabled = enable_texture_pack,
        .on_disabled = disable_texture_pack,
        .on_reordered = reorder_texture_pack,
    };
    auto texture_pack_content_type_id = recomp::mods::register_mod_content_type(texture_pack_content_type);

    // Register the .rtz texture pack file format with the previous content type as its only allowed content type.
    recomp::mods::register_mod_container_type("rtz", std::vector{ texture_pack_content_type_id }, false);

#if defined(__ANDROID__)
    ZELDA_ANDROID_STAGE("installing pending Android mods");
    install_pending_android_mods();
#endif

    ZELDA_ANDROID_STAGE("calling recomp::start");
    ZELDA_ANDROID_LOG("calling recomp::start");
    recomp::Configuration recomp_config{
        .project_version = project_version,
        .window_handle = {},
        .rsp_callbacks = rsp_callbacks,
        .renderer_callbacks = renderer_callbacks,
        .audio_callbacks = audio_callbacks,
        .input_callbacks = input_callbacks,
        .gfx_callbacks = gfx_callbacks,
        .events_callbacks = thread_callbacks,
        .error_handling_callbacks = error_handling_callbacks,
        .threads_callbacks = threads_callbacks,
        .message_queue_control = {},
    };
    recomp::start(recomp_config);
    ZELDA_ANDROID_STAGE("recomp::start returned");
    ZELDA_ANDROID_LOG("recomp::start returned");

#if defined(__ANDROID__)
    // SDLActivity can invoke SDL_main again in the same Android process after
    // an activity relaunch. The recomp runtime keeps global patch symbol state,
    // so end the process when the native runtime exits and let Android start a
    // clean one next time.
    std::fflush(nullptr);
    kill(getpid(), SIGKILL);
    _exit(EXIT_SUCCESS);
#endif

#if defined(__APPLE__)
    // RT64's Metal worker threads can trip Objective-C autorelease cleanup while
    // unwinding during shutdown. Once the recomp runtime has returned, treat that
    // as an intentional app exit and skip process teardown that macOS reports as
    // a crash.
    std::fflush(nullptr);
    std::_Exit(EXIT_SUCCESS);
#endif

#if !defined(__ANDROID__)
    NFD_Quit();
#if defined(__APPLE__)
    if (metal_view != nullptr) {
        SDL_Metal_DestroyView(metal_view);
        metal_view = nullptr;
    }
#endif
    if (window != nullptr) {
        SDL_DestroyWindow(window);
        window = nullptr;
    }
    SDL_Quit();
#endif

    if (preloaded) {
        release_preload(preload_context);
    }

#ifdef _WIN32
    // End high resolution timing period.
    timeEndPeriod(1);
#endif

    return EXIT_SUCCESS;
}
