#include "NetworkListener.h"

template<typename ParentType>
NetworkListener<ParentType>::NetworkListener(ParentType* pr) {
	NetworkManager::getInstance().AddNetworkListener<ParentType>(this);
	parentReference = pr;
}

template<typename ParentType>
ParentType* NetworkListener<ParentType>::getParentReference() { return parentReference; }