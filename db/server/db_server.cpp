#include "communicator/server.h"

int main()
{
    ServerConfig config("serverconfig.json");

    Server server(config);
    server.run();

    return 0;
}
