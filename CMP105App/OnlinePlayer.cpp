#include "OnlinePlayer.h"
#include "NetworkManager.h"

OnlinePlayer::OnlinePlayer(sf::Vector2f size, float acc, float ts, float js, int hp, int prot, int c1, bool flip, int pn, bool local, int nmi) :
	Player(size, acc, ts, js, hp, prot, c1, flip), networkManager(NetworkManager::getInstance(false)), timeManager (TimeManager::getInstance(240))
{
	verificationPacketTimer = 0;
	verificationPacketCooldown = 100.0f;
	playerNum = pn;
	isLocal = local;
	opponentNetworkManagerIndex = nmi;
	networkListenerIndex = (playerNum == 1) ? NetworkManager::ReservedEntityIndexTable::PLAYER_1 : NetworkManager::ReservedEntityIndexTable::PLAYER_2;
	networkListener = networkManager.GenerateNetworkListener<OnlinePlayer>(*this, playerNum - 1);

	for (int i{ 0 }; i < sizeof(prevKeyState)/sizeof(prevKeyState[0]); ++i) {
		prevKeyState[i] = false;
	}
}


void OnlinePlayer::handleInput(float dt, int jump, int left, int right, int down, int dodge, int jab, int kick, int sweep, int upper) {
	if (isLocal) {

		bool flippedAtStartOfFrame{ flipped };

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

		//Check if flipped state has changed - send this change to the server
		if (flippedAtStartOfFrame != flipped) {
			SendUpdateDataToNetwork(flipped);
		}
	}


	else {
		if (dodgeFramesLeft) { actionable = false; }

		//-----COMBAT-----//
		if (actionable) {
			if (!crouched) {
				if (keyIsPressed[kick]) {
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
	}
}


void OnlinePlayer::update(float dt) {
	if (isLocal) {
		bool flippedAtStartOfFrame{ flipped };
		int healthAtStartOfFrame{ health };
		Player::update(dt);
		//Check if flipped state has changed - send this change to the server
		if (flippedAtStartOfFrame != flipped) {
			SendUpdateDataToNetwork(flipped);
		}
		if (healthAtStartOfFrame != health) {
			SendUpdateDataToNetwork(health);
		}
	}
	else {

		effectiveCollider = getGlobalBounds();
		effectiveCollider.left += colliderShrinkage.x;
		effectiveCollider.top += colliderShrinkage.y;
		effectiveCollider.width -= colliderShrinkage.x * 2;
		effectiveCollider.height -= colliderShrinkage.y * 2;

		//Vertical - check if player has fallen off map
		int deathPlaneLevel{ 1920 };
		if (getPosition().y - getSize().y / 2 > deathPlaneLevel) {
			health = 0;
		}

		actionable = true;
		for (int i{}; i < 4; ++i) {
			if (attacks[i].getAttacking()) {
				attacks[i].strike(dt, getPosition().x, getPosition().y, flipped, crouched);
				actionable = false;
			}
		}

		if ((invincibilityFramesLeft || dodgeFramesLeft) && getFillColor().a != 128) {
			setFillColor(sf::Color(getFillColor().r, getFillColor().g, getFillColor().b, 128)); //Make transparent
		}
		else if (!(invincibilityFramesLeft || dodgeFramesLeft) && getFillColor().a != 255) {
			setFillColor(sf::Color(getFillColor().r, getFillColor().g, getFillColor().b, 255)); //Restore to full transparency
		}

		//Check if player is being knocked back but has hit the ground again
		if (hasKnockback && isGrounded) {
			hasKnockback = false;
		}

		//Check if limbs are to be destroyed
		for (int i{}; i < 4; ++i) {
			activeLimbs[i] = health >= 20 * (i + 1);
			UpdateTextures();
		}


		if (updateTextures) {
			updateTextures = false;

			//Clear the render texture
			playerRenderTexture->clear(sf::Color::Transparent);

			//Draw base
			playerRenderTexture->draw(*basePlayerSprite);
			playerRenderTexture->display();

			//Draw limbs
			for (int i{}; i < 4; ++i) {
				if (activeLimbs[i]) {
					//Draw alive limb
					playerRenderTexture->draw(*aliveLimbSprites[i]);
				}
				else {
					//Draw dead limb
					playerRenderTexture->draw(*deadLimbSprites[i]);
				}
			}
			playerRenderTexture->display();
			setTexture(&playerRenderTexture->getTexture());
		}

		if (invincibilityFramesLeft > 0) { invincibilityFramesLeft -= TimeManager::PhysicsClockFramerate * dt; }
		else if (invincibilityFramesLeft < 0) { invincibilityFramesLeft = 0; }

		if (dodgeFramesLeft > 0) { dodgeFramesLeft -= TimeManager::PhysicsClockFramerate * dt; }
		else if (dodgeFramesLeft < 0) {
			dodgeFramesLeft = 0;
			velocity.x = 0;
			dodgeCooldownFramesLeft = dodgeCooldownFrames;
		}

		if (dodgeCooldownFramesLeft > 0) { dodgeCooldownFramesLeft -= TimeManager::PhysicsClockFramerate * dt; }
		else if (dodgeCooldownFramesLeft < 0) { dodgeCooldownFramesLeft = 0; }
	}
}



void OnlinePlayer::SendUpdateDataToNetwork(std::vector<int> changedKeys) {
	for (int key : changedKeys) {
		bool pressed = input->isKeyDown(key);

		std::cout << key << ' ' << pressed << '\n';

		sf::Packet outgoingPacket;
		outgoingPacket << pressed << key;
		networkManager.SendDataToNetworkManager(opponentNetworkManagerIndex, networkListenerIndex, PacketCode::KeyChange, outgoingPacket);
	}
}

void OnlinePlayer::SendUpdateDataToNetwork(sf::Vector2f newPosition) {
	sf::Packet outgoingPacket;
	outgoingPacket << newPosition.x << newPosition.y;
	networkManager.SendDataToNetworkManager(opponentNetworkManagerIndex, networkListenerIndex, PacketCode::PositionChange, outgoingPacket);
}

void OnlinePlayer::SendUpdateDataToNetwork(bool flipped) {
	sf::Packet outgoingPacket;
	outgoingPacket << flipped;
	networkManager.SendDataToNetworkManager(opponentNetworkManagerIndex, networkListenerIndex, PacketCode::Flip, outgoingPacket);
}

void OnlinePlayer::SendUpdateDataToNetwork(int health) {
	sf::Packet outgoingPacket;
	outgoingPacket << health;
	networkManager.SendDataToNetworkManager(opponentNetworkManagerIndex, networkListenerIndex, PacketCode::Health, outgoingPacket);
}



int OnlinePlayer::getPlayerNum() { return playerNum; }