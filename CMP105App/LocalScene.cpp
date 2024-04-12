#include "LocalScene.h"
#include "EndScreen.h"
#include "Player.h"

LocalScene::LocalScene(sf::RenderWindow* hwnd, Input* in, SceneManager& sm) : sceneManager(sm)
{
	std::cout << "Loading test scene\n";

	window = hwnd;
	input = in;
	debugMode = false;

	InitialiseScene();
	InitialisePlayers();
	InitialiseHealthBars();

	std::cout << "Loaded test scene\n";
}


LocalScene::~LocalScene()
{
	std::cout << "Unloading test scene\n";
	delete players[0];
	delete players[1];
	std::cout << "Unloaded test scene\n";

}


void LocalScene::InitialiseScene() {
	background.setSize(sf::Vector2f(window->getSize()));
	if (!bgTexture.loadFromFile("Assets/Background/background.png")) { std::cerr << "Couldn't load background for fight scene\n"; }
	background.setFillColor(sf::Color::White);
	background.setTexture(&bgTexture);

	platforms[0] = Platform(450, 475, 300, 25, true);
	platforms[1] = Platform(1070, 475, 300, 25, true);
	platforms[2] = Platform(200, 700, 1520, 25, false); //Ground

	//audioManager.playMusicbyName("GuileTheme");
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
	players[0]->setPosition(300, 500);
	players[1]->setPosition(1300, 500);
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
	players[0]->handleInput(dt, sf::Keyboard::Space, sf::Keyboard::A, sf::Keyboard::D, sf::Keyboard::S, sf::Keyboard::R, sf::Keyboard::F, sf::Keyboard::T, sf::Keyboard::G);
	players[1]->handleInput(dt, sf::Keyboard::O, sf::Keyboard::K, sf::Keyboard::Semicolon, sf::Keyboard::L, sf::Keyboard::LBracket, sf::Keyboard::Quote, sf::Keyboard::RBracket, sf::Keyboard::Tilde);

	if (input->isKeyDown(sf::Keyboard::F3)) {
		debugMode = true;
	}
	else if (input->isKeyDown(sf::Keyboard::F4)) {
		debugMode = false;
	}
}



void LocalScene::update(float dt) {
	players[0]->update(dt);
	players[1]->update(dt);

	//Loop through both players
	for (int playerIndex{}; playerIndex < 2; ++playerIndex) {

		Player* p1 = players[playerIndex];
		Player* p2 = players[1 - playerIndex];

		PlatformCollisionCheck(p1);

		//Defending player has stun frames left, continue to the next player
		if (p1->getStunFramesLeft())
			continue;
		else {
			AttackHitboxCheck(p1, p2);
		}

	}
	HealthBarUpdate();
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
	for (int limbIndex{}; limbIndex < 4; ++limbIndex) {

		//Hitbox isn't colliding, continue to next limb
		if (!defendingPlayer->getEffectiveCollider().intersects(attackingPlayer->getAttack(limbIndex).getHitbox().getGlobalBounds()))
			continue;

		
		//Apply damage to defending player
		int damageAmount = attackingPlayer->getAttack(limbIndex).getDamage();

		//If defending player is blocking, only apply 10% of the damage
		damageAmount *= (defendingPlayer->getBlocking() ? 0.1 : 1);

		//If attacking player's limb is broken, add an extra 20% damage to the attack
		damageAmount *= (attackingPlayer->getLimbActivity(limbIndex) ? 1 : 1.2);

		defendingPlayer->setHealth(defendingPlayer->getHealth() - damageAmount);
		defendingPlayer->setStunFramesLeft(defendingPlayer->getBlocking() ? 0 : attackingPlayer->getAttack(limbIndex).getHitstun()); //If defending player isn't blocking, give them hitstun


		//Apply knockback to defending player
		sf::Vector2f knockback = attackingPlayer->getAttack(limbIndex).getKnockback();

		//If defending player is blocking, only apply 30% of the knockback
		knockback.x *= (defendingPlayer->getBlocking() ? 0.3 : 1);
		knockback.y *= (defendingPlayer->getBlocking() ? 0.3 : 1);

		//If attacking player's limb is broken, add an extra 40% knockback to the attack
		knockback.x *= (attackingPlayer->getLimbActivity(limbIndex) ? 1 : 1.4);
		knockback.y *= (attackingPlayer->getLimbActivity(limbIndex) ? 1 : 1.4);

		//Increase knockback the lower the defending player's health is using linear interpolation
		float maxHorizontalKnockbackAmount { 3 * knockback.x };
		float maxVerticalKnockbackAmount{ 3 * knockback.y };
		float interp{ static_cast<float>(defendingPlayer->getHealth()) / defendingPlayer->getMaxHealth() - 1}; //Between 0 and 1

		//Lerp formula: k=a+(b-a)*t : when t=0 (health low), k=a. when t=1 (health high), k=b
		knockback.x += (knockback.x - maxHorizontalKnockbackAmount) * interp;
		knockback.y += (knockback.y - maxVerticalKnockbackAmount) * interp;
		std::cout << knockback.x << ' ' << knockback.y << '\n';
		
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
}


void LocalScene::HealthBarUpdate() {
	int Calc1 = 6 * players[0]->getHealth();
	int Calc2 = 6 * players[1]->getHealth();

	HealthBarFront1.setSize(sf::Vector2f(Calc1, HealthBarFront1.getSize().y));
	HealthBarFront2.setSize(sf::Vector2f(Calc2, HealthBarFront1.getSize().y));
	HealthBarFront2.setPosition((1920 - Calc2 - 37), 37);

	if (players[0]->getHealth() <= 0 || players[1]->getHealth() <= 0) {
		EndScreen* endScreen = new EndScreen(window, input, sceneManager, players[0]->getHealth() > 0);
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
	for (int i{ 0 }; i < 3; ++i) {
		window->draw(platforms[i]);
	}

	if (debugMode) {
		DebugRender();
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