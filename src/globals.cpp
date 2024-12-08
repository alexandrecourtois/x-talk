#include "globals.h"
#include "zmq_server.h"
#include <memory>
#include <thread>
#include <zmq.hpp>
#include <zmq_client.h>
#include <zmq_server.h>

Aircraft aircraft;
std::string ip_address = "127.0.0.1";
bool valid_ip_address = true;
log4cpp::Appender *logfile;
log4cpp::Category *xlog;
std::atomic<bool> ask_airport_update(false);
std::atomic<bool> quit(false);
std::string monitors;
std::atomic<bool> is_monitor_connected(false);
std::string monitor_cmd;
std::atomic<bool> is_monitor_cmd(false);
