#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include "NetworkListener.h"
#include "SFML/Network.hpp"
#include <vector>

class OnlinePlayer;

class NetworkManager
{
public:


	//Entities that hold a reserved spot in the networkListeners vector, this is to ensure that, e.g.:, players 1 and 2 are always in indices 0 and 1 and in the correct order
	enum ReservedEntityIndexTable
	{
		PLAYER_1,
		PLAYER_2,
		
		REGISTRATION_SCREEN,
		LOGIN_SCREEN,
		SEND_INVITE_SCREEN,

		MATCH_INVITATION_INTERRUPT, //Add a network listener of this type to a class if it should receive match invitations

		NUM_RESERVED_ENTITIES //Size of this enum - must be last
	};


	static NetworkManager& getInstance(bool attemptConnection);
	static NetworkManager& getInstance(bool attemptConnection, sf::IpAddress _serverAddress, unsigned short _serverPort);
	~NetworkManager();
	bool AttemptToConnectToServer(); //Returns whether connection attempt was successful or not
	bool AttemptToDisconnectFromServer(); //Returns whether disconnection attempt was successful or not
	
	template <typename ParentType>
	NetworkListener<ParentType>* GenerateNetworkListener(ParentType& parentReference, int reservedSpot = -1) {
		// Place entity in reserved spot if reservedSpot param provided
		if (reservedSpot != -1) {
			//Reserved spot param provided
			NetworkListener<ParentType>* newListener{ new NetworkListener<ParentType>(parentReference) };
			networkListeners[reservedSpot] = newListener;
			return newListener;
		}
		else {
			//Reserved spot param not provided
			NetworkListener<ParentType>* newListener{ new NetworkListener<ParentType>(parentReference) };
			networkListeners.push_back(newListener);
			return newListener;
		}
	}

	void SendDataToNetworkManager(int outgoingNetworkManagerIndex, int networkListenerIndex, PacketCode packetCode, sf::Packet incomingPacket);
	void SendDataToNetworkManager(int networkListenerIndex, PacketCode packetCode, sf::Packet incomingPacket);
	void SendDataToServer(int networkListenerIndex, PacketCode packetCode, sf::Packet incomingPacket);
	void CheckForIncomingDataFromServer();
	
	int getNetworkManagerIndex();
	bool getConnectedToServer();

	void sendNums(); //Debug - sends 1000 udp packets


private:
	NetworkManager(sf::IpAddress _serverAddress, unsigned short _serverPort);

	sf::TcpSocket tcpSocket;
	sf::UdpSocket udpSocket;

	sf::IpAddress serverAddress;
	unsigned short serverPort;

	bool connectedToServer;
	int networkManagerIndex;
	
	std::vector<BaseNetworkListener*> networkListeners;
};

#endif