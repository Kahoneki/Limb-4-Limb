#include "Level.h"


Level::Level(sf::RenderWindow* hwnd, Input* in) {
	window = hwnd;
	input = in;

	// initialise game objects
	robot = Player(2200.0f, 175.0f, 900.0f, 100, 100, 0, false);
	robot.setSize(sf::Vector2f(150, 275));
	robot.setPosition(175, 375);
	robot.setInput(input);
	robot.setHealth(100);

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

	
	dummy = Player(2200.0f, 175.0f, 900.0f, 100, 100, 0, true);
	dummy.setSize(sf::Vector2f(150, 275));
	dummy.setPosition(975, 375);
	dummy.setInput(input);
	dummy.setHealth(100);
	dummy.setScale(-1.0f, 1.0f);






}

Level::~Level()
{

}

// handle user input
void Level::handleInput(float dt)
{
	// Close window on Escape pressed.
	if (input->isKeyDown(sf::Keyboard::Escape))
	{
		window->close();
	}
	
	robot.handleInput(dt, sf::Keyboard::Space, sf::Keyboard::A, sf::Keyboard::D, sf::Keyboard::R, sf::Keyboard::F, sf::Keyboard::G, sf::Keyboard::T);
	dummy.handleInput(dt, sf::Keyboard::Up, sf::Keyboard::Left, sf::Keyboard::Right, sf::Keyboard::O, sf::Keyboard::L, sf::Keyboard::SemiColon, sf::Keyboard::P);


}

// Update game objects
void Level::update(float dt)
{
	robot.update(dt);
	dummy.update(dt);
	dummy.setCollisionBox(dummy.getPosition().x, dummy.getPosition().y, -150, -275);
	for (int i{}; i < 4; ++i) {
		if (dummy.getGlobalBounds().intersects(robot.getAttack(i).getHitbox().getGlobalBounds())) {
			if (!robot.getStruck()){ 
				dummy.setHealth(dummy.getHealth() - robot.getAttack(i).getDamage());
				robot.setStruck(true);
			}
		}

		if (robot.getGlobalBounds().intersects(dummy.getAttack(i).getHitbox().getGlobalBounds())) {
			if (!dummy.getStruck()) {
				robot.setHealth(robot.getHealth() - dummy.getAttack(i).getDamage());
				dummy.setStruck(true);
			}
		}
	}
	HealthBarUpdate(robot, dummy);

}


// Render level
void Level::render()
{
	beginDraw();

	window->draw(robot);
	for (int i{}; i < 4; ++i) {
		window->draw(robot.getAttack(i).getHitbox());
	}


	window->draw(dummy);
	for (int i{}; i < 4; ++i) {
		window->draw(dummy.getAttack(i).getHitbox());
	}
	window->draw(HealthBarBack1);
	window->draw(HealthBarBack2);
	window->draw(HealthBarFront1);
	window->draw(HealthBarFront2);
	endDraw();
}


void Level::HealthBarUpdate(Player play1, Player play2) {
	int Calc1 = 4 * play1.getHealth();
	int Calc2 = 4 * play2.getHealth();

	HealthBarFront1.setSize(sf::Vector2f(Calc1, 50));
	HealthBarFront2.setSize(sf::Vector2f(Calc2, 50));
	HealthBarFront2.setPosition((window->getSize().x - Calc2 - 25), 25);

}