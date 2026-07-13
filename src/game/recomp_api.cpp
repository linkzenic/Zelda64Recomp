#include <cmath>
#include <cstdlib>
#include <vector>

#include "recomp.h"
#include "librecomp/overlays.hpp"
#include "zelda_config.h"
#include "zelda_clock_overlay.h"
#include "zelda_save_editor.h"
#include "recomp_input.h"
#include "recomp_ui.h"
#include "zelda_render.h"
#include "zelda_sound.h"
#include "librecomp/helpers.hpp"
#include "librecomp/game.hpp"
#include "librecomp/overlays.hpp"
#include "../patches/input.h"
#include "../patches/graphics.h"
#include "../patches/sound.h"
#include "ultramodern/ultramodern.hpp"
#include "ultramodern/config.hpp"

static bool android_n64_mode_enabled() {
#if defined(__ANDROID__)
    const char* n64_mode = std::getenv("APP_N64_MODE");
    const char* safe_mode = std::getenv("APP_SAFE_MODE");
    return (n64_mode && n64_mode[0] == '1') || (safe_mode && safe_mode[0] == '1');
#else
    return false;
#endif
}

extern "C" void recomp_update_inputs(uint8_t* rdram, recomp_context* ctx) {
    recomp::poll_inputs();
}

extern "C" void recomp_puts(uint8_t* rdram, recomp_context* ctx) {
    PTR(char) cur_str = _arg<0, PTR(char)>(rdram, ctx);
    u32 length = _arg<1, u32>(rdram, ctx);

    for (u32 i = 0; i < length; i++) {
        fputc(MEM_B(i, (gpr)cur_str), stdout);
    }
    fflush(stdout);
}

extern "C" void recomp_exit(uint8_t* rdram, recomp_context* ctx) {
    ultramodern::quit();
}

extern "C" void recomp_get_gyro_deltas(uint8_t* rdram, recomp_context* ctx) {
    float* x_out = _arg<0, float*>(rdram, ctx);
    float* y_out = _arg<1, float*>(rdram, ctx);

    recomp::get_gyro_deltas(x_out, y_out);
}

extern "C" void recomp_get_mouse_deltas(uint8_t* rdram, recomp_context* ctx) {
    float* x_out = _arg<0, float*>(rdram, ctx);
    float* y_out = _arg<1, float*>(rdram, ctx);

    recomp::get_mouse_deltas(x_out, y_out);
}

extern "C" void recomp_powf(uint8_t* rdram, recomp_context* ctx) {
    float a = _arg<0, float>(rdram, ctx);
    float b = ctx->f14.fl; //_arg<1, float>(rdram, ctx);

    _return(ctx, std::pow(a, b));
}

extern "C" void recomp_get_target_framerate(uint8_t* rdram, recomp_context* ctx) {
    int frame_divisor = _arg<0, u32>(rdram, ctx);

    _return(ctx, ultramodern::get_target_framerate(60 / frame_divisor));
}

extern "C" void recomp_get_window_resolution(uint8_t* rdram, recomp_context* ctx) {
    int width, height;
    recompui::get_window_size(width, height);

    gpr width_out = _arg<0, PTR(u32)>(rdram, ctx);
    gpr height_out = _arg<1, PTR(u32)>(rdram, ctx);

    MEM_W(0, width_out) = (u32)width;
    MEM_W(0, height_out) = (u32)height;
}

extern "C" void recomp_get_target_aspect_ratio(uint8_t* rdram, recomp_context* ctx) {
    ultramodern::renderer::GraphicsConfig graphics_config = ultramodern::renderer::get_graphics_config();
    float original = _arg<0, float>(rdram, ctx);
    int width, height;
    recompui::get_window_size(width, height);

    switch (graphics_config.ar_option) {
        case ultramodern::renderer::AspectRatio::Original:
        default:
            _return(ctx, original);
            return;
        case ultramodern::renderer::AspectRatio::Expand:
            _return(ctx, std::max(static_cast<float>(width) / height, original));
            return;
    }
}

extern "C" void recomp_get_targeting_mode(uint8_t* rdram, recomp_context* ctx) {
    _return(ctx, static_cast<int>(zelda64::get_targeting_mode()));
}

extern "C" void recomp_get_bgm_volume(uint8_t* rdram, recomp_context* ctx) {
    _return(ctx, zelda64::get_bgm_volume() / 100.0f);
}

extern "C" void recomp_get_low_health_beeps_enabled(uint8_t* rdram, recomp_context* ctx) {
    _return(ctx, static_cast<u32>(zelda64::get_low_health_beeps_enabled()));
}

extern "C" void recomp_time_us(uint8_t* rdram, recomp_context* ctx) {
    _return(ctx, static_cast<u32>(std::chrono::duration_cast<std::chrono::microseconds>(ultramodern::time_since_start()).count()));
}

extern "C" void recomp_get_autosave_enabled(uint8_t* rdram, recomp_context* ctx) {
    _return(ctx, static_cast<s32>(zelda64::get_autosave_mode() == zelda64::AutosaveMode::On));
}

extern "C" void recomp_get_save_anywhere_enabled(uint8_t* rdram, recomp_context* ctx) {
    _return(ctx, static_cast<s32>(!android_n64_mode_enabled() && zelda64::get_save_anywhere_enabled()));
}

extern "C" void recomp_save_editor_set_snapshot_value(uint8_t* rdram, recomp_context* ctx) {
    auto id = static_cast<zelda64::save_editor::ValueId>(_arg<0, s32>(rdram, ctx));
    zelda64::save_editor::set_snapshot_value(id, _arg<1, s32>(rdram, ctx));
}

extern "C" void recomp_save_editor_get_pending_value(uint8_t* rdram, recomp_context* ctx) {
    auto id = static_cast<zelda64::save_editor::ValueId>(_arg<0, s32>(rdram, ctx));
    _return(ctx, zelda64::save_editor::get_pending_value(id));
}

extern "C" void recomp_save_editor_should_apply_pending(uint8_t* rdram, recomp_context* ctx) {
    _return(ctx, zelda64::save_editor::should_apply_pending() ? 1 : 0);
}

extern "C" void recomp_save_editor_clear_pending(uint8_t* rdram, recomp_context* ctx) {
    zelda64::save_editor::clear_pending();
}

static gpr normalize_recomp_address(gpr addr);

extern "C" void recomp_load_overlays(uint8_t * rdram, recomp_context * ctx) {
    u32 rom = _arg<0, u32>(rdram, ctx);
    PTR(void) ram = _arg<1, PTR(void)>(rdram, ctx);
    u32 size = _arg<2, u32>(rdram, ctx);

    if (recomp::overlays::get_vrom_to_section_map().contains(rom)) {
        load_overlays(rom, ram, size);
        return;
    }

    std::span<const uint8_t> rom_data = recomp::get_rom();
    if ((rom <= rom_data.size()) && (size <= rom_data.size() - rom)) {
        gpr normalized_ram = normalize_recomp_address(ram);
        for (u32 i = 0; i < size; i++) {
            MEM_B(i, normalized_ram) = rom_data[rom + i];
        }
    }
}

static u32 read_be32(std::span<const uint8_t> data, size_t offset) {
    return (static_cast<u32>(data[offset + 0]) << 24) |
           (static_cast<u32>(data[offset + 1]) << 16) |
           (static_cast<u32>(data[offset + 2]) << 8) |
           (static_cast<u32>(data[offset + 3]) << 0);
}

extern "C" void recomp_android_load_yaz0(uint8_t* rdram, recomp_context* ctx) {
    u32 rom = _arg<0, u32>(rdram, ctx);
    u32 compressed_size = _arg<1, u32>(rdram, ctx);
    PTR(void) dst = _arg<2, PTR(void)>(rdram, ctx);
    u32 expected_size = _arg<3, u32>(rdram, ctx);

    std::span<const uint8_t> rom_data = recomp::get_rom();

    if (dst == NULLPTR || compressed_size < 0x10 || expected_size == 0 ||
        rom > rom_data.size() || compressed_size > rom_data.size() - rom) {
        _return(ctx, -1);
        return;
    }

    std::span<const uint8_t> yaz0 = rom_data.subspan(rom, compressed_size);
    if (yaz0[0] != 'Y' || yaz0[1] != 'a' || yaz0[2] != 'z' || yaz0[3] != '0') {
        _return(ctx, -2);
        return;
    }

    u32 decoded_size = read_be32(yaz0, 4);
    if (decoded_size != expected_size) {
        _return(ctx, -3);
        return;
    }

    std::vector<uint8_t> output(expected_size);
    size_t input_pos = 0x10;
    size_t output_pos = 0;

    while (input_pos < yaz0.size() && output_pos < output.size()) {
        uint8_t layout_bits = yaz0[input_pos++];

        for (int bit = 0; bit < 8 && input_pos < yaz0.size() && output_pos < output.size(); bit++) {
            if (layout_bits & 0x80) {
                output[output_pos++] = yaz0[input_pos++];
            } else {
                if (input_pos + 1 >= yaz0.size()) {
                    _return(ctx, -4);
                    return;
                }

                uint8_t first_byte = yaz0[input_pos++];
                uint8_t second_byte = yaz0[input_pos++];
                u32 offset = (((first_byte & 0x0F) << 8) | second_byte) + 1;
                u32 length = first_byte >> 4;

                if (length == 0) {
                    if (input_pos >= yaz0.size()) {
                        _return(ctx, -5);
                        return;
                    }
                    length = yaz0[input_pos++] + 0x12;
                } else {
                    length += 2;
                }

                if (offset > output_pos) {
                    _return(ctx, -6);
                    return;
                }

                for (u32 i = 0; i < length && output_pos < output.size(); i++) {
                    output[output_pos] = output[output_pos - offset];
                    output_pos++;
                }
            }

            layout_bits <<= 1;
        }
    }

    if (output_pos != output.size()) {
        _return(ctx, -7);
        return;
    }

    gpr normalized_dst = normalize_recomp_address(dst);
    for (u32 i = 0; i < expected_size; i++) {
        MEM_B(i, normalized_dst) = output[i];
    }

    _return(ctx, 0);
}

static gpr normalize_recomp_address(gpr addr) {
    u32 low = static_cast<u32>(addr);

    if (low == 0) {
        return 0;
    }

    if (low < 0x80000000U) {
        return 0xFFFFFFFF80000000ULL + low;
    }

    return 0xFFFFFFFF00000000ULL | low;
}

extern "C" void recomp_android_reset_effect_ss_table(uint8_t* rdram, recomp_context* ctx) {
    gpr table = _arg<0, PTR(void)>(rdram, ctx);
    u32 count = _arg<1, u32>(rdram, ctx);

    constexpr u32 kEffectSsSize = 0x60;
    constexpr u32 kLifeOffset = 0x5C;
    constexpr u32 kPriorityOffset = 0x5E;
    constexpr u32 kTypeOffset = 0x5F;
    constexpr u8 kDefaultPriority = 128;
    constexpr u8 kEffectSsMax = 0x27;

    if (table == NULLPTR || count > 0x400) {
        return;
    }

    table = normalize_recomp_address(table);

    for (u32 i = 0; i < count; i++) {
        gpr entry = table + i * kEffectSsSize;

        for (u32 offset = 0; offset < kLifeOffset; offset += sizeof(u32)) {
            MEM_W(offset, entry) = 0;
        }

        MEM_H(kLifeOffset, entry) = static_cast<u16>(-1);
        MEM_B(kPriorityOffset, entry) = kDefaultPriority;
        MEM_B(kTypeOffset, entry) = kEffectSsMax;
    }
}

extern "C" void recomp_android_get_entrance_scene_spawn(uint8_t* rdram, recomp_context* ctx) {
    gpr scene_entrance_table = normalize_recomp_address(_arg<0, PTR(void)>(rdram, ctx));
    u32 entrance = _arg<1, u32>(rdram, ctx) & 0xFFFF;
    gpr scene_id_out = normalize_recomp_address(_arg<2, PTR(s32)>(rdram, ctx));
    gpr spawn_num_out = normalize_recomp_address(_arg<3, PTR(s32)>(rdram, ctx));

    constexpr u32 kSceneEntranceTableEntrySize = 0x0C;
    constexpr u32 kSceneEntranceTableTableOffset = 0x04;
    constexpr u32 kEntranceTableEntrySize = 0x04;
    constexpr u32 kMaxSceneGroup = 0x6D;
    constexpr u32 kMaxEntranceGroup = 0x1F;

    u32 scene_group = entrance >> 9;
    u32 entrance_group = (entrance >> 4) & 0x1F;
    u32 spawn_index = entrance & 0x0F;

    if (scene_id_out == 0 || spawn_num_out == 0) {
        return;
    }

    if (scene_entrance_table == 0 || scene_group > kMaxSceneGroup || entrance_group > kMaxEntranceGroup) {
        MEM_W(0, scene_id_out) = 0;
        MEM_W(0, spawn_num_out) = 0;
        return;
    }

    gpr scene_entry = scene_entrance_table + scene_group * kSceneEntranceTableEntrySize;
    gpr entrance_table_list = normalize_recomp_address(static_cast<u32>(MEM_W(kSceneEntranceTableTableOffset, scene_entry)));

    if (entrance_table_list == 0) {
        MEM_W(0, scene_id_out) = 0;
        MEM_W(0, spawn_num_out) = 0;
        return;
    }

    gpr entrance_table = normalize_recomp_address(static_cast<u32>(MEM_W(entrance_group * sizeof(u32), entrance_table_list)));

    if (entrance_table == 0) {
        MEM_W(0, scene_id_out) = 0;
        MEM_W(0, spawn_num_out) = 0;
        return;
    }

    gpr entry = entrance_table + spawn_index * kEntranceTableEntrySize;

    s32 scene_id = static_cast<s8>(MEM_B(0, entry));
    s32 spawn_num = static_cast<s8>(MEM_B(1, entry));

    if (scene_id < 0) {
        scene_id = -scene_id;
    }

    MEM_W(0, scene_id_out) = scene_id;
    MEM_W(0, spawn_num_out) = spawn_num;
}

extern "C" void recomp_high_precision_fb_enabled(uint8_t * rdram, recomp_context * ctx) {
    _return(ctx, static_cast<s32>(zelda64::renderer::RT64HighPrecisionFBEnabled()));
}

extern "C" void recomp_get_resolution_scale(uint8_t* rdram, recomp_context* ctx) {
    _return(ctx, ultramodern::get_resolution_scale());
}

extern "C" void recomp_get_inverted_axes(uint8_t* rdram, recomp_context* ctx) {
    s32* x_out = _arg<0, s32*>(rdram, ctx);
    s32* y_out = _arg<1, s32*>(rdram, ctx);

    zelda64::CameraInvertMode mode = zelda64::get_camera_invert_mode();

    *x_out = (mode == zelda64::CameraInvertMode::InvertX || mode == zelda64::CameraInvertMode::InvertBoth);
    *y_out = (mode == zelda64::CameraInvertMode::InvertY || mode == zelda64::CameraInvertMode::InvertBoth);
}

extern "C" void recomp_get_analog_inverted_axes(uint8_t* rdram, recomp_context* ctx) {
    s32* x_out = _arg<0, s32*>(rdram, ctx);
    s32* y_out = _arg<1, s32*>(rdram, ctx);

    zelda64::CameraInvertMode mode = zelda64::get_analog_camera_invert_mode();

    *x_out = (mode == zelda64::CameraInvertMode::InvertX || mode == zelda64::CameraInvertMode::InvertBoth);
    *y_out = (mode == zelda64::CameraInvertMode::InvertY || mode == zelda64::CameraInvertMode::InvertBoth);
}

extern "C" void recomp_get_analog_cam_enabled(uint8_t* rdram, recomp_context* ctx) {
    _return<s32>(ctx, zelda64::get_analog_cam_mode() == zelda64::AnalogCamMode::On);
}

extern "C" void recomp_get_analog_camera_distance(uint8_t* rdram, recomp_context* ctx) {
    _return<s32>(ctx, zelda64::get_analog_camera_distance());
}

extern "C" void recomp_get_dpad_items_enabled(uint8_t* rdram, recomp_context* ctx) {
    if (android_n64_mode_enabled()) {
        _return<s32>(ctx, 0);
        return;
    }

    _return<s32>(ctx, zelda64::get_dpad_items_enabled());
}

extern "C" void recomp_get_dpad_items_mode_for_dpad_builtin(uint8_t* rdram, recomp_context* ctx) {
    if (android_n64_mode_enabled()) {
        _return<s32>(ctx, 0);
        return;
    }

    _return<s32>(ctx, static_cast<s32>(zelda64::get_dpad_items_mode()));
}

extern "C" void recomp_get_clock_style(uint8_t* rdram, recomp_context* ctx) {
    _return<s32>(ctx, zelda64::get_clock_style() == zelda64::ClockStyle::Original ? 0 : 1);
}

extern "C" void recomp_set_3ds_clock_state(uint8_t* rdram, recomp_context* ctx) {
    zelda64::set_clock_overlay_state({
        .visible = _arg<0, s32>(rdram, ctx) != 0,
        .alpha = _arg<1, s32>(rdram, ctx),
        .day = _arg<2, s32>(rdram, ctx),
        .current_time_seconds = _arg<3, s32>(rdram, ctx),
        .time_until_crash_seconds = _arg<4, s32>(rdram, ctx),
        .time_speed_offset = _arg<5, s32>(rdram, ctx),
        .final_hours = _arg<6, s32>(rdram, ctx) != 0,
    });
}

extern "C" void recomp_set_pause_save_prompt_overlay_state(uint8_t* rdram, recomp_context* ctx) {
    zelda64::set_pause_save_prompt_overlay_state({
        .visible = _arg<0, s32>(rdram, ctx) != 0,
        .alpha = _arg<1, s32>(rdram, ctx),
        .prompt_choice = _arg<2, s32>(rdram, ctx),
        .save_prompt_state = _arg<3, s32>(rdram, ctx),
    });
}

extern "C" void recomp_get_clock_texture_pack_loaded(uint8_t* rdram, recomp_context* ctx) {
    _return<s32>(ctx, zelda64::get_clock_style() == zelda64::ClockStyle::Import &&
                          zelda64::get_clock_texture_pack_loaded());
}

extern "C" void recomp_should_use_3ds_clock_overlay(uint8_t* rdram, recomp_context* ctx) {
    _return<s32>(ctx, zelda64::get_clock_style() != zelda64::ClockStyle::Original);
}

extern "C" void recomp_android_should_disable_rumble(uint8_t* rdram, recomp_context* ctx) {
    _return<s32>(ctx, recomp::android_should_disable_rumble());
}

extern "C" void recomp_android_should_use_sync_boot_dma(uint8_t* rdram, recomp_context* ctx) {
    _return<s32>(ctx, recomp::android_should_use_sync_boot_dma());
}

extern "C" void recomp_get_camera_inputs(uint8_t* rdram, recomp_context* ctx) {
    float* x_out = _arg<0, float*>(rdram, ctx);
    float* y_out = _arg<1, float*>(rdram, ctx);

    // TODO expose this in the menu
    constexpr float radial_deadzone = 0.05f;

    float x, y;

    recomp::get_right_analog(&x, &y);

    float magnitude = sqrtf(x * x + y * y);

    if (magnitude < radial_deadzone) {
        *x_out = 0.0f;
        *y_out = 0.0f;
    }
    else {
        float x_normalized = x / magnitude;
        float y_normalized = y / magnitude;

        *x_out = x_normalized * ((magnitude - radial_deadzone) / (1 - radial_deadzone));
        *y_out = y_normalized * ((magnitude - radial_deadzone) / (1 - radial_deadzone));
    }
}

extern "C" void recomp_set_right_analog_suppressed(uint8_t* rdram, recomp_context* ctx) {
    s32 suppressed = _arg<0, s32>(rdram, ctx);

    recomp::set_right_analog_suppressed(suppressed);
}
