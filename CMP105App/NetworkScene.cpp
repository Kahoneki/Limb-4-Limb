#include "NetworkScene.h"
#include "EndScreen.h"
#include "OnlinePlayer.h"
#include "NetworkManager.h"
#include "AccountManager.h"

NetworkScene::NetworkScene(sf::RenderWindow* hwnd, Input* in, SceneManager& sm, int pn, int oppNMI) : sceneManager(sm), networkManager(NetworkManager::getInstance(true)), timeManager(TimeManager::getInstance(240)), pausePopup(in)
{
	std::cout << "Loading network scene\n";

	window = hwnd;
	input = in;
	playerNum = pn;
	opponentNetworkManagerIndex = oppNMI;
	timeUntilPlayersShouldStartUpdate = 0.5f;
	playerStartUpdateTimeCountdown = timeUntilPlayersShouldStartUpdate;

	opponentNetworkManagerIndex = oppNMI;
	matchStart = false;

	matchEnd = false;
	winningPlayerNMI = -1;
	updatedRanking = -1;

	networkListener = networkManager.GenerateNetworkListener(*this, NetworkManager::ReservedEntityIndexTable::NETWORK_SCENE);

	InitialiseScene();
	InitialisePlayers();
	InitialiseHealthBars();

	//Scene has finished loading, send MatchSceneLoaded packet to server and halt until opponent's scene has also loaded
	sf::Packet outgoingPacket;
	outgoingPacket << true;
	networkManager.SendDataToServer(-1, PacketCode::MatchSceneLoaded, outgoingPacket);
	while (!matchStart) {
		networkManager.CheckForIncomingDataFromServer();
	}
	//Opponent's scene has loaded

	std::cout << "YOU ARE: PLAYER NUM " << playerNum << ", NMI: " << networkManager.getNetworkManagerIndex() << '\n';
	std::cout << "OPPONENT IS: PLAYER NUM " << ((playerNum == 1) ? 2 : 1) << ", NMI: " << opponentNetworkManagerIndex << '\n';

	std::cout << "Loaded network scene\n";
}


NetworkScene::~NetworkScene()
{
	std::cout << "Unloading network scene\n";
	delete players[0];
	delete players[1];
	std::cout << "Unloaded network scene\n";

}


void NetworkScene::InitialiseScene() {
	background.setSize(sf::Vector2f(window->getSize()));
	if (!bgTexture.loadFromFile("Assets/Background/background.png")) { std::cerr << "Couldn't load background for fight scene\n"; }
	background.setFillColor(sf::Color::White);
	background.setTexture(&bgTexture);

	platforms[0] = Platform(450, 675, 300, 25, true);   //Bottom left
	platforms[1] = Platform(1170, 675, 300, 25, true);  //Bottom right
	platforms[2] = Platform(800, 475, 320, 25, true);   //Top
	platforms[3] = Platform(200, 875, 1520, 205, false); //Ground

	//audioManager.playMusicbyName("GuileTheme");
}


void NetworkScene::InitialisePlayers() {
	//Size should be roughly in ratio 5:9
	bool player1Local{ playerNum == 1 };
	bool player2Local{ playerNum == 2 };
	players[0] = new OnlinePlayer(sf::Vector2f(56, 103), 3300.0f, 600.0f, 1350.0f, 100, 100, 0, false, 1, player1Local, player1Local ? opponentNetworkManagerIndex : -1);
	players[1] = new OnlinePlayer(sf::Vector2f(56, 103), 3300.0f, 600.0f, 1350.0f, 100, 100, 0, true, 2, player2Local, player2Local ? opponentNetworkManagerIndex : -1);

	for (OnlinePlayer* player : players) {
		player->setInput(input);
		player->setHealth(100);
		player->setOrigin(player->getLocalBounds().width / 2.f, player->getLocalBounds().height / 2.f);
	}
	players[0]->setPosition(325, 800);
	players[1]->setPosition(1270, 800);
	players[1]->setScale(-1.0f, 1.0f);
	players[1]->setFillColor(sf::Color::Red);
}


void NetworkScene::InitialiseHealthBars() {
	HealthBarFront1.setSize(sf::Vector2f(600, 75));
	HealthBarFront1.setPosition(37, 37);
	HealthBarFront1.setFillColor(sf::Color::Green);

	HealthBarBack1.setSize(sf::Vector2f(600, 75));
	HealthBarBack1.setPosition(37, 37);
	HealthBarBack1.setFillColor(sf::Color::Red);

	HealthBarFront2.setSize(sf::Vector2f(600, 75));
	HealthBarFront2.setPosition(1282, 37);
	HealthBarFront2.setFillColor(sf::Color::Green);

	HealthBarBack2.setSize(sf::Vector2f(600, 75));
	HealthBarBack2.setPosition(1282, 37);
	HealthBarBack2.setFillColor(sf::Color::Red);
}



void NetworkScene::handleInput(float dt) {

	sf::Vector2f mousePos{ window->mapPixelToCoords(sf::Mouse::getPosition(*window)) };
	pausePopup.processEvents(mousePos);

	//If pause popup is enabled, set all keys to not-pressed before handling input
	if (pausePopup.getPausePopupEnabled()) {
		input->setKeyUp(sf::Keyboard::Space);
		input->setKeyUp(sf::Keyboard::A);
		input->setKeyUp(sf::Keyboard::D);
		input->setKeyUp(sf::Keyboard::S);
		input->setKeyUp(sf::Keyboard::LShift);
		input->setKeyUp(sf::Keyboard::Semicolon);
		input->setKeyUp(sf::Keyboard::LBracket);
		input->setKeyUp(sf::Keyboard::RBracket);
		input->setKeyUp(sf::Keyboard::Enter);
	}
	players[playerNum - 1]->handleInput(dt, sf::Keyboard::Space, sf::Keyboard::A, sf::Keyboard::D, sf::Keyboard::S, sf::Keyboard::LShift, sf::Keyboard::Semicolon, sf::Keyboard::LBracket, sf::Keyboard::RBracket, sf::Keyboard::Enter);
	players[1 - (playerNum - 1)]->handleInput(dt, sf::Keyboard::Space, sf::Keyboard::A, sf::Keyboard::D, sf::Keyboard::S, sf::Keyboard::LShift, sf::Keyboard::Semicolon, sf::Keyboard::LBracket, sf::Keyboard::RBracket, sf::Keyboard::Enter);

	if (input->isKeyDown(sf::Keyboard::F3)) {
		debugMode = true;
	}
	else if (input->isKeyDown(sf::Keyboard::F4)) {
		debugMode = false;
	}
}



void NetworkScene::update(float dt) {

	if (pausePopup.getMainMenuButtonClicked()) {

		//Local player is leaving, send MatchLeave packet to server (online player will win)
		sf::Packet emptyPacket;
		networkManager.SendDataToServer(NetworkManager::ReservedEntityIndexTable::NETWORK_SCENE, PacketCode::MatchLeave, emptyPacket);

		MainMenu* mainMenu = new MainMenu(window, input, sceneManager);
		sceneManager.LoadScene(mainMenu);
		return;
	}

	if (matchEnd) {
		std::cout << "Winner: " << (winningPlayerNMI == opponentNetworkManagerIndex) ? "Opponent. :|\n" : "Me! :]\n";
		std::cout << "New ranking: " << updatedRanking << '\n';
		AccountManager accountManager{ AccountManager::getInstance() };
		accountManager.setValues(accountManager.getUsername(), updatedRanking);

		//Go to end screen
		bool localPlayerWon{ winningPlayerNMI != opponentNetworkManagerIndex };
		std::string resultText{ localPlayerWon ? "YOU WON! :]" : "YOU LOST! :[" };
		resultText += "\nYOUR RANKING IS NOW: ";
		resultText += std::to_string(updatedRanking);
		resultText += (localPlayerWon ? " (+30)" : " (-30)");
		EndScreen* endScreen{ new EndScreen(window, input, sceneManager, false, resultText) };
		sceneManager.LoadScene(endScreen);
		return;
	}

	if (playerStartUpdateTimeCountdown > 0) {
		playerStartUpdateTimeCountdown -= dt;
	}
	else {
		players[0]->update(dt);
		players[1]->update(dt);
	}

	//Loop through both players
	for (int playerIndex{}; playerIndex < 2; ++playerIndex) {

		OnlinePlayer* p1 = players[playerIndex];	 //Defending player
		OnlinePlayer* p2 = players[1 - playerIndex]; //Attacking player

		bool isGroundedBeforePlatformCollisionCheck{ p1->getGrounded() };
		PlatformCollisionCheck(p1);
		bool isGroundedAfterPlatformCollisionCheck{ p1->getGrounded() };
		bool playerIsLocal{ p1->getPlayerNum() == playerNum };

		if (isGroundedBeforePlatformCollisionCheck != isGroundedAfterPlatformCollisionCheck && playerIsLocal) {
			//Send new grounded data to opponent
			sf::Packet outgoingPacket;
			outgoingPacket << isGroundedAfterPlatformCollisionCheck;
			networkManager.SendDataToNetworkManager(opponentNetworkManagerIndex, (playerNum == 1) ? NetworkManager::ReservedEntityIndexTable::PLAYER_1 : NetworkManager::ReservedEntityIndexTable::PLAYER_2, PacketCode::Grounded, outgoingPacket);
		}


		//Defending player has invincibility frames left, continue to the next player
		if (p1->getInvincibilityFramesLeft())
			continue;
		else {
			AttackHitboxCheck(p1, p2);
		}

	}
	HealthBarUpdate();
}


void NetworkScene::PlatformCollisionCheck(OnlinePlayer* player) {
	
	//Loop through all platforms
	for (int i{ 0 }; i < sizeof(platforms) / sizeof(platforms[0]); ++i) {

		//Logic:
		//
		//If player tries to hit the left, right, or bottom of platform, don't let them
		//Logic for jumping on top of the platform / off the top of the platform:
		//Player jumps, isGrounded=false
		//Player intersects with top of platform, isOnPlatform=true, isGrounded=true, intersection checks no longer done
		//If player either jumps or walks off of platform, isGrounded=false (jumping already disables isGrounded in player class so this code only needs to update it if player walks off)
		//If isGrounded=false, isOnPlatform=false
		//
		//If player crouches while isOnPlatform=true, isFallingThroughPlatform=true, onPlatform=false, isGrounded=false (player will start falling)
		//If isFallingThroughPlatform=true, don't perform any collision checks
		//If isFallingThroughPlatform=true and isCrouched=false, check if player has fallen all the way through the platform, and if they have, isFallingThroughPlatform=false
		//The above logic allows the player to go through a platform by simply tapping crouch once before falling on another platform below
		//^(provided said platform is below the initial platform by at least one player height)^
		//Alternatively, the player can continue holding crouch to fall through all platforms below them while they hold crouch

		float playerBottom{ player->getEffectiveCollider().top + player->getEffectiveCollider().height };
		float playerTop{ player->getEffectiveCollider().top };
		float playerRight{ player->getEffectiveCollider().left + player->getEffectiveCollider().width };
		float playerLeft{ player->getEffectiveCollider().left };
		float platformBottom{ platforms[i].getPosition().y + platforms[i].getSize().y };
		float platformTop{ platforms[i].getPosition().y };
		float platformRight{ platforms[i].getPosition().x + platforms[i].getSize().x };
		float platformLeft{ platforms[i].getPosition().x };
		float playerCenterX{ player->getPosition().x };
		float playerCenterY{ player->getPosition().y };
		if (!player->getOnPlatform()) {
			if (!player->getFallingThroughPlatform()) {
				if (player->getEffectiveCollider().intersects(platforms[i].getGlobalBounds())) {

					if (playerBottom >= platformTop && playerBottom < platformBottom) {
						//Colliding on top side
						if (player->getVelocity().y < 0) {
							//Travelling downwards
							player->setPosition(sf::Vector2f(player->getPosition().x, platformTop - player->getSize().y / 2));
							player->setVelocity(sf::Vector2f(player->getVelocity().x, 0));
							player->setOnPlatform(true);
							player->setGrounded(true);
							player->setCurrentPlatorm(i);
						}
					}
					if (!platforms[i].getPassable()) {
						if (playerRight >= platformLeft && playerCenterX < platformLeft) {
							//Colliding on left side
							player->setPosition(sf::Vector2f(platformLeft - player->getSize().x / 2, player->getPosition().y));
							player->setVelocity(sf::Vector2f(0, player->getVelocity().y));
						}
						else if (playerLeft <= platformRight && playerCenterX > platformRight) {
							//Colliding on right side
							player->setPosition(sf::Vector2f(platformRight + player->getSize().x / 2, player->getPosition().y));
							player->setVelocity(sf::Vector2f(0, player->getVelocity().y));
						}
						else if (playerTop <= platformBottom && playerCenterY > platformBottom) {
							//Colliding on bottom side
							player->setPosition(sf::Vector2f(player->getPosition().x, platformBottom + player->getSize().y / 2));
							player->setVelocity(sf::Vector2f(player->getVelocity().x, 0));
						}
					}
				}
			}
			//Check if player is currently in "im falling through platforms" mode and they encounter a non-passable platform, in which case - stop their descent
			else if (player->getEffectiveCollider().intersects(platforms[i].getGlobalBounds()) && !platforms[i].getPassable()) {
				player->setFallingThroughPlatform(false);
				player->setPosition(sf::Vector2f(player->getPosition().x, platformTop - player->getSize().y / 2));
				player->setVelocity(sf::Vector2f(player->getVelocity().x, 0));
				player->setOnPlatform(true);
				player->setGrounded(true);
				player->setCurrentPlatorm(i);
			}
			//Check if player has let go of crouch while falling through platform
			else if (!player->getCrouched() && player->getCurrentPlatform() == i) {
				//Wait until player has fully made it through the platform or has fully made it above the platform before disabling fallingThroughPlatform.
				//If player starts crouching again in this time, they will continue to fall through platforms
				//Check if player top lower than platform bottom - player has fallen through; or if player bottom higher than platform top - player has made it above
				if (playerTop > platformBottom || playerBottom < platformTop) {
					player->setFallingThroughPlatform(false);
				}
			}
		}
		else {
			//Check if player has walked off platform
			bool playerRightIsLeftOfPlatformLeft{ playerRight < platformLeft };
			bool playerLeftIsRightOfPlatformRight{ playerLeft > platformRight };
			if ((playerRightIsLeftOfPlatformLeft || playerLeftIsRightOfPlatformRight) && player->getCurrentPlatform() == i) {
				player->setGrounded(false);
				player->setOnPlatform(false);
			}
			//Check if platform is passable and player is crouching - in which case, they should fall through the platform
			if (platforms[i].getPassable() && player->getCrouched() && (player->getCurrentPlatform() == i)) {
				player->setFallingThroughPlatform(true);
				player->setGrounded(false);
				player->setOnPlatform(false);
			}
		}

		//Check if player has jumped
		if (!player->getGrounded()) {
			player->setOnPlatform(false);
		}
	}
}


void NetworkScene::AttackHitboxCheck(OnlinePlayer* defendingPlayer, OnlinePlayer* attackingPlayer) {

	//Loop through all limbs to see if hitboxes are colliding
	for (int limbIndex{ 0 }; limbIndex < 4; ++limbIndex) {

		Attack& attack{ attackingPlayer->getAttack(limbIndex) };

		//If attacking player is attacking, attack is in first frame of active cycle, and self knockback hasn't been applied, apply self knockback regardless of whether attack is hitting defending player
		if (attack.getAttacking() && (attack.getCounter() >= attack.getStartup() + 1 && attack.getCounter() <= attack.getStartup() + 2) && !attack.getSelfKnockbackApplied()) {
			ApplyKnockbackToAttackingPlayer(defendingPlayer, attackingPlayer, limbIndex);
			attack.setSelfKnockbackApplied(true);
		}
		else if (!attack.getAttacking()) {
			attack.setSelfKnockbackApplied(false);
		}

		//Hitbox isn't colliding, continue to next limb
		if (!defendingPlayer->getEffectiveCollider().intersects(attack.getHitbox().getGlobalBounds()))
			continue;


		//Apply damage to defending player
		int damageAmount = attack.getDamage();

		//If defending player is blocking, only apply 10% of the damage
		damageAmount *= (defendingPlayer->getBlocking() ? 0.1 : 1);

		//If attacking player's limb is broken, add an extra 20% damage to the attack
		damageAmount *= (attackingPlayer->getLimbActivity(limbIndex) ? 1 : 1.2);

		defendingPlayer->setHealth(defendingPlayer->getHealth() - damageAmount);
		defendingPlayer->setInvincibilityFramesLeft(defendingPlayer->getBlocking() ? 0 : attackingPlayer->getAttack(limbIndex).getInvincibilityFrames()); //If defending player isn't blocking, give them invincibilityFrames


		//Apply knockback to defending player if attack is in active cycle
		if (attack.getCounter() >= attack.getStartup() + 1 && attack.getCounter() <= attack.getActive()) {
			ApplyKnockbackToDefendingPlayer(defendingPlayer, attackingPlayer, limbIndex);
		}

	}
}

void NetworkScene::ApplyKnockbackToDefendingPlayer(OnlinePlayer* defendingPlayer, OnlinePlayer* attackingPlayer, int limbIndex) {

	sf::Vector2f knockback = attackingPlayer->getAttack(limbIndex).getKnockback();

	//If defending player is blocking, only apply 30% of the knockback
	knockback.x *= (defendingPlayer->getBlocking() ? 0.3 : 1);
	knockback.y *= (defendingPlayer->getBlocking() ? 0.3 : 1);

	//If attacking player's limb is broken, add an extra 40% knockback to the attack
	knockback.x *= (attackingPlayer->getLimbActivity(limbIndex) ? 1 : 1.4);
	knockback.y *= (attackingPlayer->getLimbActivity(limbIndex) ? 1 : 1.4);

	//Increase knockback the lower the defending player's health is using linear interpolation
	float maxHorizontalKnockbackAmount{ 3 * knockback.x };
	float maxVerticalKnockbackAmount{ 3 * knockback.y };
	float interp{ static_cast<float>(defendingPlayer->getHealth()) / defendingPlayer->getMaxHealth() - 1 }; //Between 0 and 1

	//Lerp formula: k=a+(b-a)*t : when t=0 (health low), k=a. when t=1 (health high), k=b
	knockback.x += (knockback.x - maxHorizontalKnockbackAmount) * interp;
	knockback.y += (knockback.y - maxVerticalKnockbackAmount) * interp;

	//Knockback.x is positive, which will cause defending player to go to the right
	//If defending player is to the left of attacking player, they should be sent to the left, so flip knockback.x
	if (defendingPlayer->getPosition().x < attackingPlayer->getPosition().x) {
		knockback.x = -knockback.x;
	}

	defendingPlayer->setVelocity(defendingPlayer->getVelocity() + knockback);
	if (knockback.y > 0) {
		defendingPlayer->setGrounded(false);
	}
	defendingPlayer->setHasKnockback(true);
	//Adjust jump direction given new velocity
	if (defendingPlayer->getVelocity().x < 0) {
		defendingPlayer->setJumpDirection(-1);
	}
	else if (defendingPlayer->getVelocity().x > 0) {
		defendingPlayer->setJumpDirection(1);
	}
	else {
		defendingPlayer->setJumpDirection(0);
	}
}

void NetworkScene::ApplyKnockbackToAttackingPlayer(OnlinePlayer* defendingPlayer, OnlinePlayer* attackingPlayer, int limbIndex) {

	sf::Vector2f knockback = attackingPlayer->getAttack(limbIndex).getSelfKnockback();

	//Knockback.x is negative, which will cause attacking player to go to the left
	//If attacking player is facing to the right, they should be sent to the right, so flip knockback.x
	if (!attackingPlayer->getFlipped()) {
		knockback.x = -knockback.x;
	}

	attackingPlayer->setVelocity(attackingPlayer->getVelocity() + knockback);
	if (knockback.y > 0) {
		attackingPlayer->setGrounded(false);
	}
	attackingPlayer->setHasKnockback(true);

	//Adjust jump direction given new velocity
	if (attackingPlayer->getVelocity().x < 0) {
		attackingPlayer->setJumpDirection(-1);
	}
	else if (attackingPlayer->getVelocity().x > 0) {
		attackingPlayer->setJumpDirection(1);
	}
	else {
		attackingPlayer->setJumpDirection(0);
	}
}



void NetworkScene::HealthBarUpdate() {
	int Calc1 = 6 * players[0]->getHealth();
	int Calc2 = 6 * players[1]->getHealth();

	HealthBarFront1.setSize(sf::Vector2f(Calc1, HealthBarFront1.getSize().y));
	HealthBarFront2.setSize(sf::Vector2f(Calc2, HealthBarFront1.getSize().y));
	HealthBarFront2.setPosition((1920 - Calc2 - 37), 37);

	OnlinePlayer* nonLocalPlayer{ (playerNum == 1) ? players[1] : players[0] };
	if (nonLocalPlayer->getHealth() <= 0) {
		//Local player has won, send MatchWin packet to server
		sf::Packet emptyPacket;
		networkManager.SendDataToServer(NetworkManager::ReservedEntityIndexTable::NETWORK_SCENE, PacketCode::MatchWin, emptyPacket);
		//Stall until matchEnd is true
		while (!matchEnd) {
			networkManager.CheckForIncomingDataFromServer();
		}
	}
}



void NetworkScene::render()
{
	beginDraw();

	window->draw(background);

	window->draw(*players[0]);
	window->draw(*players[1]);
	for (int i{ 0 }; i < 4; ++i) {
		window->draw(players[0]->getAttack(i).getHitbox());
		window->draw(players[1]->getAttack(i).getHitbox());
	}

	window->draw(HealthBarBack1);
	window->draw(HealthBarBack2);
	window->draw(HealthBarFront1);
	window->draw(HealthBarFront2);
	for (int i{ 0 }; i < sizeof(platforms) / sizeof(platforms[0]); ++i) {
		window->draw(platforms[i]);
	}

	if (debugMode) {
		DebugRender();
	}

	if (pausePopup.getPausePopupEnabled()) {
		window->draw(pausePopup);
	}

	endDraw();
}


void NetworkScene::DebugRender() {
	for (OnlinePlayer* player : players) {
		sf::RectangleShape rect;
		rect.setSize(sf::Vector2f(player->getEffectiveCollider().width, player->getEffectiveCollider().height));
		rect.setPosition(player->getEffectiveCollider().left, player->getEffectiveCollider().top);
		rect.setFillColor(sf::Color(0, 0, 0, 0));
		rect.setOutlineColor(sf::Color::Red);
		rect.setOutlineThickness(4);
		window->draw(rect);
	}
	for (Platform platform : platforms) {
		sf::RectangleShape rect;
		rect.setSize(sf::Vector2f(platform.getGlobalBounds().width, platform.getGlobalBounds().height));
		rect.setPosition(platform.getGlobalBounds().left, platform.getGlobalBounds().top);
		rect.setFillColor(sf::Color(0, 0, 0, 0));
		rect.setOutlineColor(sf::Color::Blue);
		rect.setOutlineThickness(4);
		window->draw(rect);
	}
}