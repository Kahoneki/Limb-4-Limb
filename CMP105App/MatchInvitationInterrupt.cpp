#include "MatchInvitationInterrupt.h"
#include "NetworkManager.h"


MatchInvitationInterrupt& MatchInvitationInterrupt::getInstance()
{
	static MatchInvitationInterrupt instance;
	return instance;
}

MatchInvitationInterrupt::MatchInvitationInterrupt()
{
	networkListener = NetworkManager::getInstance(false).GenerateNetworkListener(*this, NetworkManager::ReservedEntityIndexTable::MATCH_INVITATION_INTERRUPT);
	invitationReceived = false;
	username = "N/A";
	ranking = -1;
	networkManagerIndex = -1;
}


bool MatchInvitationInterrupt::getInvitationReceived() { return invitationReceived; }

std::string MatchInvitationInterrupt::getUsername() { return username; }

sf::Int32 MatchInvitationInterrupt::getRanking() { return ranking; }

int MatchInvitationInterrupt::getNetworkManagerIndex() { return networkManagerIndex; }
