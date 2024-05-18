#include <absl/flags/flag.h>
#include <absl/flags/parse.h>
#include "server.hpp"

ABSL_FLAG(uint16_t, port, 11451, "Port to run the server on");
ABSL_FLAG(uint32_t, num_threads, 4, "Number of threads to run the server on");

int main(int argc, char **argv) {
    absl::ParseCommandLine(argc, argv);
    ChatServer server(absl::GetFlag(FLAGS_port), absl::GetFlag(FLAGS_num_threads));
    while(std::cin.get() != 'q');
    server.~ChatServer();
    return 0;
}