#include "ui_text_input.h"

#include <algorithm>
#include <cassert>

#if defined(__ANDROID__)
#include <SDL.h>
#endif

namespace recompui {

    void TextInput::process_event(const Event &e) {
        switch (e.type) {
        case EventType::Text: {
            const EventText &event = std::get<EventText>(e.variant);
            std::string sanitized_text = event.text;

            if (numeric_only) {
                sanitized_text.erase(
                    std::remove_if(sanitized_text.begin(), sanitized_text.end(),
                                   [](char c) { return c < '0' || c > '9'; }),
                    sanitized_text.end());
            }
            if (max_length != 0 && sanitized_text.size() > max_length) {
                sanitized_text.resize(max_length);
            }

            if (sanitized_text != event.text) {
                set_text(sanitized_text);
            } else {
                text = sanitized_text;
            }

            for (const auto &function : text_changed_callbacks) {
                function(text);
            }

            break;
        }
        case EventType::Focus: {
            const EventFocus &event = std::get<EventFocus>(e.variant);
#if defined(__ANDROID__)
            if (event.active) {
                SDL_StartTextInput();
            }
            else {
                SDL_StopTextInput();
            }
#endif
            if (focus_callback != nullptr) {
                focus_callback(event.active);
            }
            break;
        }
        default:
            break;
        }
    }
    
    TextInput::TextInput(Element *parent, bool text_visible) : Element(parent, Events(EventType::Text, EventType::Focus), "input") {
        if (!text_visible) {
            set_attribute("type", "password");
        }
        set_min_width(60.0f);
        set_border_color(Color{ 242, 242, 242, 255 });
        set_border_bottom_width(1.0f);
        set_padding_bottom(6.0f);
        set_focusable(true);
        set_nav_auto(NavDirection::Up);
        set_nav_auto(NavDirection::Down);
        set_tab_index_auto();
    }

    void TextInput::set_text(std::string_view text) {
        this->text = std::string(text);
        set_attribute("value", this->text);
    }

    const std::string &TextInput::get_text() {
        return text;
    }

    void TextInput::set_numeric_only(bool numeric_only) {
        this->numeric_only = numeric_only;
        set_attribute("inputmode", numeric_only ? "numeric" : "text");
        if (numeric_only) {
            set_attribute("pattern", "[0-9]*");
        }
    }

    void TextInput::set_max_length(size_t max_length) {
        this->max_length = max_length;
        if (max_length != 0) {
            set_attribute("maxlength", std::to_string(max_length));
        }
    }

    void TextInput::add_text_changed_callback(std::function<void(const std::string &)> callback) {
        text_changed_callbacks.emplace_back(callback);
    }

    void TextInput::set_focus_callback(std::function<void(bool)> callback) {
        focus_callback = callback;
    }
};
