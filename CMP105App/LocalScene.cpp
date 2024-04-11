#include "LocalScene.h"
#include "EndScreen.h"
#include "NetworkManager.h"
#include <cmath> //For min and max

LocalScene::LocalScene(sf::RenderWindow* hwnd, Input* in, SceneManager& sm) : sceneManager(sm)
{
	std::cout << "Loading test scene\n";

	window = hwnd;
	input = in;

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

void LocalScene::handleInput(float dt) {
	players[0]->handleInput(dt, sf::Keyboard::W, sf::Keyboard::A, sf::Keyboard::D, sf::Keyboard::S, sf::Keyboard::R, sf::Keyboard::F, sf::Keyboard::T, sf::Keyboard::G);
	players[1]->handleInput(dt, sf::Keyboard::O, sf::Keyboard::K, sf::Keyboard::Semicolon, sf::Keyboard::L, sf::Keyboard::LBracket, sf::Keyboard::Quote, sf::Keyboard::RBracket, sf::Keyboard::Tilde);
}



void LocalScene::update(float dt) {
	players[0]->update(dt);
	players[1]->update(dt);
	FlipCheck();

	//Loop through both players
	for (int playerIndex{}; playerIndex < 2; ++playerIndex) {
		
		Player* p1 = players[playerIndex];		
		
		//Loop through all platforms
		//for (Platform platform : platforms) {

			//Platform collision check - note: player origin is in centre of sprite, platform origin is at top left of sprite

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
			if (!p1->getOnPlatform()) {
				if (!p1->getFallingThroughPlatform()) {
					if (p1->getGlobalBounds().intersects(platform.getGlobalBounds())) {
						float playerBottom{ p1->getPosition().y + p1->getSize().y / 2 };
						float playerTop{ p1->getPosition().y - p1->getSize().y / 2 };
						float playerRight{ p1->getPosition().x + p1->getSize().x / 2 };
						float playerLeft{ p1->getPosition().x - p1->getSize().x / 2 };
						float platformBottom{ platform.getPosition().y + platform.getSize().y };
						float platformTop{ platform.getPosition().y };
						float platformRight{ platform.getPosition().x + platform.getSize().x };
						float platformLeft{ platform.getPosition().x };
						float playerCenterX{ p1->getPosition().x };
						float playerCenterY{ p1->getPosition().y };
				
						float epsilon{ 1.0f }; //Player will intersect top of platform, if the amount they intersect it by is any less than epsilon, snap them on top
						if (playerBottom >= platformTop && playerBottom-epsilon < platformTop) {
							std::cout << playerBottom << ' ' << platformTop << ' ' << playerCenterY << ' ' << platformTop << '\n';
							//Colliding on top side
							if (p1->getVelocity().y < 0) {
								//Travelling downwards
								p1->setPosition(sf::Vector2f(p1->getPosition().x, platformTop - p1->getSize().y / 2));
								p1->setVelocity(sf::Vector2f(p1->getVelocity().x, 0));
								p1->setOnPlatform(true);
								p1->setGrounded(true);
							}
						}
						if (!platform.getPassable()) {
							if (playerRight >= platformLeft && playerCenterX < platformLeft) {
								//Colliding on left side
								p1->setPosition(sf::Vector2f(platformLeft - p1->getSize().x / 2, p1->getPosition().y));
								p1->setVelocity(sf::Vector2f(0, p1->getVelocity().y));
							}
							else if (playerLeft <= platformRight && playerCenterX > platformRight) {
								//Colliding on right side
								p1->setPosition(sf::Vector2f(platformRight + p1->getSize().x / 2, p1->getPosition().y));
								p1->setVelocity(sf::Vector2f(0, p1->getVelocity().y));
							}
							else if (playerTop <= platformBottom && playerCenterY > platformBottom) {
								//Colliding on bottom side
								p1->setPosition(sf::Vector2f(p1->getPosition().x, platformBottom + p1->getSize().y / 2));
								p1->setVelocity(sf::Vector2f(p1->getVelocity().x, 0));
							}
						}
					}
				}
				//Check if player has let go of crouch while falling through platform
				if (p1->getFallingThroughPlatform() && !p1->getCrouched()) {
					//Wait until player has fully made it through the platform or has fully made it above the platform before disabling fallingThroughPlatform.
					//If player starts crouching again in this time, they will continue to fall through platforms
					if (p1->getPosition().y - p1->getSize().y / 2 > platform.getPosition().y + platform.getSize().y || //Player top lower than platform bottom - player has fallen through
						p1->getPosition().y + p1->getSize().y / 2 < platform.getPosition().y) {						   //Player bottom higher than platform top - player has made it above
						p1->setFallingThroughPlatform(false);
					}
				}
			}
			else {
				//Check if player has walked off platform
				if (p1->getPosition().x + p1->getSize().x/2 < platform.getPosition().x || p1->getPosition().x - p1->getSize().x/2 > platform.getPosition().x + platform.getSize().x) {
					p1->setGrounded(false);
					p1->setOnPlatform(false);
				}
				//Check if platform is passable and player is crouching - in which case, they should fall through the platform
				if (platform.getPassable() && p1->getCrouched()) {
					p1->setFallingThroughPlatform(true);
					p1->setGrounded(false);
					p1->setOnPlatform(false);
				}
			}
		
			//Check if player has jumped
			if (!p1->getGrounded()) {
				p1->setOnPlatform(false);
			}
		
		//}


		
		//Attack hitbox collision check
		//p1 is defending player
		Player* p2 = players[1 - playerIndex]; //p2 is attacking player

		//Defending player has stun frames left, continue to the next player
		if (p1->getStunFramesLeft())
			continue;

		//Loop through all limbs to see if hitboxes are colliding
		for (int limbIndex{}; limbIndex < 4; ++limbIndex) {

			//Hitbox isn't colliding, continue to next limb
			if (!p1->getGlobalBounds().intersects(p2->getAttack(limbIndex).getHitbox().getGlobalBounds()))
				continue;

			int damageAmount = p2->getAttack(limbIndex).getDamage();
			damageAmount *= (p1->getBlocking() ? 0.1 : 1); //If defending player is blocking, only apply 10% of the damage
			damageAmount *= (p2->getLimbActivity(limbIndex) ? 1.2 : 1); //If attacking player's limb is broken, add an extra 20% damage to the attack

			p1->setHealth(p1->getHealth() - damageAmount);
			p1->setStunFramesLeft(p1->getBlocking() ? 0 : p2->getAttack(limbIndex).getHitstun()); //If defending player isn't blocking, give them hitstun

			//Move both players away from each other a bit to stop attacking player from being able to spam attacks due to player 1's hitstun
			p1->move(sf::Vector2f(-10 + (20 * p1->getFlipped()), 0));
			p2->move(sf::Vector2f(-10 + (20 * p2->getFlipped()), 0));
		}
	}
	HealthBarUpdate();

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
	/*for (int i{ 0 }; i < 2; ++i) {
		window->draw(platforms[i]);
	}*/
	window->draw(platform);

	endDraw();
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


void LocalScene::FlipCheck() {
	bool playersFacingOppositeDirections{ (players[0]->getScale().x == 1 && players[0]->getPosition().x > players[1]->getPosition().x) ||
										   (players[0]->getScale().x == -1 && players[0]->getPosition().x < players[1]->getPosition().x) };
	if (playersFacingOppositeDirections) {
		players[0]->setFlipped(!players[0]->getFlipped());
		players[0]->setScale(-players[0]->getScale().x, 1);
		players[1]->setFlipped(!players[1]->getFlipped());
		players[1]->setScale(-players[1]->getScale().x, 1);
	}
}



void LocalScene::InitialiseScene() {
	background.setSize(sf::Vector2f(window->getSize()));
	if (!bgTexture.loadFromFile("Assets/Background/background.png")) { std::cerr << "Couldn't load background for fight scene\n"; }
	background.setFillColor(sf::Color::White);
	background.setTexture(&bgTexture);

	//platforms[0] = Platform(500, 700, 400, 25, true);
	platform = Platform(500, 450, 400, 25, true);

	//audioManager.playMusicbyName("GuileTheme");
}



void LocalScene::InitialisePlayers() {
	players[0] = new Player(3300.0f, 600.0f, 1350.0f, 100, 100, 0, false);
	players[1] = new Player(3300.0f, 600.0f, 1350.0f, 100, 100, 0, true);

	for (Player* player : players) {
		player->setSize(sf::Vector2f(112, 206));
		player->setInput(input);
		player->setHealth(100);
		player->setOrigin(player->getLocalBounds().width / 2.f, player->getLocalBounds().height / 2.f);
	}
	players[0]->setPosition(225, 562);
	players[1]->setPosition(1462, 562);
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

