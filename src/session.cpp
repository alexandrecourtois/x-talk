#include "reqrsp.h"
#include <session.h>

bool SESSION::no_audio;
bool SESSION::no_xplane;
bool SESSION::no_joystick;
bool SESSION::is_monitor;

Rsp_Dataframe SESSION::dataframe;
Rsp_Dataframe SESSION::dataframe_fake {
    Lockable(),
    1000,
    1000,
    "F-SERV",
    10000.0f,
    101300.0f,
    5.0f,
    120.0f,
    0.0f,
    0.0f,
    100.0f,
    118455,
    124400,
    1.0f,
    1.0f,
    1,
    0,
    10000.0f,
    101300.0f,
    5.0f,
    120.0f
};

const std::string SESSION::AFIS = "AFIS";
const std::string SESSION::SIV = "SIV";
std::string SESSION::call_ID = SESSION::AFIS;
std::string SESSION::cmd_prefix = "cmd_";
Speaker SESSION::agent;
ZMQ_Client* SESSION::client;
ZMQ_Client* SESSION::client_monitor;
ZMQ_Server* SESSION::server_fake;
ZMQ_Server* SESSION::server_monitor;
Aircraft SESSION::aircraft;