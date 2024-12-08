#ifndef SESSION_H
#define SESSION_H

#include <reqrsp.h>
#include <speaker.h>
#include <aircraft.h>

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
};

#endif  // SESSION_H