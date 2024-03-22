#include <string>
#include <thread>

#include <grpcpp/create_channel.h>
#include <grpcpp/security/credentials.h>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

#include "absl/flags/flag.h"
#include "absl/flags/parse.h"

#include "include/Heartbeat.h"

ABSL_FLAG(std::string, host, "localhost:3000", "GRPC host");

int main(int argc, char* argv[]) {
    absl::ParseCommandLine(argc, argv);

    std::string host = absl::GetFlag(FLAGS_host);
    auto id = boost::uuids::random_generator()();

    Heartbeat client(
        grpc::CreateChannel(host, grpc::InsecureChannelCredentials()),
        to_string(id)
    );

    client.start();
    while (client.open) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    return 0;
}