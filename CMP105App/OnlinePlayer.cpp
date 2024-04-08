#include "OnlinePlayer.h"
#include "NetworkManager.h"

OnlinePlayer::OnlinePlayer(float acc, float ts, float js, int hp, int prot, int c1, bool flip, int pn, bool local) :
	Player(acc, ts, js, hp, prot, c1, flip), networkManager(NetworkManager::getInstance()), playerNum(pn), networkListener(networkManager.GenerateNetworkListener<OnlinePlayer>(*this, playerNum-1)), isLocal(local)
{
}


void OnlinePlayer::handleInput(float dt, int up, int left, int right, int down, int jab, int kick, int sweep, int upper) {
	if (isLocal) {
		
		//Initialise prevState before calling handleInput and update
		//prevState.pos = static_cast<sf::Vector2i>(getPosition());
		//prevState.crouched = crouched;
		//prevState.health = health;
		//prevState.stunFramesLeft = stunFramesLeft;
		//for (int i{ 0 }; i < 4; ++i) {
		//	prevState.activeLimbs[i] = activeLimbs[i];
		//	prevState.attacking[i] = attacks[i].getAttacking();
		//}

		int keys[8] { up, left, right, down, jab, kick, sweep, upper };

		bool keysDownBefore[8];
		for (int i{ 0 }; i < 8; ++i) {
			keysDownBefore[i] = input->isKeyDown(keys[i]);
		}

		Player::handleInput(dt, up, left, right, down, jab, kick, sweep, upper);
	
		bool keysDownAfter[8];
		for (int i{ 0 }; i < 8; ++i) {
			keysDownAfter[i] = input->isKeyDown(keys[i]);
		}

		if (keysDownBefore != keysDownAfter) {
			std::vector<int> changedKeys;
			for (int i{ 0 }; i < 8; ++i) {
				if (keysDownBefore[i] != keysDownAfter[i]) {
					changedKeys.push_back(keys[i]);
				}
			}
			SendUpdateDataToNetwork(changedKeys);
		}
	}


	else {
		if (actionable) {
			if (isGrounded) {
				//Pressing both keys at same time or not pressing either key
				if ((keyIsPressed[left] && keyIsPressed[right]) || (!keyIsPressed[left] && (!keyIsPressed[right]))) {
					//Slow down to an immediate hault
					velocity.x = 0;
				}
				//Pressing either left or right (but not both - case covered by above check)
				else if (keyIsPressed[right] || keyIsPressed[left]) {
					//Handle movement
					if (keyIsPressed[right]) {
						velocity.x = topSpeed - (flipped * (0.3 * topSpeed));
						blocking = flipped;
					}
					if (keyIsPressed[left]) {
						velocity.x = -topSpeed + (!flipped * (0.3 * topSpeed));
						blocking = !flipped;
					}
				}
				//Jumping
				if (keyIsPressed[up]) {
					isGrounded = false;
					velocity.y = jumpSpeed;
					velocity.x *= 1.25;
				}

				//Ducking
				if (keyIsPressed[down]) {
					if (!crouched) {
						setSize(sf::Vector2f(getSize().x, getSize().y * 0.5));
						setOrigin(getLocalBounds().width / 2.f, getLocalBounds().height / 2.f);
						setPosition(sf::Vector2f(getPosition().x, getPosition().y + 225 / 4));
						crouched = true;
					}
				}
				if (crouched) {
					if (!keyIsPressed[down]) {
						setSize(sf::Vector2f(getSize().x, getSize().y / 0.5));
						setOrigin(getLocalBounds().width / 2.f, getLocalBounds().height / 2.f);
						setPosition(sf::Vector2f(getPosition().x, getPosition().y - 225 / 4));
						crouched = false;
					}
				}

				//-----GROUND COMBAT-----//
				if (!crouched) {
					if (keyIsPressed[kick]) {
						setFillColor(sf::Color(getFillColor().r, getFillColor().g, getFillColor().b, 128));
						attacks[0].setAttacking(true);
						velocity.x = 0;
					}
					if (keyIsPressed[sweep]) {
						setFillColor(sf::Color(getFillColor().r, getFillColor().g, getFillColor().b, 128));
						attacks[1].setAttacking(true);
						velocity.x = 0;
					}
					if (keyIsPressed[jab]) {
						setFillColor(sf::Color(getFillColor().r, getFillColor().g, getFillColor().b, 128));
						attacks[2].setAttacking(true);
						velocity.x = 0;
					}
					if (keyIsPressed[upper]) {
						setFillColor(sf::Color(getFillColor().r, getFillColor().g, getFillColor().b, 128));
						attacks[3].setAttacking(true);
						velocity.x = 0;
					}
				}

				velocity.y -= acceleration * dt;
			}
		}

		//Player is in air, so bring them towards ground
		if (!isGrounded) {
			velocity.y -= acceleration * dt;
		}
	}
}


//void OnlinePlayer::update(float dt) {
//	if (isLocal) {
//		Player::update(dt);
//		
//		//Initialise newState after calling handleInput and update
//		newState.pos = static_cast<sf::Vector2i>(getPosition());
//		newState.crouched = crouched;
//		newState.health = health;
//		newState.stunFramesLeft = stunFramesLeft;
//		for (int i{ 0 }; i < 4; ++i) {
//			newState.activeLimbs[i] = activeLimbs[i];
//			newState.attacking[i] = attacks[i].getAttacking();
//		}
//		
//		//Check if state has changed this frame - if it has, send update to server
//		//if (prevState != newState) {
//		//	SendUpdateDataToNetwork(prevState, newState);
//		//}
//	}
//}



void OnlinePlayer::SendUpdateDataToNetwork(std::vector<int> changedKeys) {
	int networkListenerIndex{ playerNum - 1 };

	for (int key : changedKeys) {
		bool pressed = input->isKeyDown(key);
		sf::Packet outgoingPacket;
		outgoingPacket << pressed << key;
		networkManager.SendDataToNetworkManager(networkListenerIndex, PacketCode::KeyChange, outgoingPacket);
	}
}


//void OnlinePlayer::SendUpdateDataToNetwork(OnlinePlayerState prevState, OnlinePlayerState newState) {
//	int networkListenerIndex{ playerNum - 1 };
//
//	//Check each field to see if it has changed - this looks like yandere dev code; i dont think there's an easier way of doing this
//	
//	//crouched has to come before pos
//	if (prevState.crouched != newState.crouched) {
//		sf::Packet outgoingPacket;
//		outgoingPacket << newState.crouched;
//		networkManager.SendDataToNetworkManager(networkListenerIndex, PacketCode::CrouchChange, outgoingPacket);
//	}
//
//	if (prevState.pos != newState.pos) {
//		sf::Packet outgoingPacket;
//		outgoingPacket << newState.pos.x << newState.pos.y;
//		networkManager.SendDataToNetworkManager(networkListenerIndex, PacketCode::PositionChange, outgoingPacket);
//	}
//
//	if (prevState.attacking != newState.attacking) {
//		sf::Packet outgoingPacket;
//		outgoingPacket << newState.attacking[0] << newState.attacking[1] << newState.attacking[2] << newState.attacking[3];
//		networkManager.SendDataToNetworkManager(networkListenerIndex, PacketCode::AttackingChange, outgoingPacket);
//	}
//
//	if (prevState.health != newState.health) {
//		sf::Packet outgoingPacket;
//		outgoingPacket << newState.health;
//		networkManager.SendDataToNetworkManager(networkListenerIndex, PacketCode::HealthChange, outgoingPacket);
//	}
//
//	if (prevState.stunFramesLeft != newState.stunFramesLeft) {
//		sf::Packet outgoingPacket;
//		outgoingPacket << newState.stunFramesLeft;
//		networkManager.SendDataToNetworkManager(networkListenerIndex, PacketCode::StunFrameChange, outgoingPacket);
//	}
//
//	if (prevState.activeLimbs != newState.activeLimbs) {
//		sf::Packet outgoingPacket;
//		outgoingPacket << newState.activeLimbs[0] << newState.activeLimbs[1] << newState.activeLimbs[2] << newState.activeLimbs[3];
//		networkManager.SendDataToNetworkManager(networkListenerIndex, PacketCode::ActiveLimbChange, outgoingPacket);
//	}
//}



int OnlinePlayer::getPlayerNum() { return playerNum; }

void OnlinePlayer::setKeyPressed(int key, bool pressed) {
	keyIsPressed[key] = pressed;
}
