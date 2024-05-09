#include <cstdio>
#include "SFML/Network.hpp"
#include "sqlite3.h"

#include "Server.h"
#include "TimeManager.h"
#include <iostream>

//For generating random player numbers (i.e.: player 1 and player 2)
#include <ctime>
#include <random>

void InitialiseDatabase();

int main()
{
    printf("hello from %s!\n", "TestLinuxApplication");

    srand(time(NULL));

    sf::IpAddress serverIp{ "limbforlimb.duckdns.org" };
    unsigned short serverPort{ 6900 };

    Server* server{ new Server(serverIp, serverPort) };
    TimeManager& timeManager { TimeManager::getInstance(240) };
    InitialiseDatabase();

    while (true) {
        //if (timeManager.UpdateAndCheckNetworkTickStatus()) {
            server->CheckForIncomingConnectionRequests();
            server->CheckForIncomingTCPData();
            server->CheckForIncomingUDPData();
	    //}
    }

    return 0;
}


void InitialiseDatabase() {
    //Open database
    sqlite3* db;
    sqlite3_open("LimbForLimbDatabase.db", &db);

    //Add AccountInfo table if it doesn't already exist
    {
        char* err;
        int returnCode{ sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS AccountInfo(Username varchar(20) NOT NULL, UUID BIGINT UNSIGNED NOT NULL, PRIMARY KEY (Username));", NULL, NULL, &err) };
        if (returnCode != SQLITE_OK) {
            std::cerr << "Error creating account info table: " << err << std::endl;
        }
        else {
            std::cout << "Successfully created account info table\n";
        }
    }

    //Add account ranking table if it doesn't already exist
    {
        sqlite3_exec(db, "PRAGMA foreign_keys = ON;", NULL, NULL, NULL);
        char* err;
        int returnCode{ sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS AccountRanking(Username varchar(20) NOT NULL, Ranking INT NOT NULL, PRIMARY KEY(Username), FOREIGN KEY(Username) REFERENCES AccountInfo(Username));", NULL, NULL, &err) };
        if (returnCode != SQLITE_OK) {
            std::cerr << "Error creating account rankings table: " << err << std::endl;
        }
        else {
            std::cout << "Successfully created account rankings table\n";
        }
    }
    sqlite3_close(db);
}