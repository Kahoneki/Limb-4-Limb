#include "NetworkNode.h"


void NetworkNode::CheckForIncomingDataFromNetworkManager()
{
}

sf::Packet NetworkNode::CheckForIncomingDataFromServer()
{
	return sf::Packet();
}

sf::Socket::Status NetworkNode::SendDataToNetworkManager(int NetworkManagerIndex, sf::Packet packet)
{
	return sf::Socket::Status();
}

sf::Socket::Status NetworkNode::SendDataToNetworkManager(sf::Packet packet)
{
	return sf::Socket::Status();
}
