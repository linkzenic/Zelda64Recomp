#include <cmath>
#include <cstdlib>
#include <algorithm>
#include <cctype>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

#if defined(__ANDROID__)
#include <android/log.h>
#define ZELDA_ANDROID_PMM_LOG(...) __android_log_print(ANDROID_LOG_INFO, "ZeldaPMM", __VA_ARGS__)
#define ZELDA_ANDROID_CHEATS_LOG(...) __android_log_print(ANDROID_LOG_INFO, "ZeldaCheats", __VA_ARGS__)
#else
#define ZELDA_ANDROID_PMM_LOG(...)
#define ZELDA_ANDROID_CHEATS_LOG(...)
#endif

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
#include "librecomp/mods.hpp"
#include "librecomp/overlays.hpp"
#include "librecomp/addresses.hpp"
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

static bool android_compatibility_build_enabled() {
#if defined(__ANDROID__)
    const char* compatibility_build = std::getenv("APP_ANDROID_COMPATIBILITY_BUILD");
    return compatibility_build && compatibility_build[0] == '1';
#else
    return false;
#endif
}

static gpr normalize_recomp_address(gpr addr);

namespace {
    struct CompatPlayerModelState {
        bool scanned = false;
        bool active = false;
        size_t selected_index = 0;
        std::vector<std::filesystem::path> model_paths;
    };

    std::mutex compat_player_model_mutex;
    CompatPlayerModelState compat_player_model_state;

    bool android_compat_pmm_enabled() {
        return android_compatibility_build_enabled() && !android_n64_mode_enabled();
    }

    std::filesystem::path compat_player_models_directory() {
#if defined(__ANDROID__)
        if (const char* app_folder_path = std::getenv("APP_FOLDER_PATH")) {
            return std::filesystem::path{app_folder_path} / "mod_data" / "yazmt_z64_playermodelmanager" / "models";
        }
#endif
        return {};
    }

    std::filesystem::path compat_player_models_config_path() {
#if defined(__ANDROID__)
        if (const char* app_folder_path = std::getenv("APP_FOLDER_PATH")) {
            return std::filesystem::path{app_folder_path} / "mod_data" / "yazmt_z64_playermodelmanager" / "models.ini";
        }
#endif
        return {};
    }

    std::filesystem::path compat_pmm_mod_config_path() {
#if defined(__ANDROID__)
        if (const char* app_folder_path = std::getenv("APP_FOLDER_PATH")) {
            return std::filesystem::path{app_folder_path} / "mod_config" / "yazmt_mm_playermodelmanager.json";
        }
#endif
        return {};
    }

    std::filesystem::path compat_cheats_mod_config_path() {
#if defined(__ANDROID__)
        if (const char* app_folder_path = std::getenv("APP_FOLDER_PATH")) {
            return std::filesystem::path{app_folder_path} / "mod_config" / "ProxyMM_Cheats.json";
        }
#endif
        return {};
    }

    struct CompatPmmConfigCache {
        bool loaded = false;
        std::filesystem::file_time_type last_write_time{};
        std::unordered_map<std::string, std::string> values;
    };

    std::mutex compat_pmm_config_mutex;
    CompatPmmConfigCache compat_pmm_config_cache;

    struct CompatCheatsConfigCache {
        bool loaded = false;
        std::filesystem::file_time_type last_write_time{};
        std::unordered_map<std::string, std::string> values;
    };

    std::mutex compat_cheats_config_mutex;
    CompatCheatsConfigCache compat_cheats_config_cache;

    bool compat_pmm_extract_json_value(const std::string& json, const std::string& key, std::string& out) {
        std::string needle = "\"" + key + "\"";
        size_t key_pos = json.find(needle);
        if (key_pos == std::string::npos) {
            return false;
        }

        size_t colon_pos = json.find(':', key_pos + needle.size());
        if (colon_pos == std::string::npos) {
            return false;
        }

        size_t value_pos = colon_pos + 1;
        while (value_pos < json.size() && std::isspace(static_cast<unsigned char>(json[value_pos]))) {
            value_pos++;
        }

        if (value_pos >= json.size()) {
            return false;
        }

        if (json[value_pos] == '"') {
            value_pos++;
            std::string value;
            bool escaping = false;
            for (size_t cursor = value_pos; cursor < json.size(); cursor++) {
                char c = json[cursor];
                if (escaping) {
                    value.push_back(c);
                    escaping = false;
                    continue;
                }
                if (c == '\\') {
                    escaping = true;
                    continue;
                }
                if (c == '"') {
                    out = std::move(value);
                    return true;
                }
                value.push_back(c);
            }
            return false;
        }

        size_t end_pos = value_pos;
        while (end_pos < json.size() && json[end_pos] != ',' && json[end_pos] != '}') {
            end_pos++;
        }

        out = json.substr(value_pos, end_pos - value_pos);
        while (!out.empty() && std::isspace(static_cast<unsigned char>(out.back()))) {
            out.pop_back();
        }
        return !out.empty();
    }

    bool compat_pmm_reload_config_locked() {
        std::filesystem::path path = compat_pmm_mod_config_path();
        if (path.empty()) {
            compat_pmm_config_cache.loaded = true;
            compat_pmm_config_cache.values.clear();
            return false;
        }

        std::error_code ec;
        std::filesystem::file_time_type write_time = std::filesystem::last_write_time(path, ec);
        if (!ec && compat_pmm_config_cache.loaded && compat_pmm_config_cache.last_write_time == write_time) {
            return true;
        }

        compat_pmm_config_cache.loaded = true;
        compat_pmm_config_cache.last_write_time = write_time;
        compat_pmm_config_cache.values.clear();

        std::ifstream input{path};
        if (!input) {
            return false;
        }

        std::string json{std::istreambuf_iterator<char>{input}, {}};
        for (const char* key : {"open_menu_buttons", "is_live_preview_enabled", "is_modify_tunic_color", "logging_level", "tunic_color"}) {
            std::string value;
            if (compat_pmm_extract_json_value(json, key, value)) {
                compat_pmm_config_cache.values[key] = std::move(value);
            }
        }

        return true;
    }

    bool compat_pmm_config_text(const std::string& key, std::string& out) {
        std::scoped_lock lock{compat_pmm_config_mutex};
        if (!compat_pmm_reload_config_locked()) {
            return false;
        }

        auto it = compat_pmm_config_cache.values.find(key);
        if (it == compat_pmm_config_cache.values.end()) {
            return false;
        }

        out = it->second;
        return true;
    }

    bool compat_cheats_reload_config_locked() {
        std::filesystem::path path = compat_cheats_mod_config_path();
        if (path.empty()) {
            compat_cheats_config_cache.loaded = true;
            compat_cheats_config_cache.values.clear();
            return false;
        }

        std::error_code ec;
        std::filesystem::file_time_type write_time = std::filesystem::last_write_time(path, ec);
        if (!ec && compat_cheats_config_cache.loaded && compat_cheats_config_cache.last_write_time == write_time) {
            return true;
        }

        compat_cheats_config_cache.loaded = true;
        compat_cheats_config_cache.last_write_time = write_time;
        compat_cheats_config_cache.values.clear();

        std::ifstream input{path};
        if (!input) {
            return false;
        }

        std::string json{std::istreambuf_iterator<char>{input}, {}};
        for (const char* key : {
                 "infinite_magic",
                 "infinite_health",
                 "infinite_rupees",
                 "infinite_consumables",
                 "blast_mask_cooldown",
                 "no_explosive_limit",
                 "unrestricted_items",
                 "hookshot_everything",
                 "hookshot_length_multiplier",
                 "longer_deku_flower_glide",
             }) {
            std::string value;
            if (compat_pmm_extract_json_value(json, key, value)) {
                compat_cheats_config_cache.values[key] = std::move(value);
            }
        }

        return true;
    }

    bool compat_cheats_config_text(const std::string& key, std::string& out) {
        std::scoped_lock lock{compat_cheats_config_mutex};
        if (!compat_cheats_reload_config_locked()) {
            return false;
        }

        auto it = compat_cheats_config_cache.values.find(key);
        if (it == compat_cheats_config_cache.values.end()) {
            return false;
        }

        out = it->second;
        return true;
    }

    uint32_t compat_pmm_enum_config_value(const std::string& key, const std::string& value, uint32_t fallback) {
        if (key == "is_modify_tunic_color") {
            if (value == "OFF") {
                return 0;
            }
            if (value == "AUTO") {
                return 1;
            }
            if (value == "FORCE") {
                return 2;
            }
        }
        else if (key == "logging_level") {
            if (value == "ALL") {
                return 0;
            }
            if (value == "VERBOSE") {
                return 1;
            }
            if (value == "INFO") {
                return 2;
            }
            if (value == "WARNING") {
                return 3;
            }
            if (value == "ERROR") {
                return 4;
            }
            if (value == "NONE") {
                return 5;
            }
        }

        char* end = nullptr;
        unsigned long parsed = std::strtoul(value.c_str(), &end, 10);
        if (end != value.c_str()) {
            return static_cast<uint32_t>(parsed);
        }
        return fallback;
    }

    void compat_pmm_log_config_u32(const std::string& key, const std::string& source, const std::string& raw, uint32_t value) {
#if defined(__ANDROID__)
        static std::mutex log_mutex;
        static std::unordered_map<std::string, std::string> logged_values;
        std::scoped_lock lock{log_mutex};
        std::string signature = source + ":" + raw + ":" + std::to_string(value);
        if (logged_values[key] == signature) {
            return;
        }
        logged_values[key] = signature;
        ZELDA_ANDROID_PMM_LOG("config u32 key=%s source=%s raw=%s value=%u",
                              key.c_str(),
                              source.c_str(),
                              raw.empty() ? "[empty]" : raw.c_str(),
                              value);
#endif
    }

    void compat_pmm_log_config_string(const std::string& key, const std::string& source, const std::string& value) {
#if defined(__ANDROID__)
        static std::mutex log_mutex;
        static std::unordered_map<std::string, std::string> logged_values;
        std::scoped_lock lock{log_mutex};
        std::string signature = source + ":" + value;
        if (logged_values[key] == signature) {
            return;
        }
        logged_values[key] = signature;
        ZELDA_ANDROID_PMM_LOG("config string key=%s source=%s value=%s",
                              key.c_str(),
                              source.c_str(),
                              value.empty() ? "[empty]" : value.c_str());
#endif
    }

    std::string compat_pmm_sanitize_field(std::string text) {
        for (char& c : text) {
            if (c == '\t' || c == '\n' || c == '\r') {
                c = ' ';
            }
        }
        return text;
    }

    std::string lowercase_path_string(const std::filesystem::path& path) {
        std::string text = path.generic_string();
        std::transform(text.begin(), text.end(), text.begin(), [](unsigned char c) {
            return static_cast<char>(std::tolower(c));
        });
        return text;
    }

    int compat_player_model_score(const std::filesystem::path& path) {
        std::string text = lowercase_path_string(path);
        int score = 0;

        if (text.find("object_link_child") != std::string::npos) {
            score -= 1000;
        }
        if (text.find("onlineyounglink") != std::string::npos || text.find("child_link") != std::string::npos ||
            text.find("young") != std::string::npos) {
            score -= 300;
        }
        if (text.find("mmy") != std::string::npos || text.find("mm_") != std::string::npos ||
            text.find("majora") != std::string::npos || text.find("humanmm") != std::string::npos) {
            score -= 120;
        }
        if (text.find("playas") != std::string::npos || text.find("play_as") != std::string::npos ||
            text.find("zzplayas") != std::string::npos) {
            score -= 40;
        }
        if (text.find("adult") != std::string::npos || text.find("object_link_boy") != std::string::npos ||
            text.find("skinadult") != std::string::npos) {
            score += 500;
        }

        return score;
    }

    void scan_compat_player_models_locked() {
        if (compat_player_model_state.scanned) {
            return;
        }

        compat_player_model_state.scanned = true;
        compat_player_model_state.active = false;
        compat_player_model_state.model_paths.clear();
        compat_player_model_state.selected_index = 0;

        std::error_code ec;
        std::filesystem::path models_dir = compat_player_models_directory();
        if (models_dir.empty() || !std::filesystem::exists(models_dir, ec)) {
            return;
        }

        for (std::filesystem::recursive_directory_iterator it{models_dir, ec}, end; !ec && it != end; it.increment(ec)) {
            if (ec || !it->is_regular_file(ec)) {
                continue;
            }

            std::filesystem::path path = it->path();
            std::string extension = path.extension().string();
            std::transform(extension.begin(), extension.end(), extension.begin(), [](unsigned char c) {
                return static_cast<char>(std::tolower(c));
            });

            if (extension == ".zobj") {
                compat_player_model_state.model_paths.emplace_back(std::move(path));
            }
        }

        std::sort(compat_player_model_state.model_paths.begin(), compat_player_model_state.model_paths.end(),
                  [](const std::filesystem::path& lhs, const std::filesystem::path& rhs) {
                      int lhs_score = compat_player_model_score(lhs);
                      int rhs_score = compat_player_model_score(rhs);
                      if (lhs_score != rhs_score) {
                          return lhs_score < rhs_score;
                      }
                      return lhs.generic_string() < rhs.generic_string();
                  });
    }

    const std::filesystem::path* selected_compat_player_model_locked() {
        scan_compat_player_models_locked();
        if (compat_player_model_state.model_paths.empty()) {
            return nullptr;
        }

        if (const char* selected_index = std::getenv("APP_COMPAT_PMM_SELECTED_INDEX")) {
            char* end = nullptr;
            unsigned long parsed_index = std::strtoul(selected_index, &end, 10);
            if (end != selected_index) {
                compat_player_model_state.selected_index = static_cast<size_t>(parsed_index);
            }
        }

        if (const char* active = std::getenv("APP_COMPAT_PMM_ACTIVE")) {
            compat_player_model_state.active = active[0] == '1';
        }

        compat_player_model_state.selected_index %= compat_player_model_state.model_paths.size();
        return &compat_player_model_state.model_paths[compat_player_model_state.selected_index];
    }
}

namespace {
    constexpr size_t kZ64oModloaderHeaderOffset = 0x5000;
    constexpr size_t kZ64oFormByteOffset = 0x500B;
    constexpr size_t kZ64oEmbeddedInfoOffset = 0x5500;
    constexpr size_t kZ64oMinimumStandaloneSize = 0x5801;
    constexpr size_t kPmmMaxInternalNameLength = 64;
    constexpr size_t kPmmMaxDisplayNameLength = 32;
    constexpr size_t kPmmMaxAuthorNameLength = 64;
    constexpr size_t kPmmModelTypeCountLimit = 64;
    constexpr int kPmmModelTypeChild = 1;
    constexpr int kPmmModelTypeAdult = 2;
    constexpr unsigned char kMmoFormByteChild = 0x04;
    constexpr unsigned char kMmoFormByteAdult = 0x68;
    constexpr unsigned char kOotoFormByteChild = 0x01;
    constexpr unsigned char kOotoFormByteAdult = 0x00;

    struct CompatFsModelDiskEntry {
        std::vector<char> model_data;
        bool model_types[kPmmModelTypeCountLimit]{};
        std::string internal_name;
        std::string display_name;
        std::string author_name;
    };

    std::mutex compat_fsmodels_mutex;
    std::filesystem::path compat_fsmodels_pmm_dir;
    std::vector<CompatFsModelDiskEntry> compat_fsmodels_entries;

    std::string read_rdram_cstr(uint8_t* rdram, gpr ptr) {
        if (ptr == 0) {
            return {};
        }

        gpr addr = normalize_recomp_address(ptr);
        std::string text;
        for (size_t i = 0; i < 4096; i++) {
            char c = static_cast<char>(MEM_B(i, addr));
            if (c == '\0') {
                break;
            }
            text.push_back(c);
        }
        return text;
    }

    bool write_rdram_bytes(uint8_t* rdram, gpr ptr, int buffer_size, const void* data, size_t data_size) {
        if (ptr == 0 || buffer_size < 0 || static_cast<size_t>(buffer_size) < data_size) {
            return false;
        }

        gpr addr = normalize_recomp_address(ptr);
        const char* bytes = static_cast<const char*>(data);
        for (size_t i = 0; i < data_size; i++) {
            MEM_B(i, addr) = bytes[i];
        }
        return true;
    }

    bool write_rdram_string(uint8_t* rdram, gpr ptr, int buffer_size, const std::string& text) {
        return write_rdram_bytes(rdram, ptr, buffer_size, text.c_str(), text.size() + 1);
    }

    bool compat_fsmodels_valid_zobj(const std::vector<char>& bytes) {
        static constexpr char kMagic[] = "MODLOADER64";
        if (bytes.size() < kZ64oMinimumStandaloneSize || bytes.size() <= kZ64oFormByteOffset) {
            return false;
        }
        if (std::memcmp(bytes.data() + kZ64oModloaderHeaderOffset, kMagic, sizeof(kMagic) - 1) != 0) {
            return false;
        }

        unsigned char form = static_cast<unsigned char>(bytes[kZ64oFormByteOffset]);
        return form == kMmoFormByteChild || form == kMmoFormByteAdult ||
               form == kOotoFormByteChild || form == kOotoFormByteAdult;
    }

    std::string compat_fsmodels_right_substr(const std::string& text, size_t size) {
        if (text.size() <= size) {
            return text;
        }
        return text.substr(text.size() - size);
    }

    std::string compat_fsmodels_embedded_string(const std::vector<char>& bytes, size_t offset, size_t max_size) {
        if (offset >= bytes.size()) {
            return {};
        }

        size_t end = std::min(bytes.size(), offset + max_size);
        size_t cursor = offset;
        while (cursor < end && bytes[cursor] != '\0') {
            cursor++;
        }
        return cursor > offset ? std::string(bytes.data() + offset, bytes.data() + cursor) : std::string{};
    }

    void compat_fsmodels_extract_embedded_info(CompatFsModelDiskEntry& entry) {
        static constexpr char kInfoMagic[] = "PLAYERMODELINFO";
        const std::vector<char>& bytes = entry.model_data;
        if (bytes.size() < kZ64oEmbeddedInfoOffset + sizeof(kInfoMagic) + 1) {
            return;
        }
        if (std::memcmp(bytes.data() + kZ64oEmbeddedInfoOffset, kInfoMagic, sizeof(kInfoMagic) - 1) != 0) {
            return;
        }
        if (bytes[kZ64oEmbeddedInfoOffset + sizeof(kInfoMagic) - 1] != 1) {
            return;
        }

        size_t internal_offset = kZ64oEmbeddedInfoOffset + sizeof(kInfoMagic);
        size_t display_offset = internal_offset + kPmmMaxInternalNameLength;
        size_t author_offset = display_offset + kPmmMaxDisplayNameLength;
        entry.internal_name = compat_fsmodels_embedded_string(bytes, internal_offset, kPmmMaxInternalNameLength);
        entry.display_name = compat_fsmodels_embedded_string(bytes, display_offset, kPmmMaxDisplayNameLength);
        entry.author_name = compat_fsmodels_embedded_string(bytes, author_offset, kPmmMaxAuthorNameLength);
    }

    CompatFsModelDiskEntry* compat_fsmodels_entry(int index) {
        if (index < 0 || static_cast<size_t>(index) >= compat_fsmodels_entries.size()) {
            return nullptr;
        }
        return &compat_fsmodels_entries[static_cast<size_t>(index)];
    }

    int compat_fsmodels_scan_locked() {
        compat_fsmodels_entries.clear();

        std::error_code ec;
        std::filesystem::path model_dir = compat_fsmodels_pmm_dir / "models";
        if (compat_fsmodels_pmm_dir.empty() || !std::filesystem::is_directory(model_dir, ec)) {
            ZELDA_ANDROID_PMM_LOG("FSModels scan skipped path=%s exists=%d ec=%d",
                                  model_dir.string().c_str(),
                                  std::filesystem::is_directory(model_dir),
                                  ec.value());
            return -1;
        }

        constexpr uintmax_t kMaxModelSize = 4 * 1024 * 1024;
        constexpr uintmax_t kMaxTotalSize = 128 * 1024 * 1024;
        uintmax_t running_total_size = 0;

        for (std::filesystem::recursive_directory_iterator it{model_dir, ec}, end; !ec && it != end; it.increment(ec)) {
            if (ec) {
                break;
            }

            std::error_code entry_ec;
            if (!it->is_regular_file(entry_ec) && !(it->is_symlink(entry_ec) && std::filesystem::is_regular_file(std::filesystem::weakly_canonical(it->path(), entry_ec), entry_ec))) {
                continue;
            }

            uintmax_t file_size = it->file_size(entry_ec);
            if (entry_ec || file_size == 0 || file_size >= kMaxModelSize || running_total_size >= kMaxTotalSize) {
                continue;
            }

            std::ifstream input{it->path(), std::ios::binary};
            if (!input) {
                continue;
            }

            std::vector<char> bytes{std::istreambuf_iterator<char>{input}, {}};
            if (!compat_fsmodels_valid_zobj(bytes)) {
                continue;
            }

            CompatFsModelDiskEntry entry;
            entry.model_data = std::move(bytes);
            compat_fsmodels_extract_embedded_info(entry);

            if (entry.internal_name.empty()) {
                entry.internal_name = std::filesystem::relative(it->path(), compat_fsmodels_pmm_dir, entry_ec).generic_string();
                if (entry.internal_name.size() > kPmmMaxInternalNameLength) {
                    entry.internal_name = compat_fsmodels_right_substr(entry.internal_name, kPmmMaxInternalNameLength);
                }
            }
            if (entry.internal_name.size() < kPmmMaxAuthorNameLength) {
                entry.internal_name += " ";
            }

            if (entry.display_name.empty()) {
                entry.display_name = it->path().stem().string();
                if (entry.display_name.size() > kPmmMaxDisplayNameLength) {
                    entry.display_name = entry.display_name.substr(0, kPmmMaxDisplayNameLength);
                }
            }
            if (entry.author_name.empty()) {
                entry.author_name = "N/A";
            }

            unsigned char form = static_cast<unsigned char>(entry.model_data[kZ64oFormByteOffset]);
            if (form == kOotoFormByteAdult || form == kMmoFormByteAdult) {
                entry.model_types[kPmmModelTypeAdult] = true;
            }
            else if (form == kOotoFormByteChild || form == kMmoFormByteChild) {
                entry.model_types[kPmmModelTypeChild] = true;
            }

            running_total_size += entry.model_data.size();
            compat_fsmodels_entries.emplace_back(std::move(entry));
        }

        std::sort(compat_fsmodels_entries.begin(), compat_fsmodels_entries.end(),
                  [](const CompatFsModelDiskEntry& lhs, const CompatFsModelDiskEntry& rhs) {
                      return lhs.display_name < rhs.display_name;
                  });
        ZELDA_ANDROID_PMM_LOG("FSModels scan path=%s entries=%zu",
                              model_dir.string().c_str(),
                              compat_fsmodels_entries.size());
        return static_cast<int>(compat_fsmodels_entries.size());
    }

    bool compat_fsmodels_write_u8(uint8_t* rdram, gpr ptr, uint8_t value) {
        if (ptr == 0) {
            return false;
        }
        MEM_B(0, normalize_recomp_address(ptr)) = value;
        return true;
    }

    bool compat_fsmodels_write_u16(uint8_t* rdram, gpr ptr, uint16_t value) {
        if (ptr == 0) {
            return false;
        }
        MEM_H(0, normalize_recomp_address(ptr)) = value;
        return true;
    }

    bool compat_fsmodels_write_u32(uint8_t* rdram, gpr ptr, uint32_t value) {
        if (ptr == 0) {
            return false;
        }
        MEM_W(0, normalize_recomp_address(ptr)) = value;
        return true;
    }
}

extern "C" void PMMZobj_setPMMDir(uint8_t* rdram, recomp_context* ctx) {
    std::string mods_dir = read_rdram_cstr(rdram, _arg<0, PTR(char)>(rdram, ctx));
    std::filesystem::path base_dir;
    if (mods_dir.empty()) {
#if defined(__ANDROID__)
        if (const char* app_folder_path = std::getenv("APP_FOLDER_PATH")) {
            base_dir = app_folder_path;
        }
#endif
    } else {
        base_dir = std::filesystem::path{mods_dir}.parent_path();
    }

    if (base_dir.empty()) {
        _return(ctx, false);
        return;
    }

    std::scoped_lock lock{compat_fsmodels_mutex};
    compat_fsmodels_pmm_dir = base_dir / "mod_data" / "yazmt_z64_playermodelmanager";

    std::error_code ec;
    std::filesystem::create_directories(compat_fsmodels_pmm_dir / "models", ec);
    ZELDA_ANDROID_PMM_LOG("FSModels dir set path=%s ok=%d",
                          compat_fsmodels_pmm_dir.string().c_str(),
                          ec ? 0 : 1);
    _return(ctx, !ec);
}

extern "C" void PMMZobj_scanForDiskEntries(uint8_t* rdram, recomp_context* ctx) {
    std::scoped_lock lock{compat_fsmodels_mutex};
    _return(ctx, compat_fsmodels_scan_locked());
}

extern "C" void PMMZobj_getNumDiskEntries(uint8_t* rdram, recomp_context* ctx) {
    std::scoped_lock lock{compat_fsmodels_mutex};
    _return(ctx, static_cast<int>(compat_fsmodels_entries.size()));
}

extern "C" void PMMZobj_entryInternalNameLength(uint8_t* rdram, recomp_context* ctx) {
    int index = _arg<0, s32>(rdram, ctx);
    std::scoped_lock lock{compat_fsmodels_mutex};
    CompatFsModelDiskEntry* entry = compat_fsmodels_entry(index);
    _return(ctx, entry ? static_cast<int>(entry->internal_name.size()) : 0);
}

extern "C" void PMMZobj_entryDisplayNameLength(uint8_t* rdram, recomp_context* ctx) {
    int index = _arg<0, s32>(rdram, ctx);
    std::scoped_lock lock{compat_fsmodels_mutex};
    CompatFsModelDiskEntry* entry = compat_fsmodels_entry(index);
    _return(ctx, entry ? static_cast<int>(entry->display_name.size()) : 0);
}

extern "C" void PMMZobj_entryAuthorNameLength(uint8_t* rdram, recomp_context* ctx) {
    int index = _arg<0, s32>(rdram, ctx);
    std::scoped_lock lock{compat_fsmodels_mutex};
    CompatFsModelDiskEntry* entry = compat_fsmodels_entry(index);
    _return(ctx, entry ? static_cast<int>(entry->author_name.size()) : 0);
}

extern "C" void PMMZobj_writeInternalNameToBuffer(uint8_t* rdram, recomp_context* ctx) {
    int index = _arg<0, s32>(rdram, ctx);
    gpr buffer = _arg<1, PTR(char)>(rdram, ctx);
    int buffer_size = _arg<2, s32>(rdram, ctx);
    std::scoped_lock lock{compat_fsmodels_mutex};
    CompatFsModelDiskEntry* entry = compat_fsmodels_entry(index);
    _return(ctx, entry != nullptr && write_rdram_string(rdram, buffer, buffer_size, entry->internal_name));
}

extern "C" void PMMZobj_writeDisplayNameToBuffer(uint8_t* rdram, recomp_context* ctx) {
    int index = _arg<0, s32>(rdram, ctx);
    gpr buffer = _arg<1, PTR(char)>(rdram, ctx);
    int buffer_size = _arg<2, s32>(rdram, ctx);
    std::scoped_lock lock{compat_fsmodels_mutex};
    CompatFsModelDiskEntry* entry = compat_fsmodels_entry(index);
    _return(ctx, entry != nullptr && write_rdram_string(rdram, buffer, buffer_size, entry->display_name));
}

extern "C" void PMMZobj_writeAuthorNameToBuffer(uint8_t* rdram, recomp_context* ctx) {
    int index = _arg<0, s32>(rdram, ctx);
    gpr buffer = _arg<1, PTR(char)>(rdram, ctx);
    int buffer_size = _arg<2, s32>(rdram, ctx);
    std::scoped_lock lock{compat_fsmodels_mutex};
    CompatFsModelDiskEntry* entry = compat_fsmodels_entry(index);
    _return(ctx, entry != nullptr && write_rdram_string(rdram, buffer, buffer_size, entry->author_name));
}

extern "C" void PMMZobj_getEntryFileSize(uint8_t* rdram, recomp_context* ctx) {
    int index = _arg<0, s32>(rdram, ctx);
    std::scoped_lock lock{compat_fsmodels_mutex};
    CompatFsModelDiskEntry* entry = compat_fsmodels_entry(index);
    _return(ctx, entry ? static_cast<int>(entry->model_data.size()) : 0);
}

extern "C" void PMMZobj_getEntryFileData(uint8_t* rdram, recomp_context* ctx) {
    int index = _arg<0, s32>(rdram, ctx);
    gpr buffer = _arg<1, PTR(char)>(rdram, ctx);
    int buffer_size = _arg<2, s32>(rdram, ctx);
    std::scoped_lock lock{compat_fsmodels_mutex};
    CompatFsModelDiskEntry* entry = compat_fsmodels_entry(index);
    _return(ctx, entry != nullptr && write_rdram_bytes(rdram, buffer, buffer_size, entry->model_data.data(), entry->model_data.size()));
}

extern "C" void PMMZobj_createDirectory(uint8_t* rdram, recomp_context* ctx) {
    std::string path = read_rdram_cstr(rdram, _arg<0, PTR(char)>(rdram, ctx));
    std::error_code ec;
    if (!path.empty()) {
        std::filesystem::create_directories(path, ec);
    }
    _return(ctx, !path.empty() && !ec);
}

extern "C" void PMMZobj_isDirectoryExist(uint8_t* rdram, recomp_context* ctx) {
    std::string path = read_rdram_cstr(rdram, _arg<0, PTR(char)>(rdram, ctx));
    std::error_code ec;
    _return(ctx, !path.empty() && std::filesystem::is_directory(path, ec) && !ec);
}

extern "C" void PMMZobj_readEntryU8(uint8_t* rdram, recomp_context* ctx) {
    int index = _arg<0, s32>(rdram, ctx);
    int offset = _arg<1, s32>(rdram, ctx);
    gpr out = _arg<2, PTR(uint8_t)>(rdram, ctx);
    std::scoped_lock lock{compat_fsmodels_mutex};
    CompatFsModelDiskEntry* entry = compat_fsmodels_entry(index);
    bool ok = entry != nullptr && offset >= 0 && static_cast<size_t>(offset) < entry->model_data.size() &&
              compat_fsmodels_write_u8(rdram, out, static_cast<uint8_t>(entry->model_data[static_cast<size_t>(offset)]));
    _return(ctx, ok);
}

extern "C" void PMMZobj_readEntryU16(uint8_t* rdram, recomp_context* ctx) {
    int index = _arg<0, s32>(rdram, ctx);
    int offset = _arg<1, s32>(rdram, ctx);
    gpr out = _arg<2, PTR(uint16_t)>(rdram, ctx);
    std::scoped_lock lock{compat_fsmodels_mutex};
    CompatFsModelDiskEntry* entry = compat_fsmodels_entry(index);
    bool ok = false;
    if (entry != nullptr && offset >= 0 && static_cast<size_t>(offset) + sizeof(uint16_t) <= entry->model_data.size()) {
        size_t cursor = static_cast<size_t>(offset);
        uint16_t value = (static_cast<uint8_t>(entry->model_data[cursor + 0]) << 8) |
                         (static_cast<uint8_t>(entry->model_data[cursor + 1]) << 0);
        ok = compat_fsmodels_write_u16(rdram, out, value);
    }
    _return(ctx, ok);
}

extern "C" void PMMZobj_readEntryU32(uint8_t* rdram, recomp_context* ctx) {
    int index = _arg<0, s32>(rdram, ctx);
    int offset = _arg<1, s32>(rdram, ctx);
    gpr out = _arg<2, PTR(uint32_t)>(rdram, ctx);
    std::scoped_lock lock{compat_fsmodels_mutex};
    CompatFsModelDiskEntry* entry = compat_fsmodels_entry(index);
    bool ok = false;
    if (entry != nullptr && offset >= 0 && static_cast<size_t>(offset) + sizeof(uint32_t) <= entry->model_data.size()) {
        size_t cursor = static_cast<size_t>(offset);
        uint32_t value = (static_cast<uint8_t>(entry->model_data[cursor + 0]) << 24) |
                         (static_cast<uint8_t>(entry->model_data[cursor + 1]) << 16) |
                         (static_cast<uint8_t>(entry->model_data[cursor + 2]) << 8) |
                         (static_cast<uint8_t>(entry->model_data[cursor + 3]) << 0);
        ok = compat_fsmodels_write_u32(rdram, out, value);
    }
    _return(ctx, ok);
}

extern "C" void PMMZobj_isModelType(uint8_t* rdram, recomp_context* ctx) {
    int index = _arg<0, s32>(rdram, ctx);
    int type = _arg<1, s32>(rdram, ctx);
    std::scoped_lock lock{compat_fsmodels_mutex};
    CompatFsModelDiskEntry* entry = compat_fsmodels_entry(index);
    _return(ctx, entry != nullptr && type >= 0 && static_cast<size_t>(type) < kPmmModelTypeCountLimit && entry->model_types[type]);
}

extern "C" void PMMZobj_clearDiskEntries(uint8_t* rdram, recomp_context* ctx) {
    std::scoped_lock lock{compat_fsmodels_mutex};
    compat_fsmodels_entries.clear();
}

extern "C" void PMMZobj_tryLoadOOTROM(uint8_t* rdram, recomp_context* ctx) {
    _return(ctx, false);
}

extern "C" void PMMZobj_unloadOOTROM(uint8_t* rdram, recomp_context* ctx) {
    _return(ctx, true);
}

extern "C" void PMMZobj_isOOTRomLoaded(uint8_t* rdram, recomp_context* ctx) {
    _return(ctx, false);
}

extern "C" void PMMZobj_extractGameplayKeep(uint8_t* rdram, recomp_context* ctx) {
    _return(ctx, false);
}

extern "C" void PMMZobj_extractChildLink(uint8_t* rdram, recomp_context* ctx) {
    _return(ctx, false);
}

extern "C" void PMMZobj_extractAdultLink(uint8_t* rdram, recomp_context* ctx) {
    _return(ctx, false);
}

extern "C" void PMMZobj_extractMirrorShieldRay(uint8_t* rdram, recomp_context* ctx) {
    _return(ctx, false);
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

extern "C" void recomp_android_compat_pmm_model_count(uint8_t* rdram, recomp_context* ctx) {
    if (!android_compat_pmm_enabled()) {
        _return<s32>(ctx, 0);
        return;
    }

    std::scoped_lock lock{compat_player_model_mutex};
    scan_compat_player_models_locked();
    _return<s32>(ctx, static_cast<s32>(compat_player_model_state.model_paths.size()));
}

extern "C" void recomp_android_compat_pmm_cycle_model(uint8_t* rdram, recomp_context* ctx) {
    s32 delta = _arg<0, s32>(rdram, ctx);
    if (!android_compat_pmm_enabled()) {
        _return<s32>(ctx, -1);
        return;
    }

    std::scoped_lock lock{compat_player_model_mutex};
    scan_compat_player_models_locked();
    size_t count = compat_player_model_state.model_paths.size();
    if (count == 0) {
        _return<s32>(ctx, -2);
        return;
    }

    if (!compat_player_model_state.active) {
        compat_player_model_state.active = true;
    } else if (delta >= 0) {
        compat_player_model_state.selected_index = (compat_player_model_state.selected_index + static_cast<size_t>(delta)) % count;
    } else {
        size_t step = static_cast<size_t>(-delta) % count;
        compat_player_model_state.selected_index = (compat_player_model_state.selected_index + count - step) % count;
    }

    setenv("APP_COMPAT_PMM_ACTIVE", "1", 1);
    setenv("APP_COMPAT_PMM_SELECTED_INDEX", std::to_string(compat_player_model_state.selected_index).c_str(), 1);

    _return<s32>(ctx, static_cast<s32>(compat_player_model_state.selected_index));
}

extern "C" void recomp_android_compat_pmm_load_selected_model(uint8_t* rdram, recomp_context* ctx) {
    PTR(void) dst = _arg<0, PTR(void)>(rdram, ctx);
    u32 max_size = _arg<1, u32>(rdram, ctx);

    if (!android_compat_pmm_enabled() || dst == NULLPTR || max_size == 0) {
        _return<s32>(ctx, -1);
        return;
    }

    std::filesystem::path selected_path;
    {
        std::scoped_lock lock{compat_player_model_mutex};
        const std::filesystem::path* path = selected_compat_player_model_locked();
        if (path == nullptr) {
            _return<s32>(ctx, -2);
            return;
        }
        if (!compat_player_model_state.active) {
            _return<s32>(ctx, -5);
            return;
        }
        selected_path = *path;
    }

    std::error_code ec;
    uintmax_t file_size = std::filesystem::file_size(selected_path, ec);
    if (ec || file_size == 0 || file_size > max_size) {
        _return<s32>(ctx, -3);
        return;
    }

    std::ifstream input{selected_path, std::ios::binary};
    if (!input) {
        _return<s32>(ctx, -4);
        return;
    }

    std::vector<uint8_t> bytes(static_cast<size_t>(file_size));
    input.read(reinterpret_cast<char*>(bytes.data()), static_cast<std::streamsize>(bytes.size()));
    if (!input) {
        _return<s32>(ctx, -5);
        return;
    }

    gpr normalized_dst = normalize_recomp_address(dst);
    for (size_t i = 0; i < bytes.size(); i++) {
        MEM_B(i, normalized_dst) = bytes[i];
    }
    for (size_t i = bytes.size(); i < max_size; i++) {
        MEM_B(i, normalized_dst) = 0;
    }

    _return<s32>(ctx, static_cast<s32>(bytes.size()));
}

extern "C" void recomp_android_compat_pmm_begin_save(uint8_t* rdram, recomp_context* ctx) {
    if (!android_compat_pmm_enabled()) {
        _return<s32>(ctx, -1);
        return;
    }

    std::filesystem::path config_path = compat_player_models_config_path();
    if (config_path.empty()) {
        _return<s32>(ctx, -2);
        return;
    }

    std::error_code ec;
    std::filesystem::create_directories(config_path.parent_path(), ec);
    if (ec) {
        _return<s32>(ctx, -3);
        return;
    }

    std::ofstream output{config_path, std::ios::binary | std::ios::trunc};
    if (!output) {
        _return<s32>(ctx, -4);
        return;
    }

    output << "meta\tcfg_version\t1\n";
    _return<s32>(ctx, 0);
}

extern "C" void recomp_android_compat_pmm_write_selection(uint8_t* rdram, recomp_context* ctx) {
    if (!android_compat_pmm_enabled()) {
        _return<s32>(ctx, -1);
        return;
    }

    std::string section = compat_pmm_sanitize_field(read_rdram_cstr(rdram, _arg<0, PTR(char)>(rdram, ctx)));
    std::string key = compat_pmm_sanitize_field(read_rdram_cstr(rdram, _arg<1, PTR(char)>(rdram, ctx)));
    std::string internal_name = compat_pmm_sanitize_field(read_rdram_cstr(rdram, _arg<2, PTR(char)>(rdram, ctx)));

    if (section.empty() || key.empty()) {
        _return<s32>(ctx, -2);
        return;
    }

    std::filesystem::path config_path = compat_player_models_config_path();
    if (config_path.empty()) {
        _return<s32>(ctx, -3);
        return;
    }

    std::ofstream output{config_path, std::ios::binary | std::ios::app};
    if (!output) {
        _return<s32>(ctx, -4);
        return;
    }

    output << section << '\t' << key << '\t' << internal_name << '\n';
    _return<s32>(ctx, 0);
}

extern "C" void recomp_android_compat_pmm_load_selection(uint8_t* rdram, recomp_context* ctx) {
    if (!android_compat_pmm_enabled()) {
        _return<s32>(ctx, -1);
        return;
    }

    std::string target_section = read_rdram_cstr(rdram, _arg<0, PTR(char)>(rdram, ctx));
    std::string target_key = read_rdram_cstr(rdram, _arg<1, PTR(char)>(rdram, ctx));
    PTR(void) dst = _arg<2, PTR(void)>(rdram, ctx);
    u32 max_size = _arg<3, u32>(rdram, ctx);

    if (target_section.empty() || target_key.empty() || dst == NULLPTR || max_size == 0) {
        _return<s32>(ctx, -2);
        return;
    }

    std::filesystem::path config_path = compat_player_models_config_path();
    if (config_path.empty()) {
        _return<s32>(ctx, -3);
        return;
    }

    std::ifstream input{config_path, std::ios::binary};
    if (!input) {
        _return<s32>(ctx, 0);
        return;
    }

    std::string line;
    while (std::getline(input, line)) {
        size_t first_tab = line.find('\t');
        if (first_tab == std::string::npos) {
            continue;
        }
        size_t second_tab = line.find('\t', first_tab + 1);
        if (second_tab == std::string::npos) {
            continue;
        }

        std::string section = line.substr(0, first_tab);
        std::string key = line.substr(first_tab + 1, second_tab - first_tab - 1);
        if (section != target_section || key != target_key) {
            continue;
        }

        std::string value = line.substr(second_tab + 1);
        if (!write_rdram_string(rdram, dst, static_cast<int>(max_size), value)) {
            _return<s32>(ctx, -4);
            return;
        }

        _return<s32>(ctx, 1);
        return;
    }

    _return<s32>(ctx, 0);
}

template <typename StringType>
static void return_compat_pmm_string(uint8_t* rdram, recomp_context* ctx, const StringType& str) {
    size_t alloc_size = (str.size() + 1 + 15) & ~15;
    gpr offset = reinterpret_cast<uint8_t*>(recomp::alloc(rdram, alloc_size)) - rdram;
    gpr addr = offset + 0xFFFFFFFF80000000ULL;

    for (size_t i = 0; i < str.size(); i++) {
        MEM_B(i, addr) = str[i];
    }
    MEM_B(str.size(), addr) = 0;
    ctx->r2 = addr;
}

extern "C" void recomp_android_compat_pmm_get_config_u32(uint8_t* rdram, recomp_context* ctx) {
    std::string key = read_rdram_cstr(rdram, _arg<0, PTR(char)>(rdram, ctx));
    recomp::mods::ConfigValueVariant val = recomp::mods::get_mod_config_value("yazmt_mm_playermodelmanager", key);
    std::string config_text;

    if (compat_pmm_config_text(key, config_text)) {
        uint32_t value = compat_pmm_enum_config_value(key, config_text, 0);
        compat_pmm_log_config_u32(key, "json", config_text, value);
        _return(ctx, value);
    } else if (uint32_t* as_u32 = std::get_if<uint32_t>(&val)) {
        compat_pmm_log_config_u32(key, "mod-u32", std::to_string(*as_u32), *as_u32);
        _return(ctx, *as_u32);
    } else if (double* as_double = std::get_if<double>(&val)) {
        uint32_t value = static_cast<uint32_t>(static_cast<int32_t>(*as_double));
        compat_pmm_log_config_u32(key, "mod-double", std::to_string(*as_double), value);
        _return(ctx, value);
    } else if (std::string* as_string = std::get_if<std::string>(&val)) {
        uint32_t value = compat_pmm_enum_config_value(key, *as_string, 0);
        compat_pmm_log_config_u32(key, "mod-string", *as_string, value);
        _return(ctx, value);
    } else if (key == "open_menu_buttons") {
        compat_pmm_log_config_u32(key, "fallback", "L + A", 2);
        _return(ctx, uint32_t{2});
    } else if (key == "is_live_preview_enabled") {
        compat_pmm_log_config_u32(key, "fallback", "ON", 1);
        _return(ctx, uint32_t{1});
    } else if (key == "is_modify_tunic_color") {
        compat_pmm_log_config_u32(key, "fallback", "FORCE", 2);
        _return(ctx, uint32_t{2});
    } else if (key == "logging_level") {
        compat_pmm_log_config_u32(key, "fallback", "WARNING", 3);
        _return(ctx, uint32_t{3});
    } else {
        compat_pmm_log_config_u32(key, "fallback", "0", 0);
        _return(ctx, uint32_t{0});
    }
}

extern "C" void recomp_android_compat_pmm_get_config_string(uint8_t* rdram, recomp_context* ctx) {
    std::string key = read_rdram_cstr(rdram, _arg<0, PTR(char)>(rdram, ctx));
    recomp::mods::ConfigValueVariant val = recomp::mods::get_mod_config_value("yazmt_mm_playermodelmanager", key);
    std::string config_text;

    if (compat_pmm_config_text(key, config_text)) {
        compat_pmm_log_config_string(key, "json", config_text);
        return_compat_pmm_string(rdram, ctx, config_text);
    } else if (std::string* as_string = std::get_if<std::string>(&val)) {
        compat_pmm_log_config_string(key, "mod-string", *as_string);
        return_compat_pmm_string(rdram, ctx, *as_string);
    } else if (key == "tunic_color") {
        compat_pmm_log_config_string(key, "fallback", "1E691B");
        return_compat_pmm_string(rdram, ctx, std::string{"1E691B"});
    } else {
        compat_pmm_log_config_string(key, "fallback", "[null]");
        _return(ctx, NULLPTR);
    }
}

extern "C" void recomp_android_compat_pmm_free_config_string(uint8_t* rdram, recomp_context* ctx) {
    gpr str_rdram = _arg<0, PTR(char)>(rdram, ctx);
    if (str_rdram == NULLPTR) {
        return;
    }

    gpr offset = str_rdram - 0xFFFFFFFF80000000ULL;
    recomp::free(rdram, rdram + offset);
}

static uint32_t compat_cheats_default_config_value(const std::string& key) {
    if (key == "blast_mask_cooldown") {
        return 15;
    }

    if (key == "hookshot_length_multiplier") {
        return 1;
    }

    return 0;
}

static uint32_t compat_cheats_enum_config_value(const std::string& value, uint32_t fallback) {
    if (value == "On" || value == "on" || value == "true" || value == "TRUE" || value == "1") {
        return 1;
    }

    if (value == "Off" || value == "off" || value == "false" || value == "FALSE" || value == "0") {
        return 0;
    }

    return fallback;
}

static uint32_t compat_on_off_config_value(const std::string& value, uint32_t fallback) {
    if (value == "On" || value == "on" || value == "true" || value == "TRUE" || value == "1") {
        return 1;
    }

    if (value == "Off" || value == "off" || value == "false" || value == "FALSE" || value == "0") {
        return 0;
    }

    return fallback;
}

extern "C" void recomp_android_compat_cheats_get_config_u32(uint8_t* rdram, recomp_context* ctx) {
    std::string key = read_rdram_cstr(rdram, _arg<0, PTR(char)>(rdram, ctx));
    uint32_t fallback = compat_cheats_default_config_value(key);
    static bool logged_rupees_request = false;

    if (!android_compatibility_build_enabled() || android_n64_mode_enabled()) {
        if (key == "infinite_rupees" && !logged_rupees_request) {
            ZELDA_ANDROID_CHEATS_LOG("key=%s disabled compat=%d n64=%d fallback=%u",
                                     key.c_str(), android_compatibility_build_enabled() ? 1 : 0,
                                     android_n64_mode_enabled() ? 1 : 0, fallback);
            logged_rupees_request = true;
        }
        _return(ctx, fallback);
        return;
    }

    std::string config_text;
    if (compat_cheats_config_text(key, config_text)) {
        uint32_t value = compat_cheats_enum_config_value(config_text, fallback);
        if (key == "infinite_rupees" && !logged_rupees_request) {
            ZELDA_ANDROID_CHEATS_LOG("key=%s source=json text=%s value=%u path=%s",
                                     key.c_str(), config_text.c_str(), value,
                                     compat_cheats_mod_config_path().string().c_str());
            logged_rupees_request = true;
        }
        _return(ctx, value);
        return;
    }

    recomp::mods::ConfigValueVariant val = recomp::mods::get_mod_config_value("ProxyMM_Cheats", key);
    if (uint32_t* as_u32 = std::get_if<uint32_t>(&val)) {
        if (key == "infinite_rupees" && !logged_rupees_request) {
            ZELDA_ANDROID_CHEATS_LOG("key=%s source=mods-u32 value=%u", key.c_str(), *as_u32);
            logged_rupees_request = true;
        }
        _return(ctx, *as_u32);
    }
    else if (double* as_double = std::get_if<double>(&val)) {
        uint32_t value = static_cast<uint32_t>(std::clamp(*as_double, 0.0, 99999.0));
        if (key == "infinite_rupees" && !logged_rupees_request) {
            ZELDA_ANDROID_CHEATS_LOG("key=%s source=mods-double value=%u", key.c_str(), value);
            logged_rupees_request = true;
        }
        _return(ctx, value);
    }
    else if (std::string* as_string = std::get_if<std::string>(&val)) {
        uint32_t value = compat_cheats_enum_config_value(*as_string, fallback);
        if (key == "infinite_rupees" && !logged_rupees_request) {
            ZELDA_ANDROID_CHEATS_LOG("key=%s source=mods-string text=%s value=%u",
                                     key.c_str(), as_string->c_str(), value);
            logged_rupees_request = true;
        }
        _return(ctx, value);
    }
    else {
        if (key == "infinite_rupees" && !logged_rupees_request) {
            ZELDA_ANDROID_CHEATS_LOG("key=%s source=fallback value=%u", key.c_str(), fallback);
            logged_rupees_request = true;
        }
        _return(ctx, fallback);
    }
}

static uint32_t compat_bomb_arrows_default_config_value(const std::string& key) {
    if (key == "enabled") {
        return android_compatibility_build_enabled() && !android_n64_mode_enabled() && zelda64::get_compat_bomb_arrows_enabled();
    }

    if (key == "defuse_on_submerge") {
        return 1;
    }

    return 0;
}

static uint32_t compat_bomb_arrows_enum_index_config_value(uint32_t value, uint32_t fallback) {
    if (value == 0) {
        return 1;
    }

    if (value == 1) {
        return 0;
    }

    return fallback;
}

extern "C" void recomp_android_compat_bomb_arrows_get_config_u32(uint8_t* rdram, recomp_context* ctx) {
    std::string key = read_rdram_cstr(rdram, _arg<0, PTR(char)>(rdram, ctx));
    uint32_t fallback = compat_bomb_arrows_default_config_value(key);
    bool enabled = android_compatibility_build_enabled() && !android_n64_mode_enabled() && zelda64::get_compat_bomb_arrows_enabled();

    if (key == "enabled") {
        _return(ctx, enabled);
        return;
    }

    if (!enabled) {
        _return(ctx, 0);
        return;
    }

    recomp::mods::ConfigValueVariant val = recomp::mods::get_mod_config_value("mm_recomp_bomb_arrows", key);
    if (uint32_t* as_u32 = std::get_if<uint32_t>(&val)) {
        _return(ctx, compat_bomb_arrows_enum_index_config_value(*as_u32, fallback));
    }
    else if (double* as_double = std::get_if<double>(&val)) {
        _return(ctx, compat_bomb_arrows_enum_index_config_value(static_cast<uint32_t>(*as_double), fallback));
    }
    else if (std::string* as_string = std::get_if<std::string>(&val)) {
        _return(ctx, compat_on_off_config_value(*as_string, fallback));
    }
    else {
        _return(ctx, fallback);
    }
}

static uint32_t compat_mco_default_config_value(const std::string& key) {
    if (key == "glyphs") {
        return 2;
    }

    if (key == "_recomp_texture_pack_enabled") {
        return 1;
    }

    if (key == "text_replacement") {
        return 1;
    }

    if (key == "attack_item_scale") {
        return 0;
    }

    if (key == "shoulder_position") {
        return 0;
    }

    if (key == "action_button") {
        return 3;
    }

    if (key == "attack_button") {
        return 2;
    }

    if (key == "1_button" || key == "c_left_button") {
        return 1;
    }

    if (key == "2_button" || key == "c_down_button") {
        return 0;
    }

    if (key == "3_button" || key == "c_right_button") {
        return 4;
    }

    return 0;
}

static uint32_t compat_mco_enum_config_value(const std::string& key, const std::string& value, uint32_t fallback) {
    if (key == "glyphs") {
        if (value == "Disabled") {
            return 0;
        }
        if (value == "C-buttons") {
            return 1;
        }
        if (value == "All") {
            return 2;
        }
    }

    if (key == "_recomp_texture_pack_enabled" || key == "attack_item_scale") {
        return compat_on_off_config_value(value, fallback);
    }

    if (key == "text_replacement") {
        if (value == "Disabled") {
            return 0;
        }
        if (value == "ZL") {
            return 1;
        }
        if (value == "LT") {
            return 2;
        }
        if (value == "L2") {
            return 3;
        }
    }

    if (key == "shoulder_position") {
        if (value == "Lower") {
            return 0;
        }
        if (value == "Normal") {
            return 1;
        }
        if (value == "Higher") {
            return 2;
        }
    }

    if (key == "action_button" || key == "attack_button" || key == "1_button" || key == "2_button" || key == "3_button" ||
        key == "c_left_button" || key == "c_down_button" || key == "c_right_button") {
        if (value == "Up") {
            return 0;
        }
        if (value == "Left") {
            return 1;
        }
        if (value == "Down") {
            return 2;
        }
        if (value == "Right") {
            return 3;
        }
        if (value == "Shoulder") {
            return 4;
        }
    }

    return fallback;
}

static std::string compat_mco_menu_config_key(const std::string& key) {
    if (key == "c_left_button") {
        return "1_button";
    }
    if (key == "c_down_button") {
        return "2_button";
    }
    if (key == "c_right_button") {
        return "3_button";
    }
    return key;
}

extern "C" void recomp_android_compat_mco_enabled(uint8_t* rdram, recomp_context* ctx) {
    _return(ctx, android_compatibility_build_enabled() && !android_n64_mode_enabled());
}

extern "C" void recomp_android_compat_mco_get_config_u32(uint8_t* rdram, recomp_context* ctx) {
    std::string key = read_rdram_cstr(rdram, _arg<0, PTR(char)>(rdram, ctx));
    if (!android_compatibility_build_enabled() || android_n64_mode_enabled()) {
        _return(ctx, compat_mco_default_config_value(key));
        return;
    }

    std::string menu_key = compat_mco_menu_config_key(key);
    uint32_t fallback = compat_mco_default_config_value(menu_key);
    recomp::mods::ConfigValueVariant val = recomp::mods::get_mod_config_value("mm_modern_controller_overhaul", menu_key);

    if (uint32_t* as_u32 = std::get_if<uint32_t>(&val)) {
        _return(ctx, *as_u32);
    }
    else if (double* as_double = std::get_if<double>(&val)) {
        _return(ctx, static_cast<uint32_t>(std::clamp(*as_double, 0.0, 99999.0)));
    }
    else if (std::string* as_string = std::get_if<std::string>(&val)) {
        _return(ctx, compat_mco_enum_config_value(menu_key, *as_string, fallback));
    }
    else {
        _return(ctx, fallback);
    }
}

extern "C" void recomp_android_compat_pmm_apply_log(uint8_t* rdram, recomp_context* ctx) {
    s32 event_type = _arg<0, s32>(rdram, ctx);
    s32 category = _arg<1, s32>(rdram, ctx);
    s32 entry_type = _arg<2, s32>(rdram, ctx);
    s32 result = _arg<3, s32>(rdram, ctx);
    gpr entry_name = _arg<4, u32>(rdram, ctx);
    std::string name = read_rdram_cstr(rdram, entry_name);

    ZELDA_ANDROID_PMM_LOG("apply event=%d category=%d entryType=%d result=%d entry=%s",
                          event_type,
                          category,
                          entry_type,
                          result,
                          name.empty() ? "[None]" : name.c_str());
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
    _return<s32>(ctx, !android_n64_mode_enabled() && zelda64::get_dpad_items_enabled());
}

extern "C" void recomp_get_fd_anywhere_enabled(uint8_t* rdram, recomp_context* ctx) {
    _return<s32>(ctx, !android_n64_mode_enabled() && android_compatibility_build_enabled() && zelda64::get_compat_fd_anywhere_enabled());
}

extern "C" void recomp_get_fast_mask_enabled(uint8_t* rdram, recomp_context* ctx) {
    _return<s32>(ctx, !android_n64_mode_enabled() && android_compatibility_build_enabled() && zelda64::get_compat_fast_mask_enabled());
}

extern "C" void recomp_get_owls_never_quit_enabled(uint8_t* rdram, recomp_context* ctx) {
    _return<s32>(ctx, !android_n64_mode_enabled() && android_compatibility_build_enabled() && zelda64::get_compat_owls_never_quit_enabled());
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

extern "C" void recomp_android_is_n64_mode(uint8_t* rdram, recomp_context* ctx) {
    _return<s32>(ctx, android_n64_mode_enabled());
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
