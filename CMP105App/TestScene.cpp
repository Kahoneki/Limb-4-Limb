#include "TestScene.h"

TestScene::TestScene(sf::RenderWindow* hwnd, Input* in, SceneManager& sm) : sceneManager(sm)
{
	window = hwnd;
	input = in;

	InitialiseScene();
	InitialisePlayers();
	InitialiseHealthBars();

	std::cout << "Loaded test scene\n";
}



TestScene::~TestScene()
{
	delete players[0];
	delete players[1];
}

void TestScene::handleInput(float dt) {
	players[0]->handleInput(dt, sf::Keyboard::W, sf::Keyboard::A, sf::Keyboard::D, sf::Keyboard::S, sf::Keyboard::R, sf::Keyboard::F, sf::Keyboard::G, sf::Keyboard::T);
	players[1]->handleInput(dt, sf::Keyboard::I, sf::Keyboard::J, sf::Keyboard::L, sf::Keyboard::K, sf::Keyboard::Num9, sf::Keyboard::O, sf::Keyboard::P, sf::Keyboard::Num0);
}



void TestScene::update(float dt) {
	
	players[0]->update(dt);
	players[1]->update(dt);

	//Loop through both players
	for (int playerIndex{}; playerIndex < 2; ++playerIndex) {
		Player* p1 = players[playerIndex]; //Defending player
		Player* p2 = players[1-playerIndex]; //Attacking player

		//Defending player has stun frames left, continue to the next iteration
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
			std::cout << p1->getBlocking() << '\n';
			
			p1->setHealth(p1->getHealth() - damageAmount);
			p1->setStunFramesLeft(p1->getBlocking() ? 0 : p2->getAttack(limbIndex).getHitstun()); //If defending player isn't blocking, give them hitstun
			
			//Move both players away from each other a bit to stop attacking player from being able to spam attacks due to player 1's hitstun
			p1->move(sf::Vector2f(-10 + (20 * p1->getFlipped()), 0));
			p2->move(sf::Vector2f(-10 + (20 * p2->getFlipped()), 0));
		}
	}
	HealthBarUpdate();
	FlipCheck();
}



void TestScene::render()
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



void TestScene::HealthBarUpdate() {
	int Calc1 = 4 * players[0]->getHealth();
	int Calc2 = 4 * players[1]->getHealth();

	HealthBarFront1.setSize(sf::Vector2f(Calc1, 50));
	HealthBarFront2.setSize(sf::Vector2f(Calc2, 50));
	HealthBarFront2.setPosition((window->getSize().x - Calc2 - 25), 25);

	if (players[0]->getHealth() <= 0 || players[1]->getHealth() <= 0) {
		; //----------------------------------------------------------------------ADD LOAD TO END SCREEN-------------------------------------------------------------------------------------------------//
	}
}


void TestScene::FlipCheck() {
	bool playersFacingOppositeDirections { (players[0]->getScale().x == 1 && players[0]->getPosition().x > players[1]->getPosition().x) || (players[0]->getScale().x == -1 && players[0]->getPosition().x < players[1]->getPosition().x) };
	if (playersFacingOppositeDirections) {
		players[0]->setFlipped(!players[0]->getFlipped());
		players[0]->setScale(-players[0]->getScale().x, 1);
		players[1]->setFlipped(!players[1]->getFlipped());
		players[1]->setScale(-players[1]->getScale().x, 1);
	}
}



void TestScene::InitialiseScene() {
	background.setSize(sf::Vector2f(window->getSize()));
	if (!bgTexture.loadFromFile("Assets/Background/background.png")) { std::cerr << "Couldn't load background for fight scene\n"; }
	background.setFillColor(sf::Color::White);
	background.setTexture(&bgTexture);
	
	audioManager.playMusicbyName("GuileTheme");
}



void TestScene::InitialisePlayers() {
	players[0] = new Player(2200.0f, 175.0f, 900.0f, 100, 100, 0, false);
	players[1] = new Player(2200.0f, 175.0f, 900.0f, 100, 100, 0, true);

	for (Player* player : players) {
		player->setSize(sf::Vector2f(150, 275));
		player->setInput(input);
		player->setHealth(100);
		player->setOrigin(player->getLocalBounds().width / 2.f, player->getLocalBounds().height / 2.f);
	}
	players[0]->setPosition(175, 375);
	players[1]->setPosition(975, 375);
	players[1]->setScale(-1.0f, 1.0f);
	players[1]->setFillColor(sf::Color::Red);
}



void TestScene::InitialiseHealthBars() {
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

