#include "enums.h"
#include <xprint.h>
#include "zmq_service.h"
#include <asm-generic/errno.h>
#include <chrono>
#include <mutex>
#include <stop_token>
#include <thread>
#include <zmq.hpp>
#include <zmq_client.h>
#include <reqrsp.h>

ZMQ_Client::ZMQ_Client(zmq::context_t& context, ServerType type, const Rsp_Dataframe& dataframe): ZMQ_Service(context, ServiceType::CLIENT, type, dataframe) {
    OUT::xprint(OUT::MSG_STYLE::INIT, "Creating client");
    this->getSocket().connect(type.getAddress());
    OUT::xprint(OUT::MSG_STYLE::DONE, type.getAddress());
}

void ZMQ_Client::__thread_loop(std::stop_token token) {
    while(!token.stop_requested()) {
        zmq::message_t msg_req(sizeof(Req));
        Req req;
        req.type = Request::GET_DATAFRAME;
        this->sendRequest(req, true);

        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
}

void ZMQ_Client::sendRequest(const Req& request, bool inClientThread) {
    std::scoped_lock<std::mutex> lock(this->__mutex);
    zmq::socket_t* l_socket;

    if (!inClientThread) {
        l_socket = new zmq::socket_t(ZMQ_Service::getContext(), ZMQ_REQ);
        l_socket->connect(this->getTarget().getAddress());
    } else
        l_socket = &this->getSocket();

    zmq::message_t msg_req(sizeof(Req));
    zmq::message_t msg_rep;

    memcpy(msg_req.data(), &request, sizeof(Req));

    l_socket->send(msg_req, zmq::send_flags::none);

    switch(request.type) {
        case Request::GET_DATAFRAME:   
            msg_rep = zmq::message_t(sizeof(Rsp_Dataframe));

            if (l_socket->recv(msg_rep, zmq::recv_flags::none)) {
                const_cast<Rsp_Dataframe&>(this->getDataframe()).lock();
                memcpy(&const_cast<Rsp_Dataframe&>(this->getDataframe()), msg_rep.data(), sizeof(Rsp_Dataframe));
                const_cast<Rsp_Dataframe&>(this->getDataframe()).unlock();
            }
            break;

        case Request::UPD_AIRPORT:
            msg_rep = zmq::message_t(sizeof(Rsp<>));
            l_socket->recv(msg_rep, zmq::recv_flags::none);
            break;

        default:
            break;
    }

    if (!inClientThread)
        delete l_socket;
}