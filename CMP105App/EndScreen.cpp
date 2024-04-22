#include "EndScreen.h"
#include "LocalScene.h"

EndScreen::EndScreen(sf::RenderWindow* hwnd, Input* in, SceneManager& sm, bool player1Win) : sceneManager(sm)
{

	std::cout << "Loading end screen\n";

	window = hwnd;
	input = in;

	background.setSize(sf::Vector2f(1200, 675));
	background.setPosition(0, 0);
	background.setFillColor(sf::Color::Black);

	if (!font.loadFromFile("font/arial.ttf")) { std::cout << "Error loading font\n"; }
	win = TextBox(350, 50, 520, 60, sf::Color::White, sf::Color::Red, 50, font, player1Win ? "PLAYER 1 WINS!" : "PLAYER 2 WINS");
	restart = TextBox(230, 300, 350, 40, sf::Color::White, sf::Color::Red, 30, font, "RESTART");

	std::cout << "Loaded end screen\n";
}

EndScreen::~EndScreen() {
	std::cout << "Unloading end screen\n";
	std::cout << "Unloaded end screen\n";
}

void EndScreen::handleInput(float dt) {
	bool mouseOverBox = restart.box.getGlobalBounds().contains(window->mapPixelToCoords(sf::Mouse::getPosition(*window)));
	bool mouseDown = sf::Mouse::isButtonPressed(sf::Mouse::Left) && !mousePressedLastFrame;
	if (mouseOverBox && mouseDown) {
		LocalScene* localScene = new LocalScene(window, input, sceneManager); //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!TEMP: SCENE NEEDS TO DEPEND ON WHAT SCENE USER CAME FROM!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		sceneManager.LoadScene(localScene);
	}
	mousePressedLastFrame = sf::Mouse::isButtonPressed(sf::Mouse::Left);
}

void EndScreen::update(float dt) {}

void EndScreen::render()
{
	beginDraw();
	
	window->draw(win);
	window->draw(restart);
	
	endDraw();
}
