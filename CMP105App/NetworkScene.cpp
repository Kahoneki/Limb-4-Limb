#include "NetworkScene.h"
#include "EndScreen.h"
#include "NetworkManager.h"

NetworkScene::NetworkScene(sf::RenderWindow* hwnd, Input* in, SceneManager& sm, int pn) : sceneManager(sm)
{
	std::cout << "Loading test scene\n";

	window = hwnd;
	input = in;
	playerNum = pn;

	InitialiseScene();
	InitialisePlayers();
	InitialiseHealthBars();

	std::cout << "Loaded test scene\n";
}



NetworkScene::~NetworkScene()
{
	std::cout << "Unloading test scene\n";
	delete players[0];
	delete players[1];
	std::cout << "Unloaded test scene\n";

}

void NetworkScene::handleInput(float dt) {
	players[playerNum - 1]->handleInput(dt, sf::Keyboard::Space, sf::Keyboard::A, sf::Keyboard::D, sf::Keyboard::S, sf::Keyboard::Semicolon, sf::Keyboard::LBracket, sf::Keyboard::RBracket, sf::Keyboard::Enter);
	players[1 - (playerNum - 1)]->handleInput(dt, sf::Keyboard::Space, sf::Keyboard::A, sf::Keyboard::D, sf::Keyboard::S, sf::Keyboard::Semicolon, sf::Keyboard::LBracket, sf::Keyboard::RBracket, sf::Keyboard::Enter);
}



void NetworkScene::update(float dt) {
	players[playerNum - 1]->update(dt);
	players[1 - (playerNum - 1)]->update(dt);
	FlipCheck();

	//Loop through both players
	for (int playerIndex{}; playerIndex < 2; ++playerIndex) {
		OnlinePlayer* p1 = players[playerIndex]; //Defending player
		OnlinePlayer* p2 = players[1-playerIndex]; //Attacking player

		//Defending player has invincibility frames left, continue to the next iteration
		if (p1->getInvincibilityFramesLeft())
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
			p1->setInvincibilityFramesLeft(p1->getBlocking() ? 0 : p2->getAttack(limbIndex).getInvincibilityFrames()); //If defending player isn't blocking, give them invincibilityFrames
			
			//Move both players away from each other a bit to stop attacking player from being able to spam attacks due to player 1's invincibilityFrames
			p1->move(sf::Vector2f(-10 + (20 * p1->getFlipped()), 0));
			p2->move(sf::Vector2f(-10 + (20 * p2->getFlipped()), 0));
		}
	}
	HealthBarUpdate();
	
}



void NetworkScene::render()
{
	beginDraw();
	
	window->draw(background);
	
	window->draw(*players[0]);
	window->draw(*players[1]);
	for (int i{}; i < 4; ++i) {
		window->draw(players[0]->getAttack(i).getHitbox());
		window->draw(players[1]->getAttack(i).getHitbox());
	}

	window->draw(HealthBarBack1);
	window->draw(HealthBarBack2);
	window->draw(HealthBarFront1);
	window->draw(HealthBarFront2);
	
	endDraw();
}



void NetworkScene::HealthBarUpdate() {
	int Calc1 = 4 * players[0]->getHealth();
	int Calc2 = 4 * players[1]->getHealth();

	HealthBarFront1.setSize(sf::Vector2f(Calc1, 50));
	HealthBarFront2.setSize(sf::Vector2f(Calc2, 50));
	HealthBarFront2.setPosition((window->getSize().x - Calc2 - 25), 25);

	if (players[0]->getHealth() <= 0 || players[1]->getHealth() <= 0) {
		EndScreen* endScreen = new EndScreen(window, input, sceneManager, players[0]->getHealth() > 0);
		sceneManager.LoadScene(endScreen);
	}
}


void NetworkScene::FlipCheck() {
	bool playersFacingOppositeDirections { (players[0]->getScale().x == 1 && players[0]->getPosition().x > players[1]->getPosition().x) ||
		                                   (players[0]->getScale().x == -1 && players[0]->getPosition().x < players[1]->getPosition().x) };
	if (playersFacingOppositeDirections) {
		players[0]->setFlipped(!players[0]->getFlipped());
		players[0]->setScale(-players[0]->getScale().x, 1);
		players[1]->setFlipped(!players[1]->getFlipped());
		players[1]->setScale(-players[1]->getScale().x, 1);
	}
}



void NetworkScene::InitialiseScene() {
	background.setSize(sf::Vector2f(window->getSize()));
	if (!bgTexture.loadFromFile("Assets/Background/background.png")) { std::cerr << "Couldn't load background for fight scene\n"; }
	background.setFillColor(sf::Color::White);
	background.setTexture(&bgTexture);
	
	//audioManager.playMusicbyName("GuileTheme");
}



void NetworkScene::InitialisePlayers() {
	players[0] = new OnlinePlayer(sf::Vector2f(150, 275), 2200.0f, 175.0f, 900.0f, 100, 100, 0, false, 1, playerNum == 1);
	players[1] = new OnlinePlayer(sf::Vector2f(150, 275), 2200.0f, 175.0f, 900.0f, 100, 100, 0, true, 2, playerNum == 2);

	for (OnlinePlayer* player : players) {
		player->setInput(input);
		player->setHealth(100);
		player->setOrigin(player->getLocalBounds().width / 2.f, player->getLocalBounds().height / 2.f);
	}
	players[0]->setPosition(175, 375);
	players[1]->setPosition(975, 375);
	players[1]->setScale(-1.0f, 1.0f);
	players[1]->setFillColor(sf::Color::Red);
}



void NetworkScene::InitialiseHealthBars() {
	HealthBarFront1.setSize(sf::Vector2f(400, 50));
	HealthBarFront1.setPosition(25, 25);
	HealthBarFront1.setFillColor(sf::Color::Green);

	HealthBarBack1.setSize(sf::Vector2f(400, 50));
	HealthBarBack1.setPosition(25, 25);
	HealthBarBack1.setFillColor(sf::Color::Red);

	HealthBarFront2.setSize(sf::Vector2f(400, 50));
	HealthBarFront2.setPosition((window->getSize().x - 425), 25);
	HealthBarFront2.setFillColor(sf::Color::Green);

	HealthBarBack2.setSize(sf::Vector2f(400, 50));
	HealthBarBack2.setPosition((window->getSize().x - 425), 25);
	HealthBarBack2.setFillColor(sf::Color::Red);
}

