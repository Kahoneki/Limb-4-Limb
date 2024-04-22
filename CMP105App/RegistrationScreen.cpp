#include "RegistrationScreen.h"

RegistrationScreen::RegistrationScreen(sf::RenderWindow* hwnd, Input* in, SceneManager& sm) : sceneManager(sm)
{
	std::cout << "Loading registration screen\n";

	window = hwnd;
	input = in;

	background.setSize(sf::Vector2f(1200, 675));
	background.setPosition(0, 0);
	background.setFillColor(sf::Color::Black);

	if (!font.loadFromFile("font/arial.ttf")) { std::cout << "Error loading font\n"; }
	usernameBox = InputBox(230, 300, 350, 40, sf::Color::White, sf::Color::Red, 30, font, 50, InputBox::AcceptableCharacterPresets[InputBox::AcceptableCharacterPreset::ALPHANUMERIC], "test", false, sf::Color::Green);
	usernameBox.setInput(input);
	usernameBox.setWindow(window);

	std::cout << "Loaded registration screen\n";
}


RegistrationScreen::~RegistrationScreen()
{
	std::cout << "Unloading registration screen\n";
	std::cout << "Unloaded registration screen\n";
}


void RegistrationScreen::handleInput(float dt) {}


void RegistrationScreen::update(float dt) {
	sf::Vector2f mousePos{ window->mapPixelToCoords(sf::Mouse::getPosition(*window)) };
	usernameBox.processEvents(dt, mousePos);
}


void RegistrationScreen::render()
{
	beginDraw();
	window->draw(usernameBox);
	endDraw();
}