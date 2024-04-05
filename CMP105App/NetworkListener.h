#ifndef NETWORKLISTENER_H
#define NETWORKLISTENER_H

#include "NetworkManager.h"

template<typename ParentType>
class NetworkListener {
public:
	NetworkListener(ParentType* pr);

	ParentType* getParentReference();

private:
	ParentType* parentReference; //Reference to the object that this listener is attached to - type is void pointer to allow for dynamic casting to any type
};


#endif