#include "EndScreen.h"
#include "SingleplayerScene.h"
#include "LocalScene.h"
#include "MainMenu.h"
#include "ColourPallete.h"
#include "AccountManager.h"

EndScreen::EndScreen(sf::RenderWindow* hwnd, Input* in, SceneManager& sm, int sceneType, std::string resultText, int difficulty) : sceneManager(sm)
{

	std::cout << "Loading end screen\n";

	window = hwnd;
	input = in;
	scene = sceneType;

	background.setSize(sf::Vector2f(1920, 1080));
	background.setPosition(0, 0);
	background.setFillColor(BACKGROUNDCOLOUR);

	if (!font.loadFromFile("font/arial.ttf")) { std::cout << "Error loading font\n"; }

	InitialiseCallbacks();
	if (sceneType != 2) {
		//Singleplayer or local
		result = TextBox(350, 50, 520, 60, INACTIVEBOXCOLOUR, TEXTCOLOUR, 50, font, resultText.c_str());
		restart = Button(230, 330, 350, 40, INACTIVEBOXCOLOUR, ACTIVEBOXCOLOUR, TEXTCOLOUR, 30, font, restartOnClick, "RESTART");
		exitToMainMenu = Button(800, 330, 350, 40, INACTIVEBOXCOLOUR, ACTIVEBOXCOLOUR, TEXTCOLOUR, 30, font, mainMenuOnClick, "MAIN MENU");
	}
	else {
		//Online
		result = TextBox(350, 50, 850, 140, INACTIVEBOXCOLOUR, TEXTCOLOUR, 50, font, resultText.c_str()); //Make box taller and wider
		exitToMainMenu = Button(230, 330, 350, 40, INACTIVEBOXCOLOUR, ACTIVEBOXCOLOUR, TEXTCOLOUR, 30, font, mainMenuOnClick, "MAIN MENU");
	}

	difficultySetting = difficulty;

	std::cout << "Loaded end screen\n";
}

EndScreen::~EndScreen() {
	std::cout << "Unloading end screen\n";
	std::cout << "Unloaded end screen\n";
}

void EndScreen::InitialiseCallbacks()
{
	restartOnClick = [this]() {
		if (scene == 0) {
			SinglePlayerScene* singlePlayerScene = new SinglePlayerScene(window, input, sceneManager, difficultySetting);
			sceneManager.LoadScene(singlePlayerScene);
		}
		else if (scene == 1) {
			LocalScene* localScene = new LocalScene(window, input, sceneManager);
			sceneManager.LoadScene(localScene);
		}
	};

	mainMenuOnClick = [this]() {
		MainMenu* mainMenu = new MainMenu(window, input, sceneManager);
		sceneManager.LoadScene(mainMenu);
	};
}


void EndScreen::handleInput(float dt) {
	sf::Vector2f mousePos{ window->mapPixelToCoords(sf::Mouse::getPosition(*window)) };
	exitToMainMenu.processEvents(mousePos);
	if (scene != 2) {
		//Singleplayer or local
		restart.processEvents(mousePos);
	}
}

void EndScreen::update(float dt) {}

void EndScreen::render()
{
	beginDraw();

	window->draw(background);
	window->draw(result);
	window->draw(exitToMainMenu);
	if (scene != 2) {
		//Singleplayer or local
		window->draw(restart);
	}
	
	endDraw();
}