#ifndef GLOBALS_H
#define GLOBALS_H

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

extern log4cpp::Appender *logfile;
extern log4cpp::Category *xlog;

#define SIV     " SIV    "
#define AFIS    " AFIS   "

#define LOCK    std::lock_guard<std::recursive_mutex> __lock(mtx)


extern bool quit;
typedef unsigned int        uint;

extern std::map<std::string, Mix_Chunk*> WAV_files;

extern bool recording;

extern zmq::context_t context;
extern zmq::socket_t socket;

extern std::recursive_mutex mtx;
extern uint refresh_rate;

extern const char* vmodel_name;
extern const char* json_file;
extern const char* airports_file;
extern VoskModel* vmodel;
extern VoskRecognizer* vrec;
extern nlohmann::json responses;
extern nlohmann::json* currentNode;
extern nlohmann::json airports;
extern zmq::message_t message;
extern std::thread* updater;
extern int ptt;
extern SDL_Joystick *joystick;
extern SDL_AudioDeviceID audioDeviceID;
extern std::vector<Uint8> audioData;
extern bool noxplane;
extern bool noaudio;
extern bool nojoystick;

extern struct Aircraft {
    std::string tailnum = "NO-XPL";
} aircraft;

extern std::map<std::string, std::string> phrases;

extern std::string speaker;
extern const std::string prefix_cmd;

extern struct Airport {
    std::string ICAO;
    float lat;
    float lon;
    float alt;
    int tower;
    int atis;
    std::map<std::string, int> runways;
    std::map<std::string, std::string> holdpoints;
    std::string active_rwy;
} current_airport;

extern std::string ip_address;
extern bool valid_ip_address;

#endif