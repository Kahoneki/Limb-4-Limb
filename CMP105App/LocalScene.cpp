#include "LocalScene.h"
#include "EndScreen.h"
#include "Player.h"
#include "NetworkScene.h"
#include "MatchInvitationInterrupt.h"

LocalScene::LocalScene(sf::RenderWindow* hwnd, Input* in, SceneManager& sm) : sceneManager(sm), pausePopup(in), matchInvitationInterrupt(MatchInvitationInterrupt::getInstance())
{
	std::cout << "Loading local scene\n";

	window = hwnd;
	input = in;

	debugMode = false;
	timeUntilPlayersShouldStartUpdate = 0.5f;
	playerStartUpdateTimeCountdown = timeUntilPlayersShouldStartUpdate;
	minItemBoxCooldownTime = 15;
	maxItemBoxCooldownTime = 30;
	timeUntilNextItemBox = rand() % static_cast<int>(maxItemBoxCooldownTime - minItemBoxCooldownTime + 1) + minItemBoxCooldownTime;

	itemBox = nullptr;

	InitialiseScene();
	InitialisePlayers();
	InitialiseHealthBars();

	std::cout << "Loaded local scene\n";
}


LocalScene::~LocalScene()
{
	std::cout << "Unloading local scene\n";
	delete players[0];
	delete players[1];
	if (itemBox != nullptr) { delete itemBox; }
	std::cout << "Unloaded local scene\n";

}


void LocalScene::InitialiseScene() {
	background.setSize(sf::Vector2f(window->getSize()));
	if (!bgTexture.loadFromFile("Assets/Background/background.png")) { std::cerr << "Couldn't load background for fight scene\n"; }
	background.setFillColor(sf::Color::White);
	background.setTexture(&bgTexture);

	platforms[0] = Platform(450, 675, 300, 25, true);   //Bottom left
	platforms[1] = Platform(1170, 675, 300, 25, true);  //Bottom right
	platforms[2] = Platform(800, 475, 320, 25, true);   //Top
	platforms[3] = Platform(200, 875, 1520, 25, false); //Ground

	audioManager.playMusicbyName("GuileTheme");
}


void LocalScene::InitialisePlayers() {
	//Size should be roughly in ratio 5:9
	players[0] = new Player(sf::Vector2f(56, 103), 3300.0f, 600.0f, 1350.0f, 100, 100, 0, false);
	players[1] = new Player(sf::Vector2f(56, 103), 3300.0f, 600.0f, 1350.0f, 100, 100, 0, true);

	for (Player* player : players) {
		player->setInput(input);
		player->setHealth(100);
		player->setOrigin(player->getLocalBounds().width / 2.f, player->getLocalBounds().height / 2.f);
	}
	players[0]->setPosition(325, 800);
	players[1]->setPosition(1270, 800);
	players[1]->setScale(-1.0f, 1.0f);
	players[1]->setFillColor(sf::Color::Red);
}


void LocalScene::InitialiseHealthBars() {
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



void LocalScene::handleInput(float dt) {

	sf::Vector2f mousePos{ window->mapPixelToCoords(sf::Mouse::getPosition(*window)) };

	if (matchInvitationInterrupt.getInvitationReceived()) {
		matchInvitationInterrupt.processEvents(mousePos);
	}
	else {
		pausePopup.processEvents(mousePos);
	}

	if (!pausePopup.getPausePopupEnabled() && !matchInvitationInterrupt.getInvitationReceived()) {
		players[0]->handleInput(dt, sf::Keyboard::W, sf::Keyboard::A, sf::Keyboard::D, sf::Keyboard::S, sf::Keyboard::LShift, sf::Keyboard::R, sf::Keyboard::F, sf::Keyboard::T, sf::Keyboard::G);
		players[1]->handleInput(dt, sf::Keyboard::O, sf::Keyboard::K, sf::Keyboard::Semicolon, sf::Keyboard::L, sf::Keyboard::N, sf::Keyboard::LBracket, sf::Keyboard::Quote, sf::Keyboard::RBracket, sf::Keyboard::Tilde);

		if (input->isKeyDown(sf::Keyboard::F3)) {
			debugMode = true;
		}
		else if (input->isKeyDown(sf::Keyboard::F4)) {
			debugMode = false;
		}
	}
}



void LocalScene::update(float dt) {

	if (matchInvitationInterrupt.getReadyToLoadScene()) {
		//Open network scene to start match
		NetworkScene* networkScene{ new NetworkScene(window, input, sceneManager, matchInvitationInterrupt.getPlayerNum(), matchInvitationInterrupt.getNetworkManagerIndex()) };
		sceneManager.LoadScene(networkScene);
		return;
	}

	if (pausePopup.getMainMenuButtonClicked()) {
		MainMenu* mainMenu = new MainMenu(window, input, sceneManager);
		sceneManager.LoadScene(mainMenu);
	}

	if (pausePopup.getPausePopupEnabled()) {
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

		Player* p1 = players[playerIndex];	   //Defending player
		Player* p2 = players[1 - playerIndex]; //Attacking player

		PlatformCollisionCheck(p1);

		//Defending player has invincibility frames left, continue to the next player
		if (p1->getInvincibilityFramesLeft())
			continue;
		else {
			AttackHitboxCheck(p1, p2);
		}

	}


	//Handle item box
	//Update item box if it exists - need to check nullptr each time since all of these cases might result in the item box being deleted
	if (itemBox != nullptr) {
		itemBox->update(dt);
	}
	if (itemBox != nullptr) {
		ItemBoxCollisionCheck(players[0]);
	}
	if (itemBox != nullptr) {
		ItemBoxCollisionCheck(players[1]);
	}
	if (itemBox != nullptr) {
		//Off screen boundary check for item box
		if (itemBox->getPosition().y + itemBox->getSize().y > 1920) {
			delete itemBox;
			itemBox = nullptr;
		}
	}
	else {
		timeUntilNextItemBox -= dt;
		if (timeUntilNextItemBox <= 0) {
			itemBox = new ItemBox();
			timeUntilNextItemBox = rand() % static_cast<int>(maxItemBoxCooldownTime - minItemBoxCooldownTime + 1) + minItemBoxCooldownTime;
		}
	}

	HealthBarUpdate();
}


void LocalScene::ItemBoxCollisionCheck(Player* player) {
	if (player->getEffectiveCollider().intersects(itemBox->getGlobalBounds()) && !player->getHasEffect()) {
		itemBox->ApplyToPlayer(*player);
		delete itemBox;
		itemBox = nullptr;
	}
}


void LocalScene::PlatformCollisionCheck(Player* player) {
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


void LocalScene::AttackHitboxCheck(Player* defendingPlayer, Player* attackingPlayer) {

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

void LocalScene::ApplyKnockbackToDefendingPlayer(Player* defendingPlayer, Player* attackingPlayer, int limbIndex) {

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

void LocalScene::ApplyKnockbackToAttackingPlayer(Player* defendingPlayer, Player* attackingPlayer, int limbIndex) {

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



void LocalScene::HealthBarUpdate() {
	int Calc1 = 6 * players[0]->getHealth();
	int Calc2 = 6 * players[1]->getHealth();

	HealthBarFront1.setSize(sf::Vector2f(Calc1, HealthBarFront1.getSize().y));
	HealthBarFront2.setSize(sf::Vector2f(Calc2, HealthBarFront1.getSize().y));
	HealthBarFront2.setPosition((1920 - Calc2 - 37), 37);

	if (players[0]->getHealth() <= 0 || players[1]->getHealth() <= 0) {
		std::string resultText{ (players[0]->getHealth() > 0) ? "PLAYER 1 WINS!" : "PLAYER 2 WINS!" };
		EndScreen* endScreen = new EndScreen(window, input, sceneManager, true, resultText.c_str());
		sceneManager.LoadScene(endScreen);
	}
}



void LocalScene::render()
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

	if (itemBox != nullptr) {
		window->draw(*itemBox);
	}

	if (debugMode) {
		DebugRender();
	}

	if (pausePopup.getPausePopupEnabled()) {
		window->draw(pausePopup);
	}

	if (matchInvitationInterrupt.getInvitationReceived()) {
		//Match invitation has been received
		window->draw(matchInvitationInterrupt);
	}

	endDraw();
}


void LocalScene::DebugRender() {
	for (Player* player : players) {
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