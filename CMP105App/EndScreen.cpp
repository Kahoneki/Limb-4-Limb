#include "EndScreen.h"
#include "LocalScene.h"

EndScreen::EndScreen(sf::RenderWindow* hwnd, Input* in, SceneManager& sm, bool player1Win) : sceneManager(sm)
{

	std::cout << "Loading end screen.\n";

	window = hwnd;
	input = in;

	background.setSize(sf::Vector2f(1200, 675));
	background.setPosition(0, 0);
	background.setFillColor(sf::Color::Black);


	winBox.setSize(sf::Vector2f(520, 60));
	winBox.setPosition(350, 50);
	winBox.setFillColor(sf::Color::White);


	restartBox.setSize(sf::Vector2f(350, 40));
	restartBox.setPosition(230, 300);
	restartBox.setFillColor(sf::Color::White);

	if (!font.loadFromFile("font/arial.ttf")) { std::cout << "Error loading font\n"; }

	winText.setFont(font);
	winText.setString(player1Win ? "PLAYER 1 WINS!" : "PLAYER 2 WINS!");
	winText.setCharacterSize(50);
	winText.setFillColor(sf::Color::Red);
	winText.setPosition(350, 50);


	restartText.setFont(font);
	restartText.setString("RESTART");
	restartText.setCharacterSize(30);
	restartText.setFillColor(sf::Color::Red);
	restartText.setPosition(230, 300);

	std::cout << "Loaded end screen.\n";
}

EndScreen::~EndScreen() {
	std::cout << "Unloading end screen.\n";
	std::cout << "Unloaded end screen\n";
}

void EndScreen::handleInput(float dt) {
	bool mouseOverBox = restartBox.getGlobalBounds().contains(window->mapPixelToCoords(sf::Mouse::getPosition(*window)));
	bool mouseDown = sf::Mouse::isButtonPressed(sf::Mouse::Left) && !mousePressedLastFrame;
	if (mouseOverBox && mouseDown) {
		LocalScene* localScene = new LocalScene(window, input, sceneManager); //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!0 IS A TEMP VALUE!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		sceneManager.LoadScene(localScene);
	}
	mousePressedLastFrame = sf::Mouse::isButtonPressed(sf::Mouse::Left);
}

void EndScreen::update(float dt) {}

void EndScreen::render()
{
	beginDraw();
	
	window->draw(winBox);
	window->draw(restartBox);
	window->draw(winText);
	window->draw(restartText);
	
	endDraw();
}
