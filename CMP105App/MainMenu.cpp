#include "MainMenu.h"
#include "NetworkScene.h"
#include "LocalScene.h"
#include "SceneManager.h"
#include "RegistrationScreen.h"

MainMenu::MainMenu(sf::RenderWindow* hwnd, Input* in, SceneManager& sm) : sceneManager(sm)
{
	std::cout << "Loading main menu\n";

	window = hwnd;
	input = in;
	
	background.setSize(sf::Vector2f(1200, 675));
	background.setPosition(0, 0);
	background.setFillColor(sf::Color::Black);
	
	if (!font.loadFromFile("font/arial.ttf")) { std::cout << "Error loading font\n"; }
	title = TextBox(350, 50, 520, 60, sf::Color::White, sf::Color::Red, 50, font, "LOSING LIMBS GAME");
	online = TextBox(800, 300, 350, 40, sf::Color::White, sf::Color::Red, 30, font, "ONLINE");
	local = TextBox(230, 300, 350, 40, sf::Color::White, sf::Color::Red, 30, font, "LOCAL");

	std::cout << "Loaded main menu\n";
}



MainMenu::~MainMenu()
{
	std::cout << "Unloading main menu\n";
	std::cout << "Unloaded main menu\n";
}

void MainMenu::handleInput(float dt)
{
	bool mouseOverLocalBox = local.box.getGlobalBounds().contains(window->mapPixelToCoords(sf::Mouse::getPosition(*window)));
	bool mouseOverOnlineBox = online.box.getGlobalBounds().contains(window->mapPixelToCoords(sf::Mouse::getPosition(*window)));
	bool mouseDown = sf::Mouse::isButtonPressed(sf::Mouse::Left) && !mousePressedLastFrame;
	if (mouseDown) {
		if (mouseOverLocalBox) {
			//TEMP - THIS IS CLEARLY WRONG
			RegistrationScreen* registrationScreen = new RegistrationScreen(window, input, sceneManager);
			sceneManager.LoadScene(registrationScreen);
		}
		else if (mouseOverOnlineBox) {
			NetworkScene* networkScene = new NetworkScene(window, input, sceneManager, 1);
			sceneManager.LoadScene(networkScene);
		}
	}
	mousePressedLastFrame = sf::Mouse::isButtonPressed(sf::Mouse::Left);
}

void MainMenu::update(float dt) {}

void MainMenu::render()
{
	beginDraw();
	window->draw(title);
	window->draw(local);
	window->draw(online);
	endDraw();
}