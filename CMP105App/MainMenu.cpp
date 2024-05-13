#include "MainMenu.h"
#include "OnlineSelectScreen.h"
#include "DifficultySelectScreen.h"
#include "LocalScene.h"
#include "SceneManager.h"
#include "AccountManager.h"
#include "MatchInvitationInterrupt.h"
#include "NetworkManager.h"
#include "RegistrationScreen.h"
#include "LoginScreen.h"
#include "ColourPallete.h"
#include "NetworkScene.h"


MainMenu::MainMenu(sf::RenderWindow* hwnd, Input* in, SceneManager& sm) : sceneManager(sm), accountManager(AccountManager::getInstance()), matchInvitationInterrupt(MatchInvitationInterrupt::getInstance()), networkManager(NetworkManager::getInstance(false))
{
	std::cout << "Loading main menu\n";

	window = hwnd;
	input = in;
	
	background.setSize(sf::Vector2f(1920, 1080));
	background.setPosition(0, 0);
	background.setFillColor(BACKGROUNDCOLOUR);
	
	if (!font.loadFromFile("font/arial.ttf")) { std::cout << "Error loading font\n"; }

	title = TextBox(310, 50, 1200, 140, INACTIVEBOXCOLOUR, TEXTCOLOUR, 100, font, "LIMB FOR LIMB");
	
	InitialiseCallbacks();
	difficultySelect = Button(450, 300, 960, 80, INACTIVEBOXCOLOUR, ACTIVEBOXCOLOUR, TEXTCOLOUR, 60, font, onDifficultySelectButtonClick, "SINGLEPLAYER");
	local = Button(450, 400, 450, 80, INACTIVEBOXCOLOUR, ACTIVEBOXCOLOUR, TEXTCOLOUR, 60, font, onLocalButtonClick, "LOCAL");
	online = Button(960, 400, 450, 80, INACTIVEBOXCOLOUR, ACTIVEBOXCOLOUR, TEXTCOLOUR, 60, font, onOnlineButtonClick, "ONLINE");
	registration = Button(450, 500, 450, 80, INACTIVEBOXCOLOUR, ACTIVEBOXCOLOUR, TEXTCOLOUR, 60, font, onRegistrationButtonClick, "REGISTER");
	login = Button(960, 500, 450, 80, INACTIVEBOXCOLOUR, ACTIVEBOXCOLOUR, TEXTCOLOUR, 60, font, onLoginButtonClick, "");
	quit = Button(450, 600, 960, 80, INACTIVEBOXCOLOUR, ACTIVEBOXCOLOUR, TEXTCOLOUR, 60, font, onQuitClick, "QUIT");
	switchOnlineStatus = Button(1475, 800, 400, 80, INACTIVEBOXCOLOUR, ACTIVEBOXCOLOUR, TEXTCOLOUR, 60, font, onSwitchOnlineStatusButtonClick, "");

	username = TextBox(50, 950, 500, 40, INACTIVEBOXCOLOUR, TEXTCOLOUR, 30, font, "");
	ranking = TextBox(50, 1000, 500, 40, INACTIVEBOXCOLOUR, TEXTCOLOUR, 30, font, "");

	if (accountManager.getUsername() != "N/A") {
		//User is logged in
		username.text.setString(accountManager.getUsername());
		username.RecentreText();
		ranking.text.setString(std::to_string(accountManager.getRanking()));
		ranking.RecentreText();

		login.text.setString("LOGOUT");
		login.RecentreText();
	}
	else {
		//User is logged out
		login.text.setString("LOGIN");
		login.RecentreText();
	}

	if (networkManager.getConnectedToServer()) {
		switchOnlineStatus.text.setString("GO OFFLINE");
		switchOnlineStatus.RecentreText();
	}
	else {
		switchOnlineStatus.text.setString("GO ONLINE");
		switchOnlineStatus.RecentreText();
	}

	std::cout << "Loaded main menu\n";
}

MainMenu::~MainMenu()
{
	std::cout << "Unloading main menu\n";
	std::cout << "Unloaded main menu\n";
}

void MainMenu::InitialiseCallbacks() {

	onDifficultySelectButtonClick = [this]() {
		DifficultySelectScreen* difficultySelectScreen = new DifficultySelectScreen(window, input, sceneManager);
		sceneManager.LoadScene(difficultySelectScreen);
	};

	onLocalButtonClick = [this]() {
		LocalScene* localScene = new LocalScene(window, input, sceneManager);
		sceneManager.LoadScene(localScene);
	};
	
	onOnlineButtonClick = [this]() {
		if (accountManager.getUsername() == "N/A") {
			LoginScreen* loginScreen = new LoginScreen(window, input, sceneManager);
			sceneManager.LoadScene(loginScreen);
		}
		else {
			OnlineSelectScreen* onlineSelectScreen = new OnlineSelectScreen(window, input, sceneManager);
			sceneManager.LoadScene(onlineSelectScreen);
		}
	};

	onRegistrationButtonClick = [this]() {
		RegistrationScreen* registrationScreen = new RegistrationScreen(window, input, sceneManager);
		sceneManager.LoadScene(registrationScreen);
	};

	onLoginButtonClick = [this]() {
		if (accountManager.getUsername() == "N/A") {
			//User isn't logged in
			LoginScreen* loginScreen = new LoginScreen(window, input, sceneManager);
			sceneManager.LoadScene(loginScreen);
		}
		else {
			//User is logged in
			sf::Packet outgoingData;
			networkManager.SendDataToServer(-1, PacketCode::Logout, outgoingData);
			accountManager.setValues("N/A", -1);
			login.text.setString("LOGIN");
			login.RecentreText();
		}
	};

	onQuitClick = [this]() {
		exit(0);
	};

	onSwitchOnlineStatusButtonClick = [this]() {
		if (networkManager.getConnectedToServer()) {
			//Go offline
			if (networkManager.AttemptToDisconnectFromServer()) {
				switchOnlineStatus.text.setString("GO ONLINE");
				accountManager.setValues("N/A", -1);
				login.text.setString("LOGIN");
				login.RecentreText();
			}
		}
		else {
			//Go online
			if (networkManager.AttemptToConnectToServer()) {
				switchOnlineStatus.text.setString("GO OFFLINE");
				switchOnlineStatus.RecentreText();
			}
		}
	};
}



void MainMenu::handleInput(float dt)
{
	sf::Vector2f mousePos{ window->mapPixelToCoords(sf::Mouse::getPosition(*window)) };

	if (matchInvitationInterrupt.getInvitationReceived()) {
		matchInvitationInterrupt.processEvents(mousePos);
	}

	else {
		//Only allow user to interact with other buttons if there isn't currently a match invitation pop up
		difficultySelect.processEvents(mousePos);
		local.processEvents(mousePos);
		online.processEvents(mousePos);
		registration.processEvents(mousePos);
		login.processEvents(mousePos);
		quit.processEvents(mousePos);
		switchOnlineStatus.processEvents(mousePos);
	}
}

void MainMenu::update(float dt)
{
	if (matchInvitationInterrupt.getReadyToLoadScene()) {
		//Open network scene to start match
		NetworkScene* networkScene{ new NetworkScene(window, input, sceneManager, matchInvitationInterrupt.getPlayerNum(), matchInvitationInterrupt.getNetworkManagerIndex())};
		sceneManager.LoadScene(networkScene);
	}
}

void MainMenu::render()
{
	beginDraw();
	window->draw(background);
	window->draw(title);
	window->draw(difficultySelect);
	window->draw(local);
	window->draw(online);
	window->draw(registration);
	window->draw(login);
	window->draw(quit);
	window->draw(switchOnlineStatus);

	if (accountManager.getUsername() != "N/A") { //N/A is default value
		//User is signed in
		window->draw(username);
		window->draw(ranking);
	}

	if (matchInvitationInterrupt.getInvitationReceived()) {
		//Match invitation has been received
		window->draw(matchInvitationInterrupt);
	}

	endDraw();
}