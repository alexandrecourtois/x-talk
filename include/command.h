#ifndef COMMAND_H
#define COMMAND_H

#include <string>

enum cmd_ {
    cmd_help,
    cmd_reload_json,
    cmd_reload_phrases,
    cmd_reinit_joystick,
    cmd_reinit_audio,
    cmd_getmem,
    cmd_get_radio
};

#define CMD_HELP            "cmd_help"
#define CMD_RELOAD_JSON     "cmd_reload_json"
#define CMD_RELOAD_PHRASES  "cmd_reload_phrases"
#define CMD_REINIT_JOYSTICK "cmd_reinit_joystick"
#define CMD_REINIT_AUDIO    "cmd_reinit_audio"
#define CMD_GETMEM          "cmd_getmem"
#define CMD_GET_RADIO       "cmd_get_radio"


void init_commands();
bool run_command(std::string cmd);

#endif  // COMMAND_H