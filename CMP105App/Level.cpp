#include "Level.h"


Level::Level(sf::RenderWindow* hwnd, Input* in) {
	window = hwnd;
	input = in;

	// initialise game objects
	robot = Player(2200.0f, 175.0f, 900.0f, 100, 100);
	robot.setSize(sf::Vector2f(150, 275));
	robot.setPosition(175, 375);
	robot.setInput(input);
	if (!robotTexture.loadFromFile("Assets/Sprites/Robot-Full.png")) { std::cerr << "Robot texture failed to load"; }
	robot.setTexture(&robotTexture);

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
	
	robot.handleInput(dt);

}

// Update game objects
void Level::update(float dt)
{
	robot.update(dt);
	HealthBarUpdate(robot, robot);
	robot.setHealth(robot.getHealth() - 0.0000001);
}


// Render level
void Level::render()
{
	beginDraw();

	window->draw(robot);
	window->draw(robot.getJab().getHitbox());
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