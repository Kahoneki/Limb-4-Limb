#include "OnlinePlayer.h"
#include "NetworkManager.h"

OnlinePlayer::OnlinePlayer(float acc, float ts, float js, int hp, int prot, int c1, bool flip, int pn, bool local) :
	Player(acc, ts, js, hp, prot, c1, flip), networkManager(NetworkManager::getInstance()), playerNum(pn), networkListener(networkManager.GenerateNetworkListener<OnlinePlayer>(*this, playerNum-1)), isLocal(local)
{
}


void OnlinePlayer::handleInput(float dt, int up, int left, int right, int down, int jab, int kick, int sweep, int upper) {
	if (isLocal) {
		
		//Initialise prevState before calling handleInput and update
		prevState.pos = static_cast<sf::Vector2i>(getPosition());
		prevState.crouched = crouched;
		prevState.health = health;
		prevState.stunFramesLeft = stunFramesLeft;
		for (int i{ 0 }; i < 4; ++i) {
			prevState.activeLimbs[i] = activeLimbs[i];
			prevState.attacking[i] = attacks[i].getAttacking();
		}

		Player::handleInput(dt, up, left, right, down, jab, kick, sweep, upper);
	}
}


void OnlinePlayer::update(float dt) {
	if (isLocal) {
		Player::update(dt);
		
		//Initialise newState after calling handleInput and update
		newState.pos = static_cast<sf::Vector2i>(getPosition());
		newState.crouched = crouched;
		newState.health = health;
		newState.stunFramesLeft = stunFramesLeft;
		for (int i{ 0 }; i < 4; ++i) {
			newState.activeLimbs[i] = activeLimbs[i];
			newState.attacking[i] = attacks[i].getAttacking();
		}
		
		//Check if state has changed this frame - if it has, send update to server
		if (prevState != newState) {
			SendUpdateDataToNetwork(prevState, newState);
		}
	}
}



void OnlinePlayer::SendUpdateDataToNetwork(OnlinePlayerState prevState, OnlinePlayerState newState) {
	int networkListenerIndex{ playerNum - 1 };

	//Check each field to see if it has changed - yandere dev code; i dont think there's an easier way of doing this
	if (prevState.pos != newState.pos) {
		sf::Packet outgoingPacket;
		outgoingPacket << newState.pos.x << newState.pos.y;
		networkManager.SendDataToNetworkManager(networkListenerIndex, PacketCode::PositionChange, outgoingPacket);
	}

	if (prevState.crouched != newState.crouched) {
		sf::Packet outgoingPacket;
		outgoingPacket << newState.crouched;
		networkManager.SendDataToNetworkManager(networkListenerIndex, PacketCode::CrouchChange, outgoingPacket);
	}

	if (prevState.health != newState.health) {
		sf::Packet outgoingPacket;
		outgoingPacket << newState.health;
		networkManager.SendDataToNetworkManager(networkListenerIndex, PacketCode::HealthChange, outgoingPacket);
	}

	if (prevState.stunFramesLeft != newState.stunFramesLeft) {
		sf::Packet outgoingPacket;
		outgoingPacket << newState.stunFramesLeft;
		networkManager.SendDataToNetworkManager(networkListenerIndex, PacketCode::StunFrameChange, outgoingPacket);
	}

	if (prevState.activeLimbs != newState.activeLimbs) {
		sf::Packet outgoingPacket;
		outgoingPacket << newState.activeLimbs[0] << newState.activeLimbs[1] << newState.activeLimbs[2] << newState.activeLimbs[3];
		networkManager.SendDataToNetworkManager(networkListenerIndex, PacketCode::ActiveLimbChange, outgoingPacket);
	}

	if (prevState.attacking != newState.attacking) {
		sf::Packet outgoingPacket;
		outgoingPacket << newState.attacking[0] << newState.attacking[1] << newState.attacking[2] << newState.attacking[3];
		networkManager.SendDataToNetworkManager(networkListenerIndex, PacketCode::AttackingChange, outgoingPacket);
	}
}



int OnlinePlayer::getPlayerNum() { return playerNum; }