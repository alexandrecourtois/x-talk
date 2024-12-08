#include "interp.h"
#include <callbacks.h>
#include <xprint.h>
#include "globals.h"
#include "system.h"
#include <SDL2/SDL_mixer.h>
#include <chrono>
#include <cmath>
#include <sstream>
#include <string>
#include <string_view>
#include <thread>
#include "reqrsp.h"
#include <tools.h>
#include <inputs.h>
#include <session.h>
#include <radio.h>

void CMD::__init() {
    __cmd_list[CMD_HELP]              = cmd_::cmd_help;
    __cmd_list[CMD_RELOAD_JSON]       = cmd_::cmd_reload_json;
    __cmd_list[CMD_RELOAD_PHRASES]    = cmd_::cmd_reload_phrases;
    __cmd_list[CMD_REINIT_JOYSTICK]   = cmd_::cmd_reinit_joystick;
    __cmd_list[CMD_REINIT_AUDIO]      = cmd_::cmd_reinit_audio;
    __cmd_list[CMD_GETMEM]            = cmd_::cmd_getmem;
    __cmd_list[CMD_GET_RADIO]         = cmd_::cmd_get_radio; 
    __cmd_list[CMD_GET_WEATHER]       = cmd_::cmd_get_weather;
    __cmd_list[CMD_GET_POSITION]      = cmd_::cmd_get_position;  
    __cmd_list[CMD_GET_RADIO_RANGE]   = cmd_::cmd_get_radio_range;
    __cmd_list[CMD_GET_RADIO_QUALITY] = cmd_::cmd_get_radio_quality;
    __cmd_list[CMD_MONITOR]           = cmd_::cmd_monitor;

    __isInit = true;
}

int CMD::run(std::string_view cmdline) {
    if (!cmdline.empty()) {
        std::string str;
        std::ostringstream oss;
        std::vector<std::string> args = TOOLBOX::splitString(std::string(cmdline), ' ');
        std::string cmd = args[0];
        args.erase(args.begin());

        if (!__isInit)
            __init();

        if (__cmd_list.find(cmd) != __cmd_list.end()) {
            switch(__cmd_list[cmd]) {
                case cmd_::cmd_help:
                    if (!SESSION::is_monitor) {
                        OUT::xprint(OUT::MSG_STYLE::HELP, CMD_RELOAD_JSON, "Reload JSON tree.");
                        OUT::xprint(OUT::MSG_STYLE::HELP, CMD_RELOAD_PHRASES, "Reload phrase file.");
                        OUT::xprint(OUT::MSG_STYLE::HELP, CMD_REINIT_JOYSTICK, "Reinitialize joystick.");
                        OUT::xprint(OUT::MSG_STYLE::HELP, CMD_REINIT_AUDIO, "Reinitialize audio subsystem.");
                    }

                    OUT::xprint(OUT::MSG_STYLE::HELP, CMD_GETMEM, "Display RAM usage.");
                    OUT::xprint(OUT::MSG_STYLE::HELP, CMD_GET_RADIO, "Display radio parameters.");
                    OUT::xprint(OUT::MSG_STYLE::HELP, CMD_GET_WEATHER, "Display weather known conditions.");
                    OUT::xprint(OUT::MSG_STYLE::HELP, CMD_GET_POSITION, "Display current aircraft position.");
                    OUT::xprint(OUT::MSG_STYLE::HELP, CMD_GET_RADIO_RANGE, "Display radio depending on altitude and weather.");                        OUT::xprint(OUT::MSG_STYLE::HELP, CMD_GET_RADIO_QUALITY, "Display radio and speaker noise volume.");
                    break;

                case cmd_::cmd_get_radio_quality:
                    oss << OUT::xprint(OUT::MSG_STYLE::CMD, "Max volume: ", std::to_string(MIX_MAX_VOLUME))
                        << OUT::xprint(OUT::MSG_STYLE::CMD, "Radio noise volume: ", std::to_string(RADIO::getNoiseVolume()));
                    break;

                case cmd_::cmd_get_radio_range:
                    oss << OUT::xprint(OUT::MSG_STYLE::CMD, "Radio range: ", TOOLBOX::toString(RADIO::getRange(), 2) + " km")
                        << OUT::xprint(OUT::MSG_STYLE::CMD, "In freq airport: ", TOOLBOX::toString(RADIO::getDistanceToAirport(), 2) + " km");
                    break;

                case cmd_::cmd_get_position:
                    SESSION::dataframe.lock();
                        oss << OUT::xprint(OUT::MSG_STYLE::CMD, "Latitude: ", std::to_string(SESSION::dataframe.latitude) + " deg")
                            << OUT::xprint(OUT::MSG_STYLE::CMD, "Longitude: ", std::to_string(SESSION::dataframe.longitude) + " deg")
                            << OUT::xprint(OUT::MSG_STYLE::CMD, "Altitude: ", TOOLBOX::toString(SESSION::dataframe.altitude, 0) + " ft");
                    SESSION::dataframe.unlock();
                    break;

                case cmd_::cmd_get_weather:
                    SESSION::dataframe.lock();
                        oss << OUT::xprint(OUT::MSG_STYLE::CMD, "----------------------")
                            << OUT::xprint(OUT::MSG_STYLE::CMD, "Around AIRCRAFT : ")
                            << OUT::xprint(OUT::MSG_STYLE::CMD, "    Visibility: ",     TOOLBOX::toString(SESSION::dataframe.visibility / 1000.0f, 1) + " km")
                            << OUT::xprint(OUT::MSG_STYLE::CMD, "    Wind direction: ", TOOLBOX::toString(SESSION::dataframe.winddir,0) + " deg")
                            << OUT::xprint(OUT::MSG_STYLE::CMD, "    Wind speed: ",     TOOLBOX::toString(SESSION::dataframe.windspeed, 0) + " kts")
                            << OUT::xprint(OUT::MSG_STYLE::CMD, "    QNH: ",            TOOLBOX::toString(SESSION::dataframe.qnh / 100.0f, 0) + " p")
                            << OUT::xprint(OUT::MSG_STYLE::CMD, "----------------------")
                            << OUT::xprint(OUT::MSG_STYLE::CMD, "In freq AIRPORT : ")
                            << OUT::xprint(OUT::MSG_STYLE::CMD, "    Visibility: ",     TOOLBOX::toString(SESSION::dataframe.infreq_visibility / 1000.0f, 1) + " km")
                            << OUT::xprint(OUT::MSG_STYLE::CMD, "    Wind direction: ", TOOLBOX::toString(SESSION::dataframe.infreq_winddir,0) + " deg")
                            << OUT::xprint(OUT::MSG_STYLE::CMD, "    Wind speed: ",     TOOLBOX::toString(SESSION::dataframe.infreq_windspeed, 0) + " kts")
                            << OUT::xprint(OUT::MSG_STYLE::CMD, "    QNH: ",            TOOLBOX::toString(SESSION::dataframe.infreq_qnh / 100.0f, 0) + " p")
                            << OUT::xprint(OUT::MSG_STYLE::CMD, "----------------------");
                    SESSION::dataframe.unlock();
                    break;
                            
                case cmd_::cmd_get_radio:
                    SESSION::dataframe.lock();
                        if (SESSION::dataframe.com1_active)
                            str = "ON  | ";
                        else
                            str = "OFF | ";
                            
                        str += std::to_string(SESSION::dataframe.com1_freq) + "Hz | Vol. " + std::to_string(static_cast<int>(SESSION::dataframe.com1_vol * 100)) + "%";
                        
                        oss << OUT::xprint(OUT::MSG_STYLE::CMD, "COM1: " + str);
                        
                        if (SESSION::dataframe.com2_active)
                            str = "ON  | ";
                        else
                            str = "OFF | ";
                            
                        str += std::to_string(SESSION::dataframe.com2_freq) + "Hz | Vol. " + std::to_string(static_cast<int>(SESSION::dataframe.com2_vol * 100)) + "%";
                        
                        oss << OUT::xprint(OUT::MSG_STYLE::CMD, "COM2: " + str);
                    SESSION::dataframe.unlock();
                    break;
                            
                case cmd_::cmd_getmem:
                    oss << OUT::xprint(OUT::MSG_STYLE::CMD, "Size in RAM: " + std::to_string(getMemoryUsageInMB()) + "Mb");
                    break;
                            
                case cmd_::cmd_monitor:
                    if (!args.empty())
                        monitor(args);
                    else
                        OUT::xprint(OUT::MSG_STYLE::ERROR, "Missing arguments");

                    break;

                default:
                    if (!SESSION::is_monitor) {
                        switch(__cmd_list[cmd]) {
                            case cmd_::cmd_reload_json:
                            //    init_json();
                                break;
                            
                            case cmd_::cmd_reload_phrases:
                            //    load_phrase_file();
                                break;

                            default:
                                OUT::xprint(OUT::MSG_STYLE::ERROR, "Not yet implemented command");
                                break;
                        }
                    } else
                        OUT::xprint(OUT::MSG_STYLE::ERROR, "Command unavailable in monitor mode or not yet implemented");

                    break;
            }
            
            return TOOLBOX::countLines(oss.str());
        }
        else {
            OUT::xprint(OUT::MSG_STYLE::ERROR, "Unknown command. Type cmd_help to get help");
            return 1;
        }
    } else
        return 0;   
}

void CMD::monitor(const std::vector<std::string>& args) {
    if (!args.empty()) {
        if (!__isInit)
            __init();

        for(const auto& cmd: args)
            if (__cmd_list.find("cmd_get_" + cmd) == __cmd_list.end()) {
                OUT::xprint(OUT::MSG_STYLE::ERROR, "Invalid parameter: " + cmd);
                return;
            }

        if (!is_monitor_connected.load() || SESSION::is_monitor) {
            int allcmds_lines = 0;

            for(const auto& cmd: args) {
                OUT::xprint(OUT::MSG_STYLE::INFO, "--- " + cmd +" ---");
                allcmds_lines += run("cmd_get_" + cmd);
            }

            allcmds_lines += args.size();

            for(int i = 0; i < allcmds_lines; i++)
                std::cout << CLEAN_LINE << std::flush;

            for(int i = 0; i < allcmds_lines; i++)
                std::cout << BACK_LINE << std::flush;

            while(!IN::kbhit()) {
                for(const auto& cmd: args) {
                    OUT::xprint(OUT::MSG_STYLE::INFO, "--- " + cmd +" ---");
                    run("cmd_get_" + cmd);
                }

                for(int i = 0; i < allcmds_lines; i++)
                    std::cout << BACK_LINE << std::flush;

                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }

            for(int i = 0; i < allcmds_lines; i++)
                std::cout << CLEAN_LINE << "\n" << std::flush;

            for(int i = 0; i < allcmds_lines; i++)
                std::cout << BACK_LINE << std::flush;
        } else {
            for(const auto& cmd: args) {
                monitors += (cmd + " ");
            }

            monitors.pop_back();
        }
    }
}

std::map<std::string, CMD::cmd_> CMD::__cmd_list;
bool CMD::__isInit = false;