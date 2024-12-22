#pragma once

#include <map>
#include <pch.h>
#include <aircraft.h>
#include <reqrsp.h>
#include <speaker.h>

class ZMQ_Client;
class ZMQ_Server;

class SESSION {
    private:
        SESSION() = default;

    public:
        static const std::string AFIS;
        static const std::string SIV;

        static bool no_audio;
        static bool no_xplane;
        static bool no_joystick;
        static bool is_monitor;

        static std::string ip_address;

        static Rsp_Dataframe dataframe;
        static Rsp_Dataframe dataframe_fake;

        static std::string call_ID;
        static std::string cmd_prefix;

        static Speaker agent;

        static ZMQ_Client* client;
        static ZMQ_Client* client_monitor;
        static ZMQ_Server* server_fake;
        static ZMQ_Server* server_monitor;

        static Aircraft aircraft;

        static std::map<std::string, std::pair<VoskModel*, int>> vosk_Models;
        static std::map<std::string, VoskRecognizer*> vosk_Recognizers;

        static bool show_keywords;
        static bool show_readback;

        static int levenshtein_threshold;
};