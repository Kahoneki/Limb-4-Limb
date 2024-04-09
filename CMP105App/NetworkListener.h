#ifndef NETWORKLISTENER_H
#define NETWORKLISTENER_H

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

        case PacketCode::KeyChange:
        {
            bool pressed;
            int key;
            incomingData >> pressed >> key;
            parentReference.setKeyPressed(key, pressed);
            break;
        }

        case PacketCode::Verification:
        {
            parentReference.VerifyStatus(incomingPacket);
            break;
        }

        }
    }


private:
    ParentType& parentReference;
};

#endif