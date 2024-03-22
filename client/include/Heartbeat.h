#ifndef HEARTBEAT_H
#define HEARTBEAT_H

#include <grpc/grpc.h>
#include <grpcpp/channel.h>

#include "../heartbeat.grpc.pb.h"

using grpc::Channel;
using grpc::ClientContext;

using heartbeat::HeartbeatAPI;
using heartbeat::Beat;

class Heartbeat {
    public:
        std::string id;
        bool open;

        std::thread monitor;

        Heartbeat(
            std::shared_ptr<Channel> channel,
            const std::string id
        );
        ~Heartbeat();

        void start();
        void stop();
    private:
        std::unique_ptr<HeartbeatAPI::Stub> stub_;
};

#endif // HEARTBEAT_H