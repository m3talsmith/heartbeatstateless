#include <chrono>
#include <ctime>
#include <iostream>
#include <memory>
#include <string>
#include <thread>

#include <grpc/grpc.h>
#include <grpcpp/channel.h>
#include <grpcpp/client_context.h>
#include <grpcpp/create_channel.h>
#include <grpcpp/security/credentials.h>

#include "heartbeat.grpc.pb.h"
#include "include/Heartbeat.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::ClientWriter;
using grpc::Status;
using heartbeat::HeartbeatAPI;
using heartbeat::Beat;

uint64_t get_timestamp() {
    std::chrono::time_point now = std::chrono::system_clock::now();
    uint64_t count = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();
    return count;
}

Heartbeat::Heartbeat(
    std::shared_ptr<Channel> channel,
    const std::string id
) : id(id), stub_(HeartbeatAPI::NewStub(channel)) {}

Heartbeat::~Heartbeat() {
    stop();
}

void Heartbeat::start() {
    printf("starting heartbeats...");

    open = true;
    monitor = std::thread([this] {
        while (open) {
            ::google::protobuf::Empty reply;
            ClientContext context;
            Beat beat;
            beat.set_id(id);
            beat.set_timestamp(get_timestamp());
            auto status = stub_->Send(&context, beat, &reply);

            if (!status.ok()) {
                std::cout << "error sending: " << status.error_message() << std::endl;
                open = false;
            }

            std::this_thread::sleep_for(
                std::chrono::milliseconds(1000)
            );
        }
    });

    printf("done\n");
}

void Heartbeat::stop() {
    printf("stopping heartbeats...");
    open = false;
    if (monitor.joinable()) monitor.join();
    printf("done\n");
}
