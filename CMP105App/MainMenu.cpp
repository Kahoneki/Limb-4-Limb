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
	
	local = TextBox(230, 300, 350, 40, sf::Color::White, sf::Color::Red, 30, font, "LOCAL");
	online = TextBox(800, 300, 350, 40, sf::Color::White, sf::Color::Red, 30, font, "ONLINE");
	registration = TextBox(230, 500, 350, 40, sf::Color::White, sf::Color::Red, 30, font, "REGISTER");
	login = TextBox(800, 500, 350, 40, sf::Color::White, sf::Color::Red, 30, font, "LOGIN");

	std::cout << "Loaded main menu\n";
}



MainMenu::~MainMenu()
{
	std::cout << "Unloading main menu\n";
	std::cout << "Unloaded main menu\n";
}

void MainMenu::handleInput(float dt)
{
	sf::Vector2f mousePos{ window->mapPixelToCoords(sf::Mouse::getPosition(*window)) };
	
	bool mouseOverLocalBox = local.box.getGlobalBounds().contains(mousePos);
	bool mouseOverOnlineBox = online.box.getGlobalBounds().contains(mousePos);
	bool mouseOverRegistrationBox = registration.box.getGlobalBounds().contains(mousePos);
	bool mouseOverLoginBox = login.box.getGlobalBounds().contains(mousePos);

	bool mouseDown = sf::Mouse::isButtonPressed(sf::Mouse::Left) && !mousePressedLastFrame;
	if (mouseDown) {
		if (mouseOverLocalBox) {
			LocalScene* localScene = new LocalScene(window, input, sceneManager);
			sceneManager.LoadScene(localScene);
		}
		else if (mouseOverOnlineBox) {
			int playerNum{}; //Temp
			std::cout << "Player num (1 or 2): ";
			std::cin >> playerNum;
			NetworkScene* networkScene = new NetworkScene(window, input, sceneManager, playerNum);
			sceneManager.LoadScene(networkScene);
		}
		else if (mouseOverRegistrationBox) {
			RegistrationScreen* registrationScreen = new RegistrationScreen(window, input, sceneManager);
			sceneManager.LoadScene(registrationScreen);
		}
		/*else if (mouseOverLoginBox) {
			LoginScreen* loginScreen = new LoginScreen(window, input, sceneManager);
			sceneManager.LoadScene(loginScreen);
		}*/
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
	window->draw(registration);
	window->draw(login);
	endDraw();
}