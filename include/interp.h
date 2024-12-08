#ifndef INTERP_H
#define INTERP_H

#include <string>
#include <string_view>
#include <vector>
#include <map>

#define CMD_HELP                "cmd_help"
#define CMD_RELOAD_JSON         "cmd_reload_json"
#define CMD_RELOAD_PHRASES      "cmd_reload_phrases"
#define CMD_REINIT_JOYSTICK     "cmd_reinit_joystick"
#define CMD_REINIT_AUDIO        "cmd_reinit_audio"
#define CMD_GETMEM              "cmd_getmem"
#define CMD_GET_RADIO           "cmd_get_radio"
#define CMD_GET_WEATHER         "cmd_get_weather"
#define CMD_GET_POSITION        "cmd_get_position"
#define CMD_GET_RADIO_RANGE     "cmd_get_radio_range"
#define CMD_GET_RADIO_QUALITY   "cmd_get_radio_quality"
#define CMD_MONITOR             "cmd_monitor"

class CMD {
    private:
        enum class cmd_ {
            cmd_help,
            cmd_reload_json,
            cmd_reload_phrases,
            cmd_reinit_joystick,
            cmd_reinit_audio,
            cmd_getmem,
            cmd_get_radio,
            cmd_get_weather,
            cmd_get_position,
            cmd_get_radio_range,
            cmd_get_radio_quality,
            cmd_monitor
        };

        static std::map<std::string, cmd_> __cmd_list;
        static bool __isInit;

        CMD() = default;

        static void __init();

    public:
        static int run(std::string_view cmd);
        static void monitor(const std::vector<std::string>& args);
};

#endif  // INTERP_H