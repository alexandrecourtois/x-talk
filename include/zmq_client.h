#ifndef ZMQ_CLIENT_H
#define ZMQ_CLIENT_H

#include "enums.h"
#include <stop_token>
#include <zmq.hpp>
#include <zmq_service.h>
#include <reqrsp.h>
#include <mutex>

class ZMQ_Client: public ZMQ_Service {
    private:
        std::mutex __mutex;
    
        void __thread_loop(std::stop_token token) override;

    public:
        ZMQ_Client(zmq::context_t& context, ServerType type, const Rsp_Dataframe& dataframe = SESSION::dataframe);

        void sendRequest(const Req& request, bool inClientThread = false);
};

#endif  // ZMQ_CLIENT_H