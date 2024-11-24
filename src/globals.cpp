#include "globals.h"

zmq::context_t context(1);
zmq::socket_t socket(context, ZMQ_PAIR);

std::recursive_mutex mtx;
uint refresh_rate = 1000;

bool quit = false;
bool recording = false;
const char* vmodel_name = "vosk-model-fr-0.6-linto-2.2.0";
const char* json_file = "responses.json";
const char* airports_file = "airports.json";
VoskModel* vmodel;
VoskRecognizer* vrec;
nlohmann::json responses;
nlohmann::json airports;
nlohmann::json* currentNode;
zmq::message_t message;
std::thread* updater;
int ptt;
SDL_Joystick *joystick;
SDL_AudioDeviceID audioDeviceID;
std::vector<Uint8> audioData;
bool noxplane = false;
bool noaudio = false;
bool nojoystick = false;
Aircraft aircraft;
std::map<std::string, Mix_Chunk*> WAV_files;
std::map<std::string, std::string> phrases;
std::string speaker = AFIS;
const std::string prefix_cmd = "cmd_";
Airport current_airport;
std::string ip_address = "127.0.0.1";
bool valid_ip_address = true;
log4cpp::Appender *logfile;
log4cpp::Category *xlog;