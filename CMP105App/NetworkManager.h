#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include "NetworkNode.h"
#include "NetworkListener.h"
#include "SFML/Network.hpp"
#include <vector>

class OnlinePlayer;

class NetworkManager : public NetworkNode
{
public:


	//Entities that hold a reserved spot in the networkListeners vector, this is to ensure that, e.g.:, players 1 and 2 are always in indices 0 and 1 and in the correct order
	static const enum ReservedEntityIndexTable
	{
		PLAYER_1,
		PLAYER_2,
		REGISTRATION_SCREEN,
		LOGIN_SCREEN,
		NUM_RESERVED_ENTITIES //Size of this enum - must be last
	};


	static NetworkManager& getInstance();
	static NetworkManager& getInstance(sf::IpAddress _serverAddress, unsigned short _serverPort);
	~NetworkManager();
	
	template <typename ParentType>
	NetworkListener<ParentType>* GenerateNetworkListener(ParentType& parentReference, int reservedSpot = -1) {
		// Place entity in reserved spot if it's one of the reserved types (e.g. online player)
		if (std::is_same<ParentType, OnlinePlayer>::value) {
			NetworkListener<ParentType>* newListener{ new NetworkListener<ParentType>(parentReference) };
			networkListeners[reservedSpot] = newListener;
			return newListener;
		}
		else if (std::is_same<ParentType, RegistrationScreen>::value) {
			NetworkListener<ParentType>* newListener{ new NetworkListener<ParentType>(parentReference) };
			networkListeners[reservedSpot] = newListener;
			return newListener;
		}
		else {
			NetworkListener<ParentType>* newListener{ new NetworkListener<ParentType>(parentReference) };
			networkListeners.push_back(newListener);
			return newListener;
		}
	}

	sf::Socket::Status SendDataToNetworkManager(int outgoingNetworkManagerIndex, int networkListenerIndex, PacketCode packetCode, sf::Packet incomingPacket);
	sf::Socket::Status SendDataToNetworkManager(int networkListenerIndex, PacketCode packetCode, sf::Packet incomingPacket);
	sf::Socket::Status SendDataToServer(int networkListenerIndex, PacketCode packetCode, sf::Packet incomingPacket);
	void CheckForIncomingDataFromServer() override;
	
	int getNetworkManagerIndex();
	bool getConnectedToServer();

	void sendNums(); //Debug - for determining packet loss


private:
	NetworkManager(sf::IpAddress _serverAddress, unsigned short _serverPort);

	bool connectedToServer;
	int networkManagerIndex;
	
	std::vector<BaseNetworkListener*> networkListeners;
};

#endif