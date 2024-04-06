#ifndef NETWORKLISTENER_H
#define NETWORKLISTENER_H

// Forward declaration of ParentType
template<typename ParentType>
class NetworkListener;

// Base class for polymorphism in NetworkManager::networkListeners
class BaseNetworkListener {};

template<typename ParentType>
class NetworkListener : public BaseNetworkListener {
public:
    // Constructor
    NetworkListener(ParentType& pr) : parentReference(pr) {}

    ParentType* getParentReference();

private:
    ParentType& parentReference;
};

#endif