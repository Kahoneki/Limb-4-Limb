#include "Server.h"
#include <algorithm> //For std::find
#include <iostream>


Server::Server(unsigned short _port) {
	serverPort = _port;

	socket.setBlocking(false);

	if (socket.bind(serverPort) != sf::Socket::Done) { std::cerr << "Server failed to bind to port " << serverPort; }
	else { std::cout << "Server successfully bound to port " << serverPort; }
}

void Server::CheckForIncomingData() {
	
	sf::Packet incomingData;
	sf::IpAddress incomingClientAddress;
	unsigned short incomingClientPort;

	if (socket.receive(incomingData, incomingClientAddress, incomingClientPort) != sf::Socket::Done) {
		return;
	}

	//Check if client isn't already in vector
	ClientInfo incomingClient = ClientInfo(incomingClientAddress, incomingClientPort);
	if (std::find(connectedClients.begin(), connectedClients.end(), incomingClient) == connectedClients.end()) {
		//Client isn't in vector - they're sending a test message and their information has to be added
		connectedClients.push_back(incomingClient);
		std::cout << "Client (ip: " << incomingClient.ip << ', ' << incomingClient.port << ") connected to server.\n";
		return;
	}

	//Separate packet and client index from incoming data
	sf::Packet outgoingData;
	int clientIndex;
	incomingData >> clientIndex;
	outgoingData = incomingData; //All that's left in the incomingData stream is the packet, just assign it to outgoingData - looks a bit silly but it's just so "incomingData" isn't being sent out


	//Validate data (make sure client is trying to send data to an ip+port that is in the array and make sure client isn't trying to send themselves data. possibly other checks also.)
	if ((clientIndex >= connectedClients.size()) || (connectedClients[clientIndex] != incomingClient)) {
		std::cerr << "Client (ip: " << incomingClient.ip << ', ' << incomingClient.port << ") tried to send a message to an invalid client (ip: "
                                    << connectedClients[clientIndex].ip << ", " << connectedClients[clientIndex].port << ")!\n";
		return;
	}

	//Send data to client
	socket.send(outgoingData, connectedClients[clientIndex].ip, connectedClients[clientIndex].port);

}