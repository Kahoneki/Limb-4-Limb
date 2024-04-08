#ifndef NETWORKLISTENER_H
#define NETWORKLISTENER_H

#include <type_traits>
#include <iostream>
#include "PacketCode.h"
#include "BaseNetworkListener.h"
#include "SFML/Network.hpp"

template<typename ParentType>
class NetworkListener : public BaseNetworkListener {
public:
    // Constructor
    NetworkListener(ParentType& pr) : parentReference(pr) {}


    //Apply data to parent
    void InterpretPacket(sf::Packet incomingData) {
        std::underlying_type_t<PacketCode> code;
        incomingData >> code;

        switch (code)
        {
        
        case PacketCode::PositionChange:
        {
            sf::Vector2i pos;
            incomingData >> pos.x >> pos.y;
            parentReference.setPosition(static_cast<sf::Vector2f>(pos));
        }
        case PacketCode::CrouchChange:
        {
            bool crouched;
            incomingData >> crouched;
            parentReference.setCrouched(crouched);
        }
        }
    }


private:
    ParentType& parentReference;
};

#endif