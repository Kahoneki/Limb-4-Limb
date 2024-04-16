#include <cstdio>
#include "SFML/Network.hpp"

#include "Server.h"
#include "TimeManager.h"

int main()
{
    printf("hello from %s!\n", "TestLinuxApplication");


    sf::IpAddress serverIp{ "limbforlimb.duckdns.org" };
    unsigned short serverPort{ 6900 };

    Server* server;
    server = new Server(serverIp, serverPort);
    TimeManager& timeManager = TimeManager::getInstance(240);

    while (true) {
        if (timeManager.UpdateAndCheckNetworkTickStatus()) {
            server->CheckForIncomingConnectionRequests();
            server->CheckForIncomingDataFromNetworkManager();
        }
    }

    return 0;
}
