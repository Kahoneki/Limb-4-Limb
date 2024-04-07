#include "OnlinePlayer.h"
#include "NetworkManager.h"

OnlinePlayer::OnlinePlayer(float acc, float ts, float js, int hp, int prot, int c1, bool flip, int pn, bool local) :
	Player(acc, ts, js, hp, prot, c1, flip), networkManager(NetworkManager::getInstance()), playerNum(pn), networkListener(networkManager.GenerateNetworkListener<OnlinePlayer>(*this, playerNum-1)), isLocal(local)
{
}



void OnlinePlayer::update(float dt) {
	if (isLocal) {
		OnlinePlayerState prevState;
		prevState.pos = getPosition();
		sf::Vector2f prevPosition{ getPosition() };
		
		Player::update(dt);
		
		OnlinePlayerState newState;
		newState.pos = getPosition();
		
		if (prevState != newState) {
			SendUpdateDataToNetwork(prevState, newState);
		}
	
		return;
	}
}



void OnlinePlayer::SendUpdateDataToNetwork(OnlinePlayerState prevState, OnlinePlayerState newState) {
	int networkListenerIndex{ playerNum - 1 };

	//Check each field to see if it has changed
	if (prevState.pos != newState.pos) {
		sf::Packet outgoingPacket;
		outgoingPacket << getPosition().x << getPosition().y;
		networkManager.SendDataToNetworkManager(networkListenerIndex, PacketCode::PositionChange, outgoingPacket);
	}
}



int OnlinePlayer::getPlayerNum() { return playerNum; }