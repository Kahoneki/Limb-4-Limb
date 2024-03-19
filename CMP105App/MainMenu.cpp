#include "MainMenu.h"

MainMenu::MainMenu(sf::RenderWindow* hwnd, Input* in, SceneManager& sm) : sceneManager(sm)
{
	window = hwnd;
	input = in;

	background.setSize(sf::Vector2f(1200, 675));
	background.setPosition(0, 0);
	background.setFillColor(sf::Color::Black);


	titleBox.setSize(sf::Vector2f(520, 60));
	titleBox.setPosition(350, 50);
	titleBox.setFillColor(sf::Color::White);


	startBox.setSize(sf::Vector2f(350, 40));
	startBox.setPosition(230, 300);
	startBox.setFillColor(sf::Color::White);


	if (!font.loadFromFile("font/arial.ttf")) {
		std::cout << "Error loading font\n";
	}

	titleText.setFont(font);
	titleText.setString("LOSING LIMBS GAME");
	titleText.setCharacterSize(50);
	titleText.setFillColor(sf::Color::Red);
	titleText.setPosition(350, 50);


	startText.setFont(font);
	startText.setString("START");
	startText.setCharacterSize(30);
	startText.setFillColor(sf::Color::Red);
	startText.setPosition(230, 300);

	std::cout << "Constructed\n";
}



MainMenu::~MainMenu()
{
}

void MainMenu::handleInput(float dt)
{
}

void MainMenu::update(float dt)
{
}

void MainMenu::render()
{
	beginDraw();
	window->draw(titleBox);
	window->draw(startBox);
	window->draw(titleText);
	window->draw(startText);
	endDraw();
}
