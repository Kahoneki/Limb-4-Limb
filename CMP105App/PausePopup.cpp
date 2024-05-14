#include "PausePopup.h"
#include "ColourPallete.h"
#include "Framework/AudioManager.h"
#include "MainMenu.h"
#include <iostream>

PausePopup::PausePopup(Input* in)
{
	input = in;
	escapePressedLastFrame = false;
	pausePopupEnabled = false;
	mainMenuButtonClicked = false;

	InitialiseCallbacks();

	translucentBlackScreenOverlay = sf::RectangleShape(sf::Vector2f(1920, 1080));
	translucentBlackScreenOverlay.setFillColor(sf::Color(0, 0, 0, 180));

	if (!font.loadFromFile("font/arial.ttf")) { std::cout << "Error loading font\n"; }
	popupBox = TextBox(817.5, 150, 285, 100, INACTIVEBOXCOLOUR, TEXTCOLOUR, 70, font, "PAUSED");
	resume = Button(735, 275, 200, 50, INACTIVEBOXCOLOUR, ACTIVEBOXCOLOUR, TEXTCOLOUR, 40, font, resumeOnClick, "RESUME");
	exitToMainMenu = Button(985, 275, 200, 50, INACTIVEBOXCOLOUR, ACTIVEBOXCOLOUR, TEXTCOLOUR, 40, font, mainMenuOnClick, "LEAVE");
	quit = Button(735, 350, 450, 50, INACTIVEBOXCOLOUR, ACTIVEBOXCOLOUR, TEXTCOLOUR, 40, font, quitOnClick, "QUIT GAME");
}

void PausePopup::InitialiseCallbacks()
{
	resumeOnClick = [this]() {
		pausePopupEnabled = false;
	};

	mainMenuOnClick = [this]() {
		mainMenuButtonClicked = true;
	};

	quitOnClick = [this]() {
		exit(0);
	};
}



void PausePopup::processEvents(sf::Vector2f mousePos)
{
	if (input->isKeyDown(sf::Keyboard::Escape) && !escapePressedLastFrame) {
		pausePopupEnabled = !pausePopupEnabled;
	}

	if (pausePopupEnabled) {
		resume.processEvents(mousePos);
		exitToMainMenu.processEvents(mousePos);
		quit.processEvents(mousePos);
	}

	escapePressedLastFrame = input->isKeyDown(sf::Keyboard::Escape);
}



void PausePopup::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	target.draw(translucentBlackScreenOverlay, states);
	target.draw(popupBox, states);
	target.draw(resume, states);
	target.draw(exitToMainMenu, states);
	target.draw(quit, states);
}



bool PausePopup::getPausePopupEnabled() { return pausePopupEnabled; }

bool PausePopup::getMainMenuButtonClicked() { return mainMenuButtonClicked; }