#include <stdlib.h>
#include <engine/server.h>

int main(int argc, char** argv) {
    IRC::Engine::Server server;

    if (argc > 1) {
        server.setPort(atoi(argv[1]));
    }
    if (argc > 2) {
        server.setPassword(argv[2]);
    }

    if (server.setup() != 0) {
        return 1;
    }

    return server.run();
}
