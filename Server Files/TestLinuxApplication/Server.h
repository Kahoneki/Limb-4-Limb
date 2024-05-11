#ifndef SERVER_H
#define SERVER_H

#include "SFML/Network.hpp"
#include <string>
#include <map>

//Server class for managing NetworkManagers
class Server
{

public:

	//From client side's "NetworkManager" class
	//Entities that hold a reserved spot in the networkListeners vector, this is to ensure that, e.g.:, players 1 and 2 are always in indices 0 and 1 and in the correct order
	enum ReservedEntityIndexTable
	{
		PLAYER_1,
		PLAYER_2,

		REGISTRATION_SCREEN,
		LOGIN_SCREEN,
		SEND_INVITE_SCREEN,
		NETWORK_SCENE,

		MATCH_INVITATION_INTERRUPT, //Add a network listener of this type to a class if it should receive match invitations

		NUM_RESERVED_ENTITIES //Size of this enum - must be last
	};

	Server(sf::IpAddress _ip, unsigned short _port);

	//----To be called every network tick//
	void CheckForIncomingConnectionRequests();
	void CheckForIncomingTCPData(float dt);
	void CheckForIncomingUDPData();
	//----//

private:
	sf::IpAddress serverAddress;
	unsigned short serverPort;

	float timeBetweenTimeoutCheckPackets; //Unique to each connected client, every timeBetweenTimeoutCheckPackets seconds, a packet will be sent to the client to ensure they're still online

	sf::TcpListener tcpListener;
	sf::TcpSocket tcpSocket;
	sf::UdpSocket udpSocket;
	std::map<int, sf::TcpSocket> connectedNetworkManagers; //Using a map instead of a vector for the unique property that map[map.size()] will create a new item, this is to work around sockets not being copyable
	std::map<int, unsigned short> connectedUdpPorts; //Network Manager Index + Port that its UDP socket is bound to
	std::map<int, std::string> onlineUsers; //Network Manager Index + Username that it's associated with
	std::map<int, float> timeUntilNextTimeoutCheckPacket; //Network Manager Index + time left until the server sends a packet to the client to make sure they're still online
	std::map<std::string, sf::Int32> onlineUserRankings; //Username + user's ranking
	std::map<int, int> matchedUsers; //NMI + NMI combination of users that are currently in a match - order doesn't matter (if [a,b] is in a map, [b,a] won't be)
	std::map<int, bool> userMatchSceneLoaded; //NMI + true/false whether the user's match scene has loaded (to ensure that both user's scenes have loaded before the match starts)

	std::map<int, sf::TcpSocket>::iterator DisconnectUser(int nmi); //Disconnect client associated with provided nmi and return iterator to previous valid position in connectedNetworkManagers (so that in next iteration of for-loop, ++it will go to the next valid pair)
	bool AccountExists(std::string username); //Searches the database for provided username and returns whether it was found or not
	int GetOpponentNMI(int nmi); //Get nmi of the provided-nmi's opponent
	void AwardMatchWin(int winningNMI); //Updates rankings accordingly and send MatchEnd packet to both clients
};


#endif