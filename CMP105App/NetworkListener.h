#ifndef NETWORKLISTENER_H
#define NETWORKLISTENER_H

#include <iostream>
#include "PacketCode.h"
#include "BaseNetworkListener.h"
#include "SFML/Network.hpp"

#include "OnlinePlayer.h"
#include "RegistrationScreen.h"
#include "LoginScreen.h"


template<typename ParentType>
class NetworkListener {};

template<>
class NetworkListener<OnlinePlayer> : public BaseNetworkListener {
public:
    NetworkListener(OnlinePlayer& pr) : parentReference(pr) {}

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
            parentReference.VerifyStatus(incomingData);
            break;
        }
        
        case PacketCode::PositionChange:
        {
            sf::Vector2f pos;
            incomingData >> pos.x >> pos.y;
            parentReference.setPosition(pos);
            break;
        }
        
        }
    }

private:
    OnlinePlayer& parentReference;
};



template<>
class NetworkListener<RegistrationScreen> : public BaseNetworkListener {
public:
    NetworkListener(RegistrationScreen& pr) : parentReference(pr) {}

    void InterpretPacket(sf::Packet incomingData) {
        std::underlying_type_t<PacketCode> code;
        incomingData >> code;

        switch (code)
        {
        case PacketCode::UsernameAvailabilityStatus:
        {
            sf::Int8 available;
            incomingData >> available;
            parentReference.setUsernameAvailable(available);
            break;
        }

        case PacketCode::UUID:
        {
            sf::Uint64 uuid;
            incomingData >> uuid;
            parentReference.setUUID(uuid);
            break;
        }
        }
    }

private:
    RegistrationScreen& parentReference;
};



template<>
class NetworkListener<LoginScreen> : public BaseNetworkListener {
public:
    NetworkListener(LoginScreen& pr) : parentReference(pr) {}

    void InterpretPacket(sf::Packet incomingData) {
        std::underlying_type_t<PacketCode> code;
        incomingData >> code;

        switch (code)
        {
        case PacketCode::UsernameAvailabilityStatus:
        {
            sf::Int8 available;
            incomingData >> available;
            parentReference.setLoginStatus(available);
            break;
        }

        case PacketCode::UUID:
        {
            sf::Int32 ranking;
            incomingData >> ranking;
            parentReference.setRanking(ranking);
            break;
        }
        }
    }

private:
    LoginScreen& parentReference;
};


#endif