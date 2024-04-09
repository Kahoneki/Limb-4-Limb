#include "OnlinePlayer.h"
#include "NetworkManager.h"

OnlinePlayer::OnlinePlayer(float acc, float ts, float js, int hp, int prot, int c1, bool flip, int pn, bool local) :
	Player(acc, ts, js, hp, prot, c1, flip), networkManager(NetworkManager::getInstance()), timeManager (TimeManager::getInstance(240))
{
	verificationPacketTimer = 0;
	verificationPacketCooldown = 10;
	playerNum = pn;
	isLocal = local;
	networkListener = networkManager.GenerateNetworkListener<OnlinePlayer>(*this, playerNum - 1);
	for (int i{ 0 }; i < 8; ++i) {
		prevKeyState[i] = false;
	}
}


void OnlinePlayer::handleInput(float dt, int up, int left, int right, int down, int jab, int kick, int sweep, int upper) {
	if (isLocal) {

		//Compare keys pressed this frame to keys pressed last frame, if they're different, send the differences to the network manager

		int keys[8] { up, left, right, down, jab, kick, sweep, upper };
		Player::handleInput(dt, up, left, right, down, jab, kick, sweep, upper);

		bool currentKeyState[8];
		for (int i{ 0 }; i < 8; ++i) {
			currentKeyState[i] = input->isKeyDown(keys[i]);
		}

		std::vector<int> changedKeys;
		for (int i{ 0 }; i < 8; ++i) {
			if (currentKeyState[i] != prevKeyState[i]) {
				changedKeys.push_back(keys[i]);
			}
		}

		if (!changedKeys.empty()) {
			SendUpdateDataToNetwork(changedKeys);
		}

		for (int i{ 0 }; i < 8; ++i) {
			prevKeyState[i] = currentKeyState[i];
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


void OnlinePlayer::update(float dt) {
	Player::update(dt);
	if (!isLocal) {
		return;
	}

	verificationPacketTimer += timeManager.getDeltaTime();
	if (verificationPacketTimer >= verificationPacketCooldown) {
		verificationPacketTimer = 0;

		//A verification packet is a packet sent every so often with the current player's status to be checked against the other client - this helps to mitigate the effects of packet loss.
		//A verification packet consists of:
		//-Position
		//-Health
		//-Limb status

		//Create verification packet to send to server
		int networkListenerIndex{ playerNum - 1 };
		sf::Packet outgoingPacket;
		outgoingPacket << getPosition().x << getPosition().y << health << activeLimbs[0] << activeLimbs[1] << activeLimbs[2] << activeLimbs[3];
		networkManager.SendDataToNetworkManager(networkListenerIndex, PacketCode::Verification, outgoingPacket);
	}
}



void OnlinePlayer::SendUpdateDataToNetwork(std::vector<int> changedKeys) {
	int networkListenerIndex{ playerNum - 1 };

	for (int key : changedKeys) {
		bool pressed = input->isKeyDown(key);
		sf::Packet outgoingPacket;
		outgoingPacket << pressed << key;
		networkManager.SendDataToNetworkManager(networkListenerIndex, PacketCode::KeyChange, outgoingPacket);
	}
}



void OnlinePlayer::VerifyStatus(sf::Packet verificationPacket) {
	//Compare status against verification packet - if there are any mismatches, favour the verification packet
	sf::Vector2f pos;
	verificationPacket >> pos.x >> pos.y;
	setPosition(pos);
	verificationPacket >> health >> activeLimbs[0] >> activeLimbs[1] >> activeLimbs[2] >> activeLimbs[3];
}



int OnlinePlayer::getPlayerNum() { return playerNum; }

void OnlinePlayer::setKeyPressed(int key, bool pressed) {
	keyIsPressed[key] = pressed;
}
