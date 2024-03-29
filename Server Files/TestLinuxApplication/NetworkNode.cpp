#include "NetworkNode.h"


void NetworkNode::CheckForIncomingDataFromClient()
{
}

sf::Packet NetworkNode::CheckForIncomingDataFromServer()
{
	return sf::Packet();
}

sf::Socket::Status NetworkNode::SendDataToClient(int clientIndex, sf::Packet packet)
{
	return sf::Socket::Status();
}

sf::Socket::Status NetworkNode::SendDataToClient(sf::Packet packet)
{
	return sf::Socket::Status();
}
