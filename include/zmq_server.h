#ifndef ZMQ_SERVER_H
#define ZMQ_SERVER_H

#include <stop_token>
#include <zmq.h>
#include <zmq.hpp>
#include "reqrsp.h"
#include "enums.h"
#include "zmq_service.h"


class ZMQ_Server: public ZMQ_Service {
    private:
        Req             __last_request;

        void __thread_loop(std::stop_token token) override;

    public:
        ZMQ_Server(zmq::context_t& context, ServerType type, const Rsp_Dataframe& dataframe = SESSION::dataframe);
        ~ZMQ_Server() override = default;

        Req getLastRequest();
};

#endif // ZMQ_SERVER_H