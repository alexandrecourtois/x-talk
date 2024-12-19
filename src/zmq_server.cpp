#include <zmq_server.h>
#include <xprint.h>
#include <lang.h>

ZMQ_Server::ZMQ_Server(zmq::context_t& context, ServerType type, const Rsp_Dataframe& dataframe): ZMQ_Service(context, ServiceType::SERVER, type, dataframe) {
    OUT::xprint(MSG_STYLE::INIT, lang(MSG::CREATING_SERVER));
    this->getSocket().bind(type.getAddress());
    OUT::xprint(MSG_STYLE::DONE, type.getAddress());
}

void ZMQ_Server::__thread_loop(std::stop_token token) {
    std::vector<zmq::pollitem_t> items(1);

    items[0].socket = this->getSocket();
    items[0].events = ZMQ_POLLIN;

    while(!token.stop_requested()) {
         zmq::poll(&items[0], 1, std::chrono::milliseconds(1000));

        if (items[0].revents && ZMQ_POLLIN) {
            zmq::message_t msg_req(sizeof(Req));
            if (this->getSocket().recv(msg_req, zmq::recv_flags::none)) {
                memcpy(&this->__last_request, msg_req.data(), sizeof(Req));
            }

            zmq::message_t msg_rep;

            if (this->__last_request.type == Request::GET_DATAFRAME) {
                msg_rep = zmq::message_t(sizeof(Rsp_Dataframe));
                const_cast<Rsp_Dataframe&>(this->getDataframe()).lock();
                memcpy(msg_rep.data(), &this->getDataframe(), sizeof(Rsp_Dataframe));
                const_cast<Rsp_Dataframe&>(this->getDataframe()).unlock();
            }

            if (this->__last_request.type == Request::UPD_AIRPORT) {
                msg_rep = zmq::message_t(sizeof(Rsp<>));
                Rsp<> rsp;
                rsp.value = RSP_ACK;
                memcpy(msg_rep.data(), &rsp, sizeof(Rsp<>));
            }

            this->getSocket().send(msg_rep, zmq::send_flags::none);
        }

        //std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
}

Req ZMQ_Server::getLastRequest() {
    Req request;
    this->__last_request.lock();
    request = Req(this->__last_request);
    this->__last_request.unlock();

    return request;
}