#pragma once

#include <pch.h>
#include <enums.h>
#include <zmq_service.h>

class ZMQ_Client: public ZMQ_Service {
    private:
        std::mutex __mutex;
    
        void __thread_loop(std::stop_token token) override;

    public:
        ZMQ_Client(zmq::context_t& context, ServerType type, const Rsp_Dataframe& dataframe = SESSION::dataframe);

        void sendRequest(const Req& request, bool inClientThread = false);
};