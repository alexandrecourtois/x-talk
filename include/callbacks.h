#pragma once

#include <pch.h>

class CALLBACKS {
    private:
        static bool __is_init;

        static std::map<std::string, std::function<std::string()>> __callbacks_audio;
        static std::map<std::string, std::function<std::string()>> __callbacks_text;

        CALLBACKS() = default;

        static void __init();

        static std::string __get_visibility_AUDIO();
        static std::string __get_visibility_TEXT();
        static std::string __get_qnh();
        static std::string __get_winddir();
        static std::string __get_windspeed();
        static std::string __get_runway();
        static std::string __get_holdpoint();
        static std::string __get_tailnum();

    public:
        enum {
            AUDIO,
            TEXT
        };

        static std::string getValue(int type, const std::string &str);
        static bool hasCallback(int type, const std::string& key);
};