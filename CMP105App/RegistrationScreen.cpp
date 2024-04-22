#include "RegistrationScreen.h"
#include "MainMenu.h"
#include "SceneManager.h"

RegistrationScreen::RegistrationScreen(sf::RenderWindow* hwnd, Input* in, SceneManager& sm) : sceneManager(sm)
{
	std::cout << "Loading registration screen\n";

	window = hwnd;
	input = in;

	background.setSize(sf::Vector2f(1200, 675));
	background.setPosition(0, 0);
	background.setFillColor(sf::Color::Black);

	if (!font.loadFromFile("font/arial.ttf")) { std::cout << "Error loading font\n"; }
	
	mousePressedLastFrame = false;
	
	usernameBox = InputBox(230, 300, 350, 40, sf::Color::White, sf::Color::Red, 30, font, 50, InputBox::AcceptableCharacterPresets[InputBox::AcceptableCharacterPreset::ALPHANUMERIC], "Username", false, sf::Color::Green);
	usernameBox.setInput(input);
	usernameBox.setWindow(window);

	registerButton = TextBox(1500, 800, 350, 40, sf::Color::White, sf::Color::Red, 30, font, "Register");
	backButton = TextBox(100, 100, 30, 30, sf::Color::White, sf::Color::Red, 30, font, "<-");
	statusBar = TextBox(230, 800, 700, 40, sf::Color::White, sf::Color::Red, 30, font, "Placeholder");
	displayStatusBar = false;

	std::cout << "Loaded registration screen\n";
}


RegistrationScreen::~RegistrationScreen()
{
	std::cout << "Unloading registration screen\n";
	std::cout << "Unloaded registration screen\n";
}


void RegistrationScreen::handleInput(float dt) {
	sf::Vector2f mousePos{ window->mapPixelToCoords(sf::Mouse::getPosition(*window)) };
	usernameBox.processEvents(dt, mousePos);

	bool mouseOverRegisterButton{ registerButton.box.getGlobalBounds().contains(mousePos) };
	bool mouseOverBackButton{ backButton.box.getGlobalBounds().contains(mousePos) };

	bool mouseDown = sf::Mouse::isButtonPressed(sf::Mouse::Left) && !mousePressedLastFrame;

	if (mouseDown) {
		if (mouseOverRegisterButton) {
			const char* usernameInput{ usernameBox.getTypedText() };
			std::cout << usernameInput << '\n';
		}
		else if (mouseOverBackButton) {
			MainMenu* mainMenu = new MainMenu(window, input, sceneManager);
			sceneManager.LoadScene(mainMenu);
		}
	}

	mousePressedLastFrame = sf::Mouse::isButtonPressed(sf::Mouse::Left);
}


void RegistrationScreen::update(float dt) {}


void RegistrationScreen::render()
{
	beginDraw();
	window->draw(usernameBox);
	window->draw(registerButton);
	window->draw(backButton);
	if (displayStatusBar) {
		window->draw(statusBar);
	}
	endDraw();
}