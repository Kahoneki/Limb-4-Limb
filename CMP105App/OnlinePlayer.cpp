#include "OnlinePlayer.h"

OnlinePlayer::OnlinePlayer(float acc, float ts, float js, int hp, int prot, int c1, bool flip, int pn) :
	Player(acc, ts, js, hp, prot, c1, flip), networkListener(this), networkManager(NetworkManager::getInstance()), playerNum(pn)
{
}



void OnlinePlayer::update(float dt) {

	sf::Vector2f prevPosition{ getPosition() };

	Player::update(dt);

	SendUpdateDataToNetwork(prevPosition);
}



void OnlinePlayer::SendUpdateDataToNetwork(sf::Vector2f prevPosition) {
	if (getPosition() != prevPosition) {
		int networkListenerIndex{ playerNum - 1 };
		PacketCode packetCode{PacketCode::PositionChange};
		sf::Packet outgoingPacket;
		outgoingPacket << getPosition().x << getPosition().y;
		networkManager.SendDataToNetworkManager(networkListenerIndex, packetCode, outgoingPacket);
	}
}



int OnlinePlayer::getPlayerNum() { return playerNum; }