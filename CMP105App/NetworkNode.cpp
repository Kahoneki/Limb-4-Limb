#include "NetworkNode.h"


NetworkNode::~NetworkNode()
{
}

void NetworkNode::CheckForIncomingDataFromNetworkManager()
{
}

void NetworkNode::CheckForIncomingDataFromServer()
{
}

sf::Socket::Status NetworkNode::SendDataToNetworkManager(int NetworkManagerIndex, sf::Packet packet)
{
	return sf::Socket::Status();
}

sf::Socket::Status NetworkNode::SendDataToNetworkManager(sf::Packet packet)
{
	return sf::Socket::Status();
}
