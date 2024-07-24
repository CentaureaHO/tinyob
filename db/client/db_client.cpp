#include "communicator/client.h"

int main()
{
    ClientConfig config("clientconfig.json");

    Client client(config);
    client.run();

    return 0;
}
