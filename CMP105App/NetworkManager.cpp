#include "NetworkManager.h"
#include <iostream>

NetworkManager::NetworkManager(sf::IpAddress _serverAddress, unsigned short _serverPort) {
	serverAddress = _serverAddress;
	serverPort = _serverPort;

	socket.setBlocking(true);

	//Send test packet to server to be added to server's list of connected NetworkManagers
	sf::Packet packet;
	packet << PacketCode::AddNetworkManager;
	if (socket.send(packet, serverAddress, serverPort) != sf::Socket::Done) {
		std::cerr << "Failed to send connection request to server." << std::endl;
	}
	
	//Get NetworkManager index from server
	sf::IpAddress tempIp;
	unsigned short tempPort;
	sf::Packet data;
	if (socket.receive(data, tempIp, tempPort) != sf::Socket::Done) {
		connectedToServer = false;
		std::cerr << "Failed to connect to server." << std::endl;
	}
	else {
		std::cout << "Successfully connected to server.\n";
		data >> NetworkManagerIndex;
		connectedToServer = true;
	}

	socket.setBlocking(false);
}


NetworkManager::~NetworkManager() {
	//Send message to server to tell it to remove NetworkManager from vector
	socket.setBlocking(true);
	sf::Packet outgoingPacket;
	outgoingPacket << static_cast<std::underlying_type<PacketCode>::type>(PacketCode::RemoveNetworkManager);
	if (socket.send(outgoingPacket, serverAddress, serverPort) != sf::Socket::Done) {
		std::cerr << "Failed to disconnect from server." << std::endl;
	}
	else {
		std::cout << "Successfully disconnected from server.\n";
	}
}


sf::Socket::Status NetworkManager::SendDataToNetworkManager(int outgoingNetworkManagerIndex, PacketCode code, sf::Packet incomingPacket) {
	//Combine NetworkManager index and packet code into the data packet so it can be sent to the server
	sf::Packet outgoingPacket;
	outgoingPacket << static_cast<std::underlying_type<PacketCode>::type>(code) << outgoingNetworkManagerIndex;
	outgoingPacket.append(incomingPacket.getData(), incomingPacket.getDataSize());
	return socket.send(outgoingPacket, serverAddress, serverPort);
}


sf::Socket::Status NetworkManager::SendDataToNetworkManager(PacketCode code, sf::Packet incomingPacket) {
	//This function can be called if there are only two NetworkManagers connected to the server (in which case it will just send data to the other NetworkManager).
	int outgoingNetworkManagerIndex = 1 - NetworkManagerIndex; //0->1, 1->0
	return SendDataToNetworkManager(outgoingNetworkManagerIndex, code, incomingPacket);
}


//To be called every frame
sf::Packet NetworkManager::CheckForIncomingDataFromServer() {
	sf::Packet incomingData;

	//Used to check that NetworkManager is in fact receiving data from the server and not, another NetworkManager, for example
	sf::IpAddress incomingIp;
	unsigned short incomingPort;

	//Extract data and check if it's empty
	if (socket.receive(incomingData, incomingIp, incomingPort) != sf::Socket::Done) { return sf::Packet(); }

	//Ensuring data is coming from server and not another NetworkManager
	if ((serverAddress != incomingIp) || (incomingPort != serverPort)) {
		std::cerr << "NetworkManager (ip: " << incomingIp << ", port: " << incomingPort << ") is attempting to connect directly to another NetworkManager." << std::endl;
		std::cerr << "NetworkManager should send data through the server instead." << std::endl;
		return sf::Packet();
	}


	return incomingData;

}

int NetworkManager::GetNetworkManagerIndex() {
	return NetworkManagerIndex;
}
