#ifndef ENUMS_H
#define ENUMS_H

#include <string>
#include <string_view>

enum class ServiceType {
    SERVER,
    CLIENT
};

class ServerType {
    public:
        enum class Protocol {
            TCP,
            IPC,
            INPROC
        };

    private:
        Protocol __protocol;
        std::string __address;

    public:
        const std::string& getAddress() const {
            return __address;
        }

        const Protocol& getProtocol() const {
            return __protocol;
        }

        ServerType(const ServerType& type): __protocol(type.__protocol), __address(type.__address) { }
        
    protected:
        ServerType(Protocol protocol, std::string_view address): __protocol(protocol), __address(address) { }
};

class TCP_Server: public ServerType {
    public:
        explicit TCP_Server(std::string_view address): ServerType(Protocol::TCP, "tcp://" + std::string(address)) { }
};

class IPC_Server: public ServerType {
    public:
        explicit IPC_Server(std::string_view address): ServerType(Protocol::IPC, "ipc:///tmp/" + std::string(address)) { }
};

class INPROC_Server: public ServerType {
    public:
        explicit INPROC_Server(std::string_view address): ServerType(Protocol::INPROC, "inproc://" + std::string(address)) { }
};

enum class Request {
    GET_DATAFRAME,
    UPD_AIRPORT,
    UPD_MONITOR
};

#endif  // ENUMS_H