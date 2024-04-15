#include "MainMenu.h"
#include "NetworkScene.h"
#include "LocalScene.h"
#include "SceneManager.h"

MainMenu::MainMenu(sf::RenderWindow* hwnd, Input* in, SceneManager& sm) : sceneManager(sm)
{
	std::cout << "Loading main menu\n";

	window = hwnd;
	input = in;
	
	background.setSize(sf::Vector2f(1200, 675));
	background.setPosition(0, 0);
	background.setFillColor(sf::Color::Black);
	
	
	titleBox.setSize(sf::Vector2f(520, 60));
	titleBox.setPosition(350, 50);
	titleBox.setFillColor(sf::Color::White);
	
	
	localBox.setSize(sf::Vector2f(350, 40));
	localBox.setPosition(230, 300);
	localBox.setFillColor(sf::Color::White);

	onlineBox.setSize(sf::Vector2f(350, 40));
	onlineBox.setPosition(800, 300);
	onlineBox.setFillColor(sf::Color::White);
	
	
	if (!font.loadFromFile("font/arial.ttf")) { std::cout << "Error loading font\n"; }
	
	titleText.setFont(font);
	titleText.setString("LOSING LIMBS GAME");
	titleText.setCharacterSize(50);
	titleText.setFillColor(sf::Color::Red);
	titleText.setPosition(350, 50);
	
	
	localText.setFont(font);
	localText.setString("LOCAL");
	localText.setCharacterSize(30);
	localText.setFillColor(sf::Color::Red);
	localText.setPosition(230, 300);

	onlineText.setFont(font);
	onlineText.setString("ONLINE");
	onlineText.setCharacterSize(30);
	onlineText.setFillColor(sf::Color::Red);
	onlineText.setPosition(800, 300);

	std::cout << "Loaded main menu\n";
}



MainMenu::~MainMenu()
{
	std::cout << "Unloading main menu\n";
	std::cout << "Unloaded main menu\n";
}

void MainMenu::handleInput(float dt)
{
	bool mouseOverLocalBox = localBox.getGlobalBounds().contains(window->mapPixelToCoords(sf::Mouse::getPosition(*window)));
	bool mouseOverOnlineBox = onlineBox.getGlobalBounds().contains(window->mapPixelToCoords(sf::Mouse::getPosition(*window)));
	bool mouseDown = sf::Mouse::isButtonPressed(sf::Mouse::Left) && !mousePressedLastFrame;
	if (mouseDown) {
		if (mouseOverLocalBox) {
			LocalScene* localScene = new LocalScene(window, input, sceneManager);
			sceneManager.LoadScene(localScene);
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
	window->draw(titleBox);
	window->draw(localBox);
	window->draw(onlineBox);

	window->draw(titleText);
	window->draw(localText);
	window->draw(onlineText);
	endDraw();
}