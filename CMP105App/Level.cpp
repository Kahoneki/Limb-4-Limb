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

	
	dummy = Player(2200.0f, 175.0f, 900.0f, 100, 100);
	dummy.setSize(sf::Vector2f(150, 275));
	dummy.setPosition(975, 375);
	dummy.setInput(input);
	if (!robotTexture.loadFromFile("Assets/Sprites/Robot-Full.png")) { std::cerr << "Robot texture failed to load"; }
	dummy.setTexture(&robotTexture);
	dummy.setScale(-1.f, 1.f);




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
	
	robot.handleInput(dt, sf::Keyboard::Space, sf::Keyboard::A, sf::Keyboard::D);
	dummy.handleInput(dt, sf::Keyboard::T, sf::Keyboard::F, sf::Keyboard::H);


}

// Update game objects
void Level::update(float dt)
{
	robot.update(dt);
	dummy.update(dt);
	
}


// Render level
void Level::render()
{
	beginDraw();

	window->draw(robot);
	window->draw(robot.getJab().getHitbox());

	window->draw(dummy);
	endDraw();
}
