#ifndef BASENETWORKLISTENER_H
#define BASENETWORKLISTENER_H

#include "SFML/Network.hpp"

// Base class for polymorphism in NetworkManager::networkListeners
class BaseNetworkListener {
public:
	virtual void InterpretPacket(sf::Packet incomingData) = 0;
};


#endif