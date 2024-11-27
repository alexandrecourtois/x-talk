#include <zmq.hpp>
#include "reqrsp.h"
#include <thread>
#include "globals.h"

Rsp_Dataframe fake_dataframe {
    1000,
    1000,
    "F-SERV",
    8.0f,
    101300.0f,
    6.0f,
    185.0f,
    0.0f,
    0.0f,
    400.0f,
    118455,
    124400,
    1.0f,
    0.0f,
    1,
    0,
    10.0f,
    101300.0f,
    5.5f,
    182.0f
};

void fake_server_thread(/*zmq::context_t& context, zmq::socket_t& socket*/) {
    zmq::socket_t socket(context, ZMQ_REP);

    socket.bind("inproc://fake_server");
    zmq::pollitem_t items[] = { socket, 0, ZMQ_POLLIN };
    
    while(!quit.load()) {
        bool req_received = false;
        zmq::poll(items, 1, std::chrono::milliseconds(1000));
        Req rc;

        if (items[0].revents && ZMQ_POLLIN) {
            zmq::message_t msg_req(sizeof(Req));
            socket.recv(msg_req, zmq::recv_flags::none); 
            memcpy(&rc, msg_req.data(), sizeof(Req));
            req_received = true;
        }

        if (req_received) {
            zmq::message_t msg_rep;

            if (rc.type == REQ_GET_DATAFRAME) {
                msg_rep = zmq::message_t(sizeof(Rsp_Dataframe));
                //data_mutex.lock();
                memcpy(msg_rep.data(), &fake_dataframe, sizeof(Rsp_Dataframe));
                //data_mutex.unlock();
            }

            if (rc.type == REQ_UPD_AIRPORT) {
                msg_rep = zmq::message_t(sizeof(Rsp<>));
                Rsp<> rsp{RSP_ACK};
                memcpy(msg_rep.data(), &rsp, sizeof(Rsp<>));
                //update_airport.store(true);
            }

            socket.send(msg_rep, zmq::send_flags::none);
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
}