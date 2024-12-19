#pragma once

#include <pch.h>
#include <enums.h>
#include <reqrsp.h>
#include <session.h>

class ZMQ_Service {
    private:
        zmq::context_t&         __context;
        zmq::socket_t           __socket;
        std::jthread            __svc_thread;
        ServerType              __target;
        const Rsp_Dataframe&    __dataframe;

        virtual void __thread_loop(std::stop_token token) { }

    protected:
        ZMQ_Service(zmq::context_t& context, ServiceType svc_type, const ServerType& srv_type, const Rsp_Dataframe& dataframe = SESSION::dataframe):
            __context(context),
            __target(srv_type),
            __dataframe(dataframe)
        {
            if (svc_type == ServiceType::SERVER)
                this->__socket = zmq::socket_t(__context, ZMQ_REP);

            if (svc_type == ServiceType::CLIENT)
                this->__socket = zmq::socket_t(__context, ZMQ_REQ);
        }

        zmq::context_t& getContext() {
            return this->__context;
        }

        zmq::socket_t& getSocket() {
            return this->__socket;
        }

        const Rsp_Dataframe& getDataframe() const {
            return this->__dataframe;
        }

        const ServerType& getTarget() const {
            return this->__target;
        }

    public:
        virtual ~ZMQ_Service() {
            this->stop();
            this->__socket.close();
        }

        void start() {
            this->__svc_thread = std::jthread([this](std::stop_token token) { __thread_loop(token); });
        }

        void stop() {
            this->__svc_thread.request_stop();
            
            if (__svc_thread.joinable())
                __svc_thread.join();
        }
};