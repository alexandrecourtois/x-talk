#ifndef GLOBALS_H
#define GLOBALS_H

#include "reqrsp.h"
#include <memory>
#include <vosk_api.h>
#include <nlohmann/json.hpp>
#include <zmq.hpp>
#include <mutex>
#include <thread>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <map>
#include <string>
#include <log4cpp/Category.hh>
#include <log4cpp/FileAppender.hh>
#include <log4cpp/OstreamAppender.hh>
#include <log4cpp/PatternLayout.hh>
#include <log4cpp/Priority.hh>
#include <atomic>

extern log4cpp::Appender *logfile;
extern log4cpp::Category *xlog;

extern std::string ip_address;
extern std::string monitors;
extern std::atomic<bool> is_monitor_connected;
extern std::string monitor_cmd;
extern std::atomic<bool> is_monitor_cmd;

#endif