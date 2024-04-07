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
            sf::Vector2f pos;
            incomingData >> pos.x >> pos.y;
            callChangePosition<ParentType>(pos);
        }

        }
    }


private:
    ParentType& parentReference;


    //Template functions (using SFINAE metaprogramming template techniques to check if ParentType has a specific member function so that it can be called)
    
    
    //----CHANGE POSITION----//
    //Success
    template<typename T = ParentType, typename std::enable_if_t<std::is_same_v<decltype(std::declval<T>().setPosition(std::declval<sf::Vector2f>())), void>, int> = 0>
    void callChangePosition(sf::Vector2f pos) {
        std::cout << "Success\n";
        parentReference.setPosition(pos);
    }
    //Falback
    template<typename T = ParentType, typename std::enable_if_t<!std::is_same_v<decltype(std::declval<T>().setPosition(std::declval<sf::Vector2f>())), void>, int> = 0>
    void callChangePosition(sf::Vector2f pos) {
        std::cerr << "Substitution failure reached for callChangePosition()" << std::endl;
        return;
    }
};

#endif