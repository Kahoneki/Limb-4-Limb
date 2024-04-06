#include <cstdio>
#include "SFML/Network.hpp"

#include "Server.h"

int main()
{
    printf("hello from %s!\n", "TestLinuxApplication");


    sf::IpAddress serverIp{ "limbforlimb.duckdns.org" };
    unsigned short serverPort{ 6900 };

    NetworkNode* node;
    node = new Server(serverIp, serverPort);

    while (true) {
        node->CheckForIncomingDataFromNetworkManager();
    }


    return 0;
}
