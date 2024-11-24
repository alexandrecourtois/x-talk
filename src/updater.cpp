#include "updater.h"
#include "dataframe.h"
#include "airport.h"
#include "xprint.h"
#include "globals.h"
#include <iostream>

int old_com1_freq = dataframe.com1_freq;
int old_com2_freq = dataframe.com2_freq;
int old_com1_status = dataframe.com1_active;
int old_com2_status = dataframe.com2_active;

void update_radio_status() {
    LOCK;
    
    if ((old_com1_freq != dataframe.com1_freq) && dataframe.com1_active) {
        std::string airport = get_airport_by_frequency(dataframe.com1_freq);
        
        if (airport != UNSUPPORTED_AIRPORT) {
            xprint(M_CLEAN);
            xprint(M_INFO, "Airport frequency selected on COM1: " + airport);
            xprint(M_INVITE);
        }

        old_com1_freq = dataframe.com1_freq;
    }
    
    if ((old_com2_freq != dataframe.com2_freq) && dataframe.com2_active) {
        std::string airport = get_airport_by_frequency(dataframe.com2_freq);
        
        if (airport != UNSUPPORTED_AIRPORT) {
            xprint(M_CLEAN);
            xprint(M_INFO, "Airport frequency selected on COM2: " + airport);
            xprint(M_INVITE);
        }

        old_com2_freq = dataframe.com2_freq;
    }

    if (old_com1_status != dataframe.com1_active && dataframe.com1_active && !dataframe.com2_active) {
        update_current_airport();
    }
    
    if (old_com2_status != dataframe.com2_active && dataframe.com2_active && !dataframe.com1_active) {
        update_current_airport();
    }
    
    old_com1_status = dataframe.com1_active;
    old_com2_status = dataframe.com2_active;
}

void update_dataframe() {
    while(!quit) {
        zmq::message_t message_r(sizeof(dataframe));
        zmq::message_t message_s(sizeof(selected_airport_location));

        if (socket.recv(message_r, zmq::recv_flags::dontwait))
        {
            LOCK;
            memcpy(&dataframe, message_r.data(), sizeof(dataframe));
        }

        {
            LOCK;
            selected_airport_location.latitude  = current_airport.lat;
            selected_airport_location.longitude = current_airport.lon;
            selected_airport_location.altitude  = current_airport.alt;

            if (selected_airport_location != old_airport_location) {
                memcpy(message_s.data(), &selected_airport_location, sizeof(selected_airport_location));
                old_airport_location = selected_airport_location;

                socket.send(message_s, zmq::send_flags::dontwait);
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

void update() {
    //update_dataframe();
    update_radio_status();
}