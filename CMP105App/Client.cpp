#include "Client.h"
#include <iostream>

Client::Client(sf::IpAddress _serverAddress, unsigned short _serverPort) {
	serverAddress = _serverAddress;
	serverPort = _serverPort;

	socket.setBlocking(true);

	//Send test packet to server to be added to server's list of connected clients
	sf::Packet packet;
	packet << "Test";
	if (socket.send(packet, serverAddress, serverPort) != sf::Socket::Done) {
		std::cerr << "Failed to connect to server." << std::endl;
		connectedToServer = false;
	}
	else {
		std::cout << "Successfully connected to server.\n";
		connectedToServer = true;

		//Get client index from server
		sf::IpAddress tempIp;
		unsigned short tempPort;
		sf::Packet data;
		if (socket.receive(data, tempIp, tempPort) != sf::Socket::Done) {
			std::cerr << "Failed to receive client index from server." << std::endl;
		}
		else {
			std::cout << "Successfully received client index from server.\n";
			data >> clientIndex;
		}
	}

	socket.setBlocking(false);
}


sf::Socket::Status Client::SendDataToClient(int outgoingClientIndex, sf::Packet packet) {
	//Combine client index into the data packet so it can be sent to the server
	packet << outgoingClientIndex;
	return socket.send(packet, serverAddress, serverPort);
}


sf::Socket::Status Client::SendDataToClient(sf::Packet packet) {
	//This function can be called if there are only two clients connected to the server (in which case it will just send data to the other client).
	int outgoingClientIndex = 1 - clientIndex; //0->1, 1->0
	return SendDataToClient(outgoingClientIndex, packet);
}


//To be called every frame
sf::Packet Client::CheckForIncomingDataFromServer() {

	sf::Packet incomingData;

	//Used to check that client is in fact receiving data from the server and not, another client, for example
	sf::IpAddress incomingIp;
	unsigned short incomingPort;

	//Extract data and check if 
	if (socket.receive(incomingData, incomingIp, incomingPort) != sf::Socket::Done) { return sf::Packet(); }

	//Ensuring data is coming from server and not another client
	if ((serverAddress != incomingIp) || (incomingPort != serverPort)) {
		std::cerr << "Client (ip: " << incomingIp << ", port: " << incomingPort << ") is attempting to connect directly to another client." << std::endl;
		std::cerr << "Client should send data through the server instead." << std::endl;
		return sf::Packet();
	}

	return incomingData;

}

int Client::GetClientIndex() {
	return clientIndex;
}
