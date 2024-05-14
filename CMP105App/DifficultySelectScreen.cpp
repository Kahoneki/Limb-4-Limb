#include "DifficultySelectScreen.h"
#include "MainMenu.h"
#include "SingleplayerScene.h"
#include "SceneManager.h"
#include "ColourPallete.h"
#include "MatchInvitationInterrupt.h"
#include "NetworkScene.h"

DifficultySelectScreen::DifficultySelectScreen(sf::RenderWindow* hwnd, Input* in, SceneManager& sm) : sceneManager(sm), matchInvitationInterrupt(MatchInvitationInterrupt::getInstance())
{
	std::cout << "Loading difficulty select screen\n";

	window = hwnd;
	input = in;

	background.setSize(sf::Vector2f(1920, 1080));
	background.setPosition(0, 0);
	background.setFillColor(BACKGROUNDCOLOUR);

	if (!font.loadFromFile("font/arial.ttf")) { std::cout << "Error loading font\n"; }

	selectDifficulty = TextBox(310, 50, 1200, 60, INACTIVEBOXCOLOUR, TEXTCOLOUR, 50, font, "SELECT DIFFICULTY");

	InitialiseCallbacks();
	one = Button(310, 500, 200, 200, INACTIVEBOXCOLOUR, ACTIVEBOXCOLOUR, TEXTCOLOUR, 80, font, oneOnClick, "1");
	two = Button(560, 500, 200, 200, INACTIVEBOXCOLOUR, ACTIVEBOXCOLOUR, TEXTCOLOUR, 80, font, twoOnClick, "2");
	three = Button(810, 500, 200, 200, INACTIVEBOXCOLOUR, ACTIVEBOXCOLOUR, TEXTCOLOUR, 80, font, threeOnClick, "3");
	four = Button(1060, 500, 200, 200, INACTIVEBOXCOLOUR, ACTIVEBOXCOLOUR, TEXTCOLOUR, 80, font, fourOnClick, "4");
	five = Button(1310, 500, 200, 200, INACTIVEBOXCOLOUR, ACTIVEBOXCOLOUR, TEXTCOLOUR, 80, font, fiveOnClick, "5");
	selectionBorder = sf::RectangleShape(sf::Vector2f(220, 220));
	selectionBorder.setFillColor(TEXTCOLOUR);
	start = Button(310, 750, 1200, 300, INACTIVEBOXCOLOUR, ACTIVEBOXCOLOUR, TEXTCOLOUR, 120, font, startOnClick, "START");
	backButton = Button(100, 100, 30, 30, INACTIVEBOXCOLOUR, ACTIVEBOXCOLOUR, TEXTCOLOUR, 30, font, backButtonOnClick, "<-");

	difficulty = 0;

	std::cout << "Loaded difficulty select screen\n";
}


DifficultySelectScreen::~DifficultySelectScreen()
{
	std::cout << "Unloading difficulty select screen\n";
	std::cout << "Unloaded difficulty select screen\n";
}


void DifficultySelectScreen::InitialiseCallbacks()
{
	oneOnClick = [this]() {
		difficulty = 1;
		selectionBorder.setPosition(300, 490);
	};
	twoOnClick = [this]() {
		difficulty = 2;
		selectionBorder.setPosition(550, 490);
	};
	threeOnClick = [this]() {
		difficulty = 3;
		selectionBorder.setPosition(800, 490);
	};
	fourOnClick = [this]() {
		difficulty = 4;
		selectionBorder.setPosition(1050, 490);
	};
	fiveOnClick =  [this]() {
		difficulty = 5;
		selectionBorder.setPosition(1300, 490);
	};

	startOnClick = [this]() {
		SinglePlayerScene* singlePlayerScene = new SinglePlayerScene(window, input, sceneManager, difficulty);
		sceneManager.LoadScene(singlePlayerScene);
	};

	backButtonOnClick = [this]() {
		MainMenu* mainMenu = new MainMenu(window, input, sceneManager);
		sceneManager.LoadScene(mainMenu);
	};
}



void DifficultySelectScreen::handleInput(float dt)
{
	sf::Vector2f mousePos{ window->mapPixelToCoords(sf::Mouse::getPosition(*window)) };

	if (matchInvitationInterrupt.getInvitationReceived()) {
		matchInvitationInterrupt.processEvents(mousePos);
	}

	else {
		//Only allow user to interact with other buttons if there isn't currently a match invitation pop up
		one.processEvents(mousePos);
		two.processEvents(mousePos);
		three.processEvents(mousePos);
		four.processEvents(mousePos);
		five.processEvents(mousePos);
		backButton.processEvents(mousePos);

		if (difficulty != 0) {
			start.processEvents(mousePos);
		}
	}
}

void DifficultySelectScreen::update(float dt)
{
	if (matchInvitationInterrupt.getReadyToLoadScene()) {
		//Open network scene to start match
		NetworkScene* networkScene{ new NetworkScene(window, input, sceneManager, matchInvitationInterrupt.getPlayerNum(), matchInvitationInterrupt.getNetworkManagerIndex()) };
		sceneManager.LoadScene(networkScene);
	}
}

void DifficultySelectScreen::render()
{
	beginDraw();
	window->draw(background);
	window->draw(selectDifficulty);
	if (difficulty != 0) {
		window->draw(start);
		window->draw(selectionBorder);
	}
	window->draw(one);
	window->draw(two);
	window->draw(three);
	window->draw(four);
	window->draw(five);
	window->draw(backButton);

	if (matchInvitationInterrupt.getInvitationReceived()) {
		//Match invitation has been received
		window->draw(matchInvitationInterrupt);
	}
	endDraw();
}