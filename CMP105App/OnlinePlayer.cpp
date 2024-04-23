#include "OnlinePlayer.h"
#include "NetworkManager.h"

OnlinePlayer::OnlinePlayer(sf::Vector2f size, float acc, float ts, float js, int hp, int prot, int c1, bool flip, int pn, bool local) :
	Player(size, acc, ts, js, hp, prot, c1, flip), networkManager(NetworkManager::getInstance()), timeManager (TimeManager::getInstance(240))
{
	verificationPacketTimer = 0;
	verificationPacketCooldown = 100.0f;
	playerNum = pn;
	isLocal = local;
	networkListener = networkManager.GenerateNetworkListener<OnlinePlayer>(*this, playerNum - 1);

	for (int i{ 0 }; i < sizeof(prevKeyState)/sizeof(prevKeyState[0]); ++i) {
		prevKeyState[i] = false;
	}
}


void OnlinePlayer::handleInput(float dt, int jump, int left, int right, int down, int dodge, int jab, int kick, int sweep, int upper) {
	if (isLocal) {

		//Compare keys pressed this frame to keys pressed last frame, if they're different, send the differences to the network manager

		int keys[6] { down, dodge, jab, kick, sweep, upper };
		Player::handleInput(dt, jump, left, right, down, dodge, jab, kick, sweep, upper);

		bool currentKeyState[6];
		for (int i{ 0 }; i < sizeof(currentKeyState)/sizeof(currentKeyState[0]); ++i) {
			currentKeyState[i] = input->isKeyDown(keys[i]);
		}

		std::vector<int> changedKeys;
		for (int i{ 0 }; i < sizeof(currentKeyState)/sizeof(currentKeyState[0]); ++i) {
			if (currentKeyState[i] != prevKeyState[i]) {
				changedKeys.push_back(keys[i]);
			}
		}

		if (!changedKeys.empty()) {
			SendUpdateDataToNetwork(changedKeys);
		}

		for (int i{ 0 }; i < sizeof(prevKeyState)/sizeof(prevKeyState[0]); ++i) {
			prevKeyState[i] = currentKeyState[i];
		}

		//Check if position has changed - send this change to the server
		if (prevPosition != getPosition()) {
			SendUpdateDataToNetwork(getPosition());
			prevPosition = getPosition();
		}
	}


	else {
		for (const auto& a : keyIsPressed) {
			if (a.second) {
				std::cout << a.first << " is enabled\n";
			}
		}
		if (dodgeFramesLeft) { actionable = false; }

		//-----COMBAT-----//
		if (actionable) {
			if (!crouched) {
				if (keyIsPressed[jab]) {
					attacks[0].setAttacking(true);
				}
				if (keyIsPressed[sweep]) {
					attacks[1].setAttacking(true);
				}
				if (keyIsPressed[jab]) {
					attacks[2].setAttacking(true);
				}
				if (keyIsPressed[upper]) {
					attacks[3].setAttacking(true);
				}
			}
		}


		directionKeycodesThisFrame.clear();

		if (keyIsPressed[left]) {
			directionKeycodesThisFrame.push_back(left);
		}
		if (keyIsPressed[right]) {
			directionKeycodesThisFrame.push_back(right);
		}

		if (directionKeycodesThisFrame.empty()) {
			mostRecentDirectionKeycode = -1;
		}
		else if (directionKeycodesLastFrame != directionKeycodesThisFrame) {
			//Most recent key press will be the item in directionKeycodesThisFrame that isn't in directionKeycodesLastFrame (not taking into account multiple keys being pressed down on the same frame)
			for (int kc : directionKeycodesThisFrame) {
				if (std::find(directionKeycodesLastFrame.begin(), directionKeycodesLastFrame.end(), kc) == directionKeycodesLastFrame.end()) {
					//kc was pressed this frame but wasn't pressed last frame, it is the most recent key press
					mostRecentDirectionKeycode = kc;
				}
			}
		}

		directionKeycodesLastFrame = directionKeycodesThisFrame;


		//Crouching
		if (keyIsPressed[down] && !dodgeFramesLeft) {
			if (!crouched) {
				setSize(sf::Vector2f(getSize().x, getSize().y * 0.5f));
				setOrigin(getLocalBounds().width / 2.f, getLocalBounds().height / 2.f);
				setPosition(sf::Vector2f(getPosition().x, getPosition().y + getSize().y / 2));
				crouched = true;
			}
		}
		if (crouched && !dodgeFramesLeft) {
			if (!keyIsPressed[down]) {
				setSize(sf::Vector2f(getSize().x, getSize().y / 0.5f));
				setOrigin(getLocalBounds().width / 2.f, getLocalBounds().height / 2.f);
				setPosition(sf::Vector2f(getPosition().x, (getPosition().y - getSize().y / 4) + 1)); //Adding 1 to account for floating-point rounding error that causes player to go up 1 pixel every time
				crouched = false;
			}
		}

		//Dodging
		if (!dodgeCooldownFramesLeft && !dodgeButtonPressed) {
			if (keyIsPressed[dodge] && mostRecentDirectionKeycode != -1) {
				dodgeButtonPressed = true;
				if (mostRecentDirectionKeycode == left) {
					dodgeFramesLeft = totalDodgeFrames;
					setVelocity(-dodgeVelocity, getVelocity().y);
				}
				else if (mostRecentDirectionKeycode == right) {
					dodgeFramesLeft = totalDodgeFrames;
					setVelocity(dodgeVelocity, getVelocity().y);
				}
			}
		}
		if (dodgeButtonPressed && !keyIsPressed[dodge] && !dodgeFramesLeft) {
			dodgeButtonPressed = false;
		}


		//Player is in air, so bring them towards ground
		if (!isGrounded && !(velocity.y == terminalVelocity)) {
			velocity.y -= acceleration * dt;
		}
		if (velocity.y < terminalVelocity) {
			velocity.y = terminalVelocity;
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

void OnlinePlayer::SendUpdateDataToNetwork(sf::Vector2f newPosition) {
	int networkListenerIndex{ playerNum - 1 };

	sf::Packet outgoingPacket;
	outgoingPacket << newPosition.x << newPosition.y;
	networkManager.SendDataToNetworkManager(networkListenerIndex, PacketCode::PositionChange, outgoingPacket);
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
	std::cout << key << " is " << pressed << '\n';
	keyIsPressed[key] = pressed;
}
