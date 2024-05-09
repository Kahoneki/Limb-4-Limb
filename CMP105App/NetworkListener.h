#ifndef NETWORKLISTENER_H
#define NETWORKLISTENER_H

#include "PacketCode.h"
#include "BaseNetworkListener.h"
#include "SFML/Network.hpp"

#include "OnlinePlayer.h"
#include "RegistrationScreen.h"
#include "LoginScreen.h"
#include "SendInviteScreen.h"
#include "MatchInvitationInterrupt.h"
#include "NetworkScene.h"



//This template class can be attached to a class to serve as a "network receiver component"
//When data is received by the network manager, the packet will be sent to the appropriate network listener which will deconstruct the packet and use it to perform some operation on the
//object that it is attached to



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
            parentReference.keyIsPressed[key] = pressed;
            break;
        }
        
        case PacketCode::PositionChange:
        {
            sf::Vector2f pos;
            incomingData >> pos.x >> pos.y;
            parentReference.setPosition(pos);
            break;
        }

        case PacketCode::Grounded:
        {
            bool grounded;
            incomingData >> grounded;
            parentReference.isGrounded = grounded;
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
            parentReference.usernameAvailable = available;
            break;
        }

        case PacketCode::UUID:
        {
            sf::Uint64 uuid;
            incomingData >> uuid;
            parentReference.uuid = uuid;
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
        case PacketCode::LoginStatus:
        {
            sf::Int8 available;
            incomingData >> available;
            parentReference.loginStatus = available;
            break;
        }

        case PacketCode::Ranking:
        {
            sf::Int32 ranking;
            incomingData >> ranking;
            parentReference.ranking = ranking;
            break;
        }
        }
    }

private:
    LoginScreen& parentReference;
};



template<>
class NetworkListener<SendInviteScreen> : public BaseNetworkListener {
public:
    NetworkListener(SendInviteScreen& pr) : parentReference(pr) {}

    void InterpretPacket(sf::Packet incomingData) {
        std::underlying_type_t<PacketCode> code;
        incomingData >> code;

        switch (code)
        {
        case PacketCode::UserExists:
        {
            sf::Int8 exists;
            incomingData >> exists;
            parentReference.userExists = exists;
            break;
        }

        case PacketCode::UserOnline:
        {
            sf::Int8 online;
            incomingData >> online;
            parentReference.userOnline = online;
            break;
        }
        case PacketCode::UserFree:
        {
            sf::Int8 userFree;
            incomingData >> userFree;
            parentReference.userFree = userFree;
            break;
        }
        case PacketCode::MatchAcceptanceServerToClient:
        {
            sf::Int8 acceptance;
            incomingData >> acceptance;
            parentReference.userAccept = acceptance;
            break;
        }
        case PacketCode::PlayerNum:
        {
            sf::Int8 playerNum;
            incomingData >> playerNum;
            parentReference.playerNum = playerNum;
            break;
        }
        case PacketCode::InvitedUserNetworkManagerIndex:
        {
            int invitedUserNetworkManagerIndex;
            incomingData >> invitedUserNetworkManagerIndex;
            parentReference.invitedUserNetworkManagerIndex = invitedUserNetworkManagerIndex;
            break;
        }
        }
    }

private:
    SendInviteScreen& parentReference;
};



template<>
class NetworkListener<MatchInvitationInterrupt> : public BaseNetworkListener {
public:
    NetworkListener(MatchInvitationInterrupt& pr) : parentReference(pr) {}

    void InterpretPacket(sf::Packet incomingData) {
        std::underlying_type_t<PacketCode> code;
        incomingData >> code;

        switch (code)
        {
        case PacketCode::MatchInvitation:
        {
            std::string username;
            sf::Int32 ranking;
            int networkManagerIndex;
            incomingData >> username >> ranking >> networkManagerIndex;
            parentReference.invitationReceived = true;
            parentReference.username = username;
            parentReference.ranking = ranking;
            parentReference.networkManagerIndex = networkManagerIndex;
            break;
        }
        case PacketCode::PlayerNum:
        {
            sf::Int8 playerNum;
            incomingData >> playerNum;
            parentReference.playerNum = playerNum;
            break;
        }
        }
    }

private:
    MatchInvitationInterrupt& parentReference;
};



template<>
class NetworkListener<NetworkScene> : public BaseNetworkListener {
public:
    NetworkListener(NetworkScene& pr) : parentReference(pr) {}

    void InterpretPacket(sf::Packet incomingData) {
        std::underlying_type_t<PacketCode> code;
        incomingData >> code;

        switch (code)
        {
        case PacketCode::MatchStart:
        {
            parentReference.matchStart = true;
            break;
        }
        }
    }

private:
    NetworkScene& parentReference;
};


#endif