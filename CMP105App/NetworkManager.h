#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include "NetworkNode.h"
#include "NetworkListener.h"
#include "SFML/Network.hpp"
#include <vector>


//class BaseNetworkListener;
//
//template <typename ParentType>
//class NetworkListener;

class OnlinePlayer;

class NetworkManager : public NetworkNode
{
public:
	static NetworkManager& getInstance();
	static NetworkManager& getInstance(sf::IpAddress _serverAddress, unsigned short _serverPort);
	~NetworkManager();
	
	template <typename ParentType>
	NetworkListener<ParentType>* GenerateNetworkListener(ParentType& parentReference, int reservedSpot = -1) {
		// Place entity in reserved spot if it's one of the reserved types (e.g. online player)
		if (std::is_same<ParentType, OnlinePlayer>::value) {
			NetworkListener<ParentType>* newListener{ new NetworkListener<ParentType>(parentReference) };
			networkListeners[reservedSpot] = new NetworkListener<ParentType>(parentReference);
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
	void CheckForIncomingDataFromServer() override;
	
	int GetNetworkManagerIndex();

private:
	NetworkManager(sf::IpAddress _serverAddress, unsigned short _serverPort);

	bool connectedToServer;
	int networkManagerIndex;
	
	int reservedEntities; //Number of entities that hold a reserved spot in the networkListeners vector, this is to ensure that, e.g.:, players 1-4 are always in indices 0-3 and in the correct order
	std::vector<BaseNetworkListener*> networkListeners;
};

#endif