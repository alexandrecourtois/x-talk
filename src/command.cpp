#include "command.h"
#include "xprint.h"
#include "dataframe.h"
#include "globals.h"
#include "system.h"
#include "init.h"

std::map<std::string, cmd_> cmd_list;

void init_commands() {
    cmd_list[CMD_HELP]              = cmd_help;
    cmd_list[CMD_RELOAD_JSON]       = cmd_reload_json;
    cmd_list[CMD_RELOAD_PHRASES]    = cmd_reload_phrases;
    cmd_list[CMD_REINIT_JOYSTICK]   = cmd_reinit_joystick;
    cmd_list[CMD_REINIT_AUDIO]      = cmd_reinit_audio;
    cmd_list[CMD_GETMEM]            = cmd_getmem;
    cmd_list[CMD_GET_RADIO]         = cmd_get_radio;    
}

bool run_command(std::string cmd) {
    LOCK;
    std::string str;
    
    if (cmd_list.find(cmd) != cmd_list.end()) {
        switch(cmd_list[cmd]) {
            case cmd_help:
                xprint(M_HELP, CMD_RELOAD_JSON, "Reload JSON tree.");
                xprint(M_HELP, CMD_RELOAD_PHRASES, "Reload phrase file.");
                xprint(M_HELP, CMD_REINIT_JOYSTICK, "Reinitialize joystick.");
                xprint(M_HELP, CMD_REINIT_AUDIO, "Reinitialize audio subsystem.");
                xprint(M_HELP, CMD_GETMEM, "Display RAM usage.");
                xprint(M_HELP, CMD_GET_RADIO, "Display radio parameters.");
                break;
                        
            case cmd_get_radio:
                if (dataframe.com1_active)
                    str = "ON  | ";
                else
                    str = "OFF | ";
                    
                str += std::to_string(dataframe.com1_freq) + "Hz | Vol. " + std::to_string(static_cast<int>(dataframe.com1_vol * 100)) + "%";
                
                xprint(M_INFO, "COM1: " + str);
                
                if (dataframe.com2_active)
                    str = "ON  | ";
                else
                    str = "OFF | ";
                    
                str += std::to_string(dataframe.com2_freq) + "Hz | Vol. " + std::to_string(static_cast<int>(dataframe.com2_vol * 100)) + "%";
                
                xprint(M_INFO, "COM2: " + str);
                break;
                        
            case cmd_getmem:
                xprint(M_INFO, "Size in RAM: " + std::to_string(getMemoryUsageInMB()) + "Mb");
                break;
                        
            case cmd_reload_json:
                init_json();
                break;
                        
            case cmd_reload_phrases:
                load_phrase_file();
                break;
                        
            default:
                xprint(M_ERROR, "Not yet implemented command");
                break;
        }
        
        return true;
    }
    else {
        xprint(M_ERROR, "Unknown command. Type cmd_help to get help");
        return false;
    }   
}