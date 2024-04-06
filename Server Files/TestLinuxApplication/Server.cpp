#include "Server.h"
#include <algorithm> //For std::find
#include <iostream>


NetworkManagerInfo::NetworkManagerInfo(sf::IpAddress serverIp, unsigned short serverPort) {
	ip = serverIp;
	port = serverPort;
}


Server::Server(sf::IpAddress _ip, unsigned short _port) {
	serverAddress = _ip;
	serverPort = _port;

	socket.setBlocking(false);

	if (socket.bind(serverPort) != sf::Socket::Done) { std::cerr << "Server failed to bind to port " << serverPort << std::endl; }
	else { std::cout << "Server successfully bound to port " << serverPort << '\n'; }

}


void Server::CheckForIncomingDataFromNetworkManager() {
	sf::Packet incomingData;
	sf::IpAddress incomingNetworkManagerAddress;
	unsigned short incomingNetworkManagerPort;

	if (socket.receive(incomingData, incomingNetworkManagerAddress, incomingNetworkManagerPort) != sf::Socket::Done) { return; }

	NetworkManagerInfo incomingNetworkManager {incomingNetworkManagerAddress, incomingNetworkManagerPort};

	std::cout << '\n';

	std::underlying_type<PacketCode>::type packetCode;
	incomingData >> packetCode;
	switch (packetCode)
	{
	case PacketCode::AddNetworkManager:
	{
		std::cout << "PacketCode: AddNetworkManager\n";
		//Double check that NetworkManager isn't already in vector
		if (std::find(connectedNetworkManagers.begin(), connectedNetworkManagers.end(), incomingNetworkManager) != connectedNetworkManagers.end()) { return; }
		connectedNetworkManagers.push_back(incomingNetworkManager);
		std::cout << "NetworkManager (ip: " << incomingNetworkManager.ip << ", " << incomingNetworkManager.port << ") connected to server.\n";

		//Send NetworkManager index back to NetworkManager
		sf::Packet data;
		data << (int)connectedNetworkManagers.size() - 1;
		if (socket.send(data, incomingNetworkManager.ip, incomingNetworkManager.port) != sf::Socket::Done) { std::cerr << "Failed to send NetworkManager index to NetworkManager." << std::endl; }
		else { std::cerr << "Successfully sent NetworkManager index to NetworkManager.\n"; }

		return;
	}

	case PacketCode::RemoveNetworkManager:
	{
		std::cout << "PacketCode: RemoveNetworkManager\n";
		std::cout << "NetworkManager (ip: " << incomingNetworkManager.ip << ", " << incomingNetworkManager.port << ") disconnected from server.\n";
		connectedNetworkManagers.erase(std::find(connectedNetworkManagers.begin(), connectedNetworkManagers.end(), incomingNetworkManager));
		return;
	}
	case PacketCode::PositionChange:
	{
		std::cout << "PacketCode: PositionChange\n";


		//Separate packet, networkManagerIndex, and networkListenerIndex from incoming data
		int networkManagerIndex;
		int networkListenerIndex;
		sf::Vector2f pos;
		incomingData >> networkManagerIndex >> networkListenerIndex >> pos.x >> pos.y;
		std::cout << pos.x << ' ' << pos.y << '\n';

		//Add networkListenerIndex, packetCode, and data to outgoingData
		sf::Packet outgoingData;
		outgoingData << networkListenerIndex << packetCode << pos.x << pos.y;

		//Validate data (make sure NetworkManager is trying to send data to an ip+port that is in the array and make sure NetworkManager isn't trying to send themselves data. possibly other checks also.)
		if ((networkManagerIndex >= connectedNetworkManagers.size()) || (connectedNetworkManagers[networkManagerIndex] == incomingNetworkManager)) {
			std::cerr << "NetworkManager (ip: " << incomingNetworkManager.ip << ", " << incomingNetworkManager.port << ") tried to send a message to an invalid NetworkManager (ip: "
				      << connectedNetworkManagers[networkManagerIndex].ip << ", " << connectedNetworkManagers[networkManagerIndex].port << ")!" << std::endl;
			return;
		}

		//Send data to NetworkManager
		socket.send(outgoingData, connectedNetworkManagers[networkManagerIndex].ip, connectedNetworkManagers[networkManagerIndex].port);

		return;
	}
	std::cout << "\n\nPacket code uninterpretable.\n\n";
	}
}
