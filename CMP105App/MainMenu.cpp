#include "MainMenu.h"
#include "NetworkScene.h"
#include "LocalScene.h"
#include "SceneManager.h"
#include "RegistrationScreen.h"
#include "LoginScreen.h"
#include "ColourPallete.h"


MainMenu::MainMenu(sf::RenderWindow* hwnd, Input* in, SceneManager& sm) : sceneManager(sm)
{
	std::cout << "Loading main menu\n";

	window = hwnd;
	input = in;
	
	background.setSize(sf::Vector2f(1920, 1080));
	background.setPosition(0, 0);
	background.setFillColor(BACKGROUNDCOLOUR);
	
	if (!font.loadFromFile("font/arial.ttf")) { std::cout << "Error loading font\n"; }

	title = TextBox(350, 50, 520, 60, INACTIVEBOXCOLOUR, TEXTCOLOUR, 50, font, "LOSING LIMBS GAME");
	
	InitialiseCallbacks();
	local = Button(230, 300, 350, 40, INACTIVEBOXCOLOUR, ACTIVEBOXCOLOUR, TEXTCOLOUR, 30, font, onLocalButtonClick, "LOCAL");
	online = Button(800, 300, 350, 40, INACTIVEBOXCOLOUR, ACTIVEBOXCOLOUR, TEXTCOLOUR, 30, font, onOnlineButtonClick, "ONLINE");
	registration = Button(230, 500, 350, 40, INACTIVEBOXCOLOUR, ACTIVEBOXCOLOUR, TEXTCOLOUR, 30, font, onRegistrationButtonClick, "REGISTER");
	login = Button(800, 500, 350, 40, INACTIVEBOXCOLOUR, ACTIVEBOXCOLOUR, TEXTCOLOUR, 30, font, onLoginButtonClick, "LOGIN");


	std::cout << "Loaded main menu\n";
}

MainMenu::~MainMenu()
{
	std::cout << "Unloading main menu\n";
	std::cout << "Unloaded main menu\n";
}

void MainMenu::InitialiseCallbacks() {
	onLocalButtonClick = [this]() {
		LocalScene* localScene = new LocalScene(window, input, sceneManager);
		sceneManager.LoadScene(localScene);
	};
	
	onOnlineButtonClick = [this]() {
		int playerNum{}; //Temp
		std::cout << "Player num (1 or 2): ";
		std::cin >> playerNum;
		NetworkScene* networkScene = new NetworkScene(window, input, sceneManager, playerNum);
		sceneManager.LoadScene(networkScene);
	};

	onRegistrationButtonClick = [this]() {
		RegistrationScreen* registrationScreen = new RegistrationScreen(window, input, sceneManager);
		sceneManager.LoadScene(registrationScreen);
	};

	onLoginButtonClick = [this]() {
		LoginScreen* loginScreen = new LoginScreen(window, input, sceneManager);
		sceneManager.LoadScene(loginScreen);
	};
}



void MainMenu::handleInput(float dt) {}

void MainMenu::update(float dt) {
	
	sf::Vector2f mousePos{ window->mapPixelToCoords(sf::Mouse::getPosition(*window)) };

	local.processEvents(mousePos);
	online.processEvents(mousePos);
	registration.processEvents(mousePos);
	login.processEvents(mousePos);
}

void MainMenu::render()
{
	beginDraw();
	window->draw(background);
	window->draw(title);
	window->draw(local);
	window->draw(online);
	window->draw(registration);
	window->draw(login);
	endDraw();
}