#include "OnlineSelectScreen.h"
#include "ColourPallete.h"
#include "SceneManager.h"
#include "SendInviteScreen.h"
#include "RandomMatchScreen.h"

OnlineSelectScreen::OnlineSelectScreen(sf::RenderWindow* hwnd, Input* in, SceneManager& sm) : sceneManager(sm), accountManager(AccountManager::getInstance())
{
	std::cout << "Loading online select screen\n";

	window = hwnd;
	input = in;

	background.setSize(sf::Vector2f(1920, 1080));
	background.setPosition(0, 0);
	background.setFillColor(BACKGROUNDCOLOUR);

	if (!font.loadFromFile("font/arial.ttf")) { std::cout << "Error loading font\n"; }

	title = TextBox(350, 50, 520, 60, INACTIVEBOXCOLOUR, TEXTCOLOUR, 50, font, "ONLINE MATCH");

	InitialiseCallbacks();
	sendInvite = Button(230, 300, 350, 40, INACTIVEBOXCOLOUR, ACTIVEBOXCOLOUR, TEXTCOLOUR, 30, font, onSendInviteButtonClick, "SEND INVITE");
	//randomMatch = Button(800, 300, 350, 40, INACTIVEBOXCOLOUR, ACTIVEBOXCOLOUR, TEXTCOLOUR, 30, font, onRandomMatchButtonClick, "JOIN RANDOM MATCH");
	backButton = Button(100, 100, 30, 30, INACTIVEBOXCOLOUR, ACTIVEBOXCOLOUR, TEXTCOLOUR, 30, font, onBackButtonClick, "<-");

	username = TextBox(50, 950, 500, 40, INACTIVEBOXCOLOUR, TEXTCOLOUR, 30, font, "");
	ranking = TextBox(50, 1000, 500, 40, INACTIVEBOXCOLOUR, TEXTCOLOUR, 30, font, "");

	if (accountManager.getUsername() != "N/A") {
		//User is signed in
		username.text.setString(accountManager.getUsername());
		ranking.text.setString(std::to_string(accountManager.getRanking()));
	}


	std::cout << "Loaded online select screen\n";
}

OnlineSelectScreen::~OnlineSelectScreen()
{
	std::cout << "Unloading online select screen\n";
	std::cout << "Unloaded online select screen\n";
}

void OnlineSelectScreen::InitialiseCallbacks()
{
	onSendInviteButtonClick = [this]() {
		SendInviteScreen* sendInviteScreen = new SendInviteScreen(window, input, sceneManager);
		sceneManager.LoadScene(sendInviteScreen);
	};

	/*onRandomMatchButtonClick = [this]() {
		RandomMatchScreen* randomMatchScreen = new RandomMatchScreen(window, input, sceneManager);
		sceneManager.LoadScene(randomMatchScreen);
	};*/

	onBackButtonClick = [this]() {
		MainMenu* mainMenu = new MainMenu(window, input, sceneManager);
		sceneManager.LoadScene(mainMenu);
	};
}


void OnlineSelectScreen::handleInput(float dt)
{
	sf::Vector2f mousePos{ window->mapPixelToCoords(sf::Mouse::getPosition(*window)) };
	sendInvite.processEvents(mousePos);
	randomMatch.processEvents(mousePos);
	backButton.processEvents(mousePos);
}

void OnlineSelectScreen::update(float dt)
{
}

void OnlineSelectScreen::render()
{
	beginDraw();
	window->draw(background);
	window->draw(title);
	window->draw(sendInvite);
	//window->draw(randomMatch);
	window->draw(backButton);
	endDraw();
}